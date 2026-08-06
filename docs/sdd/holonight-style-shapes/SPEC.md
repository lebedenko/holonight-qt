# SPEC: HoloNight Style Shapes

**Feature:** Shared semantic shape geometry in the Qt Widgets style
**Date:** 2026-07-23
**Status:** Implemented

## 1. Context

The QML surface-frame cycle introduced `ShapeResolver` as the shared source of truth for radii, chamfers,
semantic roles, and corner styles. `HoloniightStyle` still contains local radius constants and draws menus and
tooltips with unrelated rectangular or rounded paths. This cycle connects the Qt Widgets renderer to the same
geometry model without changing widget ownership, palettes, or layout metrics.

## 2. Goals

- Make Qt Widgets controls obtain their rounded radius from `ShapeResolver`.
- Render menu and tooltip surfaces from their semantic roles.
- Give the painter backend rounded, chamfered, and hybrid path support.
- Preserve existing fills, borders, focus behavior, and ordinary-control rounded geometry.
- Keep specialized selection, focus, groove, slider, and progress geometry local to QStyle.

## 3. Non-goals

- Runtime appearance configuration or user-selected corner styles.
- Changing window masks, native window shapes, shadows, or compositor decorations.
- Converting generic one-pixel frames, separators, tab frames, or status-bar frames into shaped surfaces.
- Removing legacy radius fields from `ColorTokens`; that requires a separate compatibility cycle.
- Migrating QML controls or downstream repositories.

## 4. Requirements

### REQ-F-001: Resolve ordinary control geometry centrally

Control panels, input frames, buttons, combo boxes, spin boxes, and other ordinary control outlines currently
using the local 6 px control radius shall resolve `SurfaceRole::Control` with rounded geometry.

Acceptance criteria:

- `HoloniightStyle` has no independent control-radius constant.
- Controls remain rounded and retain the current 6 px default on sufficiently large rectangles.
- Geometry is clamped for controls smaller than twice the requested radius.

### REQ-F-002: Render semantic menu geometry

`PE_FrameMenu` and `PE_PanelMenu` shall resolve `SurfaceRole::Menu` and use the returned shape kind, radius,
chamfer, and corner mask.

Acceptance criteria:

- Menu fill and border use the same semantic path.
- Default menu geometry is hybrid: top-right and bottom-left are chamfered; the other corners are rounded.
- The existing raised-surface fill and passive border colors are unchanged.
- Border strokes remain inset and within the paint rectangle.

### REQ-F-003: Render semantic tooltip geometry

`PE_PanelTipLabel` shall resolve `SurfaceRole::Tooltip` and use the semantic path for both fill and border.

Acceptance criteria:

- Default tooltip geometry is hybrid with the signature corners.
- The inverse-surface fill and passive border colors are unchanged.
- Small rectangles produce valid clamped paths.

### REQ-F-004: Support every resolved shape kind in the painter backend

The QStyle path builder shall render rounded, chamfered, hybrid, and pill results returned by
`ShapeResolver`.

Acceptance criteria:

- Rounded corners use quadratic curves and chamfered corners use straight cuts.
- The returned corner mask, rather than role-specific painter logic, selects chamfered corners.
- Empty rectangles are handled safely.

## 5. Quality constraints

- No new dependency beyond the existing `holonight_theme` target.
- Painter state is restored after every helper.
- Existing generic square-frame behavior remains pixel-identical.
- Tests assert observable pixels at edges and signature corners rather than private helper calls.
- Changes remain limited to QStyle rendering, focused tests, and this SDD.

## 6. Verification

- Build `holonight_style_primitive_tests` and `holonight_tests`.
- Run focused menu, tooltip, input, button, and small-geometry tests offscreen.
- Run the complete style primitive suite offscreen.
- Run the full CTest suite if focused verification passes.
- Run formatting and changed-file static checks where supported by the project toolchain.
