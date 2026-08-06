# DESIGN: Settings and state controls

**Spec:** `docs/sdd/settings-state-controls/SPEC.md`

All four controls are QML composites in the existing `Holonight.Controls` target. They use styled HoloNight
controls, palette tokens, and `HnControlMetrics`; no C++ type or dependency is added.

`HnSettingsRow` uses an explicit grid whose columns and control row are selected by `stacked`, avoiding hidden
responsive behavior. Conditional loaders own leading and control components. A focus-scope forwards focus to the
loaded control.

`HnSectionHeader` uses a row for optional slots and wrapped text above a conditional semantic divider. The root has
no focus policy, so loaded actions keep their natural traversal.

`HnEmptyState` gives a custom graphic loader precedence over the default `HnIcon`. Empty and failed default icons
have zero layout extent. `HnLoadingState` composes the styled progress bar; its public normalized value is a
read-only derivation, while `running` owns indicator visibility and indeterminate animation.

Optional loaders are inactive when absent and all loaded-item access is guarded by loader readiness where behavior
depends on the item.
