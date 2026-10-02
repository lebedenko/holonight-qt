# SPDX-License-Identifier: GPL-3.0-or-later
import os
from pathlib import Path
import subprocess
import tempfile
import unittest

RESOLVER = Path(__file__).with_name('ResolveQtTestTools.cmake').resolve()


class DiscoveryTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory(prefix='Qt tools with spaces ')
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        self.tools = self.root / 'selected tools'
        self.tools.mkdir()
        self.lint = self.executable(self.tools / 'qmllint')
        self.qml = self.executable(self.tools / 'qml')
        self.foreign = self.root / 'foreign'
        self.foreign.mkdir()
        self.executable(self.foreign / 'qmllint')
        self.executable(self.foreign / 'qml')

    def executable(self, path):
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text('#!/bin/sh\nexit 0\n')
        path.chmod(0o755)
        return path

    def configure(self, setup='', overrides=(), success=True):
        source = self.root / 'source'
        source.mkdir(exist_ok=True)
        (source / 'CMakeLists.txt').write_text(
            'cmake_minimum_required(VERSION 3.25)\nproject(fixture NONE)\n' + setup +
            f'\ninclude("{RESOLVER}")\n' +
            'file(WRITE "${CMAKE_BINARY_DIR}/selected.txt" "${HOLONIGHT_QMLLINT_EXECUTABLE}\\n${HOLONIGHT_QML_EXECUTABLE}\\n")\n')
        result = subprocess.run(['cmake', '-S', str(source), '-B', str(self.root / 'build'), *overrides],
                                env=dict(os.environ, PATH=str(self.foreign) + ':' + os.environ['PATH']),
                                capture_output=True, text=True)
        self.assertEqual(result.returncode == 0, success, result.stdout + result.stderr)
        return ((self.root / 'build/selected.txt').read_text().splitlines() if success else result.stderr)

    def target(self, property='IMPORTED_LOCATION'):
        return f'add_executable(Qt6::qmllint IMPORTED)\nset_target_properties(Qt6::qmllint PROPERTIES {property} "{self.lint}" IMPORTED_CONFIGURATIONS RELEASE)'

    def test_default_and_no_cache_on_installation_change(self):
        self.assertEqual(self.configure(self.target()), [str(self.lint), str(self.qml)])
        self.lint = self.executable(self.root / 'new installation/qmllint')
        self.qml = self.executable(self.lint.parent / 'qml')
        self.assertEqual(self.configure(self.target()), [str(self.lint), str(self.qml)])
        cache = (self.root / 'build/CMakeCache.txt').read_text()
        self.assertNotIn('HOLONIGHT_QML_EXECUTABLE:', cache)
        self.assertNotIn('HOLONIGHT_QMLLINT_EXECUTABLE:', cache)

    def test_configuration_specific(self):
        self.assertEqual(self.configure(self.target('IMPORTED_LOCATION_RELEASE')), [str(self.lint), str(self.qml)])

    def test_build_configuration_precedes_default(self):
        selected = self.executable(self.root / 'debug tools/qmllint')
        qml = self.executable(selected.parent / 'qml')
        setup = self.target() + f'\nset_target_properties(Qt6::qmllint PROPERTIES IMPORTED_LOCATION_DEBUG "{selected}")'
        self.assertEqual(self.configure(setup, ['-DCMAKE_BUILD_TYPE=Debug']), [str(selected), str(qml)])

    def test_sibling_precedes_configured_bins(self):
        other = self.executable(self.root / 'other bin/qml6')
        setup = self.target() + f'\nset(QT6_INSTALL_BINS "{other.parent}")'
        self.assertEqual(self.configure(setup), [str(self.lint), str(self.qml)])

    def test_explicit_overrides(self):
        self.assertEqual(self.configure(overrides=[f'-DHOLONIGHT_QMLLINT_EXECUTABLE={self.lint}',
                                                  f'-DHOLONIGHT_QML_EXECUTABLE={self.qml}']),
                         [str(self.lint), str(self.qml)])

    def test_relative_and_absolute_bins(self):
        self.qml.unlink()
        bins = self.root / 'installation/bin'
        qml = self.executable(bins / 'qml')
        for install in [f'set(QT6_INSTALL_PREFIX "{bins.parent}")\nset(QT6_INSTALL_BINS bin)',
                        f'set(QT6_INSTALL_BINS "{bins}")']:
            self.assertEqual(self.configure(self.target() + '\n' + install), [str(self.lint), str(qml)])

    def test_missing_tools_ignore_path(self):
        self.assertIn('Qt6::qmllint', self.configure(success=False))
        self.qml.unlink()
        self.assertIn('HOLONIGHT_QML_EXECUTABLE', self.configure(self.target(), success=False))

    def test_invalid_overrides(self):
        for tool in ['QML', 'QMLLINT']:
            self.assertIn(f'HOLONIGHT_{tool}_EXECUTABLE does not name', self.configure(
                self.target(), [f'-DHOLONIGHT_{tool}_EXECUTABLE={self.root}/missing'], success=False))
            # Remove the explicit cache override before testing the other variable.
            (self.root / 'build/CMakeCache.txt').unlink()
        self.lint.chmod(0o644)
        self.assertIn('is not executable', self.configure(self.target(), success=False))


if __name__ == '__main__':
    unittest.main()
