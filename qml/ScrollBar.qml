// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

import QtQuick
import QtQuick.Templates as T
import Holonight.Core
import Holonight.impl as Impl

T.ScrollBar {
    id: root

    readonly property Impl.ControlPalette controlColors: Impl.ControlPalette {
        palette: root.palette
        colorGroup: !root.enabled ? Impl.ControlPalette.Disabled
                                 : (root.Window.window && !root.Window.window.active
                                    ? Impl.ControlPalette.Inactive : Impl.ControlPalette.Active)
        textRole: Impl.ControlPalette.WindowText
        fillRole: Impl.ControlPalette.Base
    }

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    minimumSize: 0.05
    hoverEnabled: true

    contentItem: Rectangle {
        readonly property bool isVertical: root.orientation === Qt.Vertical

        implicitWidth: root.interactive ? 10 : 4
        implicitHeight: root.interactive ? 10 : 4

        Binding on width {
            when: root.orientation === Qt.Vertical
            value: root.interactive ? (root.hovered ? 8 : 6) : 4
        }

        Binding on height {
            when: !(root.orientation === Qt.Vertical)
            value: root.interactive ? (root.hovered ? 8 : 6) : 4
        }

        x: root.orientation === Qt.Vertical ? (root.width - width) / 2 : 0
        y: !(root.orientation === Qt.Vertical) ? (root.height - height) / 2 : 0

        radius: Math.min(width, height) / 2
        color: root.pressed
               ? root.controlColors.colors.primaryPressed
               : (root.hovered ? root.controlColors.colors.textPrimary : root.controlColors.colors.textMuted)
        opacity: root.policy === T.ScrollBar.AlwaysOn || (root.active && root.size < 1.0) ? 1.0 : 0.0

        Behavior on width { NumberAnimation { duration: 120 } }
        Behavior on height { NumberAnimation { duration: 120 } }
        Behavior on opacity { NumberAnimation { duration: 120 } }
        Behavior on color { ColorAnimation { duration: 120 } }
    }

    background: Rectangle {
        implicitWidth: 10
        implicitHeight: 10
        color: root.controlColors.colors.surface
        opacity: root.hovered && (root.policy === T.ScrollBar.AlwaysOn || (root.active && root.size < 1.0)) ? 0.6 : 0.0

        Behavior on opacity { NumberAnimation { duration: 120 } }
    }
}
