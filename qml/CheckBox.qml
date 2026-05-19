// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

import QtQuick
import QtQuick.Templates as T
import Holonight

T.CheckBox {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    padding: 4
    spacing: 6

    indicator: Rectangle {
        id: indicator
        implicitWidth: 16
        implicitHeight: 16
        x: control.leftPadding
        anchors.verticalCenter: parent.verticalCenter
        radius: 3

        color: {
            if (!control.enabled)                     return HoloniightPalette.surface
            if (control.checkState !== Qt.Unchecked)  return control.pressed ? HoloniightPalette.primaryPressed : (control.hovered ? HoloniightPalette.primaryHover : HoloniightPalette.primary)
            return control.hovered ? HoloniightPalette.surfaceHover : HoloniightPalette.surface
        }

        border.color: {
            if (!control.enabled)                     return HoloniightPalette.onSurfaceDisabled
            if (control.checkState !== Qt.Unchecked)  return color
            return control.hovered ? HoloniightPalette.primary : HoloniightPalette.outline
        }
        border.width: 1

        // Checkmark
        Rectangle {
            visible: control.checkState === Qt.Checked
            anchors.centerIn: parent
            width: 9
            height: 2
            color: HoloniightPalette.onPrimary
            rotation: -45
            transformOrigin: Item.Left
            y: -1
        }
        Rectangle {
            visible: control.checkState === Qt.Checked
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            anchors.leftMargin: 3
            anchors.bottomMargin: 4
            width: 2
            height: 5
            color: HoloniightPalette.onPrimary
            rotation: -45
            transformOrigin: Item.Bottom
        }

        // Indeterminate dash
        Rectangle {
            visible: control.checkState === Qt.PartiallyChecked
            anchors.centerIn: parent
            width: 8
            height: 2
            color: HoloniightPalette.onPrimary
        }

        // Focus ring
        Rectangle {
            visible: control.visualFocus
            anchors.centerIn: parent
            width: parent.width + 4
            height: parent.height + 4
            radius: 5
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
