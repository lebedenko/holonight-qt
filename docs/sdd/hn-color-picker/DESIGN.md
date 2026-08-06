# HnColorPicker — Architecture Design

**Feature:** HnColorPicker
**Status:** Design
**Date:** 2026-07-31
**Spec:** `docs/sdd/hn-color-picker/SPEC.md` (authoritative requirements; this document describes *how* to satisfy it)

---

## 1. Component Overview

`HnColorPicker` is a new `Holonight.Controls` composite: a row of circular color swatches with click/keyboard
single-selection, a themed default palette, and horizontal-only overflow scrolling. It sits alongside
`HnSegmentedControl.qml` (index-based single-select row) and is designed to be used either standalone or as the
`control:` payload of `HnSettingsRow` — the same integration point already used by `HnSearchField`, `HnIconComboBox`,
and other settings-row controls. Unlike `HnSegmentedControl`, which selects by integer index into an arbitrary
`model`, `HnColorPicker` selects by **color value equality** against a fixed `colors` list, and its swatches are
circles, not resizable rectangular segments.

Module placement: `qml/controls/HnColorPicker.qml`, registered as a **public** type of the `Holonight.Controls`
QML module (same tier as `HnSegmentedControl`, `HnSettingsRow`, `HnIconButton`).

---

## 2. Data Model / Properties

```qml
T.Control {
    id: root

    property list<color> colors: [
        HoloniightPalette.accentCyan,
        HoloniightPalette.accentBlue,
        HoloniightPalette.accentViolet,
        HoloniightPalette.accentYellow,
        HoloniightPalette.success,
        HoloniightPalette.warning,
        HoloniightPalette.error
    ]
    property color selectedColor: Qt.color()   // invalid/unset by default — see note below
    property int sizeRole: HnControlSize.Normal
    readonly property int resolvedSizeRole: HnControlMetrics.normalizedSizeRole(root.sizeRole)
    readonly property real swatchDiameter: HnControlMetrics.controlHeight(root.resolvedSizeRole)

    signal colorSelected(color color)
    // enabled: inherited from T.Control — no redeclaration (REQ-F-028)
}
```

Property-by-property:

- **`colors: list<color>`** (REQ-F-001–004) — QML `list<basicType>` value-type list, available since the
  structured-value-type-list feature landed in Qt 6.4; the project's minimum is Qt 6.11
  (`find_package(Qt6 6.11 …)` in the root `CMakeLists.txt`), so the type is safely usable. `list<color>` supports
  `colors: []` naturally (REQ-F-003) — an empty list, not `null`/`undefined`, so `Repeater { model: root.colors }`
  produces zero delegates without error.
- **Default value liveness** — the default is declared with `:` (not a one-time JS assignment), which in QML
  establishes a genuine property binding, exactly like `HnIconButton.normalColor: root.icon.color.a > 0 ? … :
  HoloniightPalette.textSecondary` or `HnSegmentedControl.background.color: HoloniightPalette.surface`. Every
  property referenced in `HoloniightPalette` fires `paletteChanged()` (see `qml/holoniightpalette.h`), so this
  default **stays live**: if the palette hot-reloads (the theme-config file watcher already wired into
  `HoloniightPalette`), the default 7-swatch list re-evaluates and the swatches update colors automatically,
  identical to how every other Holonight control's palette-derived defaults behave. **Decision: do not wrap it in
  `readonly property`** — `colors` must remain caller-overridable per REQ-F-001, and `readonly` would forbid that.
  A plain default-valued `property list<color>` is simultaneously (a) overridable by the caller and (b) reactive
  until overridden — this matches how every other control in this codebase expresses a themed default.
- **`selectedColor: color`** (REQ-F-005) — two-way bindable by virtue of being a normal declared QML property
  (QML properties are inherently bindable both ways without extra ceremony). Default must be the QML "invalid
  color" value. In QML/JS this is expressed as `color()` (the default-constructed `color`), which has `.a === 0`
  when read back as an *unset* value — but note: `color()` and `"transparent"` are **not** the same QColor
  (`"transparent"` is a valid, fully-specified color with alpha 0; the QML-invalid `color()` has no valid
  components at all). The spec's acceptance criterion is `selectedColor === color()`, so the property must be
  left with no explicit default (QML properties of type `color` default to the invalid `QColor()` automatically
  when no initializer is given) — do **not** write `selectedColor: "transparent"` or any other explicit value.
- **`sizeRole: int`** (REQ-F-012–014) — same convention as `HnSegmentedControl`/`HnSettingsRow`/`HnIconButton`:
  raw `int` typed property (not the enum type itself, since `HnControlSize` is a `pragma Singleton` `QtObject`
  exposing an anonymous `enum`, and consuming code always types the property as plain `int`), normalized via
  `HnControlMetrics.normalizedSizeRole()`.
