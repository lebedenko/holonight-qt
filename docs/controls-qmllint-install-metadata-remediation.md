# Controls qmllint install-metadata remediation

**Status:** Ready for implementation
**Date:** 2026-07-28
**Reported by:** `holonight-shell` shared-controls adoption checkpoint CP-S-001
**Affected revision:** `00b1b55d88a9d20f527559b8551ad664760c2a7b`

## Problem

The installed `Holonight.Controls` module loads successfully at runtime, but `qmllint` cannot
resolve newer public QML controls such as `HnNavigationDelegate`.

This blocks downstream control adoption because `holonight-shell` requires warning-free
`task qml-lint` at every visual checkpoint. The downstream component has been restored; no
linkage, copied control, warning suppression, or visual workaround was retained.

## Reproduction

Build and install `holonight-qt` to the prefix used by the shell:

```bash
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON
cmake --build build
cmake --install build --prefix /tmp/holonight-qt-prefix
```

Create or use a QML file containing:

```qml
import QtQuick
import Holonight.Controls

HnNavigationDelegate {
    title: "Appearance"
    checked: true
}
```

Run:

```bash
/usr/lib/qt6/bin/qmllint \
  -I /tmp/holonight-qt-prefix/lib/qt6/qml \
  <path-to-file.qml>
```

Observed diagnostic:

```text
HnNavigationDelegate was not found. Did you add all imports and dependencies?
```

In the downstream `NavPanel` delegate this also produces cascading, misleading
`modelData`-required-property warnings.

## Current evidence

The installed module contains:

- `Holonight/Controls/qmldir`;
- `Holonight/Controls/libholonight_controls_qml.so`;
- all public control QML files, including `HnNavigationDelegate.qml`;
- `Holonight/Controls/holonight_controls_qml.qmltypes`.

The `qmldir` declares `HnNavigationDelegate 1.0 HnNavigationDelegate.qml`, but the installed
qmltypes file is only:

```qml
import QtQuick.tooling 1.2

// Generated comments omitted.
Module {}
```

Runtime and artifact checks already pass:

- a `QQmlEngine` can import `Holonight.Core` and `Holonight.Controls`;
- `HnSurfaceFrame` instantiates through the installed prefix;
- the downstream executable dynamically discovers the module without
  `HolonightQt::Controls` linkage.

The defect is therefore in tooling/package consumption, not the runtime public API.

## Required outcome

An installed-prefix consumer must be able to run `qmllint` against every public
`Holonight.Controls` QML type without unresolved-type warnings.

The correction must preserve:

- module URI `Holonight.Controls`;
- existing public type names and versions;
- dynamic QML plugin discovery;
- installed QML paths;
- current runtime behavior;
- the downstream ability to consume the module without linking `HolonightQt::Controls`.

## Investigation targets

Inspect `qml/CMakeLists.txt`, generated module artifacts, and the install tree:

1. Determine why `qt_add_qml_module(holonight_controls_qml ...)` generates an empty qmltypes file
   for a QML-only public module.
2. Verify whether the generated `prefer :/qt/qml/Holonight/Controls/` entry prevents installed
   source-based type discovery by `qmllint`.
3. Compare build-tree and install-tree lint behavior.
4. Compare `Holonight.Controls` with the established `Holonight` and `Holonight.Core` modules.
5. Confirm whether Qt expects QML types to be represented through generated qmltypes metadata,
   installed source discovery, resource metadata, or an additional lint/import configuration.

Do not solve the issue by making a downstream application link the Controls interface target.
That would change the validated dependency model without fixing other installed-prefix tooling
consumers.

## Implementation guidance

Choose the smallest upstream correction that makes the installed module self-describing to Qt
tooling. Depending on the confirmed cause, this may involve:

- correcting `qt_add_qml_module` metadata/resource configuration;
- correcting generated or installed `qmldir` behavior;
- generating complete tooling metadata for public QML types;
- adjusting how public QML source files are installed or exposed to tooling.

Avoid handwritten metadata that can drift from `QML_FILES` unless Qt/CMake provides no generated
solution. Keep `HnSelectableDelegate` internal.

## Regression coverage

Add an installed-package test that:

1. installs the project to an isolated temporary prefix;
2. generates a minimal QML consumer importing `Holonight.Controls`;
3. references every public control declared by the module;
4. runs the Qt installation's `qmllint` with only that prefix added as the external import path;
5. fails on unresolved types or other warnings attributable to the module.

At minimum, cover:

- `HnNavigationDelegate`;
- `HnListDelegate`;
- `HnActionDelegate`;
- `HnChoiceCard`;
- `HnSegmentedControl`;
- `HnSettingsRow`;
- `HnSectionHeader`;
- `HnIconComboBox`;
- `HnSearchField`;
- `HnFormField`;
- `HnEmptyState`;
- `HnLoadingState`;
- `HnStatusIndicator`;
- `HnActionBar`;
- `HnKeyHint`;
- `HnPanelHeader`;
- `HnApplicationWindow`;
- `HnSurfaceFrame`;
- `HnTextArea`;
- `HnCardDelegate`.

The test should derive or centrally maintain the public list so adding a control cannot silently
omit lint coverage.

## Acceptance criteria

- The minimal reproduction exits successfully without unresolved-type warnings.
- The installed qmltypes/qmldir/QML artifacts are internally consistent.
- All public Controls types pass the installed-prefix lint test.
- `HnSelectableDelegate` remains internal.
- Existing Controls runtime and package-install tests pass.
- `task build`, `task lint`, and `task test` pass in `holonight-qt`.
- No downstream `HolonightQt::Controls` linkage is required.

## Downstream retry

After the fix is committed:

1. record the fixing `holonight-qt` commit;
2. rebuild and install it to `/tmp/holonight-qt-prefix`;
3. return to `holonight-shell` checkpoint CP-S-001;
4. change UP-001 to `Fixed upstream; awaiting downstream retry`;
5. retry `NavPanel` → `HnNavigationDelegate`;
6. rerun the focused Settings test, `task qml-lint`, component instantiation, and
   `git diff --check`;
7. capture the post-change screenshot and resume the existing visual-review gate.
