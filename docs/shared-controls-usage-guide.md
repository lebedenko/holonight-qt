# HoloNight Shared Controls Usage Guide

## Consumption

Link `HolonightQt::Controls` in CMake and import the unversioned Qt 6 module:

```cmake
find_package(HolonightQt REQUIRED COMPONENTS Controls)
target_link_libraries(my_app PRIVATE HolonightQt::Controls)
```

```qml
import QtQuick.Controls as Controls
import Holonight.Core
import Holonight.Controls
```

`Holonight.Core` supplies `HoloniightPalette`, `HolonightTheme`,
`HnAppearance`, `HnControlSize`, `HnMetrics`, and `HnIcon`.

Embed an overridable `:/qtquickcontrols2.conf` default with `[Controls]` and `Style=Holonight` in each executable.
Use `Controls.Type` for standard controls, attached properties and enums; use `Holonight.Core` and
`Holonight.Controls` for their public APIs. Composites keep their HoloNight appearance while their standard-control
bases follow runtime selection. See [style selection and deployment](automatic-quick-controls-style-selection.md).

## Sizing and composition

Controls with `sizeRole` accept `HnControlSize.Compact`, `Normal`, `Large`, or
`Hero`. Prefer the semantic role over fixed pixel heights. Component slots own
the object they instantiate; use the corresponding read-only `...Item` property
only for inspection or focus coordination. Leave optional slots unset so their
loaders remain inactive.

## Selection decision matrix

| Pattern | Control | Treatment |
|---|---|---|
| Primary navigation | `HnNavigationDelegate` | Quiet fill plus accent edge |
| Dense or descriptive row | `HnListDelegate` | Quiet selected fill |
| Elevated selectable object | `HnCardDelegate` | Selected outline |
| Navigating command row | `HnActionDelegate` | List row with icon/chevron |
| Small mutually exclusive mode | `HnSegmentedControl` | Checked segment |
| Visual mutually exclusive choice | `HnChoiceCard` | Outlined card in `ButtonGroup` |

`highlighted`, `checked`, and `ListView.isCurrentItem` all indicate selection on
delegate controls. Do not calculate selection colors in application code.

## Controls

### Inputs and structure

- `HnSearchField`: semantic sizes, leading/trailing slots, clear action, error,
  read-only, and disabled states.
- `HnIconComboBox`: model-backed combo box with optional icon roles. Its popup
  shows at most eight entries by default; set `maximumVisibleItems` to choose a
  different limit without replacing the popup. Both it and the HoloNight standard ComboBox compensate
  their item-based popups for translated, uniformly scaled ancestors and expose
  `effectiveScale` for diagnostics. Rotation, shear, and non-uniform scaling are
  intentionally unsupported and reported by `popupTransformSupported`.
- `HnTextArea`: shared multiline editor with semantic bounds.
- `HnFormField`: label, required marker, helper/error text, and owned control.
- `HnSettingsRow`: title, description, leading/default control/trailing slots,
  stacking, semantic size, and optional divider.
- `HnSectionHeader`: title, description, two slots, divider, and optional compact
  presentation.
- `HnPanelHeader`: stronger panel title/description with two slots and divider.
- `HnHeaderBar`: fixed-height application header region with a content slot,
  padding, and an overlaid inset divider.
- `HnActionBar`: optional leading, centered, and trailing slots with divider.

### Selection and actions

- `HnIconButton`: square icon-only action using the standard `icon.source` and
  optional `icon.color`, with semantic `sizeRole` sizing.
- `HnNavigationDelegate`: `title`, `badgeText`, leading and trailing slots.
  Elided titles show the full value on hover by default; set
  `showTitleToolTipWhenElided: false` when another disclosure mechanism exists.
- `HnListDelegate`: `title`, `subtitle`, `metadata`, leading/trailing slots, and
  divider.
- `HnCardDelegate`: elevated title/subtitle/status card with two slots.
- `HnActionDelegate`: icon source, title, description, and optional chevron.
- `HnSegmentedControl`: model, `textRole`, `valueRole`, `currentIndex`,
  read-only `currentValue`, and `activated(index, value)`.
