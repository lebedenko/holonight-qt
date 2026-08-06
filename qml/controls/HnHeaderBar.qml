// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Layouts
import Holonight.Core

Item {
    id: root

    property Component content
    property real horizontalPadding: 12
    property real verticalPadding: 8
    property bool dividerVisible: true
    property color dividerColor: HoloniightPalette.borderPassive
    property real dividerInset: 0
    readonly property alias contentItem: contentLoader.item

    implicitHeight: HnControlMetrics.headerHeight
    height: HnControlMetrics.headerHeight
    Layout.minimumHeight: HnControlMetrics.headerHeight
    Layout.preferredHeight: HnControlMetrics.headerHeight
    Layout.maximumHeight: HnControlMetrics.headerHeight

    Loader {
        id: contentLoader

        objectName: "headerContentLoader"
        sourceComponent: root.content
        anchors {
            fill: parent
            leftMargin: Math.min(Math.max(0, root.horizontalPadding), root.width / 2)
            rightMargin: Math.min(Math.max(0, root.horizontalPadding), root.width / 2)
            topMargin: Math.min(Math.max(0, root.verticalPadding), root.height / 2)
            bottomMargin: Math.min(Math.max(0, root.verticalPadding), root.height / 2)
        }
    }

    HnSeparator {
        objectName: "headerDivider"
        visible: root.dividerVisible
        color: root.dividerColor
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            leftMargin: Math.min(Math.max(0, root.dividerInset), root.width / 2)
            rightMargin: Math.min(Math.max(0, root.dividerInset), root.width / 2)
        }
    }
}
