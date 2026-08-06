# Shared list and action controls implementation brief

**Status:** Proposed
**Date:** 2026-07-29
**Source:** Review of recent `holonight-ai` conversation-list work

## Objective

Move the reusable visual and interaction contracts discovered in the `holonight-ai` conversation list into
`holonight-qt`, while keeping conversation-specific rename, delete, activation, and model behavior in the
application.

This work should deliver:

1. A reusable icon-only button.
2. Native leading-icon support in the HoloNight `MenuItem` style.
3. Shared common action-icon assets rendered through `HnIcon`.
4. Optional automatic tooltips for elided shared-delegate titles.

The implementation should be completed and verified in `holonight-qt` before a separate downstream migration of
`holonight-ai`.

## Ownership boundary

| Concern | Owner |
|---|---|
| Icon-button sizing, focus, hover, pressed, disabled, and icon rendering | `Holonight.Controls` |
| Standard menu-item icon layout and interaction states | `Holonight` QQC2 style |
| Generic action icons and semantic recoloring | `Holonight.Core` assets and `HnIcon` |
| Elided-title tooltip policy for shared delegates | `Holonight.Controls` |
| Conversation rename/delete state machine and signals | `holonight-ai` |
| Switching time metadata to an actions button on hover | `holonight-ai` |
| Inline rename and inline delete confirmation layout | `holonight-ai` |

Do not add an `HnEditableNavigationDelegate` in this cycle. There is only one demonstrated workflow, and its
state transitions are application-specific.

## 1. `HnIconButton`

### Location and type

Add `qml/controls/HnIconButton.qml` to the `Holonight.Controls` module.

The control should derive from the styled HoloNight button or an appropriate Qt template while preserving
standard button keyboard, focus, accessibility, and signal behavior. Applications should not need to replace
`contentItem` or `background` to obtain the standard icon-only appearance.

### Proposed public contract

Use the standard `AbstractButton.icon` API instead of accepting raw SVG path strings.

```qml
HnIconButton {
    icon.source: "qrc:/icons/more-vertical.svg"
    Accessible.name: qsTr("Conversation actions")
}
```

Required behavior:

- `sizeRole` defaults to `HnControlSize.Normal`.
- Width and height resolve from `HnControlMetrics.height(sizeRole)`.
- Icon size resolves from `HnControlMetrics.iconSize(sizeRole)`.
- The normal background and border are transparent.
- Hover displays `HoloniightPalette.surfaceHover`.
- Pressed displays `HoloniightPalette.surface`.
- Keyboard focus displays the semantic focus border without introducing a normal-state border.
- Disabled state uses the shared disabled icon color and opacity conventions.
- The icon is centered at all semantic sizes.
- Space and Enter activation, `clicked`, `pressed`, `released`, focus policy, and accessibility follow standard
  button behavior.
- A caller can override the semantic icon color without replacing the content item.
- Color transitions follow the existing 80 ms control transition convention.

Suggested optional properties:

```qml
property int sizeRole: HnControlSize.Normal
property int iconState: enabled ? HnIcon.Normal : HnIcon.Disabled
property color iconColor
```

Prefer the smallest API that composes naturally with `HnIcon`. Avoid simultaneously exposing multiple
overlapping color mechanisms through both `icon.color` and a custom property.

### Acceptance criteria

- No fixed 16 px icon is used when a semantic icon metric is available.
- The control remains square without requiring caller width/height bindings.
- Focus remains visible in transparent, hovered, and pressed states.
- The accessible name can be supplied normally and the decorative `HnIcon` is ignored by accessibility.
- Gallery examples cover Compact, Normal, Large, disabled, hovered/pressed where practical, and keyboard focus.

## 2. Icon-capable HoloNight `MenuItem`

### Location and approach

Extend `qml/MenuItem.qml`, the QQC2 style implementation. Do not add an application-specific menu-item
subclass solely to display an icon.

Use the standard `MenuItem.icon` group so application code remains portable:

```qml
MenuItem {
    text: qsTr("Rename")
    icon.source: "qrc:/icons/edit.svg"
}

MenuItem {
    text: qsTr("Delete")
    icon.source: "qrc:/icons/delete.svg"
}
```

### Required behavior

- Render a supplied icon in the leading column.
- Preserve checkable-item checkmarks.
- Define and test the behavior when an item is both checkable and has an icon. Prefer separate check and icon
  columns if this matches Qt platform conventions; otherwise document the precedence rule.
- Keep labels aligned across menu items with and without icons or checkmarks.
- Use semantic icon sizing, normally the Compact icon metric for the current compact menu height.
- Propagate enabled, highlighted, hovered, and pressed colors consistently to icon and label.
- Preserve submenu-arrow behavior and adequate trailing space.
- Keep the existing menu width and padding behavior unless tests or gallery inspection identify a layout defect.
- Do not make every menu item reserve unused icon space unless alignment within a mixed menu requires it.

### Interaction-state correction

The style should own all background states. Application delegates must not duplicate them.

Recommended state order:

1. disabled
2. pressed
3. highlighted/current
4. hovered
5. normal

Use `HoloniightPalette.primaryPressed` for an actively pressed highlighted item if that is consistent with other
controls. Use `HoloniightPalette.primary` for highlighted/current and `surfaceHover` for a pointer-only hover.
Keep text and icon contrast correct for every background.

