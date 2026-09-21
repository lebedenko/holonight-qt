# SDD Tasks — reusable-hn-separator

Implement the reusable, device-pixel-aligned separator specified by
`docs/sdd/reusable-hn-separator/SPEC.md`.

- [x] T-001: Add deterministic separator alignment geometry
  - REQs: REQ-F-005, REQ-F-006, REQ-F-010, REQ-NF-002
  - Check: Requested semantic thickness resolves to a stable physical-pixel count, and the leading scene
    coordinate snaps to the physical grid at DPR 1.0, 1.25, 1.5, 1.75, and 2.0.

- [x] T-002: Add reactive window and ancestor observation
  - REQs: REQ-F-006, REQ-F-010, REQ-NF-001, REQ-NF-003
  - Check: The helper recomputes after parent, ancestor transform, window, screen, and logical-DPI changes
    without polling or private Qt APIs.

- [x] T-003: Add the public `HnSeparator` QML component
  - REQs: REQ-F-001 through REQ-F-010
  - Check: The type supports both orientations, layout-owned length, semantic color and thickness, four fade
    profiles, clamped opacity, safe invalid-value fallback, and accessibility exclusion.

- [x] T-004: Register and install `HnSeparator`
  - REQs: REQ-F-001, REQ-V-001, REQ-V-003
  - Check: Build-tree metadata, installed QML files, installed module metadata, and the public type inventory
    expose `HnSeparator`.

- [x] T-005: Migrate one shared control
  - REQs: REQ-I-001, REQ-I-002
  - Check: `HnSectionHeader` uses `HnSeparator` while retaining `dividerVisible`, full-width layout, and
    `borderPassive`.

- [x] T-006: Add separator gallery examples
  - REQs: REQ-V-004
  - Check: The controls gallery builds with all fade modes, both orientations, subtle/passive colors, and
    default/increased thickness.

- [x] T-007: Add focused API and geometry tests
  - REQs: REQ-V-001, REQ-V-002
  - Check: Tests cover defaults, runtime orientation, invalid values, fades, alpha composition, opacity
    clamping, accessibility, zero/negative thickness, zero-length visibility, ancestor movement, and the full
    DPR/offset matrix.

- [x] T-008: Run focused and complete verification
  - REQs: REQ-V-003
  - Check: `holonight_controls_qml_qmllint`, focused separator tests, all QML smoke tests, install-tree checks,
    the controls gallery build, and all 16 CTest tests pass.

## Verification evidence

- `cmake --build build --target holonight_qml_smoke_tests holonight_controls_qml_qmllint -j4`
- `QT_QPA_PLATFORM=offscreen build/tests/holonight_qml_smoke_tests`
  `--gtest_filter='QmlSmoke.HnSeparator*:QmlSmoke.Controls_SectionHeaderCollapsesAndOwnsSlots'`
- `ctest --test-dir build --output-on-failure -R 'holonight_package_install_test|holonight_qml_smoke_tests'`
- `cmake --build build --target holonight_controls_gallery -j4`
- `task test`
- `task lint` completed successfully; it continues to report the repository's existing advisory warnings.

The deterministic geometry fallback is used because the offscreen backend does not provide independently
selectable fractional-DPR screens in one test process. The real-display fractional-scale visual check was
completed after the automated verification.

## Standalone rendering investigation — 2026-09-21

- [x] T-009: Audit implementation and installed Qt separator references without consumer changes.
- [x] T-010: Build isolated software/OpenGL reproducer; capture all five DPRs, orientations and fade modes,
  position sweeps, layout/clip/transform scenarios and runtime changes.
- [x] T-011: Measure physical coverage and alpha profiles, demonstrate raw-Rectangle sensitivity, reduce
  failures and verify a candidate painting correction without modifying production behavior.
- [x] T-012: Record findings, captures, commands and remaining verification gaps in
  [RENDERING-AUDIT.md](RENDERING-AUDIT.md).

Result: confirmed software Rectangle coverage and ancestor-transform issues despite all four existing smoke
checks passing. A unit-minor-size Rectangle with minor-axis Scale preserves fades and passes the unscaled
rendered matrix on both backends. Production correction and regression-test implementation remain separate
work; this investigation does not reopen or claim new application integration acceptance.

## Software painting correction — 2026-09-21

- [x] T-013: Apply the audit's unit-minor-size Rectangle and minor-axis Scale correction, preserving
  public sizing, semantic colors, gradient stops and nonpositive-thickness visibility.
- [x] T-014: Add process-isolated pixel regression tests at DPR 1, 1.25, 1.5, 1.75 and 2. Cover both
  orientations, all fades, alpha composition, runtime changes, ancestor translation, nonintegral major
  lengths, thicknesses 1–3 and nonpositive thickness; retain raw Rectangle sensitivity controls.
  The test failed before the fix at DPR 2 with two painted rows instead of one; all five DPRs pass after it.
- [x] T-015: Update smoke geometry assertions to inspect transformed scene bounds and correct the design's
  painting description, one-sided midpoint profiles and observer limitations.

This correction addresses software painting in translated, unscaled trees. Axis-aligned ancestor scale,
explicit transform invalidation, native DPR transitions and clipped-edge containment remain follow-up work
from the audit. Accelerated rendering was not rerun for this correction; the audit contains prior candidate
coverage on OpenGL. No consumer migration is included.

Verification: full `cmake --build build -j4`; `holonight_controls_qml_qmllint` (existing unrelated advisory
warnings); focused rendering and QML smoke CTest run (6/6); `task test` (96/96, including package installation,
QML policy and example startup checks); `git diff --check`. `task test` required execution outside the
sandbox because its dependency build writes to the sibling `holonight-config/build-provider` directory.

