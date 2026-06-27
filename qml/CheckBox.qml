// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

import QtQuick
import QtQuick.Templates as T
import Holonight

T.CheckBox {
    id: root

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    padding: 4
    spacing: 6

    indicator: Rectangle {
        implicitWidth: 16
        implicitHeight: 16
        x: root.leftPadding
        anchors.verticalCenter: parent.verticalCenter
        radius: 3

        color: {
            if (!root.enabled)                     return HoloniightPalette.surface
            if (root.checkState > Qt.Unchecked)  return root.pressed ? HoloniightPalette.primaryPressed : (root.hovered ? HoloniightPalette.primaryHover : HoloniightPalette.primary)
            return root.hovered ? HoloniightPalette.surfaceHover : HoloniightPalette.surface
        }

        border.color: {
            if (!root.enabled)                     return HoloniightPalette.textDisabled
            if (root.checkState > Qt.Unchecked)  return color
            return root.hovered ? HoloniightPalette.borderActive : HoloniightPalette.borderPassive
        }
        border.width: 1

        // Checkmark
        Rectangle {
            visible: root.checkState === Qt.Checked
            x: 5
            y: 6
            width: 9
            height: 2
            color: HoloniightPalette.onPrimary
            rotation: -45
            transformOrigin: Item.Left
        }
        Rectangle {
            visible: root.checkState === Qt.Checked
            anchors {
                left: parent.left
                bottom: parent.bottom
                leftMargin: 3
                bottomMargin: 4
            }
            width: 2
            height: 5
            color: HoloniightPalette.onPrimary
            rotation: -45
            transformOrigin: Item.Bottom
        }

        // Indeterminate dash
        Rectangle {
            visible: root.checkState === Qt.PartiallyChecked
            anchors.centerIn: parent
            width: 8
            height: 2
            color: HoloniightPalette.onPrimary
        }

        // Focus ring
        Rectangle {
            visible: root.visualFocus
            anchors.centerIn: parent
            width: parent.width + 4
            height: parent.height + 4
            radius: HoloniightPalette.radiusControl
            color: Qt.rgba(0, 0, 0, 0)
            border.color: HoloniightPalette.borderFocus
            border.width: HoloniightPalette.focusBorderWidth
        }
    }

    contentItem: Text {
        leftPadding: root.indicator.width + root.spacing
        text: root.text
        font: root.font
        color: root.enabled ? HoloniightPalette.textPrimary : HoloniightPalette.textDisabled
        verticalAlignment: Text.AlignVCenter
        textFormat: Text.PlainText
    }
}
