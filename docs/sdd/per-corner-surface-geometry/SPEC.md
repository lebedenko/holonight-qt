# SPEC.md: Per-Corner Surface Geometry

## Requirements

- `ShapeResolver` shall accept an optional corner-mask override without breaking its existing call signature.
- An absent (`-1`) or invalid mask shall preserve the topology resolved from role and corner style.
- Every mask from `0` through `15` shall replace the resolved topology.
- Radius and chamfer dimensions shall remain shared across corners and retain existing override and clamping rules.
- The final kind shall be Rounded for mask `0`, Chamfered for mask `15`, and Hybrid for masks `1`–`14`.
- A Pill with an all-rounded final topology shall remain Pill.
- `HnCornerMask` shall expose `Inherit`, `None`, each corner flag, and `All`.
- `HnSurfaceFrame.chamferedCornersOverride` shall default to `HnCornerMask.Inherit`.
- `chamferedCorners` shall remain a read-only final resolved mask.
- Existing consumers that do not set the property shall remain visually and source compatible.
- The Containers demo shall show left/right docked panels and both three-to-one topologies.

## Exclusions

- No global appearance mask setting.
- No per-corner radius or chamfer dimensions.
- No directional semantic roles such as `DockLeft` or `DockRight`.
- No reduction of the Panel role's 16 px chamfer scale.
- No downstream application migration.
