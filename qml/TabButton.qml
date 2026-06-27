// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

import QtQuick
import QtQuick.Templates as T
import Holonight

T.TabButton {
    id: root

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    padding: 8
    horizontalPadding: 12

    contentItem: Text {
        text: root.text
        font: root.font
        color: {
            if (!root.enabled) return HoloniightPalette.textDisabled
            if (root.checked)  return HoloniightPalette.primary
            return root.hovered ? HoloniightPalette.textPrimary : HoloniightPalette.textMuted
        }
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        textFormat: Text.PlainText
    }

    background: Item {
        // Tab body
        Rectangle {
            anchors.fill: parent
            color: root.hovered ? HoloniightPalette.surfaceHover : Qt.rgba(0, 0, 0, 0)
        }

        // Active indicator bar
        Rectangle {
            anchors.bottom: parent.bottom
            width: parent.width
            height: 2
            color: HoloniightPalette.primary
            visible: root.checked
        }

        // Focus border
        Rectangle {
            anchors.fill: parent
            color: Qt.rgba(0, 0, 0, 0)
            border.color: HoloniightPalette.borderFocus
            border.width: HoloniightPalette.borderWidth
            visible: root.visualFocus
        }
    }
}
