# File-oriented presentation

Import `Holonight.Controls` alongside `QtQuick` and `Holonight.Core`.
See [interfaces and ownership](sdd/shared-file-ui-controls/README.md) and the
Lists & Selection gallery page for examples of all five components.

```qml
HnBreadcrumbBar {
    segments: navigation.labels
    onSegmentActivated: index => navigation.goToAncestor(index)
    onLocationEditRequested: locationEditor.forceActiveFocus()
}
HnLabeledKeyHint {
    keyGroups: [[Qt.Key_Control, Qt.Key_L]]
    description: qsTr("Edit location")
}
```

Resolve breadcrumb indices through the caller's original identities; never reconstruct
paths from display labels. Supply formatted metadata, rendering components and preview
state from the caller. Set explicit preview dimensions through anchors or Layout sizing.

The bordered breadcrumb field uses compact segments separated by chevrons. The pencil
icon ("Edit location") and Ctrl+L request caller-owned location editing. Set
`editable: false` to hide the icon and disable the shortcut. Narrow fields collapse
the farthest ancestors into one menu while retaining the root, current location,
and as many consecutive parents nearest the current location as fit. Widening restores
parents progressively. Labels use their natural width capped by `segmentWidth`;
spare space is left when the next complete parent cannot fit. At very narrow widths,
the current label elides first, then the root. `collapsed` indicates that at least
one ancestor is hidden; menu entries retain their original order and activation indices.

The gallery demonstrates caller-owned editing: click the pencil or press Ctrl+L
while the breadcrumb has focus, edit the path, then press Enter to apply it or
Escape to cancel. Losing focus also cancels editing. This changes the example labels without navigating the filesystem.

HnBreadcrumbBar exposes `sizeRole` (default `HnControlSize.Normal`, normalized by
HnMetrics). Normal retains a 32 px field with 28 px buttons; Xs uses a 24 px
field and buttons. Explicit shorter heights also constrain buttons to available
height. See [breadcrumb geometry design](sdd/breadcrumb-geometry.md).

`HnBreadcrumbBar.focusCurrentSegment(): bool` focuses the final segment button with
keyboard focus indication, without activating it. It returns false and preserves
focus when disabled, hidden, empty, or the final button is unavailable (including
zero geometry). This also works for a single root segment and collapsed trails.
See [directional panel focus](sdd/directional-panel-focus.md) for the consumer contract.

`vimNavigation` (default false) enables unmodified h/l focus movement through
visible breadcrumb segment buttons and the overflow button, stopping at either end.
It skips hidden, disabled and zero-size targets, shows keyboard focus indication,
and does not activate segments. Tab traversal and Ctrl+L editing are unchanged.
Files opts in; [panel keyboard navigation](../../holonight-files/docs/sdd/panel-key-navigation.md)
defines the application contract and deferred traversal design.
