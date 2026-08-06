# SDD Tasks — shared-list-action-controls

- [x] T-001: Add and package the four shared action icons
  - REQs: REQ-F-001, REQ-I-001
- [x] T-002: Implement and register `HnIconButton`
  - REQs: REQ-F-002, REQ-I-001
- [x] T-003: Extend the HoloNight `MenuItem` style
  - REQs: REQ-F-003
- [x] T-004: Add optional elided-title tooltips to `HnNavigationDelegate`
  - REQs: REQ-F-004
- [x] T-005: Add focused source-tree and install-tree contract tests
  - REQs: REQ-V-001, REQ-I-001
- [x] T-006: Extend the controls gallery and shared-controls usage guide
  - REQs: REQ-I-002
- [x] T-007: Run focused and full verification and record evidence
  - REQs: REQ-V-001

## Verification evidence

- `cmake --build build --target holonight_qml_smoke_tests holonight_controls_qml_qmllint
  holonight_qml_qmllint holonight_controls_gallery -j4`
- Focused `QmlSmoke` filter for the new menu, icon-button, and navigation-tooltip contracts: 3/3 passed.
- `ctest --test-dir build --output-on-failure -R holonight_package_install_test`: passed.
- `task build`: passed.
- `task test`: all 16 CTest tests passed.
- `git diff --check`: passed.
- Controls-gallery inspection at `QT_SCALE_FACTOR=1.25`: icon sizing, mixed menu alignment, and button-anchored
  menu placement verified.

`holonight_qml_qmllint` retains two pre-existing `ComboBox.qml` warnings about attached `Window` lookup; no
changed QML file reports a warning.
