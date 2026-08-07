# SPEC: Appearance Configuration Foundation Adoption

**Initiative:** ACF-004
**Date:** 2026-08-07
**Status:** In Progress
**Repository baseline:** `767cf3d1640e6957455b0146d28f6ce233ff5f51`
**Umbrella contract:** `c66c5274eb4e7d906cc99c4719de709188046577`
**Provider contract:** `holonight-config` `81b01d3ae8433f3a4b017db2feb588a1ee62b714`

## Goal

Make `holonight-qt` a read-only Qt renderer of the canonical HoloNight appearance document while separating color,
metric, shape, and typography responsibilities. Remove the legacy INI/JSON configuration authorities and their
compatibility paths rather than layering the new configuration on top of them.

## Dependency contract

- Consume the installed `HoloNight::Config` package from `holonight-config`; do not copy its schema, defaults, TOML
  parser, path rules, diagnostics, or test fixtures into this repository.
- Keep theme-scheme and accent catalogs in `holonight-qt`. Catalog validation converts structurally valid neutral
  identifiers into Qt theme selections.
- Qt-specific strings, colors, fonts, file watching, palette resolution, shape-token derivation, and QML projection
  remain owned here.
- This repository never writes canonical appearance. HoloNight Settings is the only production writer.

## Canonical loading and reload

- Startup resolves and loads `appearance.toml` through `HoloNight::Config`.
- Missing startup configuration uses the package defaults without creating a file.
- A present invalid startup document uses package defaults and exposes/logs the structured diagnostic once.
- Live consumers watch both the canonical file and its parent directory so same-directory atomic replacement and
  late creation remain observable.
- A reload parses and validates the complete replacement before publishing any Qt state. A failed reload retains the
  last known-good state, emits a diagnostic, and emits no appearance-change revision.
- A semantically unchanged valid reload rearms the watcher but emits no change revision.
- `HOLONIGHT_APPEARANCE_FILE` is the only accepted appearance-path/field override.

The QML module shall have one engine-local canonical appearance state. `HnAppearance`, `HolonightTheme`, and
`HoloniightPalette` are projections of that state and must not independently parse or watch the document. Style and
platform-theme plugins may instantiate the reusable Qt reader in their own plugin lifecycle, but must use the same
loader, validation, catalog-resolution, and failure semantics.

## Configuration API cleanup

Remove the current Qt-owned persisted schema APIs:

- `ThemeConfig::load()`, `loadFile()`, `save()`, and `configFilePath()`;
- INI parsing and writing for `theme.conf`;
- `AppearanceConfig` JSON parsing, writing, and path resolution for `appearance.json`;
- KDE-as-selection fallback and field-specific environment overrides;
- persisted `AppearanceMode`, transparency, and all duplicated defaults;
- file watchers targeting the two legacy files.

Retain renderer-domain values only. The Qt projection may use a renamed value such as `ResolvedAppearance`, but it
must be constructed from a validated `HoloNight::Config::Appearance` and contain no serialization behavior.

The public Qt/QML projection uses canonical role names:

- scheme and accent;
- UI, monospace, title, and display font families and configured sizes;
- icon theme, fallback icon theme, and cursor theme;
- layout scale;
- shape style, scale, and optional base radius/chamfer.

Dark/light is derived from the selected scheme. Derived typography role sizes remain a Qt design-system concern and
must not be persisted as additional configuration fields.

## Token taxonomy

### Colors

`ColorTokens` contains only `QColor` fields. Remove `borderWidth`, `focusBorderWidth`, `separatorWidth`,
`controlHeight`, and `controlPadding` from it. Palette generation and scheme equality must therefore vary only with
color semantics.

### Metrics

Introduce an installed C++ `MetricTokens` authority. It owns at least:

| Metric | Compact | Normal | Large | Hero |
|---|---:|---:|---:|---:|
| Control height | 28 | 32 | 40 | 52 |
| Horizontal padding | 8 | 8 | 12 | 16 |
| Icon size | 16 | 16 | 20 | 24 |
| Internal spacing | 4 | 6 | 8 | 10 |

It also owns the existing shared scalar semantics: border width `1`, focus-border width `1`, separator width `1`,
header height `56`, app-title icon size `32`, app-title icon spacing `12`, and app-title text spacing `8`.

`HnMetrics` is the single QML facade over `MetricTokens`, including size-role normalization and lookup. Remove
`HnControlMetrics.qml`; do not retain a forwarding singleton. Remove metric properties from `HoloniightPalette` and
migrate every internal QML, example, demo, test, package manifest, and document call site to `HnMetrics`.

`controlPadding` is not preserved as an ambiguous alias: callers choose horizontal padding or internal spacing by
semantic use. New component-local constants stay local unless at least two independent components share the same
semantic role.

### Shapes and typography

`ShapeTokens` and `ShapeResolver` remain the shape authority. The canonical shape selection feeds them through the Qt
projection; JSON-specific `AppearanceConfig` is removed.

Configured typography values remain in the appearance projection. Derived role sizes and fallback behavior stay in
a dedicated Qt typography model and must not be added to `ColorTokens` or `MetricTokens`.

## Public behavior

- Existing built-in scheme IDs, accent resolution, generated KDE color schemes, QPalette behavior, shape precedence,
  and visual token values remain unchanged unless this SDD explicitly changes their ownership.
- `HoloniightPalette` remains the color QML singleton and emits only when resolved colors change.
- `HnMetrics` is immutable for this initiative because v1 appearance does not persist metric values.
- `HnAppearance` continues to expose shape resolution and the appearance revision; it also provides the authoritative
  config path and reload entry point for QML tests and controlled consumers.
- `HolonightTheme` exposes catalog information and typography/icon/layout projection from the same canonical state.
- Both canonical `Holonight.*` and supported lowercase compatibility imports expose the same updated types.

## Clean-break requirements

After ACF-004, `holonight-qt` does not read, write, watch, document, install, or test `theme.conf` or
`appearance.json`. Legacy files remain untouched on disk and inert. No dual-read period, migration utility, forwarding
metric property, or deprecated environment alias is part of the final commit.

## Verification

- Unit-test neutral-to-Qt projection, catalog validation, derived color mode, accent resolution, typography mapping,
  optional shapes, and structured diagnostics.
- Test missing startup, invalid startup, valid reload, invalid-reload rollback, atomic replacement, late creation,
  watcher rearming, and unchanged reload behavior.
- Test `ColorTokens` contains colors only and `MetricTokens` exposes the exact scalar and size-role values.
- QML-test `HnMetrics`, the shared appearance revision, palette/theme projections, and existing-instance shape updates.
- Search production, tests, examples, install manifests, and docs for removed legacy paths, APIs, properties, and
  environment names.
- Build, run format/tidy checks, focused tests, complete headless CTest, install/package tests, and manual Qt/QML
  dark/light, accent, typography, and shape checks.

## Non-goals

- Writing appearance configuration or providing Settings UI.
- Moving the theme catalog or semantic palette into `holonight-config`.
- Cross-toolkit export, Qt 5 support, GTK support, or adapter implementation.
- Configurable metric values in appearance schema v1.
