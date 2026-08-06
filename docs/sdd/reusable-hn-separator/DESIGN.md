# DESIGN: Reusable HnSeparator

**Spec:** `docs/sdd/reusable-hn-separator/SPEC.md`
**Status:** Implemented
**Date:** 2026-07-29

## 1. Scope

This cycle adds one public, visual-only `HnSeparator` type to `Holonight.Controls`, migrates one existing
shared control as an integration proof, extends the controls gallery, and adds focused source/install-tree,
API, geometry, and rendering tests.

Repository-wide divider migration and changes in `holonight-shell` or `holonight-ai` remain outside this
cycle. The public component contains no application behavior or compatibility aliases for the shell-local
`ContentSeparator` API.

## 2. Components and files

| File | Responsibility |
|---|---|
| `qml/controls/HnSeparator.qml` | Public API, layout contract, opacity profiles, and declarative rendering |
| `qml/hnseparatoralignment.h` | Deterministic physical-pixel alignment calculation |
| `qml/hnseparatorgeometry.h/.cpp` | Reactive DPR and scene-position observer |
| `qml/CMakeLists.txt` | Register, package, install, and lint the public QML type and internal helper |
| `qml/controls/HnSectionHeader.qml` | Focused integration proof preserving `dividerVisible` behavior |
| `examples/controls-gallery/Main.qml` | Representative separator gallery states |
| `tests/test_qml_smoke.cpp` | Public API, behavior, geometry, accessibility, and import tests |
| `tests/test_package_install.cmake.in` | Installed-file and installed-module coverage |

`HnSeparatorGeometry` is an internal QML type. It is not installed as a public header, exported from the
CMake package, or documented as consumer API.

## 3. Public QML contract

`HnSeparator` has an `Item` root and the following API:

```qml
enum FadeMode {
    Solid,
    FadeBoth,
    FadeStart,
    FadeEnd
}

property int orientation: Qt.Horizontal
property color color: HoloniightPalette.borderSubtle
property real thickness: HoloniightPalette.separatorWidth
property int fadeMode: HnSeparator.Solid
property real centerOpacity: 1.0
property real edgeOpacity: 0.0
```

Unknown orientation and fade-mode values normalize internally to `Qt.Horizontal` and `Solid`. The requested
public values remain observable so assignments are not silently rewritten.

For a horizontal separator, `implicitWidth` is zero and `implicitHeight` is the non-negative requested
thickness. For a vertical separator, those values are reversed. The component never binds its explicit
`width` or `height`; anchors and Qt Quick Layouts retain ownership of major-axis length and may override
minor-axis occupancy.

`Accessible.ignored` is true. The root is an `Item`, creates no input handlers, and does not opt into focus.

## 4. Physical-pixel geometry

The semantic thickness is interpreted as a count of standard separator-width units. With the current
`separatorWidth` value of one, the default therefore requests one physical pixel at every DPR. The normalized
physical pixel count is:

```text
requestedPixels = thickness <= 0 ? 0 : max(1, round(thickness / separatorWidth))
paintedLogicalThickness = requestedPixels / effectiveDpr
```

This preserves one logical unit of layout occupancy by default while allowing the painted line to contract to
`0.8`, `0.666…`, `0.571…`, or `0.5` logical units at fractional and doubled DPR. A zero or negative request
produces zero implicit minor size and no painted line.

The desired leading edge is the separator root's local minor-axis origin. If that point maps to scene
coordinate `s`, the internal painted item uses:

```text
snappedSceneCoordinate = round(s * effectiveDpr) / effectiveDpr
localOffset = snappedSceneCoordinate - s
```

The painted rectangle uses `localOffset` on the minor axis, `paintedLogicalThickness` as its minor dimension,
and the root's current major dimension. It is visible only when both dimensions are positive. Rectangle
antialiasing is disabled.

The helper reads DPR from the separator's actual `QQuickWindow`. Before window association it reports DPR 1
and a safe zero offset. It reconnects when the item changes parent or window, when the window's screen or DPR
changes, and when any ancestor changes position, transform, or parent. Recalculation uses public
`QQuickItem::mapToScene()` and `QWindow`/`QQuickWindow` APIs.

### Why a small C++ helper is required