- `HnChoiceCard`: checkable title/description/thumbnail card.

### Feedback and compact information

- `HnStatusIndicator`: neutral, info, success, warning, or error; dot or icon
  plus text.
- `HnKeySequenceLabel`: frameless keyboard sequences with `keyGroups`, literal `text`, `font`,
  `color`, `wrap`, and read-only `accessibleText`. It has no background, frame, or padding.
  Bind `font` to an adjacent menu label for matching typography, and bind `color` to
  `HoloniightPalette.textMuted` / `textDisabled` according to the action’s enabled state.
  It uses the same symbols, separators, accessibility names, and wrapping as `HnKeyHint`.
- `HnKeyHint`: compact accessible keyboard hints with default monospace typography. Set `keyGroups` to an ordered array of combinations,
  for example `[[Qt.Key_Control, Qt.Key_Return], [Qt.Key_Tab]]`. Keys use `+` and alternatives use ` / `
  inside one badge. Shift, Tab/Backtab, Return/Enter, Backspace, Delete, Space and arrows use font-scaled
  vector symbols. Other keys remain text; punctuation keys are passed literally as `Qt.Key_Plus` or
  `Qt.Key_Slash`. Nonempty `keyGroups` takes precedence over the compatible literal `text` API.
  Set `wrap: true` and constrain the width to wrap alternatives, then long combinations between keys.
  Individual keys stay intact; allow enough width for the longest key plus its separator.
  `accessibleText` supplies translated readable names for parent action-label composition.
  Vertical/horizontal padding scales at 2/22 and 6/22 of resolved line height. Natural
  single-line badges are at least 1.2 times their height, with centered short content.
  Corner rounding follows appearance settings up to 2 logical pixels. Explicitly
  constrained multiline badges retain their wrapping contract.
  Use the resolved `font` to change typography; do not replace `contentItem` or resize individual symbols.
- `HnEmptyState`: empty-result graphic, title, description, and action.
- `HnLoadingState`: indeterminate or normalized progress presentation.
- `HnSeparator`: horizontal or vertical pixel-aligned divider with solid,
  both-edge, start-edge, or end-edge fade profiles.
- `HnSurfaceFrame`: semantic surface, border, corner, and shape framing.
- `HnApplicationWindow`: application window using shared appearance behavior. Native controls inherit live
  appearance defaults, including when a window is created during a theme transition. Explicit application,
  window, control and color-group palette roles take precedence; resetting a role restores inheritance.
  Shared composites retain their own appearance bindings. The Core `HnWindowPalette` helper is implementation
  support for the shared window, not a new application palette-selection API. It does not change the
  application palette or the palette policy of plain Qt windows.
- `HnAppTitle`: semantic HoloNight brand and application title with an optional icon.

Common `more-vertical`, `edit`, `delete`, and `folder` glyphs are installed at
`qrc:/qt/qml/Holonight/Controls/assets/<name>.svg`. Use these URLs with
`HnIcon`, `HnIconButton.icon.source`, or the standard `MenuItem.icon.source`.
Icons are decorative; put the translated accessible name on the containing
button or menu item.

`HnAppTitle` keeps the HoloNight brand presentation consistent while allowing
the root item to participate in anchors, manual positioning, or a Qt Quick
Layout. The application name is required and should be translated. An empty or
failed icon source reserves no space. Set `skipBranding: true` to omit the
"HoloNight" brand text and its layout spacing.

```qml
HnAppTitle {
    applicationName: qsTr("Settings")
    iconSource: "qrc:/qt/qml/Holonight/Controls/assets/folder.svg"
    iconTinted: true
    Layout.fillWidth: true
}
```

```qml
HnIconButton {
    icon.source: "qrc:/qt/qml/Holonight/Controls/assets/more-vertical.svg"
    Accessible.name: qsTr("Conversation actions")
}

Controls.MenuItem {
    text: qsTr("Rename")
    icon.source: "qrc:/qt/qml/Holonight/Controls/assets/edit.svg"
}
```

