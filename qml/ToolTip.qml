// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

import QtQuick
import QtQuick.Templates as T
import Holonight

T.ToolTip {
    id: control

    x: parent ? (parent.width - implicitWidth) / 2 : 0
    y: -implicitHeight - 4

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    margins: 8
    padding: 8

    delay: 500
    timeout: 5000

    contentItem: Text {
        text: control.text
        font: control.font
        color: HoloniightPalette.textInverse
        wrapMode: Text.Wrap
    }

    background: Rectangle {
        color: HoloniightPalette.surfaceInverse
        border.color: HoloniightPalette.borderPassive
        border.width: HoloniightPalette.borderWidth
        radius: HoloniightPalette.radiusPopup
    }
}