## Ancestor scaling correction — 2026-09-21

- [x] T-016: Compensate axis-aligned signed minor-axis scale, including nested built-in `Item.scale`.
  Snap the local-origin edge and keep local painted thickness positive under mirroring. Suppress painting
  for zero scale/nonfinite geometry and restore it when geometry becomes valid. Preserve the public API,
  layout occupancy, fade profiles and unit-minor-size Rectangle correction.
- [x] T-017: Observe ancestor width/height changes using the existing connection lifecycle, so resizing
  around a transform origin updates alignment without polling.
- [x] T-018: Add deterministic DPR/scale/offset tests and software pixel coverage for both orientations,
  thicknesses 1–3, every fade mode, mirrored gradients, runtime scale changes, nested products, zero-scale
  recovery, origin changes, resizing and reparenting. Expected pixel bounds are computed from the public
  separator root transform and requested thickness, independently of helper output.
  - Before implementation, `SeparatorRendering.SignedAncestorScaleAndLifecycle` failed at DPR 1.25:
    scale 0.5 painted zero pixels where one was requested (expected red ~113, actual background red 32).
  - Before implementation, `QmlSmoke.HnSeparator_CenterOriginResizeUpdatesAlignment` failed with total
    nested scale 1: resizing to 81.3 left the edge at ~10.325 instead of physical coordinate 10.
  - Both regressions pass after implementation. Deterministic cases include DPR 1/1.25/1.5/1.75/2,
    scales ±0.5/±1/±1.25/±2, negative scene coordinates and exact/adjacent physical half-pixels.
    Invalid geometry produces finite zero outputs; a very large scale also tests zero visibility transitions.
- [x] T-019: Update design and usage guidance with the supported signed-scale contract and limitations.

Explicit transform-object invalidation, rotation/shear alignment guarantees, clipping containment, native
DPR transitions and consumer migration remain separate follow-ups. Durable OpenGL verification is the next
validation follow-up; these new rendering regressions use the software backend.

Verification: focused software rendering at all five DPRs plus the full QML smoke suite (6/6 CTest
entries); `holonight_controls_qml_qmllint` (existing unrelated advisory warnings); full
`cmake --build build -j4`; `task test` (96/96, including package installation, QML policy and example
startup checks); `git diff --check`. The task workflow required execution outside the sandbox because
its dependency build writes to the sibling `holonight-config/build-provider` directory.

## Default minor-axis occupancy correction — 2026-09-21

- [x] T-020: Bind the actual horizontal `height` and vertical `width` defaults to the non-negative
  requested thickness, while retaining implicit sizes and caller ownership of explicit dimensions.
- [x] T-021: Add anchor-only root-geometry and rendered-line coverage to QML smoke and physical-pixel
  rendering tests across every existing DPR process.
- [x] T-022: Add Files-window integration assertions for the header, sidebar, directory-header and
  footer dividers using stable separator object names.

Verification:

- `ctest --test-dir build --output-on-failure -R '^holonight_separator_rendering_dpr_'` passed at DPR
  1.0, 1.25, 1.5, 1.75 and 2.0.
- `QT_QPA_PLATFORM=offscreen QML_IMPORT_PATH=<build>/qml build/tests/holonight_qml_smoke_tests
  --gtest_filter='QmlSmoke.HnSeparator*'` passed (8 tests).
- Files' `Files.WindowAnchorOnlySeparatorsOccupyAndPaint` focused smoke test passed against its real
  `Main` window; both changed-repository QML lint targets and Files' format check passed.


## Complete geometry contract and connected consumers — 2026-09-21

This accepted redesign supersedes the earlier API and the follow-up limitations listed above.

- [x] T-023: Replace offsets with a complete scene-space snapped rectangle; preserve signed built-in
  scale, event-driven ancestry/window updates, exact physical thickness and invalid-geometry recovery.
- [x] T-024: Introduce Leading/Center/Trailing, integer thickness=1, borderPassive and ordinary opacity;
  remove legacy opacity properties and migrate shared controls, gallery and usage documentation.
- [x] T-025: Render a unit rectangle scaled on both axes with the specified three-stop fade profiles.
- [x] T-026: Cover complete strokes and junctions on explicit software and accelerated OpenGL backends,
  DPR 1/1.25/1.5/1.5625/1.75/2, thickness 1–3, transparency, fractional translations, signed scaling,
  anchors/layouts, explicit slots, lifecycle changes and simulated render-target DPR transitions.
- [x] T-027: Reproduce implicit-size binding-loop warnings at DPR 1.5625, split occupancy notifications
  from paint notifications, and assert that no QML binding-loop warning occurs in rendered tests.
- [x] T-028: Explicitly rebuild/stage the working-tree provider into Files and the other consumer prefixes.

Verification: full build including demo/gallery; full default CTest suite 96/96 before the added DPR;
final affected CTest selection 16/16 (all twelve renderer/DPR processes, QML smoke, example startup and
package installation). Controls/gallery qmllint and policy checks pass with existing unrelated advisories.
Full clang-tidy was inspected; new helper/rendering-test diagnostics were corrected and focused tidy
passes. Formatting and diff checks pass. Hardware renderer: Mesa Intel(R) Graphics (RPL-S), Qt 6.11.2.

OpenGL tests use public QQuickRenderControl with an FBO so fractional offscreen backing-buffer sizes
cannot silently invalidate capture. They reject software fallback. This does not simulate a real monitor
move; the native user check and library identity are recorded in Files' local SDD. Arbitrary rotation,
shear and custom-transform-object updates remain outside the contract. The user subsequently authorized repository commits, publication and umbrella pin updates.
