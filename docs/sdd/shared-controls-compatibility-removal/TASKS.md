# TASKS: Shared-controls compatibility removal

- [x] Confirm the shell, AI, and package-manager consumers use canonical Core and Controls imports.
- [x] Record the final single-owner module contracts and intentional compatibility break.
- [x] Make every styled `Holonight` component import its Core dependencies explicitly.
- [x] Stop re-exporting `Holonight.Core` through the `Holonight` module.
- [x] Remove `HnSurfaceFrame` and `HnApplicationWindow` from the `Holonight` source, metadata, and install surface.
- [x] Remove the lowercase `holonight` build-tree mirror, `qmldir`, copied sources, and install tree.
- [x] Delete the lowercase `HnIcon` compatibility subtype.
- [x] Update Controls internals to resolve primitives directly from `Holonight.Core`.
- [x] Move legacy smoke coverage to canonical imports and remove compatibility-only assertions.
- [x] Extend install validation for canonical ownership and absence of compatibility artifacts.
- [x] Update shared-controls, frame, color, and consumer-facing import documentation.
- [x] Run all three QML lint targets and focused ownership smoke tests.
- [x] Run package-install validation and the complete headless CTest suite.
- [x] Launch the controls gallery at scale factors 1.0 and 1.25 and record completion evidence.

## Completion evidence

- `holonight_core_qml_qmllint`, `holonight_qml_qmllint`, and `holonight_controls_qml_qmllint` completed; the
  existing `ComboBox.qml` `Window` attached-property warnings remain unchanged.
- Canonical Core/Controls ownership smoke tests passed, including rejection of shared types through `Holonight`.
- Install-tree validation passed with canonical artifacts present, legacy style exports absent, and no lowercase
  `holonight` directory.
- Complete offscreen CTest passed: 16/16 tests.
- The offscreen controls gallery remained running until the three-second smoke timeout at scale factors 1.0 and
  1.25, with no startup errors.
