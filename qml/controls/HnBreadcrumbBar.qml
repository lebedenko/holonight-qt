// SPDX-License-Identifier: GPL-3.0-or-later

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as C
import QtQuick.Layouts
import Holonight.Core

C.Control {
    id: root
    // Display labels only. Consumers resolve the original index to an identity.
    property var segments: []
    property real segmentWidth: 150
    property bool editable: true
    readonly property real expandedWidth: {
        let total = root.editable ? editButton.implicitWidth + trail.spacing : 0;
        for (let index = 0; index < segmentRepeater.count; ++index) {
            const item = segmentRepeater.itemAt(index);
            if (item)
                total += Math.min(root.segmentWidth, item.implicitWidth) + (index > 0 ? trail.spacing : 0);
        }
        return total;
    }
    readonly property bool collapsed: segments.length > 2 && width < expandedWidth
    readonly property alias overflowMenu: overflow
    signal segmentActivated(int index)
    signal locationEditRequested
    implicitWidth: expandedWidth
    implicitHeight: trail.implicitHeight
    Keys.onPressed: event => {
        if (event.key === Qt.Key_L && (event.modifiers & Qt.ControlModifier)) {
            root.locationEditRequested();
            event.accepted = true;
        }
    }
    contentItem: RowLayout {
        id: trail
        spacing: HnMetrics.internalSpacing(HnControlSize.Normal)
        C.ToolButton {
            visible: root.collapsed
            text: root.segments.length ? String(root.segments[0]) : ""
            Layout.minimumWidth: 0
            Layout.maximumWidth: root.segmentWidth
            onClicked: root.segmentActivated(0)
        }
        C.ToolButton {
            visible: root.collapsed
            text: "…"
            Accessible.name: qsTr("Ancestor locations")
            onClicked: overflow.open()
            C.Menu {
                id: overflow
                Repeater {
                    model: Math.max(0, root.segments.length - 2)
                    delegate: C.MenuItem {
                        required property int index
                        objectName: "hnBreadcrumbOverflowSegment" + (index + 1)
                        text: String(root.segments[index + 1])
                        onTriggered: root.segmentActivated(index + 1)
                    }
                }
            }
        }
        Repeater {
            id: segmentRepeater
            model: root.segments
            delegate: C.ToolButton {
                id: segment
                required property int index
                required property var modelData
                visible: !root.collapsed || index === root.segments.length - 1
                text: String(modelData)
                font.bold: index === root.segments.length - 1
                Layout.minimumWidth: 0
                Layout.maximumWidth: root.segmentWidth
                Layout.fillWidth: index === root.segments.length - 1
                contentItem: HnLabel {
                    rawText: segment.text
                    elide: Text.ElideRight
                    Accessible.ignored: true
                }
                onClicked: root.segmentActivated(index)
            }
        }
        C.ToolButton {
            id: editButton
            visible: root.editable
            text: qsTr("Edit")
            Accessible.name: qsTr("Edit location")
            onClicked: root.locationEditRequested()
        }
    }
}
