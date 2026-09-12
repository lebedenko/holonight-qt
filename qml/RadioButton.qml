// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

import QtQuick
import QtQuick.Templates as T
import Holonight.Core
import Holonight.impl as Impl

T.RadioButton {
    id: root

    readonly property Impl.ControlPalette controlColors: Impl.ControlPalette {
        palette: root.palette
        colorGroup: !root.enabled ? Impl.ControlPalette.Disabled
                                 : (root.Window.window && !root.Window.window.active
                                    ? Impl.ControlPalette.Inactive : Impl.ControlPalette.Active)
        textRole: Impl.ControlPalette.WindowText
        fillRole: Impl.ControlPalette.Base
    }

    font.family: HolonightTheme.uiFont
    font.pointSize: HolonightTheme.bodySize

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding,
                            implicitIndicatorWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding,
                             implicitIndicatorHeight + topPadding + bottomPadding)

    padding: 4
    spacing: 6

    indicator: Rectangle {
        implicitWidth: 16
        implicitHeight: 16
        x: root.mirrored ? root.width - width - root.rightPadding : root.leftPadding
        anchors.verticalCenter: parent.verticalCenter
        radius: 8

        color: {
            if (!root.enabled)   return root.controlColors.colors.surface
            if (root.checked)    return root.pressed ? root.controlColors.colors.primaryPressed : ((root.HnInputInteraction.hoverAllowed && root.hovered) ? root.controlColors.colors.primaryHover : root.controlColors.colors.primary)
            return (root.HnInputInteraction.hoverAllowed && root.hovered) ? root.controlColors.colors.surfaceHover : root.controlColors.colors.surface
        }

        border.color: {
            if (!root.enabled)   return root.controlColors.colors.disabledBorder
            if (root.checked)    return color
            return (root.HnInputInteraction.hoverAllowed && root.hovered) ? root.controlColors.colors.borderActive : root.controlColors.colors.borderPassive
        }
        border.width: 1

        // Inner dot when selected
        Rectangle {
            visible: root.checked
            anchors.centerIn: parent
            width: 6
            height: 6
            radius: 3
            color: root.enabled ? root.controlColors.colors.onPrimary : root.controlColors.colors.disabledOnPrimary
        }

        // Focus ring
        Rectangle {
            visible: root.visualFocus
            anchors.centerIn: parent
            width: parent.width + 4
            height: parent.height + 4
            radius: (parent.width + 4) / 2
            color: Qt.rgba(0, 0, 0, 0)
            border.color: root.controlColors.colors.borderFocus
            border.width: HnMetrics.focusBorderWidth
        }
    }

    contentItem: Text {
        leftPadding: !root.mirrored && root.indicator ? root.indicator.width + root.spacing : 0
        rightPadding: root.mirrored && root.indicator ? root.indicator.width + root.spacing : 0
        text: root.text
        font: root.font
        color: root.enabled ? root.controlColors.colors.textPrimary : root.controlColors.colors.textDisabled
        verticalAlignment: Text.AlignVCenter
        textFormat: Text.PlainText
    }
}
