// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

import QtQuick
import QtQuick.Templates as T
import Holonight

T.TabButton {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    padding: 8
    horizontalPadding: 12

    contentItem: Text {
        text: control.text
        font: control.font
        color: {
            if (!control.enabled) return HoloniightPalette.textDisabled
            if (control.checked)  return HoloniightPalette.primary
            return control.hovered ? HoloniightPalette.textPrimary : HoloniightPalette.textMuted
        }
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    background: Item {
        // Tab body
        Rectangle {
            anchors.fill: parent
            color: control.hovered ? HoloniightPalette.surfaceHover : "transparent"
        }

        // Active indicator bar
        Rectangle {
            anchors.bottom: parent.bottom
            width: parent.width
            height: 2
            color: HoloniightPalette.primary
            visible: control.checked
        }

        // Focus border
        Rectangle {
            anchors.fill: parent
            color: "transparent"
            border.color: HoloniightPalette.borderFocus
            border.width: HoloniightPalette.borderWidth
            visible: control.visualFocus
        }
    }
}
