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

    enum CrossAxisAlignment {
        Leading,
        Center,
        Trailing
    }

    property int orientation: Qt.Horizontal
    property color color: HoloniightPalette.borderPassive
    property int thickness: 1
    property int fadeMode: HnSeparator.Solid
    property int crossAxisAlignment: HnSeparator.Leading

    readonly property int effectiveOrientation: orientation === Qt.Vertical ? Qt.Vertical : Qt.Horizontal
    readonly property int effectiveFadeMode: fadeMode >= HnSeparator.Solid && fadeMode <= HnSeparator.FadeEnd
                                            ? fadeMode : HnSeparator.Solid
    readonly property color transparentColor: Qt.rgba(color.r, color.g, color.b, 0)

    implicitWidth: effectiveOrientation === Qt.Vertical ? geometry.logicalThickness : 0
    implicitHeight: effectiveOrientation === Qt.Horizontal ? geometry.logicalThickness : 0
    // Explicit dimensions define the slot; they never stretch the physical stroke.
    width: implicitWidth
    height: implicitHeight
    Accessible.ignored: true

    HnSeparatorGeometry {
        id: geometry

        objectName: "separatorGeometry"
        orientation: root.effectiveOrientation
        requestedThickness: root.thickness
        crossAxisAlignment: root.crossAxisAlignment
    }

    Rectangle {
        id: line

        objectName: "separatorLine"
        x: geometry.paintedRect.x
        y: geometry.paintedRect.y
        width: 1
        height: 1
        visible: geometry.paintedRect.width > 0 && geometry.paintedRect.height > 0
        antialiasing: false
        // Normalize both axes: fractional Rectangle sizes overpaint with software rendering.
        transform: Scale {
            xScale: geometry.paintedRect.width
            yScale: geometry.paintedRect.height
        }
        gradient: Gradient {
            orientation: root.effectiveOrientation === Qt.Horizontal
                         ? Gradient.Horizontal : Gradient.Vertical
            GradientStop {
                objectName: "separatorStartStop"
                position: 0
                color: root.effectiveFadeMode === HnSeparator.FadeBoth
                       || root.effectiveFadeMode === HnSeparator.FadeStart
                       ? root.transparentColor : root.color
            }
            GradientStop {
                objectName: "separatorMiddleStop"
                position: 0.5
                color: root.color
            }
            GradientStop {
                objectName: "separatorEndStop"
                position: 1
                color: root.effectiveFadeMode === HnSeparator.FadeBoth
                       || root.effectiveFadeMode === HnSeparator.FadeEnd
                       ? root.transparentColor : root.color
            }
        }
    }
}
