# DESIGN: Semantic Surface Frames

**Spec:** `docs/sdd/semantic-surface-frames/SPEC.md`
**Date:** 2026-07-22
**Status:** Implemented

---

## 1. Scope and source extraction

The source architecture recommends that `holonight-qt` own reusable geometry and appearance while
`holonight-shell` keeps its HUD semantics. Inspection of the current `HudFrame.qml` confirms that its
portable subset is:

- a filled, bordered surface;
- rounded and chamfered path geometry;
- palette-based fill and border defaults;
- size- and property-driven redraw behavior.

Variant-specific opacity, directional offsets, gradients, inner highlights, glow, and shell integration are
not required to establish the shared contract. They remain available to the future shell wrapper as overlay
effects or shell-owned decoration.

## 2. Components and files

| File | Responsibility |
|---|---|
| `src/theme/shaperesolver.h/.cpp` | Shared primitive tokens, semantic enums, role resolver, and clamping |
| `qml/hnshapetypes.h` | Thin QML enum namespaces corresponding to the shared enums |
| `qml/hnshapeprofile.h/.cpp` | QML singleton adapter over the shared resolver |
| `qml/HnSurfaceFrame.qml` | Public scene-graph-backed frame component |
| `CMakeLists.txt` | Require the Qt Quick Shapes component |
| `qml/CMakeLists.txt` | Register, copy, install, and link new public module types |
| `tests/test_qml_smoke.cpp` | API, resolution, edge-case, and compatibility tests |

The shared resolver is part of `holonight_theme`, so both the QML module and a later `HoloniightStyle`
migration can consume exactly the same contract. QML enum namespaces use `Q_NAMESPACE`, `QML_NAMED_ELEMENT`,
and `Q_ENUM_NS`; compile-time assertions keep their integer values aligned with the shared enums.

## 3. Public contracts

### 3.1 Shared shape model

```cpp
struct ShapeTokens {
  qreal radius_xs = 4.0;
  qreal radius_sm = 6.0;
  qreal radius_md = 8.0;
  qreal radius_lg = 12.0;
  qreal chamfer_xs = 4.0;
  qreal chamfer_sm = 6.0;
  qreal chamfer_md = 10.0;
  qreal chamfer_lg = 16.0;
};

struct ResolvedShape {
  ShapeKind kind;
  qreal radius;
  qreal chamfer;
  CornerMask chamfered_corners;
};
```

`SurfaceRole` includes Window, Panel, Popup, Card, Menu, Tooltip, Control, Pill, Hud, and
WorkspaceIndicator. `CornerStyle` includes Inherit, Hybrid, Rounded, and Chamfered. `CornerMask` identifies
the exact cut corners instead of making radius/chamfer values implicitly describe path topology.

### 3.2 Shape resolver

`ShapeResolver` owns deterministic role mapping and dimension clamping:

```cpp
static ResolvedShape resolve(SurfaceRole role, CornerStyle style, const QSizeF& size,
                             qreal radius_override, qreal chamfer_override,
                             const ShapeTokens& tokens);
```

The QML adapter normalizes unknown integer values to Card and Inherit, then returns kind/radius/chamfer/corners
as a `QVariantMap`. Pill radius uses half the drawable height rather than a sentinel such as 999.

### 3.3 `HnSurfaceFrame`

The root is an `Item` so the component can host arbitrary child content. Its visible background is a
`QtQuick.Shapes.Shape` with one `ShapePath`. A cached SVG path string selects line or quadratic segments from
the resolved corner mask without a JavaScript paint loop.

The component API is:

```qml
property int surfaceRole: HnSurfaceRole.Card
property int cornerStyleOverride: HnCornerStyle.Inherit
property real radiusOverride: NaN
property real chamferOverride: NaN
property color fillColor: HoloniightPalette.surface
property color borderColor: HoloniightPalette.borderPassive
property real borderWidth: HoloniightPalette.borderWidth
readonly property real effectiveRadius: ...
readonly property real effectiveChamfer: ...
readonly property int shapeKind: ...
readonly property int chamferedCorners: ...
default property alias contentData: contentItem.data
```

`Inherit` initially resolves to Hybrid. A later `HnAppearance` cycle can change the inherited value without
breaking the component API.

## 4. Geometry

