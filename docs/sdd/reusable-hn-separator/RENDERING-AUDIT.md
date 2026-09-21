# HnSeparator rendering investigation

Date: 2026-09-21. Provider: `9723cef7371a4ca5f2967d869b26ee7ff1e3c789`.
This is an investigation, not a production correction or acceptance of application usage.
Production code, public APIs, consumers, installation, commits and umbrella pins are unchanged.

## Findings

1. **Confirmed software rendering defect:** correct one-physical-pixel item geometry does not guarantee
   one-pixel Rectangle painting. At DPR 1.5 the current separator paints one or two pixels depending on
   position; at 1.75 and 2 it paints two. A plain solid Rectangle with identical aligned geometry has the
   same problem. Removing the gradient or enabling Rectangle antialiasing does not fix the reduced case.
2. **Confirmed transform limitation:** the helper treats scene-coordinate deltas as local deltas and ignores
   the scale of the minor-axis basis. Ancestor scale 1.5 produces 1.5-physical-pixel bounds and one/two painted
   pixels on OpenGL; scale 2 produces two-pixel bounds and painting. Rotation is observed but not handled
   geometrically. Arbitrarily rotated strokes cannot satisfy an axis-aligned one-row/column guarantee.
3. **Confirmed observer gaps:** explicit `Translate` changes do not refresh alignment. Changing the dimensions
   of a scaled, center-origin ancestor changes its scene transform without refreshing the helper. The current
   helper observes neither width/height nor arbitrary `QQuickTransform` changes.
4. **No fade-profile defect reproduced:** direction, both-end symmetry, color alpha, center/edge opacity, and
   root opacity agree with the expected profiles within one 8-bit channel value in the unscaled matrix.
5. **Clipping can remove strokes:** paint bounds may extend outside occupancy. The clipped-edge stress fixture
   contains disappearing strokes, including deliberately out-of-parent placements. These are not evidence of
   an independent gradient defect or a promise that out-of-bounds painting should survive a clip.

## Environment and method

- Qt 6.11.2, provider Debug build with `BUILD_TESTS=ON`; build-tree `Holonight` modules supplied explicitly to
  `QQmlEngine::addImportPath`. Rebuilt `holonight_qml_smoke_tests` before final baseline verification.
- Software: `QSGRendererInterface::Software` (1), `QT_QPA_PLATFORM=offscreen`, `QT_QUICK_BACKEND=software`,
  `QQuickView::grabWindow()`.
- Accelerated: `QSGRendererInterface::OpenGL` (3), actual log reports **Mesa Intel(R) Graphics (RPL-S)**,
  OpenGL 4.6 compatibility profile, Mesa 26.2.3-arch1.1. `QQuickRenderControl`, OpenGL context and offscreen
  surface, explicit FBO sized to logical size times DPR, `QQuickRenderTarget::setDevicePixelRatio`.
  This is actual GPU coverage, not a software fallback. GPU runs required access outside the sandbox.
- Each backend/DPR/scenario/orientation/mode combination ran in a separate process. Both window DPR and
  effective DPR, image dimensions, graphics API, Qt version and scene-space painted bounds are saved in JSON.
- Software and OpenGL: 215 process configurations each, three captures each (1,290 accepted PNG captures).
  The analyzer records 131,190 line observations, including controls and runtime phases.
- Capture every 180 ms; phases 0, 1 and 2 allow before/after runtime changes. GPU capture explicitly polishes,
  synchronizes and renders each phase. No pointer, keyboard or window-focus automation was used.

An initial OpenGL `QQuickView::grabWindow()` attempt produced a DPR-dependent framebuffer/viewport mismatch
(transparent top area and incomplete content). Those `opengl-*` artifacts are **excluded** by the analyzer;
`gpu-*` are the accepted explicit-render-target captures. Changing the render loop alone did not fix that
capture problem. Sandboxed OpenGL context creation failed; offscreen Vulkan initialization also failed and
is not counted as coverage. Neither failed attempt diagnoses HnSeparator.

## Matrix

Every row below uses both orientations, both accepted backends and DPR **1, 1.25, 1.5, 1.75, 2**.

