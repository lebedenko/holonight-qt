// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

import QtQuick
import QtQuick.Templates as T
import Holonight
import Holonight.Core

T.Menu {
    id: root

    font.family: HolonightTheme.uiFont
    font.pointSize: HolonightTheme.bodySize

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    margins: 0
    padding: 4

    delegate: MenuItem {}

    contentItem: ListView {
        implicitHeight: contentHeight
        model: root.contentModel
        interactive: contentHeight > implicitHeight
        clip: contentHeight > height
        currentIndex: root.currentIndex
    }

    background: Rectangle {
        readonly property real semanticRadius: HnAppearance.roundedRadius(HnSurfaceRole.Menu,
                                                                          width, height,
                                                                          HnAppearance.revision)

        implicitWidth: 180
        color: HoloniightPalette.surfaceRaised
        border.color: HoloniightPalette.borderPassive
        border.width: HnMetrics.borderWidth
        radius: semanticRadius
    }
}
