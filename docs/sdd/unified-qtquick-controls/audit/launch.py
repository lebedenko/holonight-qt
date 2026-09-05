#!/usr/bin/env python3
"""Collect an empty-profile application run without changing the desktop environment."""
import argparse
import datetime
import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess
import tempfile

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--prefix', required=True, type=Path)
parser.add_argument('--mode', choices=['ordinary', 'isolated', 'manual'], default='isolated')
parser.add_argument('--seconds', type=int, default=12)
parser.add_argument('--backtrace', action='store_true')
parser.add_argument('command', nargs=argparse.REMAINDER)
args = parser.parse_args()
command = args.command
if command and command[0] == '--':
    command = command[1:]
if not command:
    parser.error('an executable is required after --')
exe = shutil.which(command[0])
if not exe:
    parser.error('executable not found')
if 'polkit' in Path(exe).name:
    parser.error('authentication requires the separate-session procedure in CHECKLIST.md')
root = Path(tempfile.mkdtemp(prefix='uqc-audit-'))
env = os.environ.copy()
for key in ['QML2_IMPORT_PATH', 'QT_QUICK_CONTROLS_CONF', 'QT_QUICK_CONTROLS_FALLBACK_STYLE']:
    env.pop(key, None)
for key in ['XDG_CONFIG_HOME', 'XDG_DATA_HOME', 'XDG_CACHE_HOME', 'XDG_STATE_HOME']:
    path = root / key.lower()
    path.mkdir(mode=0o700)
    env[key] = str(path)
# Preserve XDG_RUNTIME_DIR: Wayland and logind use the existing session runtime.
prefix = args.prefix.resolve()
if not (prefix / 'lib/qt6/qml/Holonight/qmldir').is_file():
    parser.error('prefix must contain installed Holonight module')
env.update(QML_IMPORT_PATH=str(prefix / 'lib/qt6/qml'),
           QT_PLUGIN_PATH=str(prefix / 'lib/qt6/plugins'),
           QT_QUICK_CONTROLS_STYLE='Holonight', QML_IMPORT_TRACE='1', QT_DEBUG_PLUGINS='1',
           QT_LOGGING_RULES='*.debug=true;*.info=true', QT_FORCE_STDERR_LOGGING='1', DEBUGINFOD_URLS='')
if args.mode != 'manual':
    env.update(QT_QPA_PLATFORM='offscreen', QT_QUICK_BACKEND='software')
selected = ['QT_QPA_PLATFORM', 'QT_QPA_PLATFORMTHEME', 'QT_QUICK_CONTROLS_STYLE',
            'QT_QUICK_CONTROLS_CONF', 'QT_QUICK_CONTROLS_FALLBACK_STYLE',
            'QML_IMPORT_PATH', 'QML2_IMPORT_PATH', 'QT_PLUGIN_PATH', 'QT_QUICK_BACKEND',
            'QML_IMPORT_TRACE', 'QT_DEBUG_PLUGINS', 'QT_LOGGING_RULES', 'XDG_SESSION_ID']
metadata = dict(date=datetime.datetime.now(datetime.timezone.utc).isoformat(),
                executable=str(Path(exe).resolve()),
                sha256=hashlib.sha256(Path(exe).read_bytes()).hexdigest(),
                command=command, mode=args.mode, backtrace=args.backtrace,
                environment={k: env.get(k) for k in selected})
cmd = [exe, *command[1:]]
if args.backtrace:
    cmd = ['gdb', '--batch', '-ex', 'set debuginfod enabled off',
           '-ex', 'set pagination off', '-ex', 'run',
           '-ex', 'thread apply all bt', '--args', *cmd]
if args.mode != 'manual':
    cmd = ['timeout', '--signal=INT' if args.backtrace else '--signal=TERM',
           '--kill-after=5s', str(args.seconds)+'s', *cmd]
if args.mode == 'isolated':
    bus = root / 'bus.conf'
    bus.write_text('<busconfig><type>session</type><listen>unix:tmpdir=/tmp</listen>'
                   '<policy context="default"><allow user="*"/>'
                   '<allow send_destination="*"/><allow receive_sender="*"/>'
                   '<allow own="*"/></policy></busconfig>')
    cmd = ['dbus-run-session', '--config-file='+str(bus), '--', *cmd]
print(root, flush=True)
(root / 'metadata.json').write_text(json.dumps(metadata, indent=2)+'\n')
with (root / 'launch.log').open('w') as log:
    result = subprocess.run(cmd, env=env, stdout=log, stderr=subprocess.STDOUT)
metadata['exit_status'] = result.returncode
(root / 'metadata.json').write_text(json.dumps(metadata, indent=2)+'\n')
print('exit_status='+str(result.returncode), flush=True)
raise SystemExit(result.returncode)