### Destructive actions

First implement icon support without requiring a destructive-menu API. If delete actions in two or more
consumers need a shared contract, add a small semantic role rather than application background overrides, for
example:

```qml
property bool destructive: false
```

If added, define its complete normal, hover, highlighted, pressed, disabled, text, and icon color behavior and
cover it with tests. Do not add it only to color one icon red while leaving the interaction contract ambiguous.

### Acceptance criteria

- A menu item with `icon.source` needs no custom `contentItem`.
- Hover and press visuals are provided by the style.
- Text, icons, checkmarks, and submenu arrows remain vertically centered.
- Mixed icon/no-icon/checkable menus have intentional, documented alignment.
- Long translated labels and right-to-left mirroring do not overlap the submenu arrow.

## 3. Common icon assets

### Scope

Add only broadly reusable UI glyphs to the canonical shared asset location used by `Holonight.Core`:

- `more-vertical`
- `edit`
- `delete`
- `folder`

Consider adding `add`, `close`, `settings`, and `help` only if this cycle also migrates existing shared examples
or demonstrates a second consumer. Avoid turning this task into a complete icon-library project.

### Asset requirements

- Use clean SVG assets with a consistent view box, stroke/fill language, optical size, and line caps.
- Keep source geometry monochrome and compatible with `HnIcon` semantic tinting.
- Do not embed palette colors in source SVGs.
- Package and install assets with the owning QML module.
- Use stable resource URLs from public examples and tests.
- Verify icons at Compact and Normal semantic sizes and at fractional display scaling.
- Add accessible names to the buttons/menu items, not to decorative icon instances.

Raw runtime `PathSvg` strings are not part of this public contract. `HnIcon` remains the shared rendering API.

## 4. Elided-title tooltips for shared delegates

### Scope

Extend `HnNavigationDelegate` first. The internal title label already elides, so the shared control should be
able to disclose the complete title without an application replacing its `contentItem`.

Proposed property:

```qml
property bool showTitleToolTipWhenElided: true
```

Required behavior:

- Show the complete `title` only when the internal label is actually truncated.
- Follow the shared `ToolTip` delay, timeout, and visual style.
- Trigger from delegate hover without intercepting clicks.
- Do not show an empty or redundant tooltip for an untruncated label.
- Keep the behavior optional for consumers that provide a different disclosure mechanism.
- Preserve accessibility: the full title remains the delegate accessible name regardless of visual truncation.

After implementing and validating `HnNavigationDelegate`, assess `HnListDelegate` and `HnActionDelegate`.
Apply the same contract only if their title truncation behavior is equivalent; avoid introducing inconsistent
property names.

## Implementation sequence

1. Add the common icons and confirm their `HnIcon` resource URLs.
2. Implement `HnIconButton` using semantic metrics and shared icons.
3. Extend the styled `MenuItem` to consume its standard icon group.
4. Add elided-title tooltip support to `HnNavigationDelegate`.
5. Register new QML and asset files in `qml/CMakeLists.txt`.
6. Add contract tests and controls-gallery examples.
7. Update `docs/shared-controls-usage-guide.md` with the new APIs.
8. Verify source-tree and install-tree QML imports.
9. In a later `holonight-ai` session, replace `SidebarIconButton`, menu overrides, raw common icon paths, and the
   local title tooltip.

Keep the four implementation areas in focused commits if practical. `MenuItem` is a style change, whereas
`HnIconButton` and delegate behavior belong to the rich controls module.

## Expected file changes

Likely additions:

```text
qml/controls/HnIconButton.qml
qml/controls/assets/more-vertical.svg
qml/controls/assets/edit.svg
qml/controls/assets/delete.svg
qml/controls/assets/folder.svg
```

Likely modifications:

```text
qml/MenuItem.qml
qml/controls/HnNavigationDelegate.qml
qml/CMakeLists.txt
examples/controls-gallery/Main.qml
tests/test_qml_smoke.cpp
docs/shared-controls-usage-guide.md
```

Place icons under the module that owns their public URLs. Adjust the illustrative paths above if the current
CMake resource organization requires Core-owned assets elsewhere.

## Verification

Run the narrowest checks first:

```bash
qmllint <each changed QML file with the repository's configured import paths>
git diff --check
```

Then run project verification:

```bash
task build
task test
```

Also perform gallery inspection for:

- All semantic icon-button sizes.
- Pointer hover and press.
- Keyboard tab focus and activation.
- Enabled and disabled states.
- Menu items with no icon, an icon, a checkmark, icon plus checkable state, and a submenu.
- Long translated labels.
- Light and dark schemes.
- Fractional scale factors such as 1.25 and 1.5.
- Elided and non-elided navigation titles.

## Downstream migration target

After this work is installed, update `holonight-ai`:

- Replace `qml/workspace/SidebarIconButton.qml` and compatible `HeaderAction` uses with `HnIconButton`.
- Replace inline SVG paths for common actions with packaged `HnIcon` sources.
- Remove custom `contentItem` and `background` blocks from conversation menu items.
- Remove the local truncated-title tooltip once `HnNavigationDelegate` owns it.
- Keep the conversation delegate's edit/delete state machine and content swapping local.

The migration is complete when the conversation list retains its current appearance and behavior while
application code contains no duplicated icon-button or menu interaction styling.
