// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

import QtQuick
import QtQuick.Templates as T
import Holonight.Core
import Holonight.impl as Impl

T.TabButton {
    id: root

    readonly property Impl.ControlPalette controlColors: Impl.ControlPalette {
        palette: root.palette
        colorGroup: !root.enabled ? Impl.ControlPalette.Disabled
                                 : (root.Window.window && !root.Window.window.active
                                    ? Impl.ControlPalette.Inactive : Impl.ControlPalette.Active)
        textRole: Impl.ControlPalette.ButtonText
        fillRole: Impl.ControlPalette.Button
    }

    font.family: HolonightTheme.uiFont
    font.pointSize: HolonightTheme.bodySize

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
            if (!root.enabled) return root.controlColors.colors.textDisabled
            if (root.checked)  return root.controlColors.colors.primary
            return (root.HnInputInteraction.hoverAllowed && root.hovered) ? root.controlColors.colors.textPrimary : root.controlColors.colors.textMuted
        }
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        textFormat: Text.PlainText
    }

    background: Item {
        // Tab body
        Rectangle {
            anchors.fill: parent
            color: (root.HnInputInteraction.hoverAllowed && root.hovered) ? root.controlColors.colors.surfaceHover : Qt.rgba(0, 0, 0, 0)
        }

        // Active indicator bar
        Rectangle {
            anchors.bottom: parent.bottom
            width: parent.width
            height: 2
            color: root.controlColors.colors.primary
            visible: root.checked
        }

        // Focus border
        Rectangle {
            anchors.fill: parent
            color: Qt.rgba(0, 0, 0, 0)
            border.color: root.controlColors.colors.borderFocus
            border.width: HnMetrics.borderWidth
            visible: root.visualFocus
        }
    }
}
