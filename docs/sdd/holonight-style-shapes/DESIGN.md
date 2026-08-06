# DESIGN: HoloNight Style Shapes

## Decision

Keep `ShapeResolver` renderer-neutral and add a private QPainter-path adapter in `holonightstyle.cpp`.
QML continues to serialize its path for Qt Quick Shapes; QStyle converts the same `ResolvedShape` into a
`QPainterPath`. This shares policy and dimensions without coupling either renderer to the other.

## Geometry flow

1. The primitive selects a semantic `SurfaceRole`.
2. `ShapeResolver::resolve()` receives the painted rectangle size.
3. A private path builder walks clockwise around an inset `QRectF`.
4. Corners present in `ResolvedShape::chamfered_corners` become straight cuts.
5. Remaining corners use `ResolvedShape::radius`; pill is naturally represented by its resolved half-height
   radius.
6. Fill and border are drawn from the same path.

For ordinary controls, the adapter explicitly requests `CornerStyle::Rounded`. That documents the intended
Widgets behavior even if a semantic profile changes later. Menus and tooltips inherit their semantic Hybrid
profile.

## Scope choices

- Menu and tooltip primitives are semantic surfaces and migrate in this cycle.
- `PE_Widget` and `PE_FrameWindow` keep rectangular fills because painting does not establish a native window
  mask; rounding only their pixels would create misleading corners.
- Generic frames, tab frames, status bars, separators, grooves, selections, and focus rings keep their current
  geometry because they are structural or specialized shapes, not independent surfaces.
- Legacy `ColorTokens::radius*` fields remain source-compatible but cease to drive migrated QStyle paths.

## Test strategy

Image tests verify center fills, edge strokes, transparent extreme corners, and painted/transparent pixels near
the two different corner treatments. A small-rectangle test guards resolver clamping and path validity. Existing
tests continue to protect generic square frames and input focus colors.
