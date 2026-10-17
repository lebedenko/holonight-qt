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