- **`enabled`** — inherited from `T.Control`; no redeclaration, no per-swatch enable API (REQ-F-028, REQ-F-030,
  Non-Goal 1).

---

## 3. Component Structure

```qml
pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls.Basic as C
import QtQuick.Layouts
import QtQuick.Shapes
import QtQuick.Templates as T
import Holonight as H
import Holonight.Core

T.Control {
    id: root
    // properties from §2 …

    implicitHeight: root.swatchDiameter
    implicitWidth: row.implicitWidth   // flexible; container may override via Layout.fillWidth (REQ-F-024)
    activeFocusOnTab: true
    Accessible.role: Accessible.Grouping
    Keys.onLeftPressed:  root.moveFocus(Math.max(0, root.focusedIndex - 1))
    Keys.onRightPressed: root.moveFocus(Math.min(repeater.count - 1, root.focusedIndex + 1))
    Keys.onSpacePressed:  root.activate(root.focusedIndex)
    Keys.onReturnPressed: root.activate(root.focusedIndex)
    Keys.onEnterPressed:  root.activate(root.focusedIndex)   // numpad Enter

    property int focusedIndex: 0        // internal — see §5, keyboard "current" pointer, distinct from selection
    function moveFocus(index: int): void { … }
    function activate(index: int): void { … }   // sets selectedColor + emits colorSelected

    contentItem: H.ScrollView {
        id: scroll
        contentWidth: row.implicitWidth
        implicitHeight: root.swatchDiameter
        C.ScrollBar.horizontal.policy: row.implicitWidth > scroll.availableWidth
                                        ? C.ScrollBar.AsNeeded : C.ScrollBar.AlwaysOff
        C.ScrollBar.vertical.policy: C.ScrollBar.AlwaysOff   // REQ-NF-027 — horizontal only
        clip: row.implicitWidth > scroll.availableWidth

        Row {
            id: row
            spacing: HnControlMetrics.internalSpacing(root.resolvedSizeRole)

            Repeater {
                id: repeater
                model: root.colors
                delegate: /* swatch delegate — §4 */
            }
        }
    }
}
```

Notes on the structure:

- **`Row` not `RowLayout`.** `HnSegmentedControl` uses `RowLayout` because its segments are `Layout.fillWidth: true`
  rectangles that must share the available width equally. Color swatches are fixed-diameter circles — nothing
  should stretch — so a plain `Row` (fixed `spacing`, children sized by their own `implicitWidth`/`implicitHeight`)
  is the correct primitive; `HnSearchField.qml` and `HnFormField.qml` already use bare `Row` for this exact
  "fixed-size items in a line" case in this codebase.
- **`H.ScrollView` wraps the `Row`**, matching `HnTextArea`'s pattern: `contentWidth` measures the row's natural
  (unclipped) width, the horizontal `ScrollBar.policy` is `AsNeeded` (not `AlwaysOn`), and `clip` only engages when
  content actually overflows — this satisfies REQ-F-025 without incurring a permanent scrollbar/clip cost for the
  common case (few swatches, no overflow).
- **Root `implicitWidth`** intentionally forwards the row's natural width (so a standalone `HnColorPicker` sizes
  itself to fit all swatches without scrolling by default), while `implicitHeight` is pinned to the swatch
  diameter (REQ-F-015). When embedded via `HnSettingsRow { control: HnColorPicker { … } }`, the `Loader`'s
  `Layout.fillWidth: !root.stacked` in `HnSettingsRow.qml` line 91–96 does **not** force width onto the control in
  the non-stacked case (`Layout.alignment: Qt.AlignRight | Qt.AlignVCenter`, no `Layout.fillWidth`) — so
  `HnColorPicker` will render at its natural width there and only scroll if the settings row itself is narrower
  than that. This matches REQ-C-003/REQ-F-024's "usable in both places" requirement without any special-casing in
  `HnColorPicker` itself.

---

## 4. Per-Swatch Delegate Design