For a stacked `HnSettingsRow`, the row owns the loaded control root's geometry
and gives it the complete inner width, including the area below optional leading
content. Compound controls should use a layout root and vertically align
layout-managed `Switch`, `Slider`, and value items with
`Layout.alignment: Qt.AlignVCenter`. The row does not override layout attached
properties on caller-owned children. A single tab-focusable control receives
forwarded row focus; a non-focusable compound layout adds no Tab stop, so its
actionable children retain their natural keyboard order.

`HnSeparator` defaults to a horizontal `borderPassive` line, one physical pixel thick.
Set integer `thickness: 2` for two physical pixels; do not divide by DPR. Use inherited `opacity`
for strength. `FadeStart` reaches full strength at the center, `FadeEnd` stays full through the
center, and `FadeBoth` fades to transparent at both ends. Color alpha and ancestor opacity compose normally.

Anchors, layouts or explicit dimensions determine length. Default minor occupancy is the logical
physical thickness; an explicit width/height defines a slot without stretching the stroke.
Use `crossAxisAlignment: HnSeparator.Trailing` for bottom/right boundaries, `Leading` (default)
for top/left, and `Center` for a centered stroke in a larger slot. Connect rules using shared logical
boundaries, with one owner at intersections; stop a column rule at the bottom rule's top.

Both endpoints and the stroke boundary snap to the same physical grid. Built-in positive/negative
`Item.scale`, ancestor movement/resizing, reparenting and window DPR notifications update geometry.
Rotation, shear and custom transform-object updates are outside the crispness guarantee. Avoid tight
fractional clips around boundary painting: an ancestor clip can truncate a snapped edge.

## Model-backed examples

```qml
ListView {
    model: pageModel
    currentIndex: 0
    delegate: HnNavigationDelegate {
        required property string label
        required property int index
        width: ListView.view.width
        title: label
        onClicked: ListView.view.currentIndex = index
    }
}

HnSegmentedControl {
    model: [
        { text: qsTr("List"), value: "list" },
        { text: qsTr("Grid"), value: "grid" }
    ]
    onActivated: (index, value) => viewMode = value
}

Controls.ButtonGroup { id: themeChoices }
Row {
    HnChoiceCard {
        title: qsTr("Dark")
        Controls.ButtonGroup.group: themeChoices
    }
    HnChoiceCard {
        title: qsTr("Light")
        Controls.ButtonGroup.group: themeChoices
    }
}
```

## Keyboard and accessibility

Delegates and choice cards retain native button Space/Enter activation.
Keyboard focus uses the focus-border token and never erases selection.
Segmented controls use Left/Right arrows and emit the same activation signal as
pointer input. Supply translated titles and descriptions; these become
accessible names and descriptions. `HnKeyHint` is descriptive—it does not
register an application shortcut.

## Application boundary

Use shared controls for repeatable visual and interaction contracts. Keep
routing, commands, network operations, domain statuses, responsive shell policy,
three-pane layouts, chat/AI messages, and HUD decoration in the application.
Pass those outcomes into shared control properties and signals.

## Theme, scaling, and localization

Use palette roles rather than literal colors, semantic sizes rather than fixed
heights, and layouts rather than hand-calculated widths. Test at fractional scale
factors and in both light and dark schemes. Wrap application-visible strings in
`qsTr()`, allow title/description labels to grow or elide as documented, and do
not assume shortcut text or translated labels have a fixed width.

## Window input authority

`Holonight.Core` provides `HnInputInteraction` attached to an Item or Window.
`hoverAllowed` gates decorative hover feedback; keep selected, checked, pressed
and focus states visible independently. State is shared by items in the same
window and is independent between windows.

Non-modifier key presses suppress hover. Changed pointer screen coordinates
restore it; entering an item, scrolling, layout and animation do not. Pointer
presses continue to activate controls immediately. A keyboard-oriented surface
should call `HnInputInteraction.suppressHover()` each time it opens. Consumers
that manage selection can handle `HnInputInteraction.onPointerMoved` and hit-test
the delivered `scenePosition` against their current layout using `mapFromItem(null,
scenePosition.x, scenePosition.y)`. Do not use `onHoveredChanged` to authorize
selection: it can fire with a stationary pointer. The observer never consumes input.

