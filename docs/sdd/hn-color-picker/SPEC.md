# HnColorPicker Requirements Specification

## Overview

HnColorPicker is a reusable QML control in the `Holonight.Controls` module that displays a fixed set of color swatches in a single horizontal row, allowing the user to select one. The selected swatch is visually distinguished by a colored ring, soft glow, and procedurally-drawn checkmark whose color is automatically adjusted for legibility based on perceived luminance. The control is usable standalone or embedded in `HnSettingsRow`, and supports keyboard navigation, theme integration with fallback default colors, and horizontal scrolling when needed.

`autoUpdateSelectedColor` defaults to `true`, preserving standalone uncontrolled behavior. Set it to `false` when `selectedColor` is externally bound; activation then emits `colorSelected` without assigning `selectedColor` or breaking that binding.

---

## Requirements

### Component Identity & Architecture

**REQ-C-001: QML Module Registration**
- The control shall be implemented as a new file `qml/controls/HnColorPicker.qml`.
- **Acceptance Criterion**: File exists at the specified path and is registered in the `Holonight.Controls` QML module (verified by checking `qml/controls/qmldir` or CMakeLists.txt for the control name).

**REQ-C-002: Control Base Type**
- HnColorPicker shall inherit from `QtQuick.Templates.Control` (T.Control).
- **Acceptance Criterion**: The root element in HnColorPicker.qml is `T.Control` (imported as `import QtQuick.Templates as T`), and the control compiles without errors.

**REQ-C-003: Integration Pattern**
- HnColorPicker shall be usable both standalone (dropped anywhere in a QML tree) and as the value of `HnSettingsRow { control: HnColorPicker { ... } }`.
- **Acceptance Criterion**: A test QML file instantiates HnColorPicker both in isolation and nested inside an HnSettingsRow; both instantiations compile and render without binding errors or missing properties.

---

### Color Property

**REQ-F-001: Explicit Color List**
- The `colors` property shall be of type `list<color>` and accept a custom list of color values provided by the caller.
- **Acceptance Criterion**: A QML instantiation setting `colors: ["#FF0000", "#00FF00", "#0000FF"]` compiles and renders three swatches of the specified colors.

**REQ-F-002: Default Color List**
- WHEN the `colors` property is not explicitly set by the caller, the control shall default to a list of seven swatches in this exact order: `HoloniightPalette.accentCyan`, `HoloniightPalette.accentBlue`, `HoloniightPalette.accentViolet`, `HoloniightPalette.accentYellow`, `HoloniightPalette.success`, `HoloniightPalette.warning`, `HoloniightPalette.error`.
- **Acceptance Criterion**: An HnColorPicker instantiated without a `colors` property renders exactly 7 swatches, and color sampling of each swatch matches the hue and saturation of the seven named palette tokens.

**REQ-F-003: Empty Color List Handling**
- IF the caller explicitly sets `colors: []` (empty array), the control shall render zero swatches.
- **Acceptance Criterion**: A QML instantiation with `colors: []` renders a control with no visible color circles; this state is distinct from the default 7-swatch state and does not raise errors.

**REQ-F-004: Arbitrary Color Support**
- The control shall accept any valid QML `color` values in the `colors` list, including theme tokens, hex notation, named colors, and RGB values.
- **Acceptance Criterion**: A QML instantiation with mixed color types (e.g., `colors: [HoloniightPalette.error, "#ABC123", "red"]`) compiles and renders three swatches matching the specified colors.

---

### Selected Color Property

