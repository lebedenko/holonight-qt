// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

import QtQuick
import QtQuick.Controls.impl
import QtQuick.Templates as T
import Holonight.Core

T.Button {
    id: root
    hoverEnabled: true

    font.family: HolonightTheme.uiFont
    font.pointSize: HolonightTheme.bodySize

    readonly property color foregroundColor: {
        if (!root.enabled) return HoloniightPalette.textDisabled
        return root.highlighted ? HoloniightPalette.onPrimary : HoloniightPalette.textPrimary
    }

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    icon.width: HnControlMetrics.iconSize(HnControlSize.Normal)
    icon.height: HnControlMetrics.iconSize(HnControlSize.Normal)
    icon.color: foregroundColor

    padding: 6
    horizontalPadding: display === T.AbstractButton.IconOnly
                       ? HnControlMetrics.horizontalPadding(HnControlSize.Normal)
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
        implicitHeight: HoloniightPalette.controlHeight
        radius: semanticRadius

        color: {
            if (!root.enabled)    return HoloniightPalette.surfaceRaised
            if (root.highlighted) {
                if (root.down)    return HoloniightPalette.primaryPressed
                if (root.hovered) return HoloniightPalette.primaryHover
                return HoloniightPalette.primary
            }
            if (root.down)        return HoloniightPalette.surface
            if (root.hovered)     return HoloniightPalette.surfaceHover
            return HoloniightPalette.surfaceRaised
        }

        border.color: root.visualFocus ? HoloniightPalette.borderFocus : HoloniightPalette.borderPassive
        border.width: root.visualFocus ? HoloniightPalette.focusBorderWidth : HoloniightPalette.borderWidth

        opacity: root.enabled ? 1.0 : 0.5

        Behavior on color { ColorAnimation { duration: 80 } }
    }
}
