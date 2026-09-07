#!/usr/bin/env python3
"""Manual UQC authentication collection; never manages an existing agent/service."""
import argparse
import datetime
import hashlib
import json
import os
from pathlib import Path
import subprocess
from registration import registration_reply

BUNDLE = Path(__file__).resolve().parent
AGENT = '/usr/lib/hyprpolkitagent/hyprpolkitagent'


def output(*cmd):
    return subprocess.check_output(cmd, text=True).strip()


def identity():
    if os.getuid() != 1001 or output('id', '-un') != 'tux':
        raise SystemExit('STOP: run as tux (UID 1001), never through sudo/su.')
    try:
        obj = output('busctl', '--system', 'call', 'org.freedesktop.login1',
                     '/org/freedesktop/login1', 'org.freedesktop.login1.Manager',
                     'GetSessionByPID', 'u', str(os.getpid())).split('"')[1]
    except subprocess.CalledProcessError:
        raise SystemExit('STOP: this process has no resolvable logind session. '
                         'Restart the test compositor using the updated config and its '
                         'terminal.sh launcher. Do not use an ordinary Kitty window or '
                         'substitute XDG_SESSION_ID for process identity.') from None
    sid = output('busctl', '--system', 'get-property', 'org.freedesktop.login1',
                 obj, 'org.freedesktop.login1.Session', 'Id').split('"')[1]
    details = output('loginctl', 'show-session', sid, '-p', 'Id', '-p', 'User',
                     '-p', 'Type', '-p', 'Class', '-p', 'Active', '-p', 'Remote',
                     '-p', 'Seat', '-p', 'TTY', '-p', 'Leader')
    fields = dict(line.split('=', 1) for line in details.splitlines())
    if fields.get('User') != '1001' or fields.get('Active') != 'yes' or fields.get('Remote') != 'no':
        raise SystemExit('STOP: expected active, local tux login.\n' + details)
    if not os.environ.get('WAYLAND_DISPLAY'):
        raise SystemExit('STOP: run inside the test compositor, not the text console.')
    return sid, details


parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('step', choices=['preflight', 'agent', 'registration', 'challenge'])
parser.add_argument('--run')
args = parser.parse_args()
sid, details = identity()
base = Path.home() / 'uqc-auth-evidence'
base.mkdir(mode=0o700, exist_ok=True)
if args.step == 'preflight':
    run = base / datetime.datetime.now(datetime.timezone.utc).strftime('%Y%m%dT%H%M%SZ')
    run.mkdir(mode=0o700)
    (run / 'identity.txt').write_text(details + '\n')
    (run / 'session-id').write_text(sid)
    commands = {
        'sessions': ['loginctl', 'list-sessions'],
        'processes': ['ps', '-eo', 'uid,pid,ppid,cgroup,comm'],
        'user-services': ['systemctl', '--user', 'list-units', '--type=service', '--all', '--no-pager'],
        'registration-before': ['journalctl', '-b', '-u', 'polkit.service', '--no-pager'],
        'versions': ['pacman', '-Q', 'hyprpolkitagent', 'qt6-base', 'qt6-declarative', 'hyprland'],
        'policy': ['pkaction', '--action-id', 'org.freedesktop.policykit.exec', '--verbose'],
        'linkage': ['ldd', AGENT],
    }
    for name, command in commands.items():
        result = subprocess.run(command, text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        (run / (name + '.txt')).write_text(result.stdout + '\ncommand_exit=' + str(result.returncode) + '\n')
    print('Evidence directory:', run)
    print('Review identity, processes, services, registration-before and policy before continuing.')
else:
    if args.run is None or not args.run.strip():
        parser.error('--run needs the full printed evidence path. UQC_RUN may be unset in this terminal.')
    run = Path(args.run).expanduser().resolve()
    if run.parent != base.resolve():
        raise SystemExit(f'STOP: supplied run resolves to {run}; expected a timestamp directory '
                         f'directly under {base.resolve()}. Set UQC_RUN in this terminal or use the full path.')
    try:
        recorded_session = (run / 'session-id').read_text().strip()
    except OSError as error:
        raise SystemExit(f'STOP: cannot read {run / "session-id"}: {error}') from None
    if recorded_session != sid:
        raise SystemExit(f'STOP: run {run} records login session {recorded_session}, '
                         f'but this process is in session {sid}. File ownership is not the issue. '
                         'Use the same login as the test agent; do not edit session-id to bypass this check.')
    if args.step == 'agent':
        if output('pacman', '-Q', 'hyprpolkitagent') != 'hyprpolkitagent 0.1.3-10':
            raise SystemExit('STOP: agent version changed; review toolkit/contract first.')
        if 'libQt6' not in output('ldd', AGENT):
            raise SystemExit('STOP: expected Qt 6 agent.')
        prefix = BUNDLE / 'prefix'
        if not (prefix / 'lib/qt6/qml/Holonight/qmldir').is_file():
            raise SystemExit('STOP: staged provider missing.')
        if (run / 'agent.json').exists():
            raise SystemExit('STOP: create a new preflight directory for another attempt.')
        print('Review test-session processes/services; stop if a competing agent is known. Registration cannot replace one.')
        if input('Type ISOLATED only after completing the README checks: ') != 'ISOLATED':
            raise SystemExit('Stopped without launching an agent.')
        env = os.environ.copy()
        for key in ['QML2_IMPORT_PATH', 'QT_QUICK_CONTROLS_CONF', 'QT_QUICK_CONTROLS_FALLBACK_STYLE',
                    'QT_QPA_PLATFORM', 'QT_QUICK_BACKEND']:
            env.pop(key, None)
        for key in ['XDG_CONFIG_HOME', 'XDG_DATA_HOME', 'XDG_CACHE_HOME', 'XDG_STATE_HOME']:
            path = run / key.lower()
            path.mkdir(mode=0o700)
            env[key] = str(path)
        env.update(QT_QPA_PLATFORM='wayland', QT_QPA_PLATFORMTHEME='holonight',
                   QT_QUICK_CONTROLS_STYLE='Holonight', QML_IMPORT_PATH=str(prefix / 'lib/qt6/qml'),
                   QT_PLUGIN_PATH=str(prefix / 'lib/qt6/plugins'), QML_IMPORT_TRACE='1',
                   QT_DEBUG_PLUGINS='1', QT_FORCE_STDERR_LOGGING='1',
                   QT_LOGGING_RULES='*.debug=true;*.info=true', G_DBUS_DEBUG='message')
        metadata = dict(session=sid, executable=AGENT, prefix=str(prefix),
                        sha256=hashlib.sha256(Path(AGENT).read_bytes()).hexdigest(),
                        date=datetime.datetime.now(datetime.timezone.utc).isoformat())
        with (run / 'agent.log').open('w') as log:
            proc = subprocess.Popen([AGENT], env=env, stdout=log, stderr=subprocess.STDOUT,
                                    start_new_session=True)
            metadata['pid'] = proc.pid
            (run / 'agent.json').write_text(json.dumps(metadata, indent=2) + '\n')
            print('Agent PID:', proc.pid, 'Log:', run / 'agent.log', flush=True)
            print('Keep this terminal open. After cancellation, Ctrl+C stops only this child agent.', flush=True)
            try:
                status = proc.wait()
            except KeyboardInterrupt:
                proc.terminate()
                try:
                    status = proc.wait(timeout=5)
                except subprocess.TimeoutExpired:
                    proc.kill()
                    status = proc.wait()
        metadata['exit_status'] = status
        (run / 'agent.json').write_text(json.dumps(metadata, indent=2) + '\n')
        print('Agent exit:', status)
    else:
        metadata = json.loads((run / 'agent.json').read_text())
        if 'exit_status' in metadata or not Path('/proc', str(metadata['pid'])).exists():
            raise SystemExit('STOP: test agent is no longer running.')
        if Path('/proc', str(metadata['pid']), 'exe').resolve() != Path(AGENT):
            raise SystemExit('STOP: recorded PID no longer belongs to the expected agent.')
        try:
            registration = registration_reply((run / 'agent.log').read_text(), sid)
        except ValueError as error:
            raise SystemExit('STOP: ' + str(error)) from None
        owner = output('busctl', '--system', 'call', 'org.freedesktop.DBus',
                       '/org/freedesktop/DBus', 'org.freedesktop.DBus', 'GetNameOwner',
                       's', 'org.freedesktop.PolicyKit1').split('"')[1]
        bus_pid = int(output('busctl', '--system', 'call', 'org.freedesktop.DBus',
                            '/org/freedesktop/DBus', 'org.freedesktop.DBus',
                            'GetConnectionUnixProcessID', 's', registration['agent_bus']).split()[1])
        obj = output('busctl', '--system', 'call', 'org.freedesktop.login1',
                     '/org/freedesktop/login1', 'org.freedesktop.login1.Manager',
                     'GetSessionByPID', 'u', str(bus_pid)).split('"')[1]
        agent_session = output('busctl', '--system', 'get-property', 'org.freedesktop.login1',
                               obj, 'org.freedesktop.login1.Session', 'Id').split('"')[1]
        if owner != registration['authority'] or bus_pid != metadata['pid'] or agent_session != sid:
            raise SystemExit('STOP: authority, agent PID or session does not match the registration reply.')
        (run / 'registration.json').write_text(json.dumps(registration, indent=2) + '\n')
        print('PASS: registration reply, current authority, agent PID and login session match.')
        if args.step == 'registration':
            raise SystemExit(0)
        print('Registration verified. Confirm you are ready to inspect and cancel the prompt.')
        if input('Type REGISTERED after review: ') != 'REGISTERED':
            raise SystemExit('Stopped without requesting authorization.')
        print('Cancel the graphical prompt. Do not enter a password.')
        result = subprocess.run(['pkexec', '--disable-internal-agent', '/usr/bin/true'],
                                text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        (run / 'challenge.txt').write_text(result.stdout + '\nexit_status=' + str(result.returncode) + '\n')
        print('Challenge exit:', result.returncode, '(no prompt or exit 0 is not a cancellation pass)')
