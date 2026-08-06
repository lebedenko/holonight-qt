# DESIGN: Shared list and action controls

**Spec:** `docs/sdd/shared-list-action-controls/SPEC.md`

**Status:** Implemented

**Date:** 2026-07-29

## Components

| File | Responsibility |
|---|---|
| `qml/controls/assets/*.svg` | Four monochrome, tintable action glyphs |
| `qml/controls/HnIconButton.qml` | Semantic square action button using `AbstractButton` and `HnIcon` |
| `qml/MenuItem.qml` | Standard icon-group rendering and complete interaction-state presentation |
| `qml/controls/HnNavigationDelegate.qml` | Optional tooltip bound to actual title truncation |
| `qml/CMakeLists.txt` | Build/install registration |
| `tests/test_qml_smoke.cpp` | Observable QML contracts |
| `tests/test_package_install.cmake.in` | Installed type and asset availability |
| `examples/controls-gallery/Main.qml` | Visual and interaction examples |
| `docs/shared-controls-usage-guide.md` | Consumer API documentation |

## HnIconButton

The root is `T.AbstractButton`, retaining Qt's standard signals, keyboard activation, focus, and accessible
button semantics. `implicitWidth` and `implicitHeight` both equal
`HnControlMetrics.controlHeight(resolvedSizeRole)`, so layouts receive a square control without caller sizing.

The content is one accessibility-ignored `HnIcon`. Its size follows
`HnControlMetrics.iconSize(resolvedSizeRole)`. The standard `icon.color` group member is the only caller color
override: a non-transparent value becomes the icon's normal color. Otherwise normal, disabled, and pressed
states resolve to the shared palette colors through `HnIcon`.

The background is always present so its 80 ms color transition is stable. It paints transparent, `surfaceHover`,
or `surface`, in that priority after disabled handling. A separate border remains zero-width except for
`visualFocus`, ensuring focus remains visible over every fill.

## MenuItem layout

The content uses a mirrored `RowLayout` with dedicated check, icon, expanding text, and submenu-arrow columns.
Both leading columns are reserved by the style. This intentionally aligns labels for the common mixed-menu
case and permits a checkable item to have both a checkmark and an icon without precedence ambiguity. The
existing 180-by-28 implicit background and padding are retained.

The state priority is disabled, pressed, highlighted, hovered, normal. Pressed highlighted items use
`primaryPressed`; highlighted items use `primary`; pointer-only hover uses `surfaceHover`. Foreground roles
switch to `onPrimary` on primary fills and otherwise use primary/disabled text roles. No destructive semantic
role is introduced without a demonstrated second consumer.

## Tooltip

The navigation title `Label` receives a stable object name and its `truncated` property drives a `ToolTip`
attached to the delegate. Visibility requires the opt-in property, a non-empty title, delegate hover, and
actual truncation. The tooltip inherits the selected Qt Quick Controls style's delay and timeout contract and
does not add pointer handlers.

`HnListDelegate` and `HnActionDelegate` are deferred: their title/subtitle composition and multi-line height
contract differ, so equivalence has not been established.

## Verification

Run the controls QML lint target and focused `QmlSmoke` tests first, then package-install tests, the gallery
build, `git diff --check`, `task build`, and `task test`. Visual inspection covers fractional scaling, focus,
hover, press, disabled rendering, mixed menu alignment, long labels, and mirroring.