A QML binding that calls `mapToItem(null, 0, 0)` has no dependency on every ancestor's position and transform
signals. Moving an ancestor can therefore change the scene position without reevaluating the binding. Polling
with `FrameAnimation` would violate the static rendering-cost requirement. The internal observer supplies the
missing reactive value without private Qt APIs, a custom renderer, or per-frame work; rendering remains QML.

## 5. Gradient and alpha composition

One internal `Rectangle` owns a constant three-stop `Gradient`. Horizontal gradients run left to right and
vertical gradients top to bottom. Layout mirroring is not consulted, so start and end retain geometric
meaning.

The profiles resolve to:

| Mode | Start | Center | End |
|---|---:|---:|---:|
| `Solid` | center | center | center |
| `FadeBoth` | edge | center | edge |
| `FadeStart` | edge | center | center |
| `FadeEnd` | center | center | edge |

`centerOpacity` and `edgeOpacity` are clamped internally to `[0, 1]`. Each stop retains the supplied color's
RGB channels and uses `color.a * effectiveStopOpacity` as alpha. The separator root's inherited `opacity`
continues to compose after the leaf color without special handling.

## 6. Packaging and integration

`HnSeparator.qml` joins `HOLONIGHT_CONTROLS_PUBLIC_QML_FILES`, which makes the type available from the build
module, generated type metadata, installation list, and public-type inventory. The internal C++ helper joins
the `holonight_controls_qml` sources and is registered as an uncreatable or anonymous implementation detail
used only by `HnSeparator`.

`HnSectionHeader` replaces its raw divider `Rectangle` with `HnSeparator`, keeps
`color: HoloniightPalette.borderPassive`, and preserves its existing `dividerVisible`, fill-width, and
preferred-height behavior. This is the narrow integration proof; other shared controls remain unchanged.

## 7. Verification

QML smoke tests cover:

- source-tree and installed-tree type availability;
- default and overridden public properties;
- invalid orientation and fade-mode fallback;
- horizontal and vertical implicit sizing and runtime orientation changes;
- all four gradient profiles and alpha composition;
- opacity clamping;
- zero and negative thickness;
- zero major-axis length and pre-window construction;
- accessibility exclusion; and
- `HnSectionHeader` divider visibility and passive-color preservation.

The geometry helper receives deterministic unit coverage for DPR values `1.0`, `1.25`, `1.5`, `1.75`, and
`2.0` at multiple positive and fractional scene offsets in both orientations. Assertions verify integral
physical leading edges and equal requested physical thickness.

An offscreen rendering test first proves its sensitivity with an unsnapped one-logical-pixel rectangle at a
fractional DPR, then samples `HnSeparator`. If the offscreen backend cannot expose deterministic fractional
DPR output, the geometry test remains required and the task record captures a manual check on a real
fractional-scale display.

Verification order is:

1. build `holonight_qml_smoke_tests` and the controls QML lint target;
2. run focused `QmlSmoke.HnSeparator*` and integration tests offscreen;
3. run package-install coverage;
4. run `task test`; and
5. run `task lint` for the changed C++ and QML sources.

## 8. Risks and mitigations

| Risk | Mitigation |
|---|---|
| Ancestor movement leaves stale alignment | Observe the complete ancestor chain and rebuild connections after reparenting |
| Screen migration leaves stale DPR | Listen to window, screen, and DPR changes and recompute immediately |
| Rounded floating-point values produce unstable edges | Calculate from scene coordinates in double precision and assert integral physical results with a small tolerance |
| Snapping moves the painted edge slightly outside layout occupancy | Keep the root unclipped and separate layout occupancy from internal paint geometry |
| Gradient alpha replaces existing color alpha | Multiply stop opacity by source alpha |
| Public enum names become difficult to change | Expose only the four behavior-based names required by the specification |
| Internal helper leaks into consumer API | Keep registration and headers private to the QML plugin target |

## 9. Deferred decisions

- Migration of remaining divider rectangles in shared controls.
- Adoption in `holonight-shell` and `holonight-ai`.
- Compatibility aliases for `ContentSeparator`.
- Specialized behavior for rotated, sheared, or non-axis-aligned separators; the public contract covers
  horizontal and vertical separators in ordinary translated/scaled UI trees.
