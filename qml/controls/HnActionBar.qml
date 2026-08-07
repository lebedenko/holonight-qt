// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Layouts
import Holonight.Core

Item {
    id: root

    property Component leadingContent
    property Component centerContent
    property Component trailingContent
    property bool dividerVisible: true
    readonly property alias leadingItem: leadingLoader.item
    readonly property alias centerItem: centerLoader.item
    readonly property alias trailingItem: trailingLoader.item

    implicitWidth: content.implicitWidth
    implicitHeight: content.implicitHeight
    Accessible.role: Accessible.ToolBar

    ColumnLayout {
        id: content
        anchors.fill: parent
        spacing: 8
        Rectangle {
            color: HoloniightPalette.borderPassive
            visible: root.dividerVisible
            Layout.fillWidth: true
            Layout.preferredHeight: visible ? HnMetrics.separatorWidth : 0
        }
        Item {
            id: contentRow

            readonly property real slotSpacing: 8
            readonly property real leadingImplicitWidth: leadingLoader.active ? leadingLoader.implicitWidth : 0
            readonly property real centerImplicitWidth: centerLoader.active ? centerLoader.implicitWidth : 0
            readonly property real trailingImplicitWidth: trailingLoader.active ? trailingLoader.implicitWidth : 0
            readonly property real sideSpacing: centerLoader.active
                                                ? slotSpacing
                                                : (leadingLoader.active && trailingLoader.active ? slotSpacing : 0)
            readonly property real centeredSideWidth: Math.max(leadingImplicitWidth, trailingImplicitWidth)
            readonly property real sideAvailableWidth: centerLoader.active
                                                       ? Math.max(0, (width - centerLoader.width) / 2 - sideSpacing)
                                                       : width
            readonly property real sideImplicitWidth: leadingImplicitWidth + trailingImplicitWidth
            readonly property real uncenteredAvailableWidth: Math.max(0, width - sideSpacing)
            readonly property real compressionRatio: !centerLoader.active && sideImplicitWidth > uncenteredAvailableWidth
                                                     ? uncenteredAvailableWidth / sideImplicitWidth
                                                     : 1

            implicitWidth: centerLoader.active
                           ? centerImplicitWidth + 2 * (centeredSideWidth + sideSpacing)
                           : sideImplicitWidth + sideSpacing
            implicitHeight: Math.max(leadingLoader.implicitHeight, centerLoader.implicitHeight,
                                     trailingLoader.implicitHeight)
            Layout.fillWidth: true
            Layout.fillHeight: true

            Loader {
                id: leadingLoader

                active: root.leadingContent !== null
                visible: active
                sourceComponent: root.leadingContent
                width: centerLoader.active
                       ? Math.min(implicitWidth, contentRow.sideAvailableWidth)
                       : implicitWidth * contentRow.compressionRatio
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
            }
            Loader {
                id: centerLoader

                active: root.centerContent !== null
                visible: active
                sourceComponent: root.centerContent
                width: Math.min(implicitWidth, parent.width)
                anchors.centerIn: parent
            }
            Loader {
                id: trailingLoader

                active: root.trailingContent !== null
                visible: active
                sourceComponent: root.trailingContent
                width: centerLoader.active
                       ? Math.min(implicitWidth, contentRow.sideAvailableWidth)
                       : implicitWidth * contentRow.compressionRatio
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
            }
        }
    }
}
