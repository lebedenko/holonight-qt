// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Templates as T
import Holonight as H
import Holonight.Core

H.TextField {
    id: root

    property int sizeRole: HnControlSize.Normal
    property Component leadingContent
    property Component trailingContent

    readonly property int resolvedSizeRole: HnControlMetrics.normalizedSizeRole(root.sizeRole)
    readonly property bool clearButtonVisible: clearButton.visible

    implicitHeight: HnControlMetrics.controlHeight(root.resolvedSizeRole)
    leftPadding: HnControlMetrics.horizontalPadding(root.resolvedSizeRole)
                 + leadingArea.width + HnControlMetrics.internalSpacing(root.resolvedSizeRole)
    rightPadding: HnControlMetrics.horizontalPadding(root.resolvedSizeRole)
                  + trailingArea.width
                  + (trailingArea.width > 0 ? HnControlMetrics.internalSpacing(root.resolvedSizeRole) : 0)

    Keys.priority: Keys.BeforeItem
    Keys.onEscapePressed: event => {
        if (root.length > 0) {
            root.clear()
            event.accepted = true
        } else {
            event.accepted = false
        }
    }

    Item {
        id: leadingArea

        objectName: "hnSearchLeadingArea"
        anchors.left: parent.left
        anchors.leftMargin: HnControlMetrics.horizontalPadding(root.resolvedSizeRole)
        anchors.verticalCenter: parent.verticalCenter
        width: customLeadingLoader.active
               ? Math.max(customLeadingLoader.implicitWidth, customLeadingLoader.width)
               : HnControlMetrics.iconSize(root.resolvedSizeRole)
        height: Math.max(HnControlMetrics.iconSize(root.resolvedSizeRole),
                         customLeadingLoader.implicitHeight)

        HnIcon {
            objectName: "hnSearchDefaultIcon"
            anchors.centerIn: parent
            source: Qt.resolvedUrl("assets/search.svg")
            size: HnControlMetrics.iconSize(root.resolvedSizeRole)
            iconState: root.enabled ? HnIcon.Muted : HnIcon.Disabled
            visible: !customLeadingLoader.active
        }

        Loader {
            id: customLeadingLoader

            objectName: "hnSearchCustomLeading"
            anchors.centerIn: parent
            active: root.leadingContent !== null
            sourceComponent: root.leadingContent
        }
    }

    Row {
        id: trailingArea

        objectName: "hnSearchTrailingArea"
        anchors.right: parent.right
        anchors.rightMargin: HnControlMetrics.horizontalPadding(root.resolvedSizeRole)
        anchors.verticalCenter: parent.verticalCenter
        spacing: HnControlMetrics.internalSpacing(root.resolvedSizeRole)

        Loader {
            id: customTrailingLoader

            objectName: "hnSearchCustomTrailing"
            anchors.verticalCenter: parent.verticalCenter
            active: root.trailingContent !== null
            sourceComponent: root.trailingContent
        }

        T.AbstractButton {
            id: clearButton

            objectName: "hnSearchClearButton"
            property string accessibleName: qsTr("Clear search")

            anchors.verticalCenter: parent.verticalCenter
            implicitWidth: HnControlMetrics.iconSize(root.resolvedSizeRole)
            implicitHeight: HnControlMetrics.iconSize(root.resolvedSizeRole)
            visible: root.enabled && !root.readOnly && root.length > 0
            activeFocusOnTab: false
            Accessible.role: Accessible.Button
            Accessible.name: clearButton.accessibleName

            onClicked: {
                root.clear()
                root.forceActiveFocus()
            }

            contentItem: HnIcon {
                source: Qt.resolvedUrl("assets/clear.svg")
                size: HnControlMetrics.iconSize(root.resolvedSizeRole)
                iconState: clearButton.down ? HnIcon.Active : HnIcon.Muted
            }

            background: Item {}
        }
    }
}
