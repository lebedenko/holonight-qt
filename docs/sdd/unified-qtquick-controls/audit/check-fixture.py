#!/usr/bin/env python3
"""Check actual type URLs and loaded provider library in fresh fixture processes."""
import argparse
import os
from pathlib import Path
import re
import subprocess
import tempfile

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('prefix', type=Path)
parser.add_argument('executable', type=Path)
args = parser.parse_args()
root = Path(tempfile.mkdtemp(prefix='uqc-fixture-'))
print(root, flush=True)
for mode in ['default', 'fusion', 'haruna-fallback']:
    env = os.environ.copy()
    for key in ['QT_QUICK_CONTROLS_STYLE', 'QT_QUICK_CONTROLS_CONF',
                'QT_QUICK_CONTROLS_FALLBACK_STYLE', 'QML2_IMPORT_PATH', 'UQC_FUSION_FALLBACK']:
        env.pop(key, None)
    for key in ['XDG_CONFIG_HOME', 'XDG_DATA_HOME', 'XDG_CACHE_HOME', 'XDG_STATE_HOME']:
        path = root / mode / key
        path.mkdir(parents=True)
        env[key] = str(path)
    env.update(QT_FORCE_STDERR_LOGGING='1', QT_LOGGING_RULES='*.debug=true;*.info=true',
               QML_DISABLE_DISK_CACHE='1', QT_QPA_PLATFORM='offscreen',
               QT_QPA_PLATFORMTHEME='generic', QT_QUICK_BACKEND='software', QML_IMPORT_TRACE='1',
               QML_IMPORT_PATH=str(args.prefix.resolve() / 'lib/qt6/qml'),
               QT_PLUGIN_PATH=str(args.prefix.resolve() / 'lib/qt6/plugins'))
    if mode == 'fusion':
        env['QT_QUICK_CONTROLS_STYLE'] = 'Fusion'
    if mode == 'haruna-fallback':
        env['UQC_FUSION_FALLBACK'] = '1'
    result = subprocess.run([str(args.executable.resolve())], env=env, capture_output=True,
                            text=True, timeout=15)
    log = result.stdout + result.stderr
    (root / (mode+'.log')).write_text(log)
    assert result.returncode == 0 and 'fixture-created' in log, (mode, result.returncode)
    fallback = 'Basic' if mode == 'default' else 'Fusion'
    for control in ['ApplicationWindow', 'Label', 'ToolButton', 'Dialog', 'Button', 'TextField']:
        origin = ('qrc:/qt/qml/Holonight/' if control in ['Button', 'TextField'] and mode != 'fusion'
                  else 'qrc:/qt-project.org/imports/QtQuick/Controls/'+fallback+'/')
        expected = origin+control+'.qml'
        lines = [line for line in log.splitlines()
                 if 'resolveType: file:///uqc-discovery.qml "C.'+control+'"' in line]
        assert any(expected in line for line in lines), (mode, control, lines)
        print(mode, control, expected)
    libraries = re.findall(r'"([^"\n]*libholonight[^"\n]*\.so)" loaded library', log)
    if mode != 'fusion':
        assert any(p.endswith('/Holonight/libholonight_qml.so') for p in libraries), libraries
        assert all(p.startswith(str(args.prefix.resolve())+'/') for p in libraries), libraries
    else:
        assert not libraries, libraries
    print(mode, 'PASS')
