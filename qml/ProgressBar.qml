// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

import QtQuick
import QtQuick.Templates as T
import Holonight

T.ProgressBar {
    id: root

    property real barHeight: 8

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    contentItem: Item {
        implicitWidth: 200
        implicitHeight: root.barHeight

        Rectangle {
            width: root.indeterminate ? root.contentItem.width * 0.3 : root.contentItem.width * root.visualPosition
            height: root.contentItem.height
            radius: height / 2
            color: HoloniightPalette.primary

            // Indeterminate animation
            SequentialAnimation on x {
                running: root.indeterminate
                loops: Animation.Infinite
                NumberAnimation { from: -root.contentItem.width * 0.3; to: root.contentItem.width; duration: 1200; easing.type: Easing.InOutSine }
            }
        }
    }

    background: Rectangle {
        implicitWidth: 200
        implicitHeight: root.barHeight
        radius: height / 2
        color: HoloniightPalette.surface
    }
}
