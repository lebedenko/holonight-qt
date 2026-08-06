# SPEC: Legacy Radius Token Cleanup

**Feature:** Semantic QML control radii with compatibility aliases
**Date:** 2026-07-23
**Status:** Implemented

## Goals

- Make the semantic shape system the only internal source of QML control radii.
- Apply live appearance scaling and base-radius overrides to existing Qt Quick Controls.
- Preserve the legacy palette radius properties for downstream compatibility.

## Requirements

- Built-in QML controls shall not read `HoloniightPalette.radius*`.
- Rounded `Rectangle` backgrounds shall request a radius using their semantic surface role and current dimensions.
- Radius bindings shall re-resolve when `HnAppearance.revision` changes.
- `HnAppearance.roundedRadius()` shall force rounded geometry because `Rectangle` cannot render chamfers or hybrid
  corners, while still using the configured semantic shape tokens.
- `radiusTight`, `radiusControl`, `radiusPopup`, and `radiusPill` shall remain available as deprecated compatibility
  properties and C++ fields.

## Non-goals

- Removing the legacy radius fields or changing their scheme-specific values.
- Replacing every control background with `HnSurfaceFrame`.
- Making rounded `Rectangle` backgrounds follow the configured corner style; that requires a later renderer migration.
- Replacing specialized local geometry such as the checkbox indicator radius.

## Verification

- QML-test semantic default values and live appearance updates on existing controls.
- Keep a smoke test proving the legacy palette properties remain accessible.
- Run QML lint and the full CTest suite.
