// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

import QtQuick
import QtQuick.Templates as T
import Holonight.Core
import Holonight.impl as Impl
import QtQuick.Controls.impl

T.ToolButton {
    id: root

    Impl.ControlPalette {
        id: colors
        palette: root.palette
        colorGroup: !root.enabled ? Impl.ControlPalette.Disabled : (root.Window.window && !root.Window.window.active ? Impl.ControlPalette.Inactive : Impl.ControlPalette.Active)
        textRole: Impl.ControlPalette.ButtonText
        fillRole: Impl.ControlPalette.Button
    }

    font.family: HolonightTheme.uiFont
    font.pointSize: HolonightTheme.bodySize

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    hoverEnabled: true
    padding: 6
    horizontalPadding: HnMetrics.horizontalPadding(HnControlSize.Normal)
    spacing: 6
    icon.width: HnMetrics.iconSize(HnControlSize.Normal)
    icon.height: HnMetrics.iconSize(HnControlSize.Normal)

    contentItem: IconLabel {
        spacing: root.spacing
        mirrored: root.mirrored
        display: root.display
        icon: root.icon
        text: root.text
        font: root.font
        color: defaultIconColor
        defaultIconColor: root.highlighted ? colors.colors.onPrimary
                               : (root.enabled ? colors.colors.textPrimary : colors.colors.textDisabled)
    }

    background: Rectangle {
        implicitWidth: HnMetrics.controlHeight(HnControlSize.Normal)
        implicitHeight: HnMetrics.controlHeight(HnControlSize.Normal)
        radius: HnAppearance.roundedRadius(HnSurfaceRole.Control, width, height, HnAppearance.revision)
        color: {
            if (root.highlighted) {
                if (root.down) return colors.colors.primaryPressed
                if (root.hovered) return colors.colors.primaryHover
                return colors.colors.primary
            }
            if (root.down) return colors.colors.buttonPressed
            if (root.checked) return root.hovered ? colors.colors.surfaceSelectedHover : colors.colors.surfaceSelected
            if (root.hovered) return colors.colors.buttonHover
            return "transparent"
        }
        border.color: colors.colors.borderFocus
        border.width: root.visualFocus ? HnMetrics.focusBorderWidth : 0
    }
}
