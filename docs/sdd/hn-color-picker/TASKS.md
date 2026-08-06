# SDD Tasks — hn-color-picker

- [x] T-001: Create HnColorPicker.qml with T.Control root and core properties
  - REQs: REQ-C-001, REQ-C-002, REQ-F-005, REQ-F-012, REQ-F-028
  - Check: HnColorPicker.qml exists at qml/controls/ with T.Control root, declared properties for colors/selectedColor/sizeRole/resolvedSizeRole/swatchDiameter, colorSelected signal, and function stubs for moveFocus/activate.

- [x] T-002: Register HnColorPicker in qml/CMakeLists.txt HOLONIGHT_CONTROLS_PUBLIC_QML_FILES
  - REQs: REQ-C-001
  - Check: HnColorPicker.qml is listed in the HOLONIGHT_CONTROLS_PUBLIC_QML_FILES set (after HnSegmentedControl.qml) and the control compiles without module resolution errors.

- [x] T-003: Implement ScrollView wrapping, Row layout, and Repeater per DESIGN.md §3
  - REQs: REQ-F-023, REQ-F-024, REQ-F-025, REQ-C-026, REQ-NF-027, REQ-F-015
  - Check: A test instance with 10 swatches in a 200px-wide container shows horizontal scrollbar and renders all swatches in a single row without vertical scrolling.

- [x] T-004: Implement T.AbstractButton swatch delegate base structure
  - REQs: REQ-F-016 (partial - structure)
  - Check: The Repeater's delegate is a T.AbstractButton with implicitWidth/implicitHeight derived from swatch diameter and onClicked handler wired to root.activate.

- [x] T-005: Implement swatch diameter derivation and unselected circle rendering
  - REQs: REQ-F-013, REQ-F-014, REQ-F-015, REQ-F-016
  - Check: A Compact swatch renders as a 28px circle; a Hero swatch as 52px; invalid sizeRole: 999 normalizes to 32px (Normal); Repeater model count matches colors.length.

- [x] T-006: Implement selection matching by color equality and selection ring rendering per DESIGN.md §4(a-c)
  - REQs: REQ-F-006, REQ-F-007, REQ-F-008, REQ-F-017, REQ-F-019
  - Check: Setting selectedColor to a swatch's color renders a colored ring only on matching swatches; unset selectedColor renders no rings; duplicate swatches light up simultaneously.

- [x] T-007: Implement selection glow with semi-transparent overlay per DESIGN.md §4(b)
  - REQs: REQ-F-018, REQ-F-019
  - Check: A selected swatch displays a soft, semi-transparent (approximately 25% alpha) oversized circle halo derived from that swatch's own color.

- [x] T-008: Implement procedural checkmark Shape with PathSvg and scale transform per DESIGN.md §4(d) and §12
  - REQs: REQ-F-020
  - Check: A selected swatch displays a visible checkmark mark centered and scaled inside the circle that maintains correct proportions across size roles (Compact through Hero).

- [x] T-009: Implement tick color computation using luminance formula and threshold per DESIGN.md §4(d)
  - REQs: REQ-F-021, REQ-F-022
  - Check: A selected bright-colored swatch (luminance > 0.6, e.g., yellow) renders a dark checkmark; a dark swatch (luminance ≤ 0.6, e.g., navy) renders a white checkmark.

- [x] T-010: Implement colors property accepting arbitrary QML color values
  - REQs: REQ-F-001, REQ-F-004
  - Check: Instantiation with colors: [HoloniightPalette.error, "#ABC123", "red"] compiles and renders three swatches of specified colors without type errors.

- [x] T-011: Implement default colors list (7 swatches from palette tokens)
  - REQs: REQ-F-002
  - Check: An HnColorPicker with no colors property explicitly set renders exactly 7 swatches with colors matching HoloniightPalette.accentCyan, accentBlue, accentViolet, accentYellow, success, warning, error in that exact order.

- [x] T-012: Handle empty colors list rendering without errors
  - REQs: REQ-F-003
  - Check: Instantiation with colors: [] renders zero swatches without errors and is visually/behaviorally distinct from the default 7-swatch state.

- [x] T-013: Implement click selection, selectedColor update, and colorSelected signal emission
  - REQs: REQ-F-009, REQ-F-010, REQ-F-011
  - Check: Clicking a swatch updates root.selectedColor to that swatch's color and emits colorSelected(color) with the same color value without requiring external signal handlers.

- [x] T-014: Implement keyboard focus support and focus ring rendering with visualFocus binding per DESIGN.md §5
  - REQs: REQ-F-031
  - Check: Pressing Tab in a window containing HnColorPicker moves focus to the control; the focused swatch displays a focus ring matching Holonight's standard indicator.

