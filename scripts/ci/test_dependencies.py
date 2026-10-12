#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-or-later
"""Offline discovery and compatibility regressions for the adopted bundle."""
import json
import os
from pathlib import Path
import subprocess
import tempfile
import unittest
from unittest.mock import patch
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[2] / "tooling"))
import external
import workflow


class DependencyTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory(prefix='dependency prefix with spaces ')
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        self.config = {'module': 'consumer', 'dependencies': [], 'external_dependencies':
                       [{'id': 'gtest', 'scope': ['test'], 'targets': ['GTest::gtest']}]}

    def installed_gtest(self, version='1.14.0', gmock=True):
        package = self.root / 'lib/cmake/GTest'
        package.mkdir(parents=True, exist_ok=True)
        config = 'add_library(GTest::gtest INTERFACE IMPORTED)\n'
        if gmock:
            config += 'add_library(GTest::gmock INTERFACE IMPORTED)\n'
        (package / 'GTestConfig.cmake').write_text(config)
        (package / 'GTestConfigVersion.cmake').write_text(
            f'set(PACKAGE_VERSION "{version}")\n'
            'if(PACKAGE_VERSION VERSION_LESS PACKAGE_FIND_VERSION)\n'
            '  set(PACKAGE_VERSION_COMPATIBLE FALSE)\n'
            'else()\n  set(PACKAGE_VERSION_COMPATIBLE TRUE)\nendif()\n')

    def test_installed_config_with_spaces_and_stable_evidence(self):
        self.installed_gtest()
        with patch.dict(os.environ, {'HOLONIGHT_EXTERNAL_PREFIX': str(self.root)}):
            first = external.check(self.config, ('test',))
            second = external.check(self.config, ('test',))
        self.assertEqual(first, second)
        self.assertIn('GTest_DIR=' + str(self.root / 'lib/cmake/GTest'), first['resolved'])

    def test_incompatible_installed_version_fails_without_acquisition(self):
        self.installed_gtest(version='1.13.0')
        # Disable the normal system installation so the incompatible prefix is the only candidate.
        original = external.probe_source
        def isolated(*args, **kwargs):
            return original(*args, **kwargs).replace('find_package(GTest',
                'set(CMAKE_FIND_USE_CMAKE_SYSTEM_PATH FALSE)\nset(CMAKE_FIND_USE_SYSTEM_ENVIRONMENT_PATH FALSE)\nfind_package(GTest')
        with patch.dict(os.environ, {'HOLONIGHT_EXTERNAL_PREFIX': str(self.root)}), patch.object(external, 'probe_source', isolated):
            with self.assertRaisesRegex(RuntimeError, '1.14'):
                external.check(self.config, ('test',))

    def test_changed_provider_configuration_archives_incompatible_install_tree(self):
        source = self.root / 'source'
        source.mkdir()
        (source / 'CMakeLists.txt').write_text('project(provider)')
        prefix = self.root / 'build/deps/prefix'
        package = prefix / 'lib/cmake/Provider'
        package.mkdir(parents=True)
        (package / 'ProviderConfig.cmake').write_text('old')
        (self.root / 'build/deps/provider-state.json').write_text('{"old": true}')
        config = {'dependencies': [{'module': 'provider', 'package': 'Provider', 'source_variable': 'PROVIDER_SOURCE'}]}
        def install(args, **kwargs):
            if args[1] == '--install':
                package.mkdir(parents=True)
                (package / 'ProviderConfig.cmake').write_text('new')
        with patch.dict(os.environ, {'PROVIDER_SOURCE': str(source)}, clear=True), \
             patch.object(workflow, 'ROOT', self.root), patch.object(workflow, 'check_external', return_value={}), \
             patch.object(workflow, 'toolchain_signature', return_value={'compiler': 'new'}), \
             patch.object(workflow, 'run', side_effect=install) as run:
            workflow.prepare(config)
        self.assertEqual((self.root / 'build/deps.previous-1/prefix/lib/cmake/Provider/ProviderConfig.cmake').read_text(), 'old')
        self.assertEqual((package / 'ProviderConfig.cmake').read_text(), 'new')
        self.assertEqual(run.call_count, 3)

    def test_reuse_is_invalidated_by_compiler_qt_options_features_build_type_and_source(self):
        for changed in ('compiler', 'qt', 'options', 'features', 'build_type', 'source'):
            with self.subTest(changed=changed), tempfile.TemporaryDirectory() as directory:
                root = Path(directory)
                source = root / 'source'
                source.mkdir()
                file = source / 'CMakeLists.txt'
                file.write_text('project(provider)')
                package = root / 'build/deps/prefix/lib/cmake/Provider'
                config = {'dependencies': [{'module': 'provider', 'package': 'Provider',
                                           'source_variable': 'PROVIDER_SOURCE'}], 'wayland': False}
                compiler = {'identity': 'first'}
                qt = {'resolved': ['Qt6_VERSION=6.11.0']}
                def install(args, **kwargs):
                    if args[1] == '--install':
                        package.mkdir(parents=True, exist_ok=True)
                        (package / 'ProviderConfig.cmake').write_text('installed')
                with patch.dict(os.environ, {'PROVIDER_SOURCE': str(source)}, clear=True), \
                     patch.object(workflow, 'ROOT', root), patch.object(workflow, 'check_external', side_effect=lambda *a, **kw: dict(qt)), \
                     patch.object(workflow, 'toolchain_signature', side_effect=lambda *a: dict(compiler)), \
                     patch.object(workflow, 'run', side_effect=install) as run:
                    workflow.prepare(config)
                    self.assertEqual(run.call_count, 3)
                    workflow.prepare(config)
                    self.assertEqual(run.call_count, 3, 'unchanged provider must be reused')
                    if changed == 'compiler':
                        compiler['identity'] = 'second'
                    elif changed == 'qt':
                        qt['resolved'] = ['Qt6_VERSION=6.12.0']
                    elif changed == 'options':
                        config['dependencies'][0]['options'] = ['-DBUILD_COMPONENT=OFF']
                    elif changed == 'features':
                        config['wayland'] = True
                    elif changed == 'build_type':
                        os.environ['HOLONIGHT_PROVIDER_BUILD_TYPE'] = 'Release'
                    else:
                        file.write_text('project(changed_provider)')
                    workflow.prepare(config)
                    self.assertEqual(run.call_count, 6)
                    self.assertTrue((root / 'build/deps.previous-1/prefix').exists())

    def test_cmake_rejects_external_internal_provider_and_replaces_stale_directory(self):
        internal = self.root / 'internal'
        outside = self.root / 'external'
        for prefix, value in [(internal, 'internal'), (outside, 'external')]:
            directory = prefix / 'lib/cmake/Provider'
            directory.mkdir(parents=True)
            (directory / 'ProviderConfig.cmake').write_text(f'set(SELECTED_PROVIDER {value})')
        source = self.root / 'consumer'
        source.mkdir()
        helper = external.BUNDLE / 'providers.cmake'
        (source / 'CMakeLists.txt').write_text(f'cmake_minimum_required(VERSION 3.25)\nproject(consumer)\n'
            f'include("{helper}")\nholonight_require_provider(Provider)\nfind_package(Provider CONFIG REQUIRED)\n'
            'file(WRITE "${CMAKE_BINARY_DIR}/selected" "${SELECTED_PROVIDER}")\n')
        args = ['cmake', '-S', str(source), '-B', str(source / 'build'), '-G', 'Ninja',
                '-DHOLONIGHT_DEPENDENCY_PREFIX=' + str(internal), '-DHOLONIGHT_EXTERNAL_PREFIX=' + str(outside),
                '-DProvider_DIR=' + str(outside / 'lib/cmake/Provider')]
        result = subprocess.run(args, text=True, capture_output=True)
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        self.assertEqual((source / 'build/selected').read_text(), 'internal')
        args[-3] = '-DHOLONIGHT_DEPENDENCY_PREFIX=' + str(self.root / 'missing')
        result = subprocess.run(args, text=True, capture_output=True)
        self.assertNotEqual(result.returncode, 0)
        self.assertIn('Missing Provider in approved internal provider prefixes', result.stderr)

    def test_changed_package_exports_are_recorded_even_at_same_version(self):
        self.installed_gtest()
        with patch.dict(os.environ, {'HOLONIGHT_EXTERNAL_PREFIX': str(self.root)}):
            before = external.check(self.config, ('test',))
            config = self.root / 'lib/cmake/GTest/GTestConfig.cmake'
            config.write_text(config.read_text() + 'set(ADDITIONAL_EXPORTED_OPTION ON)\n')
            after = external.check(self.config, ('test',))
        self.assertEqual(before['resolved'], after['resolved'])
        self.assertNotEqual(before['package_config_signatures'], after['package_config_signatures'])

    def test_gmock_is_not_required(self):
        self.installed_gtest(gmock=False)
        with patch.dict(os.environ, {'HOLONIGHT_EXTERNAL_PREFIX': str(self.root)}):
            external.check(self.config, ('test',))

    def test_normal_commands_preserve_stale_cache_without_preparing(self):
        config = json.loads((external.BUNDLE / 'module.json').read_text())
        (self.root / 'tooling').mkdir()
        (self.root / 'tooling/module.json').write_text(json.dumps(config))
        stale = self.root / 'build/test/CMakeCache.txt'
        stale.parent.mkdir(parents=True)
        stale.write_text('preserve stale cache')
        with patch.object(workflow, 'ROOT', self.root), patch.dict(os.environ, {'HOLONIGHT_DEPENDENCY_PREFIX': str(self.root / 'missing')}), patch.object(workflow, 'prepare') as prepare:
            for command in ('configure', 'build', 'test'):
                with patch('sys.argv', ['workflow', command, '--preset', 'test']):
                    with self.assertRaisesRegex(RuntimeError, 'Missing HoloNightConfig'):
                        workflow.main()
                self.assertEqual(stale.read_text(), 'preserve stale cache')
                self.assertFalse((self.root / 'build/deps').exists())
            prepare.assert_not_called()

    def test_disabled_features_avoid_optional_discovery(self):
        config = json.loads((external.BUNDLE / 'module.json').read_text())
        features = {key: False for key in config['external_features']}
        selected = external.requirements(config, ('build', 'test'), features)
        self.assertFalse({'qt-wayland', 'wayland', 'wayland-scanner', 'syntax', 'qt5'} & {item['id'] for item in selected})
        self.assertIn('QmlPrivate', external.probe_source([config], ('test',), features))

    def test_missing_gtest_target_fails(self):
        self.installed_gtest(gmock=False)
        (self.root / 'lib/cmake/GTest/GTestConfig.cmake').write_text('')
        with patch.dict(os.environ, {'HOLONIGHT_EXTERNAL_PREFIX': str(self.root)}):
            with self.assertRaisesRegex(RuntimeError, 'lacks GTest::gtest'):
                external.check(self.config, ('test',))

    def test_missing_enabled_packages_fail_offline(self):
        for dependency, package in [('qt', 'Qt6'), ('gtest', 'GTest'), ('syntax', 'KF6SyntaxHighlighting'), ('qt-wayland', 'Qt6WaylandScannerTools')]:
            with self.subTest(package=package):
                config = {'module': 'qt-regression', 'external_dependencies': [{'id': dependency, 'scope': ['build']}]}
                with self.assertRaisesRegex(RuntimeError, package):
                    external.check(config, cmake_options={'CMAKE_DISABLE_FIND_PACKAGE_' + package: 'TRUE'})

    def qt_fixture(self, public='6.11.0', private='6.11.0'):
        for package, version in [('Qt6', public), ('Qt6QmlPrivate', private)]:
            directory = self.root / 'lib/cmake' / package
            directory.mkdir(parents=True, exist_ok=True)
            (directory / (package + 'ConfigVersion.cmake')).write_text(
                f'set(PACKAGE_VERSION "{version}")\n'
                'if(PACKAGE_VERSION VERSION_EQUAL PACKAGE_FIND_VERSION)\n'
                'set(PACKAGE_VERSION_EXACT TRUE)\nendif()\n'
                'if(NOT PACKAGE_VERSION VERSION_LESS PACKAGE_FIND_VERSION)\n'
                'set(PACKAGE_VERSION_COMPATIBLE TRUE)\nendif()\n')
            (directory / (package + 'Config.cmake')).write_text('')
        (self.root / 'lib/cmake/Qt6/Qt6Config.cmake').write_text(
            f'set(Qt6_VERSION "{public}")\n'
            'if("QmlPrivate" IN_LIST Qt6_FIND_COMPONENTS)\n'
            'set(exact)\nif(Qt6_FIND_VERSION_EXACT)\nset(exact EXACT)\nendif()\n'
            'find_package(Qt6QmlPrivate ${Qt6_FIND_VERSION} ${exact} CONFIG REQUIRED '
            f'PATHS "{self.root}/lib/cmake/Qt6QmlPrivate" NO_DEFAULT_PATH)\nendif()\n')

    def test_old_public_and_mismatched_private_qt_fail(self):
        config = {'module': 'qt-regression', 'external_dependencies': [
            {'id': 'qt', 'scope': ['build'], 'components': ['Core', 'QmlPrivate']}]}
        original = external.probe_source
        def isolated(*args, **kwargs):
            return original(*args, **kwargs).replace('find_package(Qt6',
                'set(CMAKE_FIND_USE_CMAKE_SYSTEM_PATH FALSE)\nset(CMAKE_FIND_USE_SYSTEM_ENVIRONMENT_PATH FALSE)\nfind_package(Qt6')
        with patch.dict(os.environ, {'HOLONIGHT_EXTERNAL_PREFIX': str(self.root)}), patch.object(external, 'probe_source', isolated):
            self.qt_fixture(public='6.10.0')
            with self.assertRaisesRegex(RuntimeError, '6.11'):
                external.check(config)
            self.qt_fixture(private='6.12.0')
            with self.assertRaisesRegex(RuntimeError, 'exactly matches'):
                external.check(config)
            self.qt_fixture()
            external.check(config)

if __name__ == '__main__':
    unittest.main()