| Fixture | Modes | Variations |
|---|---|---|
| `simple` | All four | Unclipped, unscaled; offset sweep |
| `anchors` | Solid | Major length supplied by anchors |
| `layout` | Solid | ColumnLayout owns geometry; both orientations |
| `clip` | All four | Parent clipped to nominal 120-by-1 or 1-by-120 occupancy; offset sweep crosses edges |
| `move` | Solid | Ancestor minor translation 0, .23, .46 logical units |
| `scale` | Solid | Fixed ancestor scale 1.25, top-left origin |
| `scalechange` | Solid | Ancestor scale 1, 1.5, 2, top-left origin |
| `resize` | Solid | Scale 1.25, center origin; ancestor dimensions 120, 120.7, 121.4 |
| `transform` | Solid | Explicit Translate minor translation 0, .23, .46 |
| `reparent` | Solid | Reparent into another item at minor offset .37 |
| `mode` | Runtime Solid/Both/Start | Property changes between captures; static matrix also covers End |
| `normalized` | All four | Same scene bounds; aligned controls use unit minor size plus minor-axis Scale |
| `reduced` | Solid/constant gradient | Five isolated Rectangle painting alternatives |

For the repeated-line fixtures the unique offset set is:

```text
-1, -.75, -.5, -.25, 0, .125, .25, .375, .5, .625, .75, .875, 1
plus (k + .5) / DPR + epsilon, k in {-1,0,1}, epsilon in {-.001,0,.001}
```

Negative values in the image matrix are local offsets inside a positively positioned cell. A separate
`observer.cpp` probe checks genuinely negative scene positions, including a negative half-pixel boundary.
At DPR 1.5, window attach/detach/reattach changes reported DPR 1 → 1.5 → 1 → 1.5 and thickness accordingly.
This checks association with two windows on the same screen, **not** a native screen transition.

Five parallel controls share color `#80ffffff`, root opacity .8, center opacity .9, edge opacity .1 and
major length 120: HnSeparator, unsnapped solid, unsnapped gradient, aligned solid, aligned gradient.
Horizontal screenshots show those columns left to right; vertical screenshots transpose the grid.
Solid controls intentionally stay solid when gradient controls change fade mode. Aligned controls use the
same geometry helper as the component. Each control has its own helper, so comparison does not assume that
separate scene positions happen to share the same fractional pixel origin.

## Physical-pixel measurements

The analyzer samples transverse profiles at 10%, 25%, 50%, 75%, 90% of the major axis. Coverage counts pixels
more than 3 channel values above the `#202020` background. Stroke width uses the emphasized sample (50% for
Solid/Both, 90% for Start, 10% for End), never the transparent endpoint. Sample windows include three adjacent
pixels on each side of the reported bounds, exposing unintended adjacent-pixel coverage.

For simple, unscaled fixtures, observed physical coverage is:

| DPR | Current, software | Current, OpenGL | Raw solid, software | Raw solid, OpenGL | Normalized aligned controls, both backends |
|---:|---:|---:|---:|---:|---:|
| 1 | 1 | 1 | 1 | 1 | 1 |
| 1.25 | 1 | 1 | 1–2 | 1–2 | 1 |
| 1.5 | 1–2 | 1 | 1–2 | 1–2 | 1 |
| 1.75 | 2 | 1 | 1–2 | 1–2 | 1 |
| 2 | 2 | 1 | 2 | 1–2 | 1 |

All current component scene bounds in the simple matrix have integral physical leading edges and thickness
1. No simple current-component stroke disappears. The raw controls demonstrate sensitivity to the original
unsnapped-rectangle regression. The component and matching aligned-gradient control have equal coverage;
their sampled profiles differ by at most 1 channel value.

Expected pixel-channel delta is `223 * (128/255) * .8 * profileOpacity`. Use constant .9 for Solid,
`.1 + .8*(1-abs(2*t-1))` for Both, `.1+.8*t` for Start, and `.9-.8*t` for End. With a tolerance of 3 channel
values for 8-bit composition, sample-center placement and rasterization, the maximum measured error was
**0.8811** across simple and normalized gradient fixtures. This validates the sampled profiles and their
symmetry/direction, not every possible color, opacity, or subpixel-length combination.

Clipped fixtures have zero-coverage cases on both backends. They deliberately move the separator across the
parent's edges, so aggregate disappearance counts are not defect counts. For example, at DPR 1 a one-unit
clip and separator at local minor coordinate .5 snap the painted leading edge to 1, outside the clip. The
unclipped component does not protect its painting from a clipped ancestor.

## Smallest rendering reduction and candidate

The essential software failure is independent of HoloNight:

```qml
Rectangle {
    x: 20; y: 20
    width: 120; height: 0.5 // one physical pixel at DPR 2
    color: "#80ffffff"
    antialiasing: false
}
```

It paints two software pixel rows but one OpenGL row. Adding a constant horizontal gradient or setting
`antialiasing: true` still produces two software rows in the reduction. Keep the same scene-space bounds but
change only how the minor dimension is represented:

