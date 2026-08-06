# Theme Typography Usage Inconsistency

## Summary

Typography changes loaded by `HolonightTheme` do not reach every HoloNight control. Components that use `HnLabel`
update when the theme is reloaded, while several shared controls render text with a plain `Label`, `Text`, or an
unconfigured Qt Quick Templates `font` property. Those elements therefore keep the application or platform default
font.

This is visible in `holonight-settings` after changing the interface font or base size and applying the configuration:
page headings and other `HnLabel` instances update, but navigation entries, settings-row titles and descriptions,
footer status text, and buttons do not.

## Expected behavior

All shared HoloNight controls should derive their default typography from `HolonightTheme`. After
`HolonightTheme.reload()`, already-created controls should update their font family and semantic point size unless a
consumer has explicitly overridden their font.

## Current behavior and cause

`HnLabel` binds its font family and point size to `HolonightTheme`, so its bindings react to the singleton's
`themeChanged` notification.

The following shared controls bypass that path:

- `qml/controls/HnNavigationDelegate.qml` uses plain `Label` instances for its title and badge.
- `qml/controls/HnSettingsRow.qml` uses plain `Label` instances for its title and description.
- `qml/controls/HnStatusIndicator.qml` uses a plain `Label` for status text.
- `qml/Button.qml` passes `root.font` into `IconLabel`, but the template root has no theme-backed font default.
- Other template-based controls follow the same `font: root.font` pattern and should be audited, including combo
  boxes, item delegates, text fields, switches, check boxes, radio buttons, menu items, tab buttons, spin boxes,
  tooltips, and text areas.

The configuration reader and `HolonightTheme.reload()` are not the source of this discrepancy. Reload correctly
updates properties used by existing reactive bindings; the affected controls simply have no such bindings.

## Ownership

The defect belongs in `holonight-qt`, because these are shared control defaults. Adding bindings at individual
`holonight-settings` call sites would create application-specific workarounds and leave every other consumer
inconsistent.

## Recommended direction

1. Define a theme-backed default font on each public template control that exposes `font`, while preserving explicit
   consumer overrides.
2. Replace internal plain labels with `HnLabel` where semantic roles map cleanly to the intended presentation.
3. Assign explicit semantic roles to compound-control text, such as body text for navigation and row titles and
   caption text for descriptions or secondary status text where appropriate.
4. Audit all exported QML controls for plain `Label`/`Text` usage and unbound template font properties.
5. Add integration tests that instantiate each representative control, call `HolonightTheme.reload()`, and verify that
   existing text items change family and point size.

Care is needed when setting template defaults: assigning a complete `font` value in a nested content item can mask
consumer overrides or break property bindings. The theme default should live at the public control boundary, and
internal content should continue to consume that public `font` unless it intentionally uses another semantic role.

## Acceptance criteria

- Navigation titles, settings-row titles and descriptions, status text, and button labels update after a successful
  theme reload.
- Existing controls update without recreation.
- Explicit per-instance font overrides remain supported.
- Font family and size use the appropriate `HolonightTheme` semantic values.
- Representative tests cover both theme defaults and explicit overrides.
