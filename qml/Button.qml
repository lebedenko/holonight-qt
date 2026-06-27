// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

import QtQuick
import QtQuick.Controls.impl
import QtQuick.Templates as T
import Holonight

T.Button {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    padding: 6
    horizontalPadding: 12
    spacing: 4

    contentItem: IconLabel {
        spacing: control.spacing
        mirrored: control.mirrored
        display: control.display
        icon: control.icon
        text: control.text
        font: control.font
        color: HoloniightPalette.textPrimary
    }

    background: Rectangle {
        implicitWidth: 80
        implicitHeight: HoloniightPalette.controlHeight
        radius: HoloniightPalette.radiusControl

        color: {
            if (!control.enabled)    return HoloniightPalette.surfaceRaised
            if (control.pressed)     return HoloniightPalette.surface
            if (control.highlighted) return HoloniightPalette.primary
            if (control.hovered)     return HoloniightPalette.surfaceHover
            return HoloniightPalette.surfaceRaised
        }

        border.color: control.visualFocus ? HoloniightPalette.borderFocus : HoloniightPalette.borderPassive
        border.width: control.visualFocus ? HoloniightPalette.focusBorderWidth : HoloniightPalette.borderWidth

        opacity: control.enabled ? 1.0 : 0.5

        Behavior on color { ColorAnimation { duration: 80 } }
    }
}
