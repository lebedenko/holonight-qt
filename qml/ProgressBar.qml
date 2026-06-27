// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

import QtQuick
import QtQuick.Templates as T
import Holonight

T.ProgressBar {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    property real barHeight: 8

    contentItem: Item {
        implicitWidth: 200
        implicitHeight: control.barHeight

        Rectangle {
            width: control.indeterminate ? parent.width * 0.3 : parent.width * control.visualPosition
            height: parent.height
            radius: height / 2
            color: HoloniightPalette.primary

            // Indeterminate animation
            SequentialAnimation on x {
                running: control.indeterminate
                loops: Animation.Infinite
                NumberAnimation { from: -parent.width * 0.3; to: parent.width; duration: 1200; easing.type: Easing.InOutSine }
            }
        }
    }

    background: Rectangle {
        implicitWidth: 200
        implicitHeight: control.barHeight
        radius: height / 2
        color: HoloniightPalette.surface
    }
}