```qml
Rectangle {
    x: 20; y: 20
    width: 120; height: 1
    color: "#80ffffff"
    transform: Scale { yScale: 0.5 }
}
```

This paints one row on both backends. Constant gradients also pass. The full normalized diagnostic then
retains the original helper offset, sets the local minor dimension to 1 and uses `paintedThickness` as the
minor-axis scale; all offsets, orientations and fade modes pass at all five DPRs. Minor floating-point
transform error reaches approximately 0.000046 physical pixels without causing extra coverage.

**Recommendation:** first implement and regression-test this narrowly scoped internal painting change,
retaining the existing gradient stops, public sizing and API. It is a verified candidate for unscaled
ancestors, not a production-ready claim for arbitrary transforms, clipping, or all thickness values.
The experiments identify a Rectangle software-rendering behavior; they do not establish the precise Qt
internal cause or justify relying on a particular private implementation.

Separately correct axis-aligned transform handling: measure the mapped minor-axis basis, divide physical
thickness by DPR times its magnitude, and convert the snapped scene displacement back into local coordinates.
Account for negative scale and zero scale explicitly. Add size and relevant transform invalidation. Determine
an explicit policy for rotation/shear, where the existing axis-aligned physical-row promise is insufficient.
Do not solve clipping by silently changing occupancy or fading behavior; define placement/containment policy
first if clipped-edge support is required.

## Implementation audit

- `HnSeparator.qml` preserves caller-controlled explicit dimensions; implicit minor occupancy is
  `max(0, thickness)`. Default `HnMetrics.separatorWidth` is 1. Painting can be thinner than occupancy.
- `separatorAlignment()` computes `max(1, round(thickness/standardThickness))/max(1,DPR)` for valid positive
  thickness. It rounds the scene leading edge with `std::round` (ties away from zero), and returns a signed
  offset up to half a physical pixel. That can be negative or extend paint beyond the trailing layout edge.
- The helper maps only the origin to scene space. Adding a scene displacement directly to a local Rectangle
  coordinate is correct for translations, not a scaled/rotated basis; dividing only by DPR misses ancestor scale.
- The complete parent chain observes x/y, rotation, scale, transform origin and parent changes. Translation
  and reparent fixtures refresh correctly. Window changes rebuild connections. Width/height and arbitrary
  transform-object changes have no corresponding observation; resize and Translate fixtures expose stale state.
- Window screen changes and screen logical-DPI changes trigger updates. There is no explicit handling of
  `QEvent::DevicePixelRatioChange` or use of `effectiveDevicePixelRatio()` in the helper. Whether an actual
  platform DPR transition without those signals goes stale remains unverified. The accepted render target
  deliberately uses the same DPR as the window; alternate render-target DPR is not covered.
- Before window association, DPR is 1 but the offset is not necessarily zero: negative scene y -.3 gives
  offset +.3. This differs from the design document's "safe zero offset" wording; no invalid geometry resulted.
- Stops run left-to-right or top-to-bottom, not by layout direction. Color alpha multiplies clamped stop
  opacity; inherited Item opacity composes normally. Runtime mode changes repaint. The one-sided midpoint is
  the arithmetic mean of center and edge, producing a linear ramp across the entire length.
- The DESIGN profile table incorrectly describes one-sided middle stops as full center opacity. The code and
  observed linear profiles agree with SPEC's endpoint requirements. DESIGN also predates the HnMetrics split.
- Installed Qt Basic ToolSeparator/MenuSeparator use ordinary logical-size Rectangle content (1 or 2 for
  high contrast), with padding. Fusion ToolSeparator combines a two-unit rectangle with a one-unit child;
  Fusion MenuSeparator uses a one-unit rectangle. Local HoloNight ToolSeparator/MenuSeparator also use
  logical-size Rectangles and padding with semantic colors. None implements this physical-grid helper or
  the four fading profiles; they are references, not evidence of equivalent guarantees.

## Regression cases needed before a production fix

- Process-isolated rendered tests at all five DPRs, both orientations and backends; assert actual API and DPR.
  Include raw controls that fail the intended physical-width assertion at fractional DPR.
- Aligned solid versus constant-gradient rectangles, current component versus normalized candidate; assert
  one-pixel coverage and zero adjacent-pixel coverage at emphasized portions, plus sampled alpha profiles.
- Negative offsets, half-pixel boundaries on both sides, semantic thickness >1 and nonpositive thickness,
  runtime orientation/mode changes, and nonintegral major lengths.
- Ancestor movement, scale, center-origin resizing, explicit Translate/Scale changes, reparenting and window
  attachment; verify scene-space bounds as well as raster output.