HoloNight standard controls and owned composites use this policy for hover
rendering. Fusion keeps its standard-control rendering. An unattached item has no
window authority; suppression requested before attachment applies when attached.

### Semantic SVG icon colors

`HnIcon` supports KDE's eight exact class tokens: `ColorScheme-Text`,
`ColorScheme-Accent`, `ColorScheme-Background`, `ColorScheme-Highlight`,
`ColorScheme-HighlightedText`, `ColorScheme-PositiveText`,
`ColorScheme-NeutralText`, and `ColorScheme-NegativeText`. Declare their `color`
inside `<style id="current-color-scheme">` and use `currentColor` on the assigned
shapes. Other styles, fixed paints, gradients and layer opacity remain authored.
Assigned semantic classes, including Accent alone, suppress the whole-image
symbolic mask even when an omitted color leaves their declarations untouched.

Pass a control's Qt Quick palette with `paletteContext: owner.palette`. Core's
adapter observes application palette events, supplied palette changes and appearance
updates without importing a Controls style. QML contexts use WindowText as the
foreground. Explicit foreground roles take precedence
over token defaults; `normalColor`, `mutedColor`, `disabledColor` and `activeColor`
remain available as direct overrides. Accent and Highlight are independent even
though the default palette assigns both the primary token. Override individual roles
with `accentColor`, `backgroundColor`, `highlightColor`, `highlightedTextColor`,
`positiveColor`, `neutralColor` and `negativeColor`. Disabled equivalents are
`disabledAccentColor`, `disabledBackgroundColor`, `disabledHighlightColor`,
`disabledHighlightedTextColor`, `disabledPositiveColor`, `disabledNeutralColor`
and `disabledNegativeColor`. Item composites (`HnAppTitle`, `HnEmptyState`,
`HnStatusIndicator`) forward an optional `paletteContext`; control composites
forward their owning palette automatically.

Normal uses the context foreground, Window background, Highlight selection color,
HighlightedText selection foreground, independent Accent, and appearance success,
warning and error tokens. Muted and Active change the foreground through their
existing override properties. `HnIcon.Selected` is enum value 4; existing values
0–3 are unchanged. Selected maps Text, Highlight and status roles to HighlightedText,
and maps Background and HighlightedText to Highlight. Accent blends 15% toward
HighlightedText while retaining its alpha, following
[KDE's selected-state mapping](https://github.com/KDE/kiconthemes/blob/master/src/kiconcolors.cpp).
Combo-box highlighted popup icons use Selected; disabled takes precedence.
Pressed buttons remain Active. Qt's `QIcon::Active` and `QIcon::On` do not select.

Disabled uses disabled-group palette roles. Default Accent and status colors blend
50% toward the background, retaining source alpha. This HoloNight token policy
preserves decorative layers and does not apply KDE's whole-image disabled effect.
The Qt icon engine resolves the live application palette and appearance tokens on
rendering, using Text as its foreground. `QIconEngine` has no owning-widget palette
context; QML can supply that context explicitly.

`HnIconProvider.sourceUrl(source, size, color, highlight, positive, neutral,
negative, revision, semantic)` retains its original signature. New callers use
`sourceUrlWithOptions(source, size, options)`: `color`, `highlight`, `positive`,
`neutral`, `negative`, `accent`, `background`, `highlightedText`, optional
`disabledColor`, `disabledHighlight`, `disabledPositive`, `disabledNeutral`,
`disabledNegative`, `disabledAccent`, `disabledBackground`,
`disabledHighlightedText`, `state`, `revision`, `semantic` and `dpr`.
State is resolved once before the eight RGBA colors are serialized. Old URLs
retain white fallbacks for invalid/missing original five colors; missing or invalid
new colors leave authored declarations unchanged. Their state is already represented
by serialized colors, so image decoding never applies the state a second time.

URLs include every resolved RGBA color, revision, state, DPR and source digest.
Rendered caches also include source content, physical dimensions and symbolic mode.
Accent-only, alpha-only and selection changes therefore update displayed icons
without renaming sources or clearing caches. Asset lookup uses logical size and DPR;
Qt Quick's requested physical render size is used directly. `Original` bypasses
semantic recoloring and state effects and renders authored SVG bytes.
