# DESIGN: HnSeparator complete rectangle geometry

Status: Accepted redesign, 2026-09-21. See SPEC.md and the umbrella crisp-separator-joins initiative.

## Ownership

`qml/controls/HnSeparator.qml` owns the small public API and Qt rectangle/gradient composition.
`qml/hnseparatoralignment.h` computes the complete rectangle. `hnseparatorgeometry.{h,cpp}` observes
public Qt item/window signals. The C++ helper is module-internal, with no installed public header.

## Geometry

Map root origin and unit x/y axes to scene space. Reject non-axis-aligned/degenerate transforms.
Multiply scene origin, signed scales and slot extents by effective render-target DPR. Snap all boundaries
using floor(x + .5), which is invariant under integer translation, including across zero. Snap both major
endpoints. Leading selects the snapped local origin; Trailing selects the snapped local far edge;
Center snaps the unrounded desired leading edge. Extend by the signed physical pixel count and map back
to local coordinates. Local width/height stay positive under mirroring. Validate all results for finiteness.

Logical occupancy depends on physical thickness, effective DPR and absolute minor scale, independently
of the slot. Explicit dimensions and layouts own allocation. QML uses a 1-by-1 Rectangle plus a Scale
on both axes, avoiding the software rasterizer's fractional Rectangle extent rounding. Stop strengths
at 0/.5/1 are Solid 1/1/1, FadeStart 0/1/1, FadeEnd 1/1/0, FadeBoth 0/1/0. Alpha/opacity remain standard Qt.

## Synchronization

Observe the complete ancestor chain: x/y/width/height/rotation/scale/transformOrigin, parent changes.
Rebuild connections on attachment/reparenting/screen changes. Observe QQuickWindow's public Qt 6.11
`devicePixelRatioChanged` signal and screen logical-DPI notifications. Read effectiveDevicePixelRatio.
Logical occupancy has a separate notification from the painted rectangle. Slot changes must not
re-notify implicitWidth/implicitHeight while their bindings are updating actual dimensions. This prevents
the binding loops reproduced at DPR 1.5625. No frame callback or private API is used.

Qt 6.11's setRenderTarget does not itself notify DPR changes. A custom redirected-rendering host that
changes target DPR must deliver QEvent::DevicePixelRatioChange on its GUI thread; the test host does this
explicitly. Native windows receive that event from their platform. See
[Qt's implementation](https://github.com/qt/qtdeclarative/blob/6.11/src/quick/items/qquickwindow.cpp).
This is a simulated platform event, not evidence of a physical monitor transition.

## Composition and clipping

Header and right-hand sidebar strokes use Trailing, footer Leading. Column rules end at the bottom
rule's leading logical boundary. Translation-invariant snapping makes subtracting an exact physical
stroke width yield the same endpoint on both rules. No intersecting translucent paint is duplicated.

Rendered testing found that tight fractional clips can discard a correctly snapped edge on both
backends; replacing the rectangle renderer cannot override an ancestor clip. Files' column header
therefore relies on label elision and leaves boundary painting unclipped. Viewport clipping remains.
Arbitrary rotation/shear/custom-transform updates are outside the contract.

## Tests

Smoke tests cover API, layout defaults, removed properties, events and deterministic geometry with
negative coordinates. Pixel tests share fixtures between a software QQuickWindow and an OpenGL
QQuickRenderControl with an explicit FBO and DPR. Full stroke bounds and adjacent pixels are tested;
connected-boundary fixtures compare complete regions with half-alpha lines to detect double coverage.
Accelerated CTest registration is opt-in (`HOLONIGHT_ACCELERATED_SEPARATOR_TESTS=ON`) and never silently
falls back. Files uses its actual application window for the consumer regression and narrow-column states.