- [x] T-015: Implement moveFocus helper and arrow key navigation with clamping per DESIGN.md §5
  - REQs: REQ-F-032, REQ-C-034
  - Check: With focus on swatch index 2, Left moves focus to index 1; pressing Right on the last swatch keeps focus at the last index (no wraparound); function shape mirrors HnSegmentedControl's pattern.

- [x] T-016: Implement activate helper and keyboard activation with Space/Enter keys per DESIGN.md §5
  - REQs: REQ-F-033
  - Check: With keyboard focus on an unselected swatch, pressing Space updates selectedColor and emits colorSelected; pressing Enter and (numpad) Return have identical effect; activate never triggers from click.

- [x] T-017: Implement disabled state visual dimming and disabled-state cascade inheritance
  - REQs: REQ-F-028, REQ-F-029, REQ-F-030
  - Check: An HnColorPicker with enabled: false renders all swatches with dimmed color (40% alpha) and non-clickable state; no per-swatch enable API exists; clicking does not change selectedColor.

- [x] T-018: Implement accessibility properties (root Grouping, swatch RadioButton, names/checked)
  - REQs: REQ-A-035, REQ-A-036, REQ-A-037
  - Check: Root has Accessible.role: Accessible.Grouping; each swatch has Accessible.role: Accessible.RadioButton, Accessible.name: color.toString() (hex string), and Accessible.checked bound to swatch.selected.

- [x] T-019: Test default colors match palette tokens (test case 1 of 7)
  - REQs: REQ-F-002
  - Check: A TEST_F(QmlSmoke, Controls_ColorPickerDefaultColorsMatchPalette) case in test_qml_smoke.cpp instantiates without colors property and asserts repeater model count equals 7 with color sampling matching tok.accentCyan/Blue/Violet/Yellow/success/warning/error.

- [x] T-020: Test empty colors list renders zero swatches (test case 2 of 7)
  - REQs: REQ-F-003
  - Check: A TEST_F(QmlSmoke, Controls_ColorPickerEmptyColorsRendersNoSwatches) case asserts that colors: [] results in repeater model count of 0 without errors.

- [x] T-021: Test selection by color equality and duplicate swatch handling (test case 3 of 7)
  - REQs: REQ-F-006, REQ-F-008
  - Check: A TEST_F case with colors: ["#FF0000","#00FF00","#FF0000"] and selectedColor: "#FF0000" asserts both matching delegate instances report selected: true and the middle one reports false via findChild/property inspection.

- [x] T-022: Test click selection triggers colorSelected signal and updates selectedColor (test case 4 of 7)
  - REQs: REQ-F-009, REQ-F-010, REQ-F-011
  - Check: A TEST_F case invokes clicked() on a swatch via QMetaObject::invokeMethod and asserts root.selectedColor updated to that swatch's color and colorSelected signal fired with matching color value.

- [x] T-023: Test size roles (28/32/40/52 px) and invalid sizeRole normalization (test case 5 of 7)
  - REQs: REQ-F-013, REQ-F-014, REQ-F-015
  - Check: A TEST_F case instantiates four variants with sizeRole: Compact/Normal/Large/Hero and asserts swatchDiameter and implicitHeight equal 28/32/40/52; another variant with sizeRole: 999 asserts resolvedSizeRole normalizes to Normal (32).

- [x] T-024: Test disabled state (visual dimming and non-interactivity) (test case 6 of 7)
  - REQs: REQ-F-029, REQ-F-030
  - Check: A TEST_F case with enabled: false asserts swatches render at reduced opacity, and invoking clicked() does not change selectedColor or emit colorSelected.

- [x] T-025: Test keyboard navigation (arrow clamping) and activation (Space/Enter) (test case 7 of 7)
  - REQs: REQ-F-031, REQ-F-032, REQ-F-033
  - Check: A TEST_F case using Window { visible: true } and QTest::keyClick verifies Left/Right arrows move focusedIndex with clamping (no wrap), focus ring renders on focused swatch, and Space/Enter/Return on a given index updates selectedColor and emit colorSelected.

- [x] T-026: Test integration pattern (standalone and nested in HnSettingsRow)
  - REQs: REQ-C-003
  - Check: A TEST_F case or standalone QML smoke-test file instantiates HnColorPicker both in isolation and as control: HnColorPicker { ... } inside HnSettingsRow; both compile and render without binding errors or missing properties.

- [x] T-027: Run full build, clang-tidy, and ctest verification
  - REQs: All (pre-commit verification)
  - Check: cmake --build build -j$(nproc) completes without warnings, cmake --build build --target tidy reports zero new issues, and QT_QPA_PLATFORM=offscreen ctest --test-dir build --output-on-failure passes all tests including the new HnColorPicker control tests.