- Clipped leading/trailing boundaries under an agreed containment policy. Native DPR transitions and target-DPR
  overrides need dedicated tests; rotation/shear should be covered only once their supported contract is defined.

The existing four `QmlSmoke.HnSeparator*` cases test properties, stops, a no-window translation and arithmetic
alignment. They never capture an image. All four pass before and after this investigation and therefore miss
both the software painting defect and transform failures described here.

## Evidence and reproduction

Build artifacts: [`build/separator-investigation`](../../../build/separator-investigation/).
The fixture generator and both capture harnesses remain there as requested; a build-directory cleanup removes
them. Durable selected captures are included below, and the minimal reduction above is self-contained.

- `run.py`: full configuration/offset generator and process launcher; reuse existing completed captures.
- `main.cpp` / `view-capture.cpp`: software view capture. `rendercontrol.cpp`: explicit OpenGL render target.
- `reduce.py`: isolated Rectangle alternatives. `observer.cpp`, `observer.jsonl`: window/negative-coordinate probe.
- `measure.py`, `measurements.json`, `summary.json`, `summary.txt`: per-line and grouped physical coverage.
- `analyze.py`, `analysis.json`: profile tolerance, candidate coverage and control-comparison assertions.
- `<backend>-<DPR>-<scenario>-<h|v>-<mode>.qml/.log` and `-<phase>.png/.json`: complete source,
  backend logs, rendered images and settled geometry. Accepted backend prefixes are `software` and `gpu`.

Commands from the umbrella root (compiler and runtime dependencies were already available; no installation):

```sh
cmake --build holonight-qt/build --target holonight_qml_smoke_tests -j4
QT_QPA_PLATFORM=offscreen holonight-qt/build/tests/holonight_qml_smoke_tests --gtest_filter='QmlSmoke.HnSeparator*'
c++ -std=c++17 -fPIC holonight-qt/build/separator-investigation/main.cpp -o holonight-qt/build/separator-investigation/capture $(pkg-config --cflags --libs Qt6Quick)
c++ -std=c++17 -fPIC holonight-qt/build/separator-investigation/rendercontrol.cpp -o holonight-qt/build/separator-investigation/capture-rhi $(pkg-config --cflags --libs Qt6Quick Qt6OpenGL)
python3 holonight-qt/build/separator-investigation/run.py software
python3 holonight-qt/build/separator-investigation/run.py gpu
python3 holonight-qt/build/separator-investigation/reduce.py
python3 holonight-qt/build/separator-investigation/measure.py
python3 holonight-qt/build/separator-investigation/analyze.py
c++ -std=c++17 -fPIC holonight-qt/build/separator-investigation/observer.cpp -o holonight-qt/build/separator-investigation/observer $(pkg-config --cflags --libs Qt6Quick)
QT_QPA_PLATFORM=offscreen QT_QUICK_BACKEND=software QT_SCALE_FACTOR=1.5 holonight-qt/build/separator-investigation/observer "$PWD/holonight-qt/build/qml"
```

The GPU commands require the same approved GPU/context access as this investigation. The generator sets
`QT_SCALE_FACTOR`, backend variables and offscreen platform per process. Final harness compilation has no
warnings; accepted configuration logs contain no warning/error/binding-loop diagnostics. Baseline: 4/4 pass.
No full application build/test or CI run was needed for this diagnostic/documentation-only activity.

Durable numerical evidence: [grouped measurements](evidence/measurement-summary.csv),
[analysis results](evidence/analysis.json), and [fixture source hashes](evidence/fixture-manifest.json).

Selected captures (original pixels, no rescaling):

| Capture | Interpretation |
|---|---|
| [Software DPR 2 reduction](evidence/software-dpr2-reduced.png) | Rows: plain, AA enabled, constant gradient, unit rectangle with Scale, same with gradient. First three paint two rows; last two paint one. |
| [OpenGL DPR 2 reduction](evidence/opengl-dpr2-reduced.png) | Same five alternatives all paint one row. |
| [Software DPR 1.5 FadeBoth](evidence/software-dpr1.5-fade-both.png) | Current and aligned direct-size controls show unequal thickness; unsnapped controls also expose the original problem. |
| [Software DPR 1.5 normalized FadeBoth](evidence/software-dpr1.5-normalized-fade-both.png) | Last two columns use the normalized candidate, with uniform one-pixel coverage and preserved fade. |

Remaining limits: native cross-screen DPR migration, hardware other than this Intel OpenGL device, other Qt
versions/backends, arbitrary rotation/shear, alternate target DPR, non-default requested thickness rendering,
and application composition. No result here certifies any application's use of the component.
