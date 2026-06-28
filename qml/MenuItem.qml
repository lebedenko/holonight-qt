// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

import QtQuick
import QtQuick.Templates as T
import Holonight

T.MenuItem {
    id: root

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    padding: 6
    leftPadding: 12
    rightPadding: 12
    spacing: 6

    contentItem: Row {
        spacing: root.spacing

        Item {
            width: 16
            height: 16
            anchors.verticalCenter: parent.verticalCenter

            // Checkmark for checkable items
            visible: root.checkable
            Text {
                anchors.centerIn: parent
                text: root.checked ? "✓" : ""
                color: HoloniightPalette.primary
                font.pixelSize: 12
                textFormat: Text.PlainText
            }
        }

        Text {
            text: root.text
            font: root.font
            color: root.enabled ? (root.highlighted ? HoloniightPalette.onPrimary : HoloniightPalette.textPrimary) : HoloniightPalette.textDisabled
            verticalAlignment: Text.AlignVCenter
            textFormat: Text.PlainText
        }
    }

    arrow: Text {
        x: root.width - width - 8
        anchors.verticalCenter: parent.verticalCenter
        visible: root.subMenu
        text: "›"
        color: root.enabled ? HoloniightPalette.textPrimary : HoloniightPalette.textDisabled
        font.pixelSize: 14
        textFormat: Text.PlainText
    }

    background: Rectangle {
        implicitWidth: 180
        implicitHeight: 28
        visible: root.pressed || root.highlighted || root.hovered
        radius: HoloniightPalette.radiusControl
        color: root.pressed || root.highlighted ? HoloniightPalette.primary : HoloniightPalette.surfaceHover
    }
}
