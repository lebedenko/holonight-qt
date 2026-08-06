# SPEC: Controls qmllint install metadata

**Status:** Implemented
**Date:** 2026-07-28
**Source:** `docs/controls-qmllint-install-metadata-remediation.md`

## Goal

Make an installed `Holonight.Controls` module self-describing to Qt tooling. A consumer shall be able to import
the module and reference every public control with `qmllint` using only the installed QML root as its additional
import path.

The correction shall fix the upstream package metadata or discovery behavior. Consumers shall not need to link
`HolonightQt::Controls`, copy controls, suppress warnings, or add project-specific tooling workarounds.

## Tooling contract

After installing the project to an isolated prefix, this command pattern shall succeed for a minimal consumer:

```bash
/usr/lib/qt6/bin/qmllint \
  --bare \
  -I <prefix>/lib/qt6/qml \
  -I /usr/lib/qt6/qml \
  <consumer.qml>
```

`--bare` is part of the isolation contract: without it, `qmllint` searches its default user and system import
directories before or alongside additional `-I` directories and may resolve an older installed
`Holonight.Controls` module.

The consumer shall import `Holonight.Controls` and reference every public 1.0 type:

- `HnActionBar`
- `HnActionDelegate`
- `HnApplicationWindow`
- `HnCardDelegate`
- `HnChoiceCard`
- `HnEmptyState`
- `HnFormField`
- `HnIconComboBox`
- `HnKeyHint`
- `HnListDelegate`
- `HnLoadingState`
- `HnNavigationDelegate`
- `HnPanelHeader`
- `HnSearchField`
- `HnSectionHeader`
- `HnSegmentedControl`
- `HnSettingsRow`
- `HnStatusIndicator`
- `HnSurfaceFrame`
- `HnTextArea`

`HnSelectableDelegate` shall remain internal and shall not become a public type merely to satisfy tooling.

## Investigation requirements

The investigation established:

- the Controls qmltypes output contains an empty `Module {}` because `qmltyperegistrar` describes
  plugin-supplied C++ types; QML-file types remain declared by the generated `qmldir`;
- removing the generated `prefer :/qt/qml/Holonight/Controls/` entry does not change the observed result;
- both build-tree and installed-prefix linting resolve every current type when default imports are disabled;
- stale modules in `~/.local/lib/qt6/qml` and `/usr/lib/qt6/qml` contained only the older Controls surface and
  shadowed the isolated prefix during the reported plain `-I` invocation;
- Qt's generated `qmldir` plus installed QML sources are sufficient metadata for this QML-only public surface.

The implementation therefore retains Qt-generated metadata, avoids handwritten qmltypes, establishes a
canonical public inventory, and makes installed-prefix lint coverage hermetic.

## Packaging and compatibility

The change shall preserve:

- module URI `Holonight.Controls` and public version 1.0;
- all existing public type names and behavior;
- dynamic QML plugin discovery;
- the installed QML directory layout;
- runtime imports from build and install trees;
- downstream consumption without linking `HolonightQt::Controls`;
- the internal status of `HnSelectableDelegate`.

The installed `qmldir`, qmltypes metadata, QML sources, plugin, and resources shall agree on the module's public
surface.

## Regression coverage

Add an automated installed-prefix test that installs into an isolated temporary prefix, generates a minimal QML
consumer, and invokes the Qt installation's `qmllint` with only that prefix added as an external import path.
The test shall fail for unresolved module types and for other module-attributable warnings.

The public type inventory shall have one maintained source, or mechanically verified sources, so adding a public
control cannot silently omit it from install and lint coverage.

## Acceptance criteria

- The isolated `HnNavigationDelegate` reproduction exits successfully.
- Every public Controls type passes the installed-prefix lint test.
- Installed Controls metadata and artifacts describe the same public surface.
- `HnSelectableDelegate` remains internal.
- Existing build-tree lint, runtime smoke, and package-install tests pass.
- `task build`, `task lint`, and `task test` pass.
- No downstream linkage or warning suppression is introduced.

## Out of scope

Changing public control APIs or visuals, renaming modules or targets, changing the installed QML root, migrating
downstream UI, and redesigning the broader QML packaging layout are outside this cycle.
