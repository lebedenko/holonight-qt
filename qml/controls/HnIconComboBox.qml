// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Templates as T
import Holonight as H
import Holonight.Core

H.ComboBox {
    id: root

    property int sizeRole: HnControlSize.Normal
    property string iconRole: "icon"

    readonly property int resolvedSizeRole: HnMetrics.normalizedSizeRole(root.sizeRole)
    readonly property url currentIconSource: root.roleValue(root.currentIndex, root.iconRole)

    function roleValue(index: int, role: string): var {
        if (index < 0 || !root.model || !role)
            return ""

        if (typeof root.model.get === "function") {
            const row = root.model.get(index)
            return row && row[role] !== undefined ? row[role] : ""
        }

        const row = root.model[index]
        if (row && typeof row === "object" && row[role] !== undefined)
            return row[role]
        return ""
    }

    implicitHeight: HnMetrics.controlHeight(root.resolvedSizeRole)
    leftPadding: HnMetrics.horizontalPadding(root.resolvedSizeRole)
    rightPadding: HnMetrics.horizontalPadding(root.resolvedSizeRole) + root.indicator.width
    topPadding: 0
    bottomPadding: 0

    delegate: iconDelegate

    contentItem: T.TextField {
        rightPadding: HnMetrics.internalSpacing(root.resolvedSizeRole)
        text: root.editable ? root.editText : root.displayText
        enabled: root.editable
        autoScroll: root.editable
        font: root.font
        color: root.enabled ? HoloniightPalette.textPrimary : HoloniightPalette.textDisabled
        selectionColor: HoloniightPalette.primary
        selectedTextColor: HoloniightPalette.onPrimary
        verticalAlignment: Text.AlignVCenter

        HnIcon {
            id: selectedIcon

            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            source: root.currentIconSource
            size: HnMetrics.iconSize(root.resolvedSizeRole)
            iconState: root.enabled ? HnIcon.Normal : HnIcon.Disabled
            visible: source.toString().length > 0 && !selectedIcon.hasError
        }

        leftPadding: selectedIcon.visible
                     ? selectedIcon.width + HnMetrics.internalSpacing(root.resolvedSizeRole)
                     : 0
    }

    Component {
        id: iconDelegate

        H.ItemDelegate {
            id: delegateRoot

            required property int index
            required property var model
            readonly property var rowModel: model
            readonly property url iconSource: rowModel && rowModel[root.iconRole] !== undefined
                                                ? rowModel[root.iconRole]
                                                : root.roleValue(index, root.iconRole)

            width: root.popup.availableWidth
            height: HnMetrics.controlHeight(root.resolvedSizeRole)
            text: root.textAt(index)
            highlighted: root.highlightedIndex === index
            leftPadding: HnMetrics.horizontalPadding(root.resolvedSizeRole)
                         + (delegateIcon.visible
                            ? delegateIcon.width + HnMetrics.internalSpacing(root.resolvedSizeRole)
                            : 0)

            HnIcon {
                id: delegateIcon

                anchors.left: parent.left
                anchors.leftMargin: HnMetrics.horizontalPadding(root.resolvedSizeRole)
                anchors.verticalCenter: parent.verticalCenter
                source: delegateRoot.iconSource
                size: HnMetrics.iconSize(root.resolvedSizeRole)
                iconState: delegateRoot.enabled ? HnIcon.Normal : HnIcon.Disabled
                visible: source.toString().length > 0 && !delegateIcon.hasError
            }
        }
    }
}