```qml
delegate: T.AbstractButton {
    id: swatch
    required property int index
    required property color modelData
    readonly property bool selected: root.selectedColor.valid && swatch.modelData === root.selectedColor
    readonly property bool keyboardFocused: swatch.index === root.focusedIndex && root.visualFocus
    readonly property real luminance: 0.299 * swatch.modelData.r
                                     + 0.587 * swatch.modelData.g
                                     + 0.114 * swatch.modelData.b
    readonly property color tickColor: swatch.luminance > 0.6 ? "#1A1A1A" : "#FFFFFF"

    implicitWidth: root.swatchDiameter
    implicitHeight: root.swatchDiameter
    hoverEnabled: true
    Accessible.role: Accessible.RadioButton
    Accessible.name: swatch.modelData.toString()  // hex string, e.g. "#ff0000" — see §11 decision
    Accessible.checked: swatch.selected
    onClicked: root.activate(swatch.index)

    background: Item {
        // glow — outermost, only when selected (REQ-F-018/019)
        Rectangle {
            visible: swatch.selected
            anchors.centerIn: parent
            width: root.swatchDiameter + 12
            height: width
            radius: width / 2
            color: Qt.rgba(swatch.modelData.r, swatch.modelData.g, swatch.modelData.b, 0.25)
        }
        // focus ring — independent of selection (REQ-F-031)
        Rectangle {
            visible: swatch.keyboardFocused
            anchors.centerIn: parent
            width: root.swatchDiameter + 6
            height: width
            radius: width / 2
            color: "transparent"
            border.width: HoloniightPalette.focusBorderWidth
            border.color: HoloniightPalette.borderFocus
        }
        // swatch fill + selection ring (REQ-F-016/017)
        Rectangle {
            anchors.centerIn: parent
            width: root.swatchDiameter
            height: width
            radius: width / 2
            color: swatch.enabled ? swatch.modelData
                                  : Qt.rgba(swatch.modelData.r, swatch.modelData.g, swatch.modelData.b, 0.4)
            border.width: swatch.selected ? Math.max(2, HoloniightPalette.borderWidth + 1) : 0
            border.color: swatch.modelData   // full opacity, own color (REQ-F-017/019)
        }
        // checkmark — procedural tick (REQ-F-020/021/022)
        Shape {
            visible: swatch.selected
            anchors.centerIn: parent
            width: root.swatchDiameter * 0.5
            height: width
            preferredRendererType: Shape.CurveRenderer
            ShapePath {
                strokeColor: swatch.tickColor
                strokeWidth: Math.max(1.5, root.swatchDiameter * 0.06)
                fillColor: "transparent"
                capStyle: ShapePath.RoundCap
                joinStyle: ShapePath.RoundJoin
                // check tick scaled to a 24x24 box: down-stroke then up-stroke
                PathSvg { path: "M 5 13 L 10 18 L 19 6" }
            }
        }
    }
}
```

Delegate design decisions:

- **(a) Circle rendering** — plain `Rectangle` with `radius: width / 2`, the same primitive `HnStatusIndicator.qml`
  already uses for its status dot (`radius: width / 2` on a `Rectangle`). No `Shape` is needed for the fill/ring
  circles — `Shape` is reserved for the checkmark, which is a genuine open path, not a rounded rect.
- **(b) Selection via color equality** — `swatch.modelData === root.selectedColor`, guarded by
  `root.selectedColor.valid` so the QML-invalid default never accidentally equals a swatch (REQ-F-007). Because
  this is a `readonly property` recomputed per-delegate from `root.selectedColor`, REQ-F-008's duplicate-color
  case falls out for free: every delegate whose `modelData` equals `selectedColor` independently evaluates
  `selected: true`, so N duplicate swatches all light up simultaneously with no special-casing.
- **(c) Ring + glow derived per-swatch** — both read `swatch.modelData` directly (never a theme token), satisfying
  REQ-F-019. The glow is a second, larger, same-radius-ratio `Rectangle` with alpha 0.25 sitting *behind* the
  fill circle in paint order (declared before it in the `Item`), which is how this codebase already layers
  "selected overlay" rectangles (`HnChoiceCard.qml`'s `hnChoiceCardSelectedOverlay` on top of
  `hnChoiceCardBase`, same z-order-by-declaration-order technique). No `Qt5Compat.GraphicalEffects` /
  `MultiEffect` blur is used — nothing in this codebase imports either module; a soft look is approximated with a
  flat semi-transparent oversized circle, consistent with how `HoloniightPalette` already exposes flat
  `glowCyanSoft`/`glowBlueSoft`/`glowVioletSoft` tokens (themed flat colors, not blurred layers) for "glow" in
  other parts of the palette.
