// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

import QtQuick
import QtQuick.Templates as T
import Holonight

T.RadioButton {
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
        radius: 8

        color: {
            if (!root.enabled)   return HoloniightPalette.surface
            if (root.checked)    return root.pressed ? HoloniightPalette.primaryPressed : (root.hovered ? HoloniightPalette.primaryHover : HoloniightPalette.primary)
            return root.hovered ? HoloniightPalette.surfaceHover : HoloniightPalette.surface
        }

        border.color: {
            if (!root.enabled)   return HoloniightPalette.textDisabled
            if (root.checked)    return color
            return root.hovered ? HoloniightPalette.borderActive : HoloniightPalette.borderPassive
        }
        border.width: 1

        // Inner dot when selected
        Rectangle {
            visible: root.checked
            anchors.centerIn: parent
            width: 6
            height: 6
            radius: 3
            color: root.enabled ? HoloniightPalette.onPrimary : HoloniightPalette.textDisabled
        }

        // Focus ring
        Rectangle {
            visible: root.visualFocus
            anchors.centerIn: parent
            width: parent.width + 4
            height: parent.height + 4
            radius: (parent.width + 4) / 2
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
