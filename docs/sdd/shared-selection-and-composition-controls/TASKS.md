# Shared Selection and Composition Controls — Tasks

- [x] Specify behavior, accessibility, state precedence, compatibility, and ownership.
- [x] Design palette derivation, internal selection composition, slots, keyboard behavior, and packaging.
- [x] Add selection palette roles to all schemes, C++ tokens, QML exposure, and token documentation.
- [x] Update the standard `ItemDelegate` selection and keyboard-focus visuals.
- [x] Add the internal selectable-delegate foundation.
- [x] Add navigation, list, card, and action delegates.
- [x] Add status indicator, key hint, panel header, segmented control, choice card, and action bar.
- [x] Extend settings row and section header compatibly.
- [x] Register and install all controls in `Holonight.Controls` 1.0.
- [x] Add gallery coverage for representative state, size, and slot combinations.
- [x] Add palette, QML behavior, and package regression tests.
- [x] Run focused QML tests and lint.
- [x] Run build, package validation, full headless CTest, and gallery startup at 1.0 and 1.25 scale.
- [x] Generate `docs/shared-controls-usage-guide.md`.
- [x] Record final verification commands and results.

## Completion evidence

- `cmake --build build -j4` — passed.
- Focused `holonight_qml_smoke_tests` selection/control filter — 8/8 passed.
- Focused `holonight_tests --gtest_filter='SelectionTokens.*'` — 1/1 passed.
- `cmake --build build --target holonight_qml_qmllint holonight_core_qml_qmllint holonight_controls_qml_qmllint -j4`
  — passed; the base style target retains two pre-existing `ComboBox.Window` warnings.
- `QT_QPA_PLATFORM=offscreen ctest --test-dir build --output-on-failure` — 16/16 passed,
  including install-tree construction of every new public type.
- Controls gallery ran without QML diagnostics for three seconds at
  `QT_SCALE_FACTOR=1.0` and `QT_SCALE_FACTOR=1.25`; `timeout` returned 124 as expected.
- `git diff --check` — passed.
