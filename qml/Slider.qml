// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

import QtQuick
import QtQuick.Templates as T
import Holonight

T.Slider {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    padding: 6

    handle: Rectangle {
        id: handle
        x: control.leftPadding + (control.horizontal ? control.visualPosition * (control.availableWidth - width) : (control.availableWidth - width) / 2)
        y: control.topPadding + (control.horizontal ? (control.availableHeight - height) / 2 : control.visualPosition * (control.availableHeight - height))
        implicitWidth: 12
        implicitHeight: 12
        radius: 2

        color: {
            if (!control.enabled) return HoloniightPalette.onSurfaceDisabled
            if (control.pressed)  return HoloniightPalette.primaryPressed
            if (control.hovered)  return HoloniightPalette.primaryHover
            return HoloniightPalette.primary
        }

        border.color: HoloniightPalette.borderPassive
        border.width: 1

        Rectangle {
            visible: control.visualFocus
            anchors.centerIn: parent
            width: parent.width + 4
            height: parent.height + 4
            radius: 4
            color: "transparent"
            border.color: HoloniightPalette.focusRing
            border.width: 1
        }
    }

    background: Item {
        x: control.leftPadding
        y: control.topPadding
        implicitWidth: control.horizontal ? 200 : 14
        implicitHeight: control.horizontal ? 14 : 200
        width: control.availableWidth
        height: control.availableHeight

        // Full groove
        Rectangle {
            anchors.centerIn: parent
            width: control.horizontal ? parent.width : 4
            height: control.horizontal ? 4 : parent.height
            radius: 2
            color: HoloniightPalette.borderPassive
        }

        // Filled portion
        Rectangle {
            anchors {
                left: control.horizontal ? parent.left : undefined
                right: control.horizontal ? undefined : undefined
                horizontalCenter: control.horizontal ? undefined : parent.horizontalCenter
                top: control.horizontal ? undefined : control.handle.verticalCenter
                bottom: control.horizontal ? undefined : parent.bottom
                verticalCenter: control.horizontal ? parent.verticalCenter : undefined
            }
            width: control.horizontal ? control.handle.x + control.handle.width / 2 : 4
            height: control.horizontal ? 4 : parent.height - (control.handle.y + control.handle.height / 2)
            radius: 2
            color: control.enabled ? HoloniightPalette.primary : HoloniightPalette.onSurfaceDisabled
        }
    }
}
