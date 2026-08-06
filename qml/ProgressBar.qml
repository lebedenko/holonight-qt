// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

import QtQuick
import QtQuick.Templates as T
import Holonight.Core

T.ProgressBar {
    id: root

    property real barHeight: 8

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    contentItem: Item {
        id: track

        readonly property real nominalIndicatorWidth: width * 0.3
        property real indicatorX: 0
        property real indicatorWidth: 0

        implicitWidth: 200
        implicitHeight: root.barHeight

        Rectangle {
            id: indicator

            objectName: "hnProgressBarIndicator"
            x: root.indeterminate ? track.indicatorX : 0
            width: root.indeterminate ? track.indicatorWidth : track.width * root.visualPosition
            height: root.contentItem.height
            radius: height / 2
            color: HoloniightPalette.primary

            // Indeterminate animation
            SequentialAnimation {
                running: root.indeterminate
                loops: Animation.Infinite

                ParallelAnimation {
                    NumberAnimation {
                        target: track
                        property: "indicatorX"
                        from: 0
                        to: 0
                        duration: 250
                    }
                    NumberAnimation {
                        objectName: "hnProgressBarIndeterminateEnter"
                        target: track
                        property: "indicatorWidth"
                        from: 0
                        to: track.nominalIndicatorWidth
                        duration: 250
                        easing.type: Easing.OutSine
                    }
                }

                NumberAnimation {
                    objectName: "hnProgressBarIndeterminateTravel"
                    target: track
                    property: "indicatorX"
                    from: 0
                    to: track.width - track.nominalIndicatorWidth
                    duration: 700
                    easing.type: Easing.InOutSine
                }

                ParallelAnimation {
                    NumberAnimation {
                        target: track
                        property: "indicatorX"
                        from: track.width - track.nominalIndicatorWidth
                        to: track.width
                        duration: 250
                    }
                    NumberAnimation {
                        objectName: "hnProgressBarIndeterminateExit"
                        target: track
                        property: "indicatorWidth"
                        from: track.nominalIndicatorWidth
                        to: 0
                        duration: 250
                        easing.type: Easing.InSine
                    }
                }
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
