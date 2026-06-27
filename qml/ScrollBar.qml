// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

import QtQuick
import QtQuick.Templates as T
import Holonight

T.ScrollBar {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    minimumSize: 0.05

    contentItem: Rectangle {
        implicitWidth: control.interactive ? (control.hovered ? 10 : 6) : 4
        implicitHeight: control.interactive ? (control.hovered ? 10 : 6) : 4

        radius: width / 2
        color: control.hovered ? HoloniightPalette.textPrimary : HoloniightPalette.textMuted
        opacity: control.policy === T.ScrollBar.AlwaysOn || (control.active && control.size < 1.0) ? 1.0 : 0.0

        Behavior on implicitWidth { NumberAnimation { duration: 120 } }
        Behavior on implicitHeight { NumberAnimation { duration: 120 } }
        Behavior on opacity { NumberAnimation { duration: 120 } }
    }

    background: Rectangle {
        color: HoloniightPalette.surface
        opacity: control.hovered ? 0.6 : 0.3

        Behavior on opacity { NumberAnimation { duration: 120 } }
    }
}
