# SPEC: Shared Appearance Configuration

**Feature:** Live shared shape appearance configuration
**Date:** 2026-07-23
**Status:** Implemented

## Goals

- Load shape appearance from `$XDG_CONFIG_HOME/holonight/appearance.json`.
- Expose the active configuration through the `HnAppearance` QML singleton.
- Re-resolve existing `HnSurfaceFrame` instances when the file changes.
- Keep parsing and shape-token derivation in the shared C++ theme library.
- Preserve the semantic profile and current dimensions when the file is missing or invalid.

## Configuration contract

```json
{
  "version": 1,
  "cornerStyle": "inherit",
  "shapeScale": 1.0,
  "baseRadius": null,
  "baseChamfer": null
}
```

- `cornerStyle`: `inherit`, `hybrid`, `rounded`, or `chamfered`.
- `shapeScale`: finite value in `[0.25, 4.0]`, applied to primitive radius and chamfer tokens.
- `baseRadius` and `baseChamfer`: optional finite values in `[0, 128]`; when present, replace the corresponding
  scaled token family. They are advanced overrides, not required configuration.
- Unknown keys are ignored. Unsupported versions, malformed JSON, wrong types, and invalid values fall back
  independently to safe defaults.
- `HOLONIGHT_APPEARANCE_FILE` may override the path for tests and controlled launches.

## Requirements

- `AppearanceConfig` shall be a renderer-neutral C++ value with deterministic parsing and token derivation.
- Missing or invalid files shall resolve to `inherit`, scale `1.0`, and no base overrides.
- A caller-provided component style other than `Inherit` shall take precedence over the configured global style.
- `HnAppearance` shall expose the path, style, scale, optional overrides, revision, and `reload()`.
- `HnAppearance` shall watch both the file and parent directory so atomic replacement and late creation reload.
- `HnSurfaceFrame` shall resolve through `HnAppearance` and update when its revision changes.
- Both `Holonight` and lowercase `holonight` imports shall expose the singleton.

## Non-goals

- Writing or editing the configuration file.
- Combining this file with the existing color/font/icon `theme.conf`.
- Migrating `HoloniightStyle` to live appearance updates in this cycle.
- Removing legacy palette radius properties.

## Verification

- Unit-test defaults, valid parsing, partial invalid fallback, path resolution, scaling, and precedence.
- QML-test public properties, explicit reload, and existing-frame re-resolution.
- Build the QML module, run focused tests, then run full CTest.
