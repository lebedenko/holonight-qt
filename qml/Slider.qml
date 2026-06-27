// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

import QtQuick
import QtQuick.Templates as T
import Holonight

T.Slider {
    id: root

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    padding: 6

    handle: Rectangle {
        x: root.leftPadding + (root.horizontal ? root.visualPosition * (root.availableWidth - width) : (root.availableWidth - width) / 2)
        y: root.topPadding + (root.horizontal ? (root.availableHeight - height) / 2 : root.visualPosition * (root.availableHeight - height))
        implicitWidth: 12
        implicitHeight: 12
        radius: 2

        color: {
            if (!root.enabled) return HoloniightPalette.textDisabled
            if (root.pressed)  return HoloniightPalette.primaryPressed
            if (root.hovered)  return HoloniightPalette.primaryHover
            return HoloniightPalette.primary
        }

        border.color: HoloniightPalette.borderPassive
        border.width: 1

        Rectangle {
            visible: root.visualFocus
            anchors.centerIn: parent
            width: parent.width + 4
            height: parent.height + 4
            radius: 4
            color: Qt.rgba(0, 0, 0, 0)
            border.color: HoloniightPalette.borderFocus
            border.width: HoloniightPalette.borderWidth
        }
    }

    background: Item {
        x: root.leftPadding
        y: root.topPadding
        implicitWidth: root.horizontal ? 200 : 14
        implicitHeight: root.horizontal ? 14 : 200
        width: root.availableWidth
        height: root.availableHeight

        // Full groove
        Rectangle {
            anchors.centerIn: parent
            width: root.horizontal ? parent.width : 4
            height: root.horizontal ? 4 : parent.height
            radius: 2
            color: HoloniightPalette.borderPassive
        }

        // Filled portion
        Rectangle {
            x: root.horizontal ? 0 : (parent.width - width) / 2
            y: root.horizontal ? (parent.height - height) / 2 : root.handle.y + root.handle.height / 2
            width: root.horizontal ? root.handle.x + root.handle.width / 2 : 4
            height: root.horizontal ? 4 : parent.height - (root.handle.y + root.handle.height / 2)
            radius: 2
            color: root.enabled ? HoloniightPalette.primary : HoloniightPalette.textDisabled
        }
    }
}
