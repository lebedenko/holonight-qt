# SPEC: Shared-controls compatibility migration

**Status:** Implemented
**Date:** 2026-07-27

## Goal

Complete the ownership boundary established by the shared-controls foundation without breaking existing QML or
CMake consumers. This cycle changes import availability and implementation ownership only; public behavior,
rendering, names, versions, and singleton identity remain stable.

## Public contracts

### `Holonight.Core`

`import Holonight.Core` shall export:

- `HoloniightPalette`, `HolonightTheme`, and `HnAppearance` as process configuration singletons;
- `HnShapeProfile` as the shape-resolution singleton;
- `HnSurfaceRole`, `HnCornerStyle`, `HnShapeKind`, and `HnCornerMask` with their existing enum values;
- `HnIconProvider` as the semantic SVG service singleton;
- `HnIcon` with its existing properties, defaults, error state, and `IconState` enum;
- the existing `HnControlSize` and `HnControlMetrics` singletons.

Importing `Holonight.Core` and `Holonight` in one engine shall resolve palette, theme, and appearance to the same
objects. No duplicate singleton registrations are permitted.

### `Holonight.Controls`

`import Holonight.Controls` shall additionally export `HnSurfaceFrame` and `HnApplicationWindow`. Their default
properties, content ownership, named slots, geometry normalization, shape resolution, and window defaults shall
remain unchanged.

### Compatibility

- `import Holonight` shall continue to expose every type it exposed before this migration at module version 1.0.
- `import holonight` shall retain its existing component surface, including `HnIcon.Active`,
  `HnSurfaceFrame.contentData`, and all `HnApplicationWindow` slots.
- No lowercase Core or Controls module shall be added.
- Standard Qt Quick Controls style components remain owned by `Holonight`.
- Installed CMake targets remain `HolonightQt::Core` and `HolonightQt::Controls`.

## Packaging requirements

Canonical Core and Controls install trees shall contain their plugin, `qmldir`, generated type metadata, and owned
QML sources. The legacy install trees shall remain importable. Build ordering and linkage shall ensure all three
canonical modules can be consumed directly from both the build tree and an installation.

## Out of scope

Downstream shell, AI, and package-manager adoption; process-wide density; primitive control sizing; compatibility
deprecation/removal; package renaming; and visual redesign are separate cycles.
