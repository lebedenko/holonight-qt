// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

import QtQuick
import QtQuick.Templates as T
import Holonight.Core
import Holonight.impl as Impl

T.CheckBox {
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
        radius: 3

        color: {
            if (!root.enabled)                     return root.controlColors.colors.surface
            if (root.checkState > Qt.Unchecked)  return root.pressed ? root.controlColors.colors.primaryPressed : (root.hovered ? root.controlColors.colors.primaryHover : root.controlColors.colors.primary)
            return root.hovered ? root.controlColors.colors.surfaceHover : root.controlColors.colors.surface
        }

        border.color: {
            if (!root.enabled)                     return root.controlColors.colors.disabledBorder
            if (root.checkState > Qt.Unchecked)  return color
            return root.hovered ? root.controlColors.colors.borderActive : root.controlColors.colors.borderPassive
        }
        border.width: 1

        // Checkmark
        Rectangle {
            visible: root.checkState === Qt.Checked
            x: 5
            y: 6
            width: 9
            height: 2
            color: root.enabled ? root.controlColors.colors.onPrimary : root.controlColors.colors.disabledOnPrimary
            rotation: -45
            transformOrigin: Item.Left
        }
        Rectangle {
            visible: root.checkState === Qt.Checked
            width: 2
            height: 5
            color: root.enabled ? root.controlColors.colors.onPrimary : root.controlColors.colors.disabledOnPrimary
            rotation: -45
            transformOrigin: Item.Bottom
            anchors {
                left: parent.left
                bottom: parent.bottom
                leftMargin: 3
                bottomMargin: 4
            }
        }

        // Indeterminate dash
        Rectangle {
            visible: root.checkState === Qt.PartiallyChecked
            anchors.centerIn: parent
            width: 8
            height: 2
            color: root.enabled ? root.controlColors.colors.onPrimary : root.controlColors.colors.disabledOnPrimary
        }

        // Focus ring
        Rectangle {
            readonly property real semanticRadius: HnAppearance.roundedRadius(HnSurfaceRole.Control,
                                                                              width, height,
                                                                              HnAppearance.revision)

            visible: root.visualFocus
            anchors.centerIn: parent
            width: parent.width + 4
            height: parent.height + 4
            radius: semanticRadius
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
