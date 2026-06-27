// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

import QtQuick
import QtQuick.Templates as T
import Holonight

T.Switch {
    id: root

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    padding: 4
    spacing: 6

    indicator: Item {
        implicitWidth: 34
        implicitHeight: 16
        x: root.leftPadding
        anchors.verticalCenter: parent.verticalCenter

        // Track
        Rectangle {
            anchors.fill: parent
            radius: 8
            color: {
                if (!root.enabled)  return HoloniightPalette.surface
                if (root.checked)   return root.hovered ? HoloniightPalette.primaryHover : HoloniightPalette.primary
                return root.hovered ? HoloniightPalette.textMuted : HoloniightPalette.borderPassive
            }

            Behavior on color { ColorAnimation { duration: 150 } }
        }

        // Thumb
        Rectangle {
            width: 12
            height: 12
            radius: 6
            anchors.verticalCenter: parent.verticalCenter
            x: root.checked ? parent.width - width - 2 : 2

            color: {
                if (!root.enabled) return HoloniightPalette.textDisabled
                return root.checked ? HoloniightPalette.onPrimary : HoloniightPalette.textMuted
            }

            Behavior on x { NumberAnimation { duration: 150; easing.type: Easing.InOutQuad } }
            Behavior on color { ColorAnimation { duration: 150 } }

            // Focus ring
            Rectangle {
                visible: root.visualFocus
                anchors.centerIn: parent
                width: parent.width + 4
                height: parent.height + 4
                radius: (parent.width + 4) / 2
                color: Qt.rgba(0, 0, 0, 0)
                border.color: HoloniightPalette.borderFocus
                border.width: HoloniightPalette.borderWidth
            }
        }
    }

    contentItem: Text {
        leftPadding: root.indicator.width + root.spacing
        text: root.text
        font: root.font
        color: root.enabled ? HoloniightPalette.textPrimary : HoloniightPalette.textDisabled
        verticalAlignment: Text.AlignVCenter
        textFormat: Text.PlainText
    }
}
