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
    property int sizeRole: HnControlSize.Normal
    readonly property int resolvedSizeRole: HnMetrics.normalizedSizeRole(root.sizeRole)
    readonly property real buttonHeight: Math.min(HnMetrics.controlHeight(HnControlSize.Compact),
                                                   Math.max(0, root.availableHeight))
    font.family: HolonightTheme.uiFont
    font.pointSize: HolonightTheme.bodySize
    font.weight: Font.Normal
    palette: compositePalette.appearancePalette
    readonly property Impl.ControlPalette compositePalette: Impl.ControlPalette {}
    readonly property Impl.ControlPalette frameColors: Impl.ControlPalette {
        palette: root.palette
        fillRole: Impl.ControlPalette.Base
    }
    padding: HnMetrics.internalSpacing(HnControlSize.Compact)
    leftPadding: Math.min(root.gap, root.width / 2)
    rightPadding: leftPadding
    topPadding: 0
    bottomPadding: 0
    readonly property real separatorWidth: separatorMetrics.advanceWidth + root.gap * 2
    TextMetrics { id: separatorMetrics; text: "›"; font: root.font }
    readonly property real gap: HnMetrics.internalSpacing(HnControlSize.Compact)
    readonly property var segmentWidths: {
        const widths = [];
        for (let index = 0; index < segmentRepeater.count; ++index) {
            const item = segmentRepeater.itemAt(index);
            widths.push(item ? Math.max(0, Math.min(root.segmentWidth, item.naturalWidth)) : 0);
        }
        return widths;
    }
    readonly property real editWidth: root.editable ? editButton.implicitWidth + root.gap : 0
    readonly property real expandedWidth: 2 * root.gap + root.editWidth
        + root.segmentWidths.reduce((total, value) => total + value, 0)
        + Math.max(0, root.segments.length - 1) * root.separatorWidth
    readonly property int firstVisibleAncestor: {
        const count = root.segmentWidths.length;
        if (count <= 2 || root.width >= root.expandedWidth)
            return 1;
        let first = count - 1;
        let used = root.segmentWidths[0] + root.segmentWidths[count - 1]
            + overflowButton.naturalWidth + 2 * root.separatorWidth;
        const budget = root.availableWidth - root.editWidth;
        while (first > 2 && used + root.segmentWidths[first - 1] + root.separatorWidth <= budget) {
            --first;
            used += root.segmentWidths[first] + root.separatorWidth;
        }
        return first;
    }
    readonly property bool collapsed: root.segments.length > 2 && root.firstVisibleAncestor > 1
    // Shrink current before root; complete intermediate ancestors retain their widths.
    readonly property int visibleCount: root.segments.length - (root.collapsed ? root.firstVisibleAncestor - 2 : 0)
    readonly property real navigationWidth: Math.max(0, root.availableWidth - root.editWidth)
    readonly property real actualSeparatorWidth: Math.min(root.separatorWidth,
        root.navigationWidth / Math.max(1, root.visibleCount - 1))
    readonly property real overflowWidth: root.collapsed ? Math.min(overflowButton.naturalWidth,
        Math.max(0, root.navigationWidth - (root.visibleCount - 1) * root.actualSeparatorWidth)) : 0
    readonly property real labelBudget: Math.max(0, root.navigationWidth - root.overflowWidth
        - Math.max(0, root.visibleCount - 1) * root.actualSeparatorWidth)
    readonly property real parentWidth: root.segmentWidths.slice(root.collapsed ? root.firstVisibleAncestor : 1,
        Math.max(1, root.segments.length - 1)).reduce((total, value) => total + value, 0)
    readonly property real rootWidth: Math.min(root.segmentWidths[0] || 0, root.labelBudget)
    readonly property real currentWidth: Math.min(root.segmentWidths[root.segments.length - 1] || 0,
        Math.max(0, root.labelBudget - (root.segments.length > 1 ? root.rootWidth : 0) - root.parentWidth))
    readonly property alias overflowMenu: overflow
    signal segmentActivated(int index)
    signal locationEditRequested
    implicitWidth: expandedWidth
    implicitHeight: HnMetrics.controlHeight(root.resolvedSizeRole)
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
        implicitHeight: root.buttonHeight
        Layout.alignment: Qt.AlignVCenter
        Layout.maximumHeight: root.buttonHeight
        padding: HnMetrics.internalSpacing(HnControlSize.Compact)
        Layout.minimumWidth: 0
        Layout.maximumWidth: root.segmentWidth
        Layout.preferredWidth: Math.min(root.segmentWidth, implicitWidth)
        contentItem: HnLabel {
            rawText: button.text
            color: button.current ? root.palette.link : root.palette.placeholderText
            font: root.font
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
        Layout.alignment: Qt.AlignVCenter
        Layout.minimumWidth: 0
        rawText: "›"
        font: root.font
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
        spacing: 0
        SegmentButton {
            visible: root.collapsed
            objectName: "hnBreadcrumbRoot"
            text: root.segments.length ? String(root.segments[0]) : ""
            Layout.minimumWidth: 0
            Layout.preferredWidth: root.rootWidth
            Layout.maximumWidth: root.rootWidth
            onClicked: root.segmentActivated(0)
        }
        Separator {
            visible: root.collapsed
            Layout.preferredWidth: root.actualSeparatorWidth
            horizontalAlignment: Text.AlignHCenter
        }
        SegmentButton {
            visible: root.collapsed
            id: overflowButton
            readonly property real naturalWidth: overflowMetrics.advanceWidth + padding * 2
            TextMetrics { id: overflowMetrics; text: "…"; font: root.font }
            Layout.preferredWidth: root.overflowWidth
            Layout.maximumWidth: root.overflowWidth
            objectName: "hnBreadcrumbOverflow"
            text: "…"
            Accessible.name: qsTr("Ancestor locations")
            onClicked: overflow.open()
            C.Menu {
                id: overflow
                Repeater {
                    model: root.collapsed ? root.firstVisibleAncestor - 1 : 0
                    delegate: C.MenuItem {
                        required property int index
                        objectName: "hnBreadcrumbOverflowSegment" + (index + 1)
                        text: String(root.segments[index + 1])
                        onTriggered: root.segmentActivated(index + 1)
                    }
                }
            }
        }
        Separator {
            visible: root.collapsed
            Layout.preferredWidth: root.actualSeparatorWidth
            horizontalAlignment: Text.AlignHCenter
        }
        Repeater {
            id: segmentRepeater
            model: root.segments
            delegate: Item {
                id: segmentRow
                required property int index
                required property var modelData
                TextMetrics {
                    id: labelMetrics
                    text: String(segmentRow.modelData)
                    font: root.font
                }
                readonly property real naturalWidth: labelMetrics.advanceWidth + segmentButton.padding * 2
                implicitWidth: naturalWidth
                visible: !root.collapsed || index >= root.firstVisibleAncestor
                implicitHeight: segmentButton.implicitHeight
                Layout.alignment: Qt.AlignVCenter
                Layout.maximumHeight: root.buttonHeight
                Layout.minimumWidth: 0
                Layout.preferredWidth: (index === root.segments.length - 1 ? root.currentWidth
                    : index === 0 ? root.rootWidth : root.segmentWidths[index] || 0)
                    + (separator.visible ? root.actualSeparatorWidth : 0)
                Layout.maximumWidth: Layout.preferredWidth
                Separator {
                    id: separator
                    visible: segmentRow.index > (root.collapsed ? root.firstVisibleAncestor : 0)
                    width: root.actualSeparatorWidth
                    horizontalAlignment: Text.AlignHCenter
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                }
                SegmentButton {
                    id: segmentButton
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    width: Math.max(0, segmentRow.width - (separator.visible ? root.actualSeparatorWidth : 0))
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
            implicitHeight: root.buttonHeight
            Layout.alignment: Qt.AlignVCenter
            Layout.maximumHeight: root.buttonHeight
            Layout.minimumWidth: 0
            Layout.maximumWidth: Math.max(0, root.availableWidth - root.gap)
            Layout.leftMargin: Math.min(root.gap, root.availableWidth)
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
