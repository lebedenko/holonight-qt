// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

import QtQuick
import QtQuick.Controls.impl
import QtQuick.Templates as T
import Holonight.Core
import Holonight.impl as Impl

T.Button {
    id: root

    readonly property Impl.ControlPalette controlColors: Impl.ControlPalette {
        palette: root.palette
        colorGroup: !root.enabled ? Impl.ControlPalette.Disabled
                                 : (root.Window.window && !root.Window.window.active
                                    ? Impl.ControlPalette.Inactive : Impl.ControlPalette.Active)
        textRole: Impl.ControlPalette.ButtonText
        fillRole: Impl.ControlPalette.Button
    }
    hoverEnabled: true

    font.family: HolonightTheme.uiFont
    font.pointSize: HolonightTheme.bodySize

    readonly property color foregroundColor: {
        if (!root.enabled) return root.controlColors.colors.textDisabled
        return root.highlighted ? root.controlColors.colors.onPrimary : root.controlColors.colors.textPrimary
    }

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    icon.width: HnMetrics.iconSize(HnControlSize.Normal)
    icon.height: HnMetrics.iconSize(HnControlSize.Normal)
    icon.color: foregroundColor

    padding: 6
    horizontalPadding: display === T.AbstractButton.IconOnly
                       ? HnMetrics.horizontalPadding(HnControlSize.Normal)
                       : 12
    spacing: 4

    contentItem: IconLabel {
        spacing: root.spacing
        mirrored: root.mirrored
        display: root.display
        icon: root.icon
        text: root.text
        font: root.font
        color: root.foregroundColor
    }

    background: Rectangle {
        readonly property real semanticRadius: HnAppearance.roundedRadius(HnSurfaceRole.Control,
                                                                          width, height,
                                                                          HnAppearance.revision)

        implicitWidth: 80
        implicitHeight: HnMetrics.controlHeight(HnControlSize.Normal)
        radius: semanticRadius

        color: {
            if (!root.enabled)    return root.controlColors.colors.surfaceRaised
            if (root.highlighted) {
                if (root.down)    return root.controlColors.colors.primaryPressed
                if ((root.HnInputInteraction.hoverAllowed && root.hovered)) return root.controlColors.colors.primaryHover
                return root.controlColors.colors.primary
            }
            if (root.down)        return root.controlColors.colors.buttonPressed
            if ((root.HnInputInteraction.hoverAllowed && root.hovered))     return root.controlColors.colors.surfaceHover
            return root.controlColors.colors.surfaceRaised
        }

        border.color: root.visualFocus ? root.controlColors.colors.borderFocus : root.controlColors.colors.borderPassive
        border.width: root.visualFocus ? HnMetrics.focusBorderWidth : HnMetrics.borderWidth

        opacity: root.enabled || !root.controlColors.defaultDisabled ? 1.0 : 0.5

        Behavior on color { ColorAnimation { duration: 80 } }
    }
}
