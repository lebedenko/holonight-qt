# DESIGN.md: Per-Corner Surface Geometry

**Spec:** `docs/sdd/per-corner-surface-geometry/SPEC.md`

## Design

`ShapeResolver` remains the single owner of surface geometry. Its existing overload is preserved and delegates to
a new overload that accepts an integer corner-mask override. The override is applied after semantic role,
effective corner style, radius/chamfer overrides, and dimension clamping. This keeps existing callers source
compatible and makes the returned `ResolvedShape` the final renderer-ready result.

The mask uses four bits in clockwise order: top-left (`1`), top-right (`2`), bottom-right (`4`), and bottom-left
(`8`). `-1` means inherit. Values `0`–`15` replace the resolved topology; every other value inherits it.

Final kind is classified from the final mask: `None` is `Rounded`, `All` is `Chamfered`, and partial masks are
`Hybrid`. A Pill whose final mask is `None` remains `Pill`.

`HnSurfaceFrame.chamferedCornersOverride` exposes the local override in QML. Both `HnAppearance.resolve()` and
`HnShapeProfile.resolve()` retain their existing invokable signatures and add compatible overloads accepting the
mask. Live appearance changes still recompute dimensions and style while preserving an explicit local mask.

## Layout guidance

Directional semantics stay in layout composition rather than new surface roles. A left panel chamfers its right
corners; a right panel chamfers its left corners. Exterior corners stay rounded, and chamfers point toward
internal seams. Central content should normally use quieter Card or inherited Window geometry. Bright borders
remain reserved for active or focused states.

Panel continues to use its existing 16 px chamfer primitive. No `DockLeft` or `DockRight` roles or global mask
setting are introduced.
