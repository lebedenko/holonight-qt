// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

pragma ComponentBehavior: Bound

import QtQuick
import Holonight.Core

Item {
    id: root

    enum FadeMode {
        Solid,
        FadeBoth,
        FadeStart,
        FadeEnd
    }

    property int orientation: Qt.Horizontal
    property color color: HoloniightPalette.borderSubtle
    property real thickness: HnMetrics.separatorWidth
    property int fadeMode: HnSeparator.Solid
    property real centerOpacity: 1.0
    property real edgeOpacity: 0.0

    readonly property int effectiveOrientation: orientation === Qt.Vertical ? Qt.Vertical : Qt.Horizontal
    readonly property int effectiveFadeMode: fadeMode >= HnSeparator.Solid && fadeMode <= HnSeparator.FadeEnd
                                            ? fadeMode : HnSeparator.Solid
    readonly property real effectiveCenterOpacity: Math.max(0, Math.min(1, centerOpacity))
    readonly property real effectiveEdgeOpacity: Math.max(0, Math.min(1, edgeOpacity))
    readonly property color centerColor: Qt.rgba(color.r, color.g, color.b,
                                                 color.a * effectiveCenterOpacity)
    readonly property color edgeColor: Qt.rgba(color.r, color.g, color.b,
                                               color.a * effectiveEdgeOpacity)
    readonly property color middleColor: {
        if (effectiveFadeMode === HnSeparator.FadeStart
                || effectiveFadeMode === HnSeparator.FadeEnd) {
            return Qt.rgba(color.r, color.g, color.b,
                           color.a * (effectiveCenterOpacity + effectiveEdgeOpacity) / 2)
        }
        return centerColor
    }

    implicitWidth: effectiveOrientation === Qt.Vertical ? Math.max(0, thickness) : 0
    implicitHeight: effectiveOrientation === Qt.Horizontal ? Math.max(0, thickness) : 0
    Accessible.ignored: true

    HnSeparatorGeometry {
        id: geometry

        objectName: "separatorGeometry"
        orientation: root.effectiveOrientation
        requestedThickness: root.thickness
        standardThickness: HnMetrics.separatorWidth
    }

    Rectangle {
        id: line

        objectName: "separatorLine"
        x: root.effectiveOrientation === Qt.Vertical ? geometry.paintedOffset : 0
        y: root.effectiveOrientation === Qt.Horizontal ? geometry.paintedOffset : 0
        width: root.effectiveOrientation === Qt.Vertical ? geometry.paintedThickness : root.width
        height: root.effectiveOrientation === Qt.Horizontal ? geometry.paintedThickness : root.height
        visible: width > 0 && height > 0
        antialiasing: false
        gradient: Gradient {
            orientation: root.effectiveOrientation === Qt.Horizontal
                         ? Gradient.Horizontal : Gradient.Vertical
            GradientStop {
                objectName: "separatorStartStop"
                position: 0
                color: root.effectiveFadeMode === HnSeparator.FadeBoth
                       || root.effectiveFadeMode === HnSeparator.FadeStart
                       ? root.edgeColor : root.centerColor
            }
            GradientStop {
                objectName: "separatorMiddleStop"
                position: 0.5
                color: root.middleColor
            }
            GradientStop {
                objectName: "separatorEndStop"
                position: 1
                color: root.effectiveFadeMode === HnSeparator.FadeBoth
                       || root.effectiveFadeMode === HnSeparator.FadeEnd
                       ? root.edgeColor : root.centerColor
            }
        }
    }
}
