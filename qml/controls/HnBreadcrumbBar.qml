// SPDX-License-Identifier: GPL-3.0-or-later

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as C
import QtQuick.Layouts
import Holonight.Core
import Holonight.impl as Impl

C.Control {
    id: root
    // Display labels only. Consumers resolve the original index to an identity.
    property var segments: []
    property real segmentWidth: 150
    property bool editable: true
    palette: compositePalette.appearancePalette
    readonly property Impl.ControlPalette compositePalette: Impl.ControlPalette {}
    readonly property Impl.ControlPalette frameColors: Impl.ControlPalette {
        palette: root.palette
        fillRole: Impl.ControlPalette.Base
    }
    padding: HnMetrics.internalSpacing(HnControlSize.Compact)
    topPadding: 0
    bottomPadding: 0
    readonly property real separatorWidth: separatorMetrics.advanceWidth + trail.spacing * 2
    TextMetrics { id: separatorMetrics; text: "›"; font: root.font }
    readonly property real expandedWidth: {
        let total = root.leftPadding + root.rightPadding + (root.segments.length ? trail.spacing : 0)
                    + (root.editable ? editButton.implicitWidth + trail.spacing : 0);
        for (let index = 0; index < segmentRepeater.count; ++index) {
            const item = segmentRepeater.itemAt(index);
            if (item)
                total += Math.min(root.segmentWidth, item.implicitWidth) + (index > 0 ? root.separatorWidth : 0);
        }
        return total;
    }
    readonly property bool collapsed: segments.length > 2 && width < expandedWidth
    readonly property alias overflowMenu: overflow
    signal segmentActivated(int index)
    signal locationEditRequested
    implicitWidth: expandedWidth
    implicitHeight: HnMetrics.controlHeight(HnControlSize.Normal)
    background: Rectangle {
        color: root.frameColors.colors.surface
        border.width: HnMetrics.borderWidth
        border.color: root.frameColors.colors.borderPassive
        radius: HnAppearance.roundedRadius(HnSurfaceRole.Control, width, height, HnAppearance.revision)
    }

    component SegmentButton: C.ToolButton {
        id: button
        property bool current: false
        palette: root.palette
        implicitWidth: contentItem.implicitWidth + leftPadding + rightPadding
        implicitHeight: HnMetrics.controlHeight(HnControlSize.Compact)
        padding: HnMetrics.internalSpacing(HnControlSize.Compact)
        Layout.minimumWidth: 0
        Layout.maximumWidth: root.segmentWidth
        Layout.preferredWidth: Math.min(root.segmentWidth, implicitWidth)
        contentItem: HnLabel {
            rawText: button.text
            color: button.current ? root.palette.link : root.palette.placeholderText
            font.weight: Font.Normal
            elide: Text.ElideRight
            verticalAlignment: Text.AlignVCenter
            Accessible.ignored: true
        }
        background: Rectangle {
            color: button.down ? root.palette.mid : button.hovered ? root.palette.alternateBase : "transparent"
            radius: HnAppearance.roundedRadius(HnSurfaceRole.Control, width, height, HnAppearance.revision)
            border.width: button.visualFocus ? HnMetrics.focusBorderWidth : 0
            border.color: root.palette.highlight
        }
    }
    component Separator: HnLabel {
        rawText: "›"
        color: root.palette.placeholderText
        Accessible.ignored: true
    }
    Keys.onPressed: event => {
        if (root.editable && event.key === Qt.Key_L && (event.modifiers & Qt.ControlModifier)) {
            root.locationEditRequested();
            event.accepted = true;
        }
    }
    contentItem: RowLayout {
        id: trail
        spacing: HnMetrics.internalSpacing(HnControlSize.Compact)
        SegmentButton {
            visible: root.collapsed
            text: root.segments.length ? String(root.segments[0]) : ""
            Layout.minimumWidth: 0
            Layout.maximumWidth: root.segmentWidth
            onClicked: root.segmentActivated(0)
        }
        Separator { visible: root.collapsed }
        SegmentButton {
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
        Separator { visible: root.collapsed }
        Repeater {
            id: segmentRepeater
            model: root.segments
            delegate: Item {
                id: segmentRow
                required property int index
                required property var modelData
                readonly property real naturalWidth: segmentButton.implicitWidth
                implicitWidth: naturalWidth
                visible: !root.collapsed || index === root.segments.length - 1
                implicitHeight: segmentButton.implicitHeight
                Layout.minimumWidth: 0
                Layout.preferredWidth: Math.min(root.segmentWidth, naturalWidth)
                                       + (separator.visible ? root.separatorWidth : 0)
                Layout.maximumWidth: Layout.preferredWidth
                Separator {
                    id: separator
                    visible: !root.collapsed && segmentRow.index > 0
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                }
                SegmentButton {
                    id: segmentButton
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    width: Math.max(0, segmentRow.width - (separator.visible ? root.separatorWidth : 0))
                    objectName: "hnBreadcrumbSegment" + segmentRow.index
                    text: String(segmentRow.modelData)
                    current: segmentRow.index === root.segments.length - 1
                    onClicked: root.segmentActivated(segmentRow.index)
                }
            }
        }
        Item { Layout.fillWidth: true }
        C.ToolButton {
            id: editButton
            visible: root.editable
            objectName: "hnBreadcrumbEdit"
            palette: root.palette
            implicitWidth: HnMetrics.controlHeight(HnControlSize.Compact)
            implicitHeight: implicitWidth
            padding: HnMetrics.internalSpacing(HnControlSize.Compact)
            icon.source: Qt.resolvedUrl("assets/edit.svg")
            icon.width: HnMetrics.iconSize(HnControlSize.Compact)
            icon.height: HnMetrics.iconSize(HnControlSize.Compact)
            C.ToolTip.visible: hovered
            C.ToolTip.text: qsTr("Edit location")
            Accessible.name: qsTr("Edit location")
            onClicked: root.locationEditRequested()
        }
    }
}
