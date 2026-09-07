# UQC-101 — provider implementation

Status: In Progress. Assigned repository: holonight-qt.
Exact published upstream baseline: `033d6001fd088a96ac6e4ff936b6bafcf6ab5d4c` (origin/main, checked 2026-09-07).
Umbrella acceptance checkpoint: `8764bc8`. REVIEW contains the user-approved scope; DESIGN contains the audited
contracts and verification requirements. Only provider files are changed in this work package.

## Implementation order and requirements

1. Correct CheckBox/RadioButton/Switch indicator-only implicit sizing and mirrored layout. Preserve labeled
   controls, size roles, checked/disabled geometry and trailing-card containment. Add behavioral regressions.
2. Give standard controls application/control palette authority while preserving HoloNight session defaults.
   Map text, surfaces, highlights, disabled groups, borders and hover/pressed states; preserve Core/composite
   appearance authority. Verify default-theme parity and black/white/black application-palette changes.
3. Add the nine accepted Templates implementations and installed metadata: ApplicationWindow, Label, ToolButton,
   ToolBar, ToolSeparator, MenuSeparator, Popup, MenuBar and MenuBarItem. Preserve Basic and explicit Fusion fallback.
4. Migrate composites to runtime Controls, preserving public error/sizing/corner APIs and scaled ComboBox geometry.
   HnLabel uses Templates.Label to keep Core free of selected-style dependencies.
5. Migrate demo/gallery, embed root qtquickcontrols2.conf defaults, retain explicit overrides, update import policy,
   guides and installed/negative/rendering fixtures. Do not turn discovery defect assertions into acceptance tests.
6. Run focused checks first, then the provider suite and installed consumers. Record limitations and publish the
   verified provider before requesting an umbrella handoff. Consumer migration and ecosystem acceptance are separate.

## Progress

| Slice | State | Verification |
|---|---|---|
| Indicator geometry | Done | Original code fails the new regression; fixed code passes all five focused tests and all 28 CTest entries (2026-09-07). |
| Palette and nine controls | Planned | — |
| Composites and executable defaults | Planned | — |
| Policy, documentation and installed verification | Planned | — |

Application-painted sliders and the license-popup composition remain accepted boundaries. Other catalog types
remain fallback controls. No full manual application/state or two-compositor integration pass is implied here.

Geometry files: qml/CheckBox.qml, RadioButton.qml, Switch.qml and tests/test_qml_smoke.cpp.
Switch thumb placement follows visualPosition for mirrored state. Full build and CTest passed; installed-package
smoke is included. UQC-101 remains In Progress pending the other implementation slices.
