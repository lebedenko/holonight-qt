# SPEC: Shared-controls compatibility removal

**Status:** Implemented
**Date:** 2026-07-28

## Goal

Remove the temporary QML compatibility surface now that all known consumers use the canonical shared-control
modules. Each public type shall be exported by exactly one module:

- `Holonight.Core` owns design-system primitives and `HnIcon`;
- `Holonight.Controls` owns reusable composite controls, `HnSurfaceFrame`, and `HnApplicationWindow`;
- `Holonight` owns only the Qt Quick Controls style components.

This is an intentional compatibility break for the legacy shared-type imports. It shall not change rendering,
runtime behavior, singleton identity, canonical type names, or canonical CMake targets.

## Removal contract

- Remove the lowercase `holonight` QML module from the build tree and installation.
- Remove the lowercase `HnIcon` compatibility subtype and its packaging.
- Stop re-exporting `Holonight.Core` types through `import Holonight`.
- Stop exporting `HnIcon`, `HnSurfaceFrame`, and `HnApplicationWindow` from `Holonight`.
- Keep `HnIcon` available from `Holonight.Core`.
- Keep `HnSurfaceFrame` and `HnApplicationWindow` available from `Holonight.Controls`.
- Keep the styled Qt Quick Controls available from `Holonight`.

The removal shall leave no generated or installed lowercase `holonight` directory and no duplicate source or
metadata entry for a canonical shared type.

## Internal and packaging requirements

Style components shall import the Core module explicitly for palette, appearance, shape, enum, theme, and icon
dependencies instead of relying on transitive visibility through their own module.

`Holonight.Controls` may continue to depend on both `Holonight` and `Holonight.Core`. Installed package components
and targets remain:

```text
HolonightQt::Core
HolonightQt::Controls
```

The canonical build and install trees shall retain their plugins, `qmldir` files, generated type metadata, QML
sources, and assets.

## Documentation and tests

Repository examples and current tests shall use explicit canonical imports:

- `import Holonight.Core` for primitives and `HnIcon`;
- `import Holonight.Controls` for shared composites, frames, and application windows;
- `import Holonight` only for styled Qt Quick Controls.

Compatibility-only tests shall be removed or replaced with canonical ownership checks. Install validation shall
assert that the lowercase module is absent and that each removed legacy export is absent from the `Holonight`
module metadata.

## Out of scope

Renaming `Holonight`, changing module or package versions, changing the CMake namespace, removing deprecated
palette property aliases, redesigning controls, changing public properties, and adding new controls are separate
changes.
