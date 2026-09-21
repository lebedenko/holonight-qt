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
