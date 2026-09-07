#!/usr/bin/env python3
"""Collect existing behavior; assertions describe audit findings, not acceptance tests."""
import argparse
import json
import os
from pathlib import Path
import subprocess
import tempfile

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('prefix', type=Path)
parser.add_argument('executable', type=Path)
args = parser.parse_args()
root = Path(tempfile.mkdtemp(prefix='uqc-layout-'))
print(root, flush=True)
qml = Path(__file__).with_name('layout-probe.qml').resolve()
for style in ['Holonight', 'Fusion', 'Basic']:
    for constrained in [False, True]:
        name = style + ('-bounded' if constrained else '')
        env = os.environ.copy()
        for key in ['QML2_IMPORT_PATH', 'QT_QUICK_CONTROLS_CONF', 'QT_QUICK_CONTROLS_FALLBACK_STYLE',
                    'UQC_CONSTRAIN_CONTENT']:
            env.pop(key, None)
        for key in ['XDG_CONFIG_HOME', 'XDG_DATA_HOME', 'XDG_CACHE_HOME', 'XDG_STATE_HOME']:
            path = root / name / key
            path.mkdir(parents=True)
            env[key] = str(path)
        env.update(QT_QPA_PLATFORM='offscreen', QT_QUICK_BACKEND='software',
                   QT_QPA_PLATFORMTHEME='generic', QT_QUICK_CONTROLS_STYLE=style,
                   QML_IMPORT_PATH=str(args.prefix.resolve() / 'lib/qt6/qml'),
                   QT_PLUGIN_PATH=str(args.prefix.resolve() / 'lib/qt6/plugins'),
                   QT_FORCE_STDERR_LOGGING='1')
        if constrained:
            env['UQC_CONSTRAIN_CONTENT'] = '1'
        result = subprocess.run([str(args.executable.resolve()), str(qml)], env=env,
                                stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True, timeout=15)
        (root / (name + '.log')).write_text(result.stdout)
        assert result.returncode == 0, result.stdout
        assert 'Binding loop' not in result.stdout and 'failed to load' not in result.stdout, result.stdout
        geometry = [json.loads(line.split(' ', 1)[1]) for line in result.stdout.splitlines()
                    if line.startswith('report ')][0]
        palettes = [json.loads(line.split(' ', 1)[1]) for line in result.stdout.splitlines()
                    if line.startswith('paletteReport ')]
        assert all(geometry[c]['width'] == 0 for c in ['checkbox', 'radio', 'switchControl']), geometry
        assert (geometry['checkbox']['height'] == 0) == (style == 'Holonight'), geometry
        scroll = geometry['scroll']
        assert (scroll['labelWidth'] <= scroll['availableWidth']) == constrained, scroll
        assert (scroll['lineCount'] > 1) == constrained, scroll
        assert (scroll['contentWidth'] <= scroll['availableWidth']) == constrained, scroll
        assert len(palettes) == 3, palettes
        assert palettes[0]['kirigamiText'] != palettes[1]['kirigamiText'], palettes
        assert (palettes[0]['fieldBackground'] == palettes[1]['fieldBackground']) == (style == 'Holonight'), palettes
        print(name, json.dumps(geometry), 'palette split' if style == 'Holonight' else 'palette follows', flush=True)
print('PASS: six characterization runs')
