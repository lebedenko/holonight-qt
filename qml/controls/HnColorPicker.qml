// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls.Basic as C
import QtQuick.Shapes
import QtQuick.Templates as T
import Holonight as H
import Holonight.Core

T.Control {
    id: root

    font.family: HolonightTheme.uiFont
    font.pointSize: HolonightTheme.bodySize

    property var colors: {
        const base = [
            HoloniightPalette.accentCyan,
            HoloniightPalette.accentBlue,
            HoloniightPalette.accentViolet,
            HoloniightPalette.accentYellow,
            HoloniightPalette.success,
            HoloniightPalette.warning,
            HoloniightPalette.error
        ]
        return base.filter((c, i) => base.findIndex(o => o.toString() === c.toString()) === i)
    }
    property color selectedColor
    property bool autoUpdateSelectedColor: true
    property int sizeRole: HnControlSize.Normal
    property int focusedIndex: 0
    readonly property int resolvedSizeRole: HnControlMetrics.normalizedSizeRole(root.sizeRole)
    readonly property real swatchDiameter: HnControlMetrics.controlHeight(root.resolvedSizeRole)
    readonly property real ringExtra: 10
    readonly property real swatchBoxSize: root.swatchDiameter + root.ringExtra

    signal colorSelected(color color)

    function moveFocus(index: int): void {
        root.focusedIndex = Math.max(0, Math.min(repeater.count - 1, index))
    }
    function activate(index: int): void {
        if (index < 0 || index >= repeater.count)
            return
        root.focusedIndex = index
        if (root.autoUpdateSelectedColor)
            root.selectedColor = root.colors[index]
        root.colorSelected(root.colors[index])
    }

    implicitWidth: row.implicitWidth
    implicitHeight: row.implicitHeight
    activeFocusOnTab: true
    Accessible.role: Accessible.Grouping
    Keys.onLeftPressed: root.moveFocus(root.focusedIndex - 1)
    Keys.onRightPressed: root.moveFocus(root.focusedIndex + 1)
    Keys.onSpacePressed: root.activate(root.focusedIndex)
    Keys.onReturnPressed: root.activate(root.focusedIndex)
    Keys.onEnterPressed: root.activate(root.focusedIndex)

    contentItem: H.ScrollView {
        id: scroll

        contentWidth: row.implicitWidth
        implicitHeight: row.implicitHeight
        C.ScrollBar.horizontal.policy: row.implicitWidth > scroll.availableWidth
                                        ? C.ScrollBar.AsNeeded : C.ScrollBar.AlwaysOff
        C.ScrollBar.vertical.policy: C.ScrollBar.AlwaysOff
        clip: row.implicitWidth > scroll.availableWidth

        Row {
            id: row

            spacing: HnControlMetrics.internalSpacing(root.resolvedSizeRole)

            Repeater {
                id: repeater

                model: root.colors

                delegate: T.AbstractButton {
                    id: swatch

                    required property int index
                    required property color modelData
                    readonly property bool selected: root.selectedColor.valid
                                                      && swatch.modelData === root.selectedColor
                    readonly property bool keyboardFocused: swatch.index === root.focusedIndex
                                                             && root.visualFocus
                    readonly property real luminance: 0.299 * swatch.modelData.r
                                                      + 0.587 * swatch.modelData.g
                                                      + 0.114 * swatch.modelData.b
                    readonly property color tickColor: swatch.luminance > 0.6 ? "#1A1A1A" : "#FFFFFF"
                    readonly property real checkmarkSize: root.swatchDiameter * 0.5

                    function tickPath(): string {
                        const s = swatch.checkmarkSize / 24
                        return `M ${5 * s} ${13 * s} L ${10 * s} ${18 * s} L ${19 * s} ${6 * s}`
                    }

                    implicitWidth: root.swatchBoxSize
                    implicitHeight: root.swatchBoxSize
                    hoverEnabled: true
                    Accessible.role: Accessible.RadioButton
                    Accessible.name: swatch.modelData.toString()
                    Accessible.checked: swatch.selected
                    onClicked: root.activate(swatch.index)

                    background: Item {
                        Rectangle {
                            visible: swatch.selected
                            anchors.centerIn: parent
                            width: root.swatchDiameter + root.ringExtra
                            height: width
                            radius: width / 2
                            color: "transparent"
                            border.width: 2
                            border.color: swatch.modelData
                        }
                        Rectangle {
                            visible: swatch.keyboardFocused
                            anchors.centerIn: parent
                            width: root.swatchDiameter + root.ringExtra + 6
                            height: width
                            radius: width / 2
                            color: "transparent"
                            border.width: HoloniightPalette.focusBorderWidth
                            border.color: HoloniightPalette.borderFocus
                        }
                        Rectangle {
                            anchors.centerIn: parent
                            width: root.swatchDiameter
                            height: width
                            radius: width / 2
                            color: swatch.enabled
                                   ? swatch.modelData
                                   : Qt.rgba(swatch.modelData.r, swatch.modelData.g, swatch.modelData.b, 0.4)
                        }
                        Shape {
                            visible: swatch.selected
                            anchors.centerIn: parent
                            width: swatch.checkmarkSize
                            height: width
                            preferredRendererType: Shape.CurveRenderer
                            ShapePath {
                                strokeColor: swatch.tickColor
                                strokeWidth: Math.max(1.5, root.swatchDiameter * 0.06)
                                fillColor: "transparent"
                                capStyle: ShapePath.RoundCap
                                joinStyle: ShapePath.RoundJoin
                                PathSvg { path: swatch.tickPath() }
                            }
                        }
                    }
                }
            }
        }
    }
}