The drawable rectangle is inset by half the normalized border width on every side, keeping the centered Shape
stroke inside the item. The maximum usable corner extent is half the smaller drawable dimension.

Resolution order is:

1. Select inherited or explicit style.
2. Resolve role defaults for that style.
3. Apply a finite, non-negative local override when present.
4. Clamp radius and chamfer to the available corner extent.
5. Use the resolved corner mask to select rounded and chamfered path segments.

Hybrid surfaces use top-right and bottom-left signature chamfers. Chamfered surfaces cut all four corners;
rounded surfaces cut none. Cards and controls stay rounded in the inherited profile, while panels and
workspace indicators are chamfered.

For zero or negative drawable dimensions, the Shape remains empty and all effective values resolve to zero.

## 5. Rendering choice

`QtQuick.Shapes` is selected over copying the shell's `Canvas` implementation. A Shape path participates in
the scene graph and does not execute JavaScript painting on the GUI thread whenever size or appearance changes.
It also provides declarative property dependencies and avoids manual `requestPaint()` signal fan-out.
The top-level Qt package lookup will explicitly require Quick Shapes, and the QML module will declare the
corresponding module dependency so build-tree and installed imports resolve consistently.

A custom `QQuickItem` renderer would offer more control but adds C++ scene-graph lifecycle and rendering code
before profiling demonstrates a need. A `QQuickPaintedItem` would still rasterize through a painter-backed
texture, so it offers little benefit for this simple path.

## 6. Data flow

```text
consumer sets surfaceRole / optional overrides
  -> HnSurfaceFrame selects effective style
  -> HnShapeProfile adapts the request to the shared ShapeResolver
  -> ShapeResolver maps primitive tokens and clamps to current bounds
  -> ShapePath bindings update scene-graph geometry
  -> HoloniightPalette changes update fill and border bindings
```

There is no persistence or cross-process state in this cycle.

## 7. Compatibility and migration

Adding public QML types is backward compatible. `qml/CMakeLists.txt` must add `HnSurfaceFrame.qml` to both
`qt_add_qml_module(QML_FILES ...)` and `HOLONIGHT_QML_FILES`, because the latter populates the lowercase
compatibility module and installation list.

The future shell migration can preserve its current public API:

```qml
HnSurfaceFrame {
    surfaceRole: HnSurfaceRole.Hud
}
```

The wrapper will map `HudFrame.Variant` values and retain gradients, glow, offsets, and shell defaults outside
the shared component. No shell file changes are part of this SDD.

The Widgets style is deliberately not migrated in this cycle. Its later renderer will consume
`ShapeResolver` and construct equivalent `QPainterPath` geometry; ordinary controls will remain rounded while
menus, tooltips, and suitable major frames can use their semantic shapes.

## 8. Testing strategy

Tests instantiate QML snippets through the existing `QmlSmoke` fixture and inspect public properties. A compact
QML `Repeater` can instantiate the role/style matrix, while C++ asserts component readiness and absence of
creation errors. Focused cases validate exact defaults, override precedence, fallback behavior, Pill radius,
small/zero dimensions, palette defaults, content parenting, and both import spellings.

Visual pixel comparison is intentionally deferred. The first contract is geometry resolution and safe
instantiation; a downstream shell migration should add visual comparison against representative HUD frames.

## 9. Risks and mitigations

| Risk | Mitigation |
|---|---|
| Shape antialiasing differs from Canvas at fractional scale | Inset the centered stroke and add small-size/fractional-width smoke cases; visually verify before shell adoption |
| Public enum names become difficult to change | Keep the first role set aligned with the architecture document and normalize unknown values |
| Hybrid geometry is ambiguous | Carry an explicit mask and use top-right plus bottom-left signature chamfers |
| Lowercase module misses new files | Keep the explicit compatibility copy/install test |
| Appearance persistence expands this cycle | Keep `Inherit` mapped to built-in Hybrid until the dedicated `HnAppearance` SDD |

## 10. Decisions deferred

- The JSON schema and precedence between profile, scale, and advanced overrides.
- Directory-watch behavior for atomic writes.
- Whether application windows use a Control, Window, or composition-based root.
- Shell mapping for all existing `HudFrame.Variant` visual effects.
- GPU effects such as gradients, glow, blur, and shadows.
