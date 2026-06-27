// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

import QtQuick
import QtQuick.Templates as T
import Holonight

T.ScrollBar {
    id: root

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    minimumSize: 0.05

    contentItem: Rectangle {
        implicitWidth: root.interactive ? 10 : 4
        implicitHeight: root.interactive ? 10 : 4
        width: root.interactive ? (root.hovered ? 10 : 6) : 4
        height: root.interactive ? (root.hovered ? 10 : 6) : 4

        radius: width / 2
        color: root.hovered ? HoloniightPalette.textPrimary : HoloniightPalette.textMuted
        opacity: root.policy === T.ScrollBar.AlwaysOn || (root.active && root.size < 1.0) ? 1.0 : 0.0

        Behavior on width { NumberAnimation { duration: 120 } }
        Behavior on height { NumberAnimation { duration: 120 } }
        Behavior on opacity { NumberAnimation { duration: 120 } }
    }

    background: Rectangle {
        color: HoloniightPalette.surface
        opacity: root.hovered ? 0.6 : 0.3

        Behavior on opacity { NumberAnimation { duration: 120 } }
    }
}
