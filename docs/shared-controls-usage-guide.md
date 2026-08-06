# HoloNight Shared Controls Usage Guide

## Consumption

Link `HolonightQt::Controls` in CMake and import the unversioned Qt 6 module:

```cmake
find_package(HolonightQt REQUIRED COMPONENTS Controls)
target_link_libraries(my_app PRIVATE HolonightQt::Controls)
```

```qml
import Holonight.Core
import Holonight.Controls
```

`Holonight.Core` supplies `HoloniightPalette`, `HolonightTheme`,
`HnAppearance`, `HnControlSize`, `HnControlMetrics`, and `HnIcon`.

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
  different limit without replacing the popup.
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
- `HnKeyHint`: compact accessible shortcut text.
- `HnEmptyState`: empty-result graphic, title, description, and action.
- `HnLoadingState`: indeterminate or normalized progress presentation.
- `HnSeparator`: horizontal or vertical pixel-aligned divider with solid,
  both-edge, start-edge, or end-edge fade profiles.
- `HnSurfaceFrame`: semantic surface, border, corner, and shape framing.
- `HnApplicationWindow`: application window using shared appearance behavior.
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

MenuItem {
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

`HnSeparator` defaults to a horizontal `borderSubtle` line using the semantic
`separatorWidth`. Set `orientation: Qt.Vertical` for a vertical divider and use
`fadeMode: HnSeparator.FadeBoth`, `FadeStart`, or `FadeEnd` when a divider
should blend into adjacent content. `centerOpacity` and `edgeOpacity` are
clamped to `0`–`1`. Give a horizontal separator an explicit width, or a
vertical separator an explicit height; the component aligns its painted
thickness to device pixels, including at fractional display scale factors.

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

ButtonGroup { id: themeChoices }
Row {
    HnChoiceCard {
        title: qsTr("Dark")
        ButtonGroup.group: themeChoices
    }
    HnChoiceCard {
        title: qsTr("Light")
        ButtonGroup.group: themeChoices
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
