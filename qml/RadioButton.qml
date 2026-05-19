// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

import QtQuick
import QtQuick.Templates as T
import Holonight

T.RadioButton {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    padding: 4
    spacing: 6

    indicator: Rectangle {
        implicitWidth: 16
        implicitHeight: 16
        x: control.leftPadding
        anchors.verticalCenter: parent.verticalCenter
        radius: 8

        color: {
            if (!control.enabled)   return HoloniightPalette.surface
            if (control.checked)    return control.pressed ? HoloniightPalette.primaryPressed : (control.hovered ? HoloniightPalette.primaryHover : HoloniightPalette.primary)
            return control.hovered ? HoloniightPalette.surfaceHover : HoloniightPalette.surface
        }

        border.color: {
            if (!control.enabled)   return HoloniightPalette.onSurfaceDisabled
            if (control.checked)    return color
            return control.hovered ? HoloniightPalette.borderActive : HoloniightPalette.borderPassive
        }
        border.width: 1

        // Inner dot when selected
        Rectangle {
            visible: control.checked
            anchors.centerIn: parent
            width: 6
            height: 6
            radius: 3
            color: HoloniightPalette.onPrimary
        }

        // Focus ring
        Rectangle {
            visible: control.visualFocus
            anchors.centerIn: parent
            width: parent.width + 4
            height: parent.height + 4
            radius: (parent.width + 4) / 2
            color: "transparent"
            border.color: HoloniightPalette.focusRing
            border.width: 2
        }
    }

    contentItem: Text {
        leftPadding: control.indicator.width + control.spacing
        text: control.text
        font: control.font
        color: control.enabled ? HoloniightPalette.onSurface : HoloniightPalette.onSurfaceDisabled
        verticalAlignment: Text.AlignVCenter
    }
}
