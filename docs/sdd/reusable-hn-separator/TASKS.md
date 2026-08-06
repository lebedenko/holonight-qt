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
