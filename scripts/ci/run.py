#!/usr/bin/env python3
"""Run push validation against isolated current working-tree inputs."""
import argparse
import datetime
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile

ROOT = Path(__file__).resolve().parents[2]
LANES = ('full', 'minimal', 'release', 'static', 'licensing')
CONFIG_REVISION = '0ccd25f323662f50a47bb048be636160f2b7c135'


def git(*args):
    return subprocess.check_output(['git', '-C', str(ROOT), *args])


def snapshot(destination):
    tracked = git('ls-files', '-z').split(b'\0')
    new = git('ls-files', '--others', '--exclude-standard', '-z').split(b'\0')
    for raw in dict.fromkeys(tracked + new):
        if not raw:
            continue
        name = os.fsdecode(raw)
        source = ROOT / name
        if source.is_symlink() or source.is_file():
            target = destination / name
            target.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(source, target, follow_symlinks=False)
        elif source.exists():
            raise RuntimeError(f'Unsupported snapshot input: {name}')
    return [os.fsdecode(raw) for raw in new if raw]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--lane', choices=LANES, action='append')
    parser.add_argument('--acquire-images', action='store_true', help='Explicitly pull required images before offline acceptance')
    parser.add_argument('--prepare', action='store_true', help='Acquire inputs and prepare Config explicitly')
    parser.add_argument('--config-source', type=Path, help='Use an existing checkout containing the exact Config revision')
    args = parser.parse_args()
    output = ROOT / 'build' / 'ci'
    output.mkdir(parents=True, exist_ok=True)
    output = Path(tempfile.mkdtemp(prefix=datetime.datetime.now(datetime.timezone.utc).strftime('%Y%m%dT%H%M%SZ-'), dir=output))
    print(f'CI logs: {output}', flush=True)
    report = {'revision': git('rev-parse', 'HEAD').decode().strip(),
              'status': git('status', '--porcelain=v1', '-z').decode(errors='surrogateescape'),
              'platform': 'linux/amd64', 'lanes': {}}
    report['dirty'] = bool(report['status'])
    failed = False
    try:
        runtime = shutil.which('docker') or shutil.which('podman')
        if not runtime:
            raise RuntimeError('Neither Docker nor Podman is available; install a container runtime.')
        report['runtime'] = subprocess.check_output([runtime, '--version'], text=True).strip()
        images = json.loads((ROOT / 'scripts/ci/images.json').read_text())
        prepared = ROOT / 'build/ci-prepared'
        report['config_revision'] = CONFIG_REVISION
        if args.acquire_images or args.prepare:
            for image in dict.fromkeys(images['licensing' if lane == 'licensing' else 'build'] for lane in args.lane or LANES):
                subprocess.run([runtime, 'pull', '--platform', report['platform'], image], check=True)
        if args.prepare:
            prepared.mkdir(parents=True, exist_ok=True)
            config = prepared / 'config'
            if config.exists():
                raise RuntimeError('Preparation already exists; choose a fresh build/ci-prepared directory by archiving the old one.')
            if args.config_source:
                archive = subprocess.check_output(['git', '-C', str(args.config_source), 'archive', CONFIG_REVISION])
            else:
                acquired = prepared / 'acquired-config'
                subprocess.run(['git', 'clone', '--no-checkout', 'https://github.com/lebedenko/holonight-config.git', str(acquired)], check=True)
                archive = subprocess.check_output(['git', '-C', str(acquired), 'archive', CONFIG_REVISION])
            config.mkdir()
            subprocess.run(['tar', '-x', '-C', str(config)], input=archive, check=True)
        elif any(lane != 'licensing' for lane in args.lane or LANES):
            if not (prepared / 'manifest.json').is_file():
                raise RuntimeError('Missing prepared Config artifact; run task ci:prepare explicitly.')
            manifest = json.loads((prepared / 'manifest.json').read_text())
            if manifest != {'config_revision': CONFIG_REVISION, 'image': images['build']}:
                raise RuntimeError('Prepared Config identity mismatch; run explicit CI preparation.')
        with tempfile.TemporaryDirectory(prefix='holonight-qt-ci-') as temporary:
            source = Path(temporary) / 'source'
            source.mkdir()
            account_options = []
            if os.getuid() != 0:
                passwd = Path(temporary) / 'passwd'
                group = Path(temporary) / 'group'
                passwd.write_text('root:x:0:0:root:/root:/bin/sh\n'
                                  f'ci:x:{os.getuid()}:{os.getgid()}:CI:/work/build/home:/bin/sh\n'
                                  'nobody:x:65534:65534:nobody:/nonexistent:/bin/sh\n')
                group.write_text('root:x:0:\n' + f'ci:x:{os.getgid()}:\n' + 'nobody:x:65534:\n')
                account_options = ['--mount', f'type=bind,src={passwd},dst=/etc/passwd,readonly',
                                   '--mount', f'type=bind,src={group},dst=/etc/group,readonly']
            report['untracked_inputs'] = snapshot(source)
            if report['untracked_inputs']:
                print('Untracked inputs included; add before pushing:')
                for name in report['untracked_inputs']:
                    print(f'  {name!r}')
            for lane in ('prepare',) if args.prepare else args.lane or LANES:
                image = images['licensing' if lane == 'licensing' else 'build']
                command = [runtime, 'run', '--rm', '--pull=never', '--network=none', '--platform', report['platform'], '--user', f'{os.getuid()}:{os.getgid()}',
                           '--mount', f'type=bind,src={source},dst=/input,readonly',
                           '--tmpfs', '/work:rw,exec,mode=1777', '--workdir', '/work',
                           '--env', 'JOBS=' + os.environ.get('JOBS', '2'),
                           '--entrypoint', '/bin/sh', image, '/input/scripts/ci/lane.sh', lane]
                if Path(runtime).name == 'podman' and os.getuid() != 0:
                    command.insert(2, '--userns=keep-id')
                if lane != 'licensing':
                    prepared.mkdir(parents=True, exist_ok=True)
                    index = command.index(image)
                    command[index:index] = ['--mount', f'type=bind,src={prepared},dst=/prepared' + ('' if lane == 'prepare' else ',readonly')]
                image_index = command.index(image)
                command[image_index:image_index] = account_options
                print(f'Running {lane}: {image}', flush=True)
                with (output / f'{lane}.log').open('w') as log:
                    result = subprocess.run(command, stdout=log, stderr=subprocess.STDOUT)
                report['lanes'][lane] = {'image': image, 'exit_code': result.returncode}
                print(f'{lane}: {"PASS" if result.returncode == 0 else "FAIL"} (see {output / (lane + ".log")})', flush=True)
                if result.returncode != 0:
                    print((output / f'{lane}.log').read_text(errors='replace'), flush=True)
                failed |= result.returncode != 0
                if lane == 'prepare' and result.returncode == 0:
                    (prepared / 'manifest.json').write_text(json.dumps({'config_revision': CONFIG_REVISION, 'image': images['build']}, indent=2) + '\n')
    except (OSError, RuntimeError, subprocess.CalledProcessError) as error:
        report['error'] = str(error)
        print(f'CI failed: {error}', file=sys.stderr)
        failed = True
    finally:
        (output / 'results.json').write_text(json.dumps(report, indent=2) + '\n')
    return int(failed)


if __name__ == '__main__':
    sys.exit(main())
