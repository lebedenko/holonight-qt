# SDD Tasks — semantic-surface-frames

Implement the first shared surface-frame foundation for HoloNight QML consumers. Requirements refer to
`docs/sdd/semantic-surface-frames/SPEC.md`.

- [x] T-001: Add shared primitive shape tokens, enums, corner masks, and `ShapeResolver`
  - REQs: REQ-F-001, REQ-F-002, REQ-F-003
  - Check: The theme library exposes one typed role-to-shape resolver with deterministic primitive sizes,
    override precedence, Pill handling, and dimension clamping.

- [x] T-002: Add QML enum namespaces and the `HnShapeProfile` adapter
  - REQs: REQ-F-003, REQ-F-005
  - Check: QML enums remain aligned with shared enums; unknown values fall back to Card/Inherit; the singleton
    delegates all geometry resolution to shared C++.

- [x] T-003: Add `HnSurfaceFrame.qml` with the specified public API and content container
  - REQs: REQ-F-004, REQ-C-002, REQ-C-004
  - Check: Component uses scene-graph Shape primitives, exposes observable effective geometry, and parents
    consumer content through its default property.

- [x] T-004: Implement rounded, chamfered, and Hybrid paths with safe clamping
  - REQs: REQ-F-004, REQ-F-005
  - Check: Border remains inside bounds; zero, small, and fractional dimensions create valid paths without QML
    warnings or negative effective geometry.

- [x] T-005: Require Qt Quick Shapes and register/install the new types in the canonical QML module
  - REQs: REQ-F-006, REQ-C-003
  - Check: CMake requires the first-party Quick Shapes component; `qt_add_qml_module` includes all C++ and
    QML sources, declares the QML dependency, and generated type metadata exposes the API.

- [x] T-006: Update the lowercase compatibility module copy/install list
  - REQs: REQ-F-006
  - Check: `import holonight` instantiates `HnSurfaceFrame` and accesses enum namespaces and the resolver.

- [x] T-007: Add exact resolver and API tests to `tests/test_qml_smoke.cpp`
  - REQs: REQ-F-001, REQ-F-002, REQ-F-003, REQ-F-004
  - Check: Tests cover defaults, role selection, explicit style, override precedence, NaN inheritance, palette
    defaults, content parenting, and invalid enum fallback.

- [x] T-008: Add role/style matrix and geometry edge-case tests
  - REQs: REQ-F-005
  - Check: All ten roles instantiate with all three concrete styles; zero size, undersized surfaces, Pill
    heights, and fractional border widths produce finite non-negative values and no creation errors.

- [x] T-009: Build and run focused verification
  - REQs: REQ-F-005, REQ-F-006, REQ-C-004
  - Check: Build `holonight_qml_smoke_tests`, then run `QmlSmoke.HnSurface*` and
    `QmlSmoke.HnShapeProfile*` with `QT_QPA_PLATFORM=offscreen`.

- [x] T-010: Run complete QML verification and lint applicable files
  - REQs: REQ-C-002, REQ-C-003, REQ-C-004
  - Check: Complete `holonight_qml_smoke_tests` passes offscreen and available QML/C++ lint checks report no
    findings in changed files.

- [x] T-011: Review public API before downstream adoption
  - REQs: REQ-C-001, REQ-C-005
  - Check: Diff contains no shell/chat dependencies or persistence code; API is documented well enough for a
    separate `holonight-shell` migration SDD.

## Follow-up cycles (not tasks in this SDD)

- `shared-appearance-config`: `HnAppearance`, JSON schema, XDG path, directory watching, and live changes.
- `application-window-frame`: shared application title bar and client-content surface.
- `holonight-style-shapes`: migrate appropriate Widgets paths/constants to the shared resolver.
- `holonight-shell` SDD: wrap `HnSurfaceFrame` from `HudFrame.qml` while retaining shell-only behavior.
