// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

import QtQuick
import QtQuick.Templates as T
import Holonight

T.Menu {
    id: root

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    margins: 0
    padding: 4

    delegate: MenuItem {}

    contentItem: ListView {
        implicitHeight: Math.min(contentHeight, Window.window ? Math.max(0, Window.window.height - root.y - 8) : contentHeight)
        model: root.contentModel
        interactive: contentHeight > implicitHeight
        clip: true
        currentIndex: root.currentIndex
    }

    background: Rectangle {
        implicitWidth: 180
        color: HoloniightPalette.surfaceRaised
        border.color: HoloniightPalette.borderPassive
        border.width: HoloniightPalette.borderWidth
        radius: HoloniightPalette.radiusPopup
    }
}
