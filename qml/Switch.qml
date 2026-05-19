// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

import QtQuick
import QtQuick.Templates as T
import Holonight

T.Switch {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    padding: 4
    spacing: 6

    indicator: Item {
        implicitWidth: 34
        implicitHeight: 16
        x: control.leftPadding
        anchors.verticalCenter: parent.verticalCenter

        // Track
        Rectangle {
            anchors.fill: parent
            radius: 8
            color: {
                if (!control.enabled)  return HoloniightPalette.surfaceVariant
                if (control.checked)   return control.hovered ? HoloniightPalette.primaryHover : HoloniightPalette.primary
                return control.hovered ? HoloniightPalette.onSurfaceVariant : HoloniightPalette.borderPassive
            }

            Behavior on color { ColorAnimation { duration: 150 } }
        }

        // Thumb
        Rectangle {
            id: thumb
            width: 12
            height: 12
            radius: 6
            anchors.verticalCenter: parent.verticalCenter
            x: control.checked ? parent.width - width - 2 : 2

            color: {
                if (!control.enabled) return HoloniightPalette.onSurfaceDisabled
                return control.checked ? HoloniightPalette.onPrimary : HoloniightPalette.onSurfaceVariant
            }

            Behavior on x { NumberAnimation { duration: 150; easing.type: Easing.InOutQuad } }
            Behavior on color { ColorAnimation { duration: 150 } }

            // Focus ring
            Rectangle {
                visible: control.visualFocus
                anchors.centerIn: parent
                width: parent.width + 4
                height: parent.height + 4
                radius: (parent.width + 4) / 2
                color: "transparent"
                border.color: HoloniightPalette.focusRing
                border.width: 1
            }
        }
    }

    contentItem: Text {
        leftPadding: control.indicator.width + control.spacing
        text: control.text
        font: control.font
        color: control.enabled ? HoloniightPalette.onSurface : HoloniightPalette.onSurfaceDisabled
        verticalAlignment: Text.AlignVCenter
    }
}