- **(d) Procedural tick** — `Shape`/`ShapePath`/`PathSvg`, same trio `HnSegmentedControl.qml` uses for its
  rounded-corner segment background path (`preferredRendererType: Shape.CurveRenderer`, `PathSvg { path: … }`).
  The tick path `M 5 13 L 10 18 L 19 6` is a checkmark in a 24×24 coordinate box (short down-stroke from
  the left-middle to the bottom-center, long up-stroke to the top-right), scaled to the delegate's `width`/`height`
  (set to `swatchDiameter * 0.5`, so the 24-unit path box is auto-scaled by the `Shape`'s implicit coordinate
  space — the `Shape` item's `width`/`height` do not auto-scale `PathSvg` coordinates in QtQuick.Shapes; **this
  needs a real transform**, either pre-computing the path string with values proportional to
  `root.swatchDiameter` inside a JS function, or wrapping the `ShapePath` in a `transform: Scale` bound to
  `width/24, height/24`. This is flagged in §12 as an implementation detail to get right, not something to gloss
  over in a design doc that says "sketch, don't fully implement."
- **Tick color luminance formula** — computed inline per REQ-F-021 exactly as `0.299*r + 0.587*g + 0.114*b` on
  the 0–1 scale QML's `color.r/.g/.b` already use (QML color channels are 0.0–1.0 reals, not 0–255 ints, so no
  extra normalization is needed). Threshold `> 0.6` picks near-black (`#1A1A1A`, not literal `#000000`, for softer
  contrast — matches REQ-F-022's "e.g., near-black" wording) vs. white.

---

## 5. Interaction & State Management

**Swatch delegate type: `T.AbstractButton`, not a bare `Item` + `MouseArea`/`TapHandler`.**

This is a deliberate deviation from the brief's suggestion to consider `Item`/`MouseArea` for a "circular, not
rectangular" hit target. A repo-wide search (`grep -rn "TapHandler\|MouseArea" qml/`) turns up **zero** matches
anywhere in `qml/` — every clickable primitive in this codebase (`HnIconButton`, `HnSegmentedControl`'s segments,
`HnChoiceCard` via `T.CheckDelegate`, all top-level `Button`/`ItemDelegate`/`RadioButton` style implementations) is
built on a `QtQuick.Templates` button type, never a raw pointer-handling primitive. `HnIconButton` in particular
already proves the pattern for "visually circular/iconic, rectangular hit box" controls: it is `T.AbstractButton`
with a background `Rectangle` whose corner radius is computed, not a `MouseArea`. Using `T.AbstractButton` per
swatch here means:

- Hover/pressed (`hovered`, `down`) come for free and are already the vocabulary every other delegate's background
  binding reads (`segment.down`, `segment.hovered` in `HnSegmentedControl`).
- **Disabled cascade is automatic and requires no code**: `QQuickItem::isEnabled()` already factors in ancestor
  `enabled` state, so when `root.enabled` is `false`, every child `T.AbstractButton` swatch is implicitly
  non-interactive (`swatch.enabled` reads `false`) without `HnColorPicker` writing `enabled: root.enabled` on each
  delegate — this satisfies REQ-F-028/029/030 "for free," the same way `HnSegmentedControl`'s segments never
  explicitly forward `enabled` either.
- `Accessible.role`/`Accessible.checked` integrate cleanly with `T.AbstractButton`'s existing accessible-attached
  properties, rather than having to hand-roll accessibility on a plain `Item`.

**Click → selection**: `onClicked: root.activate(swatch.index)`; `activate(index)` sets `root.selectedColor =
root.colors[index]` and emits `colorSelected(root.colors[index])` (REQ-F-009/010/011) — entirely inside
`HnColorPicker`, no parent wiring required.

**Keyboard model — flagged conflict, resolved.** REQ-C-034 asks the arrow-key implementation to "follow the same
pattern as `HnSegmentedControl.qml`," but `HnSegmentedControl`'s pattern is **single-phase**: `Keys.onLeftPressed:
activate(Math.max(0, currentIndex - 1))` — the arrow key immediately changes the selection, there is no
"focused-but-not-selected" intermediate state, and no `Space`/`Enter` step exists at all in that control. REQ-F-032
and REQ-F-033, however, explicitly describe a **two-phase** model with concrete acceptance criteria: pressing an
arrow key only "moves focus" (their own AC never mentions `selectedColor` changing), and only a subsequent
`Space`/`Enter` "changes `selectedColor`… and emits `colorSelected`." These two requirements cannot both be
satisfied by literally copying `HnSegmentedControl`'s single-phase `activate`-on-arrow code. **Resolution**: keep
`HnSegmentedControl`'s code *shape* (root-level `Keys.onLeftPressed`/`Keys.onRightPressed` with
`Math.max(0, …)`/`Math.min(count-1, …)` clamping, no wraparound) but split it into two named helpers instead of
one:

- `moveFocus(index)` — clamped, updates an **internal** `property int focusedIndex` only; never touches
  `selectedColor` or emits `colorSelected`. Bound to `Keys.onLeftPressed`/`Keys.onRightPressed`.
- `activate(index)` — sets `selectedColor` + emits `colorSelected`. Bound to `onClicked` (mouse) *and*
  `Keys.onSpacePressed`/`Keys.onReturnPressed`/`Keys.onEnterPressed` (keyboard commit).

This keeps REQ-C-034's "internal `activate(index)` or equivalent helper function" intact (both helpers exist, and
`activate` is literally named `activate` as the segmented control's is) while actually satisfying REQ-F-032/033's
explicit two-step acceptance criteria. This tension between REQ-C-034 and REQ-F-032/033 should be called out to
the spec owner; this document resolves it in favor of the more specific, testable REQ-F-032/033 ACs.

**Focus is single-stop, not per-swatch.** Exactly like `HnSegmentedControl`, only the root `T.Control` has
`activeFocusOnTab: true`; individual swatch `T.AbstractButton`s are never Tab-reachable themselves (default
`activeFocusOnTab: false`, unchanged). `root.focusedIndex` is a plain internal pointer (default `0`) that the
root's `Keys` handlers move; each delegate paints its own focus ring by comparing
`index === root.focusedIndex && root.visualFocus` (§4(this uses `visualFocus`, the `T.Control`-provided property
that also encodes the platform's "only show focus rings after keyboard interaction" policy — the same property
`HnSegmentedControl.background.border.color: root.visualFocus ? … ` and `HnChoiceCard.background`'s third
`Rectangle` already key off).

---

## 6. Disabled State Design

`root.enabled: false` requires zero explicit propagation code (see §5): QQuickItem's implicit enabled-inheritance
already disables every `T.AbstractButton` swatch and suppresses their `onClicked`/key handling. Visually:

- Swatch fill dims via `swatch.enabled ? swatch.modelData : Qt.rgba(r, g, b, 0.4)` — alpha-blending each swatch's
  *own* color down to 40% rather than mapping to a fixed `HoloniightPalette.textDisabled`-style token, because the
  swatches' entire visual identity is their individual hue; flattening every swatch to one disabled gray would
  defeat the purpose of a color picker (REQ-F-029's "dimmed appearance," read as "muted, still identifiably that
  hue" rather than "replaced by a neutral disabled token").
- Ring/glow/checkmark visibility is unaffected by `enabled` — they are still governed purely by `selected`; a
  disabled-but-previously-selected swatch keeps showing its ring/tick, just at the dimmed base color, which is the
  expected "read-only, shows current state" treatment for a disabled control.
- No swatch is individually enabled/disabled — there is no per-swatch `enabled` property or QML API surface at all
  (REQ-F-030, Non-Goal 1).

---

## 7. Sizing

- Swatch diameter = `HnControlMetrics.controlHeight(root.resolvedSizeRole)`: 28/32/40/52 px for
  Compact/Normal/Large/Hero (REQ-F-013), same lookup every other sized Holonight control uses.
- `sizeRole` normalization via `HnControlMetrics.normalizedSizeRole()` — an invalid value (e.g. `99`) silently
  resolves to `Normal` (REQ-F-014), matching `HnSearchField`/`HnIconComboBox`'s already-tested `resolvedSizeRole`
  contract (see `tests/test_qml_smoke.cpp`'s `Controls_SearchFieldSizesAndInvalidFallback` for the exact pattern
  to mirror in HnColorPicker's own test).
- Control `implicitHeight` = swatch diameter exactly (REQ-F-015); no extra padding/margins are added around the
  row vertically, since `Control.topPadding`/`bottomPadding` default to `0` and are not set otherwise.
- `implicitWidth` forwards the unclipped row width so a standalone instance shows all swatches without scrolling
  by default; a container (`Layout.fillWidth: true`, or `HnSettingsRow`'s non-fillWidth
  `Layout.alignment`-only slot) is free to constrain it narrower, at which point `H.ScrollView` takes over
  (§3).

---

## 8. Accessibility

- Root: `Accessible.role: Accessible.Grouping` (REQ-A-035) — literally `Accessible.Grouping`, same value
  `HnSegmentedControl.qml` already sets on its own root, since both are "a row of related selectable things."
- Each swatch: `Accessible.role: Accessible.RadioButton` (REQ-A-036), plus `Accessible.checked: swatch.selected` so
  assistive tech reports selection state per-item (T.AbstractButton exposes `Accessible.checked` as a writable
  attached property; RadioButton semantics expect it).
- **Accessible name — decision: hex string.** REQ-A-037 leaves the concrete scheme open ("hex value, a named color
  label, or an index"). There is no color-naming utility anywhere in this codebase (`grep` across `qml/`, `src/`
  found nothing resembling a name-from-hex lookup), and building one is out of scope for a color-swatch control.
  **Decision: `Accessible.name: swatch.modelData.toString()`** — QML's `color.toString()` yields a stable
  `"#rrggbb"` (or `"#aarrggbb"` when alpha ≠ 1) string, which is simple, deterministic, requires no new
  infrastructure, and is enough for a screen reader to distinguish swatches ("radio button, pound f f zero zero
  zero zero, checked"). This is not maximally friendly, but per REQ-A-037's own AC wording ("hex value… **or**") it
  satisfies the letter of the requirement without inventing an unrequested color-naming feature. Flag this choice
  to design review if a friendlier name (e.g. "Red") is later desired — it would require a new small utility, not
  present today.

---

## 9. Module Registration

Add the file to `qml/CMakeLists.txt`'s `HOLONIGHT_CONTROLS_PUBLIC_QML_FILES` list (currently lines 74–98), e.g.
directly after `controls/HnSegmentedControl.qml`:

```cmake
set(HOLONIGHT_CONTROLS_PUBLIC_QML_FILES
    ...
    controls/HnSegmentedControl.qml
    controls/HnColorPicker.qml        # NEW
    controls/HnChoiceCard.qml
    ...
)
```

Nothing else needs manual editing — this project generates `qmldir`/`.qmltypes` via `qt_add_qml_module`
(`URI "Holonight.Controls"`, target `holonight_controls_qml`), and the surrounding `foreach` loop
(`qml/CMakeLists.txt:105-109`) automatically derives the `QT_RESOURCE_ALIAS` and appends the type name to the
`HOLONIGHT_CONTROLS_PUBLIC_TYPES` global property that `tests/CMakeLists.txt` asserts is non-empty. The `install()`
call at the bottom of `qml/CMakeLists.txt` (lines 223–229) installs everything in
`HOLONIGHT_CONTROLS_PUBLIC_QML_FILES` verbatim, so no separate install-list edit is needed either. No new C++
`SOURCES`, no `qmldir` hand-editing (per CLAUDE.md's "this project uses Qt6 `qt_add_qml_module`, not a manually
maintained `qmldir`").

`HnColorPicker.qml` needs no new imports registered at the module level — `QtQuick.Shapes` (for the checkmark) is
already linked into `holonight_qml`'s and `holonight_controls_qml`'s dependency graph transitively via
`Qt6::QuickShapes` (see `target_link_libraries(holonight_qml PRIVATE … Qt6::QuickShapes …)` at
`qml/CMakeLists.txt:189-194`); `holonight_controls_qml` itself only explicitly links
`Qt6::Qml`/`Qt6::Quick`/`Qt6::QuickControls2` but transitively depends on `holonight_qml` via
`add_dependencies(holonight_controls_qml holonight_core_qml holonight_qml)`, and QML module resolution at runtime
goes through the import path, not the CMake link graph — `import QtQuick.Shapes` inside `HnColorPicker.qml` will
resolve as long as `Qt6::QuickShapes` is a build dependency somewhere reachable, which it already is. If a build
error surfaces here regardless, add `Qt6::QuickShapes` directly to `holonight_controls_qml`'s
`target_link_libraries` — cheap, safe fallback.

---

## 10. Testing Approach

Follow the existing single-file smoke-test pattern: **do not create a new test binary.** Every existing control
(`HnSegmentedControl`, `HnSettingsRow`, `HnSearchField`, `HnIconComboBox`, `HnTextArea`, `HnFormField`, …) has its
smoke tests added as new `TEST_F(QmlSmoke, …)` cases inside the single `tests/test_qml_smoke.cpp`, compiled into
the one `holonight_qml_smoke_tests` binary (`tests/CMakeLists.txt:165-199`). `HnColorPicker`'s tests should be new
`TEST_F(QmlSmoke, Controls_ColorPicker...)` cases appended to that same file — no `tests/CMakeLists.txt` changes
are needed at all, since the binary already:

- sets `HOLONIGHT_QML_IMPORT_PATH="${CMAKE_BINARY_DIR}/qml"` as a compile definition, consumed by
  `QmlSmoke::SetUp()`'s `engine_.addImportPath(QStringLiteral(HOLONIGHT_QML_IMPORT_PATH))` (per CLAUDE.md's
  documented MODULE-library constraint — `holonight_controls_qml` cannot appear in `target_link_libraries` because
  it is a `qt_add_qml_module` `MODULE_LIBRARY`);
- already declares `add_dependencies(holonight_qml_smoke_tests holonight_qml holonight_core_qml
  holonight_controls_qml)` (`tests/CMakeLists.txt:194-197`), preserving build ordering so the QML module is built
  before the test binary runs against it — this already covers `HnColorPicker.qml` once it's added to
  `holonight_controls_qml`'s `QML_FILES`, no new `add_dependencies` line needed.

Suggested cases (mirroring `Controls_SearchFieldSizesAndInvalidFallback`, `Controls_TextAreaNormalizesBoundsAnd…`,
and `Button_HoverEnabledAndStateColors`'s structure of "instantiate several variants inside one `Item`, assert via
`root->property(...)`"):

1. **Default colors** — instantiate with no `colors` set; assert `repeater`/model count is 7 and (via
   `Holonight::ThemeResolver::resolve(Holonight::ThemeConfig::defaults())`, the same `tok` pattern
   `Button_HoverEnabledAndStateColors` already uses) that each default swatch color matches
   `tok.accentCyan/accentBlue/accentViolet/accentYellow/success/warning/error` in that exact order.
2. **Empty colors** — `colors: []` renders 0 delegates, no error.
3. **Selection by equality + duplicates** — `colors: ["#FF0000","#00FF00","#FF0000"]`, `selectedColor: "#FF0000"`;
   assert both matching delegates report `selected: true` via `findChild`/property inspection, the middle one
   `false`.
4. **Click → signal + property** — instantiate standalone with no external handler, invoke the swatch's `clicked()`
   via `QMetaObject::invokeMethod` (same technique `Controls_SearchFieldClearActionRestoresFocusAndHonorsStates`
   uses on `clearButton`), assert `selectedColor` updated and a `colorSelected` spy fired
   (`QSignalSpy` on the root object, or a QML-side counter property incremented in `onColorSelected`, following
   this file's existing preference for QML-side counters like `propagatedEscapes` in
   `Controls_SearchFieldEscapeClearsThenPropagatesWhenEmpty`).
5. **Size roles** — Compact/Normal/Large/Hero instances assert `swatchDiameter`/`implicitHeight` equal
   28/32/40/52, and an invalid `sizeRole: 999` instance asserts `resolvedSizeRole === HnControlSize.Normal`.
6. **Disabled state** — `enabled: false`; assert clicking does not change `selectedColor` and does not emit
   `colorSelected` (reuse the invoke-clicked-then-assert-unchanged pattern).
7. **Keyboard navigation** — a `Window`-hosted instance (like `Controls_SearchFieldEscapeClearsThenPropagatesWhen…`
   and the `ComboBox` tests use `Window { visible: true }` + `QTest::keyClick`), asserting `Left`/`Right` move
   `focusedIndex` with clamping at both ends and no wraparound, and `Space`/`Return` on a given `focusedIndex`
   updates `selectedColor`/emits `colorSelected` without a prior click.

Test file: append to `tests/test_qml_smoke.cpp` (no new file). No `tests/CMakeLists.txt` edits required.

---

## 11. Key Decisions With Rationale

| # | Decision | Rationale | Alternative considered & rejected |
|---|---|---|---|
| 1 | Swatch delegate = `T.AbstractButton`, not `Item`+`MouseArea`/`TapHandler` | Zero uses of either primitive exist anywhere in `qml/`; every interactive element in this codebase is a QQC2 template type; gets hover/down/disabled-cascade/accessible-attached-properties for free (§5) | Bare `Item` + `TapHandler` — would require manually reimplementing hover, disabled-gating, and accessible-checked wiring that `T.AbstractButton` already provides |
| 2 | `colors` default declared with `:` (plain live binding), not wrapped in `readonly` | Must stay caller-overridable (REQ-F-001) *and* should react to palette hot-reload, matching every other palette-derived default in this codebase (`HnIconButton.normalColor`, etc.) | `readonly property list<color> defaultColors: [...]` + `colors: root.defaultColors` — adds indirection with no behavioral benefit and doesn't change overridability |
| 3 | Glow = flat oversized semi-transparent `Rectangle`, not `Qt5Compat.GraphicalEffects`/`MultiEffect` blur | Neither module is imported anywhere in this codebase; `HoloniightPalette` itself expresses "glow" as flat pre-blended tokens (`glowCyanSoft` etc.), not blurred layers — same visual-approximation philosophy | Real Gaussian blur via `MultiEffect` — extra module dependency, extra GPU cost, no precedent in this repo |
| 4 | Two-phase keyboard model (`moveFocus` for arrows, `activate` for Space/Enter/click), diverging from `HnSegmentedControl`'s single-phase "arrow immediately selects" | REQ-F-032/033's acceptance criteria are explicit and testable about focus-then-commit being two distinct steps; REQ-C-034's "same pattern as HnSegmentedControl" cannot be taken 100% literally without contradicting those ACs (see §5) | Literal single-phase copy of `HnSegmentedControl` — would fail REQ-F-032/033's stated ACs (Left/Right would change `selectedColor` immediately, no Space/Enter distinction possible) |
| 5 | Accessible name = `color.toString()` hex string | No color-naming utility exists in this codebase; inventing one is out of scope; REQ-A-037's own wording accepts hex as one of several valid options | A hand-built RGB→name lookup table — meaningfully out of scope, adds maintenance surface for a "nice to have" not required by the spec |
| 6 | Circles bypass `HnAppearance`/`HnShapeProfile`'s corner-style/chamfer system entirely (`radius: width/2` always) | SPEC.md repeatedly and unconditionally says "circle"/"ring around the circle"; Non-Goal 7 explicitly forecloses per-instance shape overrides | Route swatch shape through `HnAppearance.roundedRadius(HnSurfaceRole.Control, …)` like other controls' corners — would make swatches chamfered/squarish under a chamfer corner-style config, contradicting the spec's explicit "circle" language |
| 7 | `Row` (not `RowLayout`) hosts the swatches | Fixed-size, non-stretching children — exactly what `HnSearchField.qml`/`HnFormField.qml` already use `Row` for in this codebase | `RowLayout` with `Layout.preferredWidth`/`Layout.preferredHeight` pinned on each delegate — works but is needless Layout-engine overhead for items that never need to grow/shrink |

---

## 12. Known Risks

- **`list<color>` QML feature availability.** `list<basicType>` properties for value types (not just `list<Item>`)
  were introduced as a QML language feature in a Qt 6.4+ timeframe; the project requires Qt 6.11
  (`find_package(Qt6 6.11 …)`), which should safely postdate it, but this repo has **zero prior usage** of
  `list<`-typed properties anywhere (`grep -rn "property list<"` across `qml/`+`src/` returns nothing) — the
  closest existing pattern is `HnSegmentedControl.property var model: []`. Verify with a `qmllint`/build pass
  early in implementation rather than assuming; if `list<color>` is rejected by this Qt version's QML engine for
  any reason, the fallback is `property var colors: [...]` (JS array of QML `color` values), which loses static
  typing but is otherwise behaviorally identical for a `Repeater { model: … }` consumer.
- **Color equality on QML `color` values.** `color === color` comparisons in QML/JS compare the underlying `QColor`
  value including alpha; two colors that look identical but differ in alpha (e.g. one swatch given
  `"#FF0000"` and `selectedColor` set to `Qt.rgba(1,0,0,1)`) are equal, but a caller who sets `selectedColor` with
  a *different* alpha than the swatch entry (e.g. `Qt.rgba(1,0,0,0.99)`) will silently fail to match — worth an
  explicit test case, since REQ-F-006's "color value equality" is otherwise ambiguous about alpha strictness. There
  is no rounding/tolerance built into QColor equality, so this is exact-match-or-nothing by construction; document
  this behavior rather than attempting fuzzy matching, since the spec does not ask for fuzzy matching.
  - Also note: the QML-invalid `color()` value's `.valid` property (used in the `selected` guard, §4) — verify at
  implementation time that `color().valid` really does read `false` in this Qt/QML version rather than some other
  sentinel; this codebase has no prior test exercising an intentionally-invalid `color` property, so the exact
  runtime behavior at this Qt 6.11 vintage should be confirmed with a quick throwaway `qml6` REPL check before
  relying on it in the guard.
- **Small-diameter hit testing.** At `Compact` (28 px), the delegate's rectangular hit box (implicit in
  `T.AbstractButton`'s square `implicitWidth`/`implicitHeight`) is slightly larger than the painted circle's
  corners — clicks in the square's corners (outside the visual circle) will still register as a hit. This is
  standard/expected for every other "visually round, rectangular hit box" control in this codebase (e.g.
  `HnIconButton`) and is not a defect, but should be sanity-checked visually at `Compact` size where the
  discrepancy is proportionally largest.
- **`H.ScrollView` + `Repeater`-in-`Row` content sizing.** `H.ScrollView`'s `contentWidth` must be driven from the
  `Row`'s `implicitWidth` (not the `Row`'s bound `width`, which would just track the viewport and never overflow).
  `HnTextArea.qml`'s analogous case explicitly computes `contentWidth` from `editor.contentWidth` rather than the
  editor's `width` for the same reason (line 97-99) — get this backwards in `HnColorPicker` and REQ-F-025's
  scrolling silently never activates, since the `ScrollView` would believe content and viewport are the same size.
  Verify visually with 10+ swatches inside a 200px-wide container as the spec's own acceptance criterion demands.
- **`HnControlMetrics.controlHeight` values were designed for rectangular controls (28/32/40/52).** These may read
  as too small or too large once rendered as *circles with rings + glow* rather than rounded-rect backgrounds —
  a glow that extends `+12px` beyond a 28 px `Compact` circle nearly doubles its visual footprint and may collide
  with adjacent swatches at the default `internalSpacing(Compact) = 4px`. This needs a visual pass once built;
  the spec fixes the diameter to `controlHeight()` exactly (REQ-F-013), so if this reads poorly the fix is
  `Row.spacing`, not swatch diameter, since diameter is normatively fixed by the spec.
- **Scale transform on the checkmark `Shape`.** As flagged in §4(d), `PathSvg`'s coordinate space is not
  automatically rescaled to the containing `Shape`'s `width`/`height` — the pseudo-code's `M 5 13 L 10 18 L 19 6`
  path (authored for a 24×24 box) needs either a computed-at-runtime path string (interpolating each coordinate by
  `root.swatchDiameter / 48`) or an explicit `transform: Scale { xScale: width/24; yScale: height/24 }` on the
  `ShapePath`/`Shape`. Get this wrong and the tick renders at a fixed pixel size regardless of `sizeRole`,
  silently failing to scale across Compact/Normal/Large/Hero.
