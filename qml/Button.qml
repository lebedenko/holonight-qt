// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

import QtQuick
import QtQuick.Controls.impl
import QtQuick.Templates as T
import Holonight

T.Button {
    id: root

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    padding: 6
    horizontalPadding: 12
    spacing: 4

    contentItem: IconLabel {
        spacing: root.spacing
        mirrored: root.mirrored
        display: root.display
        icon: root.icon
        text: root.text
        font: root.font
        color: {
            if (!root.enabled) return HoloniightPalette.textDisabled
            return root.highlighted ? HoloniightPalette.onPrimary : HoloniightPalette.textPrimary
        }
    }

    background: Rectangle {
        implicitWidth: 80
        implicitHeight: HoloniightPalette.controlHeight
        radius: HoloniightPalette.radiusControl

        color: {
            if (!root.enabled)    return HoloniightPalette.surfaceRaised
            if (root.pressed)     return HoloniightPalette.surface
            if (root.highlighted) return root.hovered ? HoloniightPalette.primaryHover : HoloniightPalette.primary
            if (root.hovered)     return HoloniightPalette.surfaceHover
            return HoloniightPalette.surfaceRaised
        }

        border.color: root.visualFocus ? HoloniightPalette.borderFocus : HoloniightPalette.borderPassive
        border.width: root.visualFocus ? HoloniightPalette.focusBorderWidth : HoloniightPalette.borderWidth

        opacity: root.enabled ? 1.0 : 0.5

        Behavior on color { ColorAnimation { duration: 80 } }
    }
}
