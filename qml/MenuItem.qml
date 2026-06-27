// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

import QtQuick
import QtQuick.Templates as T
import Holonight

T.MenuItem {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    padding: 6
    leftPadding: 12
    rightPadding: 12
    spacing: 6

    contentItem: Row {
        spacing: control.spacing

        Rectangle {
            width: 16
            height: 16
            anchors.verticalCenter: parent.verticalCenter
            color: "transparent"

            // Checkmark for checkable items
            visible: control.checkable
            Text {
                anchors.centerIn: parent
                text: control.checked ? "✓" : ""
                color: HoloniightPalette.primary
                font.pixelSize: 12
            }
        }

        Text {
            text: control.text
            font: control.font
            color: control.enabled ? (control.highlighted ? HoloniightPalette.onPrimary : HoloniightPalette.textPrimary) : HoloniightPalette.textDisabled
            verticalAlignment: Text.AlignVCenter
        }
    }

    arrow: Text {
        x: control.width - width - 8
        anchors.verticalCenter: parent.verticalCenter
        visible: control.subMenu
        text: "›"
        color: control.enabled ? HoloniightPalette.textPrimary : HoloniightPalette.textDisabled
        font.pixelSize: 14
    }

    background: Rectangle {
        implicitWidth: 180
        implicitHeight: 28
        radius: HoloniightPalette.radiusControl
        color: {
            if (control.pressed || control.highlighted) return HoloniightPalette.primary
            if (control.hovered)                        return HoloniightPalette.surfaceHover
            return "transparent"
        }
    }
}