**REQ-F-005: Selected Color Binding**
- The `selectedColor` property shall be of type `color`, two-way bindable, and default to an invalid/unset color (QML's default-constructed `color` value).
- **Acceptance Criterion**: A newly instantiated HnColorPicker has `selectedColor === color()` (the invalid default); no swatch renders a selection ring or checkmark on initialization.

**REQ-F-006: Selection by Color Equality**
- Selection matching shall be performed by color value equality: a swatch is marked as selected if and only if its color equals the value of `selectedColor`.
- **Acceptance Criterion**: When `selectedColor` is set to a hex color value matching one of the swatches in `colors`, that swatch renders a ring and tick; other swatches do not. Changing `selectedColor` to a different value updates the visual selection in the next frame.

**REQ-F-007: No Implicit Default Selection**
- WHILE `selectedColor` is set to an invalid/unset color (the default), no swatch shall render a selection ring or checkmark, even if one of the `colors` entries happens to match a commonly used theme default.
- **Acceptance Criterion**: With default initialization (no `selectedColor` explicitly set), no visible ring, tick, or glow appears on any swatch.

**REQ-F-008: Duplicate Color Handling**
- IF `colors` contains duplicate or equal color values, ALL swatches whose color equals `selectedColor` shall render a ring, glow, and checkmark simultaneously.
- **Acceptance Criterion**: A QML instantiation with `colors: ["#FF0000", "#00FF00", "#FF0000"]` and `selectedColor: "#FF0000"` renders rings and checkmarks on both the 1st and 3rd swatches.

---

### Swatch Interaction & Signals

**REQ-F-009: Click Selection**
- WHEN a swatch is clicked or activated, the control shall set `root.selectedColor` to the color of that swatch.
- **Acceptance Criterion**: Clicking a swatch changes the value of `selectedColor` to match that swatch's color, and the ring/glow/checkmark appear on that swatch in the next rendered frame.

**REQ-F-010: Color Selected Signal**
- WHEN a swatch is clicked or activated, the control shall emit a signal `colorSelected(color color)` with the swatch's color as the argument.
- **Acceptance Criterion**: A QML parent connecting to `onColorSelected: { ... }` receives the selected color value at the moment the swatch is clicked; the emitted value equals the swatch's color.

**REQ-F-011: Self-Contained Selection**
- The control's selection behavior (updating `selectedColor` and emitting `colorSelected`) shall occur without requiring external rebinding or signal handlers.
- **Acceptance Criterion**: A standalone HnColorPicker with no external `onColorSelected` handler or `selectedColor` binding correctly updates `selectedColor` when a swatch is clicked, without parent-side code to propagate the selection.

---

### Size Role & Sizing

**REQ-F-012: Size Role Property**
- The control shall expose a `sizeRole: int` property using the shared `HnControlSize` enum (`Compact`, `Normal`, `Large`, `Hero`), following the same convention as `HnSegmentedControl` and `HnSettingsRow`.
- **Acceptance Criterion**: HnColorPicker accepts `sizeRole: HnControlSize.Normal` and other enum values without errors; the property is of type `int` (matching the enum's underlying type).

**REQ-F-013: Swatch Diameter Derivation**
- The diameter of each swatch circle shall be derived from `HnControlMetrics.controlHeight(resolvedSizeRole)`, which returns 28 px for `Compact`, 32 px for `Normal`, 40 px for `Large`, and 52 px for `Hero`.
- **Acceptance Criterion**: An HnColorPicker with `sizeRole: HnControlSize.Compact` renders swatches with a diameter of 28 px; `sizeRole: HnControlSize.Hero` renders swatches with a diameter of 52 px.

**REQ-F-014: Size Role Normalization**
- IF an invalid or unrecognized `sizeRole` value is provided, the control shall normalize to `Normal` via `HnControlMetrics.normalizedSizeRole()`.
- **Acceptance Criterion**: Setting `sizeRole: 99` (an invalid enum value) does not crash or error; the control renders swatches at the `Normal` (32 px) diameter.

**REQ-F-015: Control Dimensions**
- The control's own height shall equal the swatch diameter (as derived from `sizeRole`); its width shall be flexible and resizable by its container.
- **Acceptance Criterion**: An HnColorPicker with `sizeRole: HnControlSize.Normal` reports `implicitHeight` of 32 px; when placed in a resizable container, the control width adapts to available space.

---

### Visual Rendering: Unselected Swatches

**REQ-F-016: Unselected Swatch Appearance**
- Unselected swatches shall render as flat, filled circles with no ring, no glow, and no checkmark.
- **Acceptance Criterion**: A screenshot of an HnColorPicker with 3 swatches (one selected) shows 2 unselected swatches as solid circles with no visible ring or tick.

---

### Visual Rendering: Selected Swatches

**REQ-F-017: Selection Ring**
- The selected swatch shall render a ring/stroke around the circle whose color is the swatch's own color at full opacity.
- **Acceptance Criterion**: When a swatch is selected, a visible colored ring appears around the circle; the ring color matches the swatch's own color at full opacity.

**REQ-F-018: Selection Glow**
- The selected swatch shall render a soft outer glow using the swatch's own color at reduced alpha (approximately 25%).
- **Acceptance Criterion**: A selected swatch displays a soft, blurred halo around the circle; the halo color matches the swatch color but is semi-transparent (approximately 25% opacity) and does not obscure adjacent swatches.

**REQ-F-019: Per-Swatch Glow Derivation**
- The ring and glow colors shall be derived per-swatch from that swatch's own color (not from a fixed theme token), ensuring correct appearance for arbitrary/custom color lists.
- **Acceptance Criterion**: When `colors` includes custom hex colors (e.g., `#ABC123`), the ring and glow of a selected swatch derived from that color match the swatch color correctly.

**REQ-F-020: Checkmark Rendering**
- The selected swatch shall render a checkmark tick drawn procedurally (using QtQuick.Shapes `Shape`/`PathSvg` or `Canvas`) inside the circle.
- **Acceptance Criterion**: A selected swatch displays a visible checkmark mark centered inside the circle; the mark is procedurally drawn, not an external SVG asset reference.

**REQ-F-021: Tick Color Computation**
- The checkmark tick's color shall be computed at runtime using the luminance formula `0.299*r + 0.587*g + 0.114*b`, where r, g, b are the swatch color's red, green, blue components on a 0–1 scale.
- **Acceptance Criterion**: The tick color computation is performed using the luminance formula specified; this is verifiable via code inspection of HnColorPicker.qml.

**REQ-F-022: Tick Color Selection**
- IF the computed luminance is greater than 0.6, the tick shall be a dark color (e.g., near-black); otherwise, the tick shall be white.
- **Acceptance Criterion**: A selected swatch with a bright color (e.g., yellow, luminance ~0.9) renders a dark checkmark; a selected swatch with a dark color (e.g., navy, luminance ~0.15) renders a white checkmark. Both ticks remain legible.

---

### Layout & Scrolling

**REQ-F-023: Horizontal Row Layout**
- All swatches shall be laid out in a single horizontal row with no wrapping to multiple rows under any circumstance.
- **Acceptance Criterion**: An HnColorPicker with 10 swatches in a narrow container (narrower than 10 × swatch diameter) does not wrap any swatch to a second row; all swatches remain in one row.

**REQ-F-024: Width Flexibility**
- The control's width shall be flexible and resizable by its container (e.g., when placed in a layout with `Layout.fillWidth: true`).
- **Acceptance Criterion**: An HnColorPicker inside a Row with `Layout.fillWidth: true` correctly expands and contracts when the window is resized.

**REQ-F-025: Horizontal Scrolling**
- WHEN the container/available width is insufficient to display all swatches at once, the row shall become horizontally scrollable via the project's existing `Holonight` style `ScrollView` component (referenced as `H.ScrollView` after `import Holonight as H`).
- **Acceptance Criterion**: An HnColorPicker with 10 swatches inside a fixed-width container (e.g., width: 200 px) displays horizontal scrollbars and allows scrolling to reveal hidden swatches.

**REQ-C-026: Scroll Implementation Pattern**
- The scrolling mechanism shall use the same `H.ScrollView` component already implemented in the Holonight style (as used by `HnTextArea.qml`).
- **Acceptance Criterion**: HnColorPicker's QML source imports `Holonight as H` and wraps the swatch row in `H.ScrollView { ... }`, matching the architectural pattern in HnTextArea.qml.

**REQ-NF-027: No Vertical Scrolling**
- The control shall not support vertical scrolling or wrapping to multiple rows; overflow is horizontal only.
- **Acceptance Criterion**: An HnColorPicker in a very tall container does not allow vertical scrolling; the control's height remains fixed at the swatch diameter.

---

### Enabled/Disabled State

**REQ-F-028: Enabled State Inheritance**
- The control shall respect the standard inherited Qt Quick `Control.enabled` property (no new property needed).
- **Acceptance Criterion**: Setting `enabled: false` on an HnColorPicker propagates to all internal swatches without explicit code in the control; the property is inherited, not redefined.

**REQ-F-029: Disabled Visual Treatment**
- WHILE the control's `enabled` property is `false`, all swatches shall show a disabled visual treatment (dimmed appearance) and be non-interactive.
- **Acceptance Criterion**: An HnColorPicker with `enabled: false` renders all swatches with reduced opacity or muted colors; clicking a swatch does not change `selectedColor` or emit `colorSelected`.

**REQ-F-030: All-or-Nothing Disabling**
- Disabling the control shall apply to all swatches simultaneously (all-or-nothing); there is no per-swatch enable/disable API.
- **Acceptance Criterion**: An HnColorPicker with `enabled: false` disables all swatches together; no QML property or method exists to enable individual swatches.

---

### Keyboard Navigation & Accessibility

**REQ-F-031: Keyboard Focus Support**
- The control shall support `activeFocusOnTab: true`, consistent with other interactive Holonight controls, allowing Tab to move focus into the control.
- **Acceptance Criterion**: Pressing Tab in a QML window containing an HnColorPicker moves focus to the control; the focused swatch is visually indicated with a focus ring matching Holonight's standard focus indicator.

**REQ-F-032: Arrow Key Navigation**
- WHEN the control has keyboard focus, pressing the Left arrow key shall move focus to the previous swatch; pressing the Right arrow key shall move focus to the next swatch. Focus shall clamp at boundaries (no wrapping), matching `HnSegmentedControl`'s `Math.max(0, …)` / `Math.min(count - 1, …)` behavior.
- **Acceptance Criterion**: With focus on the 3rd swatch, pressing Left moves focus to the 2nd swatch; pressing Right from the 2nd swatch moves focus to the 3rd. Pressing Left on the 1st (first) swatch keeps focus on the 1st swatch; pressing Right on the last swatch keeps focus on the last swatch.

**REQ-F-033: Activation Keys**
- WHEN the control has keyboard focus, pressing Space or Enter shall select (activate) the currently focused swatch, setting `selectedColor` to that swatch's color and emitting `colorSelected`.
- **Acceptance Criterion**: With focus on an unselected swatch, pressing Space changes `selectedColor` to that swatch's color and emits `colorSelected(color)`; pressing Enter has the identical effect.

**REQ-C-034: Navigation Pattern**
- Arrow key navigation shall follow the same pattern as `HnSegmentedControl.qml`, using `Keys.onLeftPressed`/`Keys.onRightPressed` and an internal `activate(index)` or equivalent helper function.
- **Acceptance Criterion**: Code inspection of HnColorPicker.qml shows Left/Right arrow key handlers and navigation logic modeled directly after HnSegmentedControl's existing implementation.

**REQ-A-035: Root Accessibility Role**
- The root control shall have `Accessible.role: Accessible.Grouping`.
- **Acceptance Criterion**: A screen reader tool (e.g., via Qt accessibility inspection) reports the root element as Grouping.

**REQ-A-036: Swatch Accessibility Role**
- Each swatch shall have `Accessible.role: Accessible.RadioButton`, reflecting that only one swatch can be selected at a time.
- **Acceptance Criterion**: A screen reader inspecting the control reports each swatch as a RadioButton; assistive technology correctly identifies the selected state of each swatch.

**REQ-A-037: Accessible Names**
- Each swatch shall have a descriptive accessible name or description that aids screen readers (e.g., the color's hex value, a named color label, or an index).
- **Acceptance Criterion**: A screen reader reading an HnColorPicker aloud provides sufficient information for a blind user to understand which swatches are available and which is selected (e.g., "Color swatch, Red, selected").

---

## Non-Goals

The following features are explicitly out of scope for this specification:

1. **Per-Swatch Enable/Disable**: No API to enable or disable individual swatches; disabling is all-or-nothing at the control level.
2. **Custom Color Picker/Dialog**: No hue wheel, RGB sliders, hex text input, or system color picker integration; the control only displays and selects among a fixed, provided (or theme-default) list of colors.
3. **Multi-Select**: Only one swatch can be the current selection at a time; no checkbox-style multi-select variant.
4. **Drag-to-Reorder**: Swatches are in a fixed order; no dragging to rearrange them.
5. **Vertical or Grid Layout**: No vertical layout variant or grid/wrap layout; swatches are always in a single horizontal row.
6. **Animated Transitions**: No animation spec for selection changes, focus changes, or state transitions (controls may include animations, but they are not required by this specification).
7. **Custom Styling/Themability**: The ring, glow, and checkmark visual characteristics are determined by the spec (swatch color, luminance thresholds, ~25% alpha); there are no QML properties to override ring thickness, glow blur radius, or tick size on a per-instance basis.

---

## Verification Checklist

- [ ] HnColorPicker.qml exists at `qml/controls/HnColorPicker.qml` and is registered in `Holonight.Controls` module.
- [ ] The control compiles without warnings.
- [ ] Default `colors` list provides 7 swatches matching the specified palette tokens in the exact order.
- [ ] `selectedColor` defaults to invalid color; no implicit selection occurs.
- [ ] Clicking a swatch updates `selectedColor` and emits `colorSelected`.
- [ ] Ring, glow, and checkmark render on selected swatch(es) per color equality matching.
- [ ] Tick color is correctly computed based on luminance (dark for bright swatches, white for dark swatches).
- [ ] Swatch diameter matches `HnControlMetrics.controlHeight(sizeRole)` for all four size roles (28, 32, 40, 52 px).
- [ ] Swatches layout in a single horizontal row; no wrapping occurs.
- [ ] Horizontal scrolling works when swatches overflow container width (via `H.ScrollView`).
- [ ] Disabled state applies to all swatches; clicking is non-interactive when disabled.
- [ ] Left/Right arrow keys navigate between swatches with clamping (no wrap) at boundaries; Space/Enter activates.
- [ ] Focus can be moved to the control via Tab; focus ring is visible on the focused swatch.
- [ ] Accessible.role values are set on root (Grouping) and swatches (RadioButton).
- [ ] Accessible names/descriptions are provided on each swatch.
- [ ] Empty `colors: []` renders zero swatches without error.
- [ ] Arbitrary custom colors (hex, named, RGB) work in `colors` list.
- [ ] Ring and glow colors derive from each swatch's own color, not theme tokens.
- [ ] Control works standalone and nested in `HnSettingsRow { control: ... }`.
- [ ] Control respects `enabled: false` and shows disabled visual treatment.
- [ ] `sizeRole` normalization handles invalid values gracefully (defaults to Normal).
