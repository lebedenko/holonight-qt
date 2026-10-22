# Directional panel focus

HnBreadcrumbBar owns segment delegates, so consumers must not inspect its child tree
to focus a segment. The additive `focusCurrentSegment(): bool` API resolves the last
repeated row internally and focuses its button with Qt.TabFocusReason. It never
emits segmentActivated. Disabled, hidden, empty and unavailable targets are no-ops.
The final segment remains the target when ancestors collapse or only root exists.

Provider smoke tests cover focus, visualFocus, collapse, root and no-op conditions.
[Files design](../../../holonight-files/docs/sdd/directional-panel-focus.md) defines routing.
