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

    readonly property int resolvedSizeRole: HnMetrics.normalizedSizeRole(root.sizeRole)
    readonly property bool clearButtonVisible: clearButton.visible

    implicitHeight: HnMetrics.controlHeight(root.resolvedSizeRole)
    leftPadding: HnMetrics.horizontalPadding(root.resolvedSizeRole)
                 + leadingArea.width + HnMetrics.internalSpacing(root.resolvedSizeRole)
    rightPadding: HnMetrics.horizontalPadding(root.resolvedSizeRole)
                  + trailingArea.width
                  + (trailingArea.width > 0 ? HnMetrics.internalSpacing(root.resolvedSizeRole) : 0)

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
        anchors.leftMargin: HnMetrics.horizontalPadding(root.resolvedSizeRole)
        anchors.verticalCenter: parent.verticalCenter
        width: customLeadingLoader.active
               ? Math.max(customLeadingLoader.implicitWidth, customLeadingLoader.width)
               : HnMetrics.iconSize(root.resolvedSizeRole)
        height: Math.max(HnMetrics.iconSize(root.resolvedSizeRole),
                         customLeadingLoader.implicitHeight)

        HnIcon {
            objectName: "hnSearchDefaultIcon"
            anchors.centerIn: parent
            source: Qt.resolvedUrl("assets/search.svg")
            size: HnMetrics.iconSize(root.resolvedSizeRole)
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
        anchors.rightMargin: HnMetrics.horizontalPadding(root.resolvedSizeRole)
        anchors.verticalCenter: parent.verticalCenter
        spacing: HnMetrics.internalSpacing(root.resolvedSizeRole)

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
            implicitWidth: HnMetrics.iconSize(root.resolvedSizeRole)
            implicitHeight: HnMetrics.iconSize(root.resolvedSizeRole)
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
                size: HnMetrics.iconSize(root.resolvedSizeRole)
                iconState: clearButton.down ? HnIcon.Active : HnIcon.Muted
            }

            background: Item {}
        }
    }
}
