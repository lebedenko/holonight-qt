// SPDX-License-Identifier: GPL-3.0-or-later

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import QtQuick.Shapes
import QtQuick.Templates as T
import Holonight.Core

T.Control {
    id: root

    font.family: HolonightTheme.uiFont
    font.pointSize: HolonightTheme.bodySize

    property var model: []
    property string textRole: "text"
    property string valueRole: "value"
    property int currentIndex: modelCount() > 0 ? 0 : -1
    readonly property var currentValue: valueAt(currentIndex)
    signal activated(int index, var value)

    function modelCount(): int {
        if (!root.model)
            return 0
        if (root.model.count !== undefined)
            return root.model.count
        return root.model.length !== undefined ? root.model.length : 0
    }
    function rowAt(index: int): var {
        if (!root.model || index < 0)
            return undefined
        if (typeof root.model.get === "function")
            return root.model.get(index)
        return root.model[index]
    }
    function roleAt(index: int, role: string, fallback: var): var {
        const row = rowAt(index)
        if (row === undefined || row === null)
            return fallback
        if (typeof row === "object" && row[role] !== undefined)
            return row[role]
        return row
    }
    function textAt(index: int): string {
        return String(roleAt(index, root.textRole, ""))
    }
    function valueAt(index: int): var {
        return roleAt(index, root.valueRole, undefined)
    }
    function activate(index: int): void {
        if (!root.enabled || index < 0 || index >= repeater.count)
            return
        root.currentIndex = index
        root.activated(index, root.valueAt(index))
    }

    implicitWidth: content.implicitWidth + leftPadding + rightPadding
    implicitHeight: Math.max(32, content.implicitHeight + topPadding + bottomPadding)
    padding: Math.max(3, HnMetrics.borderWidth + 2)
    activeFocusOnTab: true
    Accessible.role: Accessible.Grouping
    Keys.onLeftPressed: activate(Math.max(0, currentIndex - 1))
    Keys.onRightPressed: activate(Math.min(repeater.count - 1, currentIndex + 1))

    contentItem: RowLayout {
        id: content
        spacing: 2
        Repeater {
            id: repeater
            model: root.model
            T.Button {
                id: segment

                required property int index
                readonly property bool firstSegment: index === 0
                readonly property bool lastSegment: index === repeater.count - 1
                readonly property real outerRadius: Math.max(0, HnAppearance.roundedRadius(HnSurfaceRole.Control, segment.width, segment.height, HnAppearance.revision) - root.padding)
                readonly property color segmentColor: {
                    if (!segment.enabled)
                        return HoloniightPalette.surface
                    if (segment.down)
                        return HoloniightPalette.surfaceElevated
                    if (segment.checked)
                        return HoloniightPalette.surfaceHover
                    return segment.hovered ? HoloniightPalette.surfaceRaised : HoloniightPalette.surface
                }

                function segmentPath(): string {
                    const right = segment.width
                    const bottom = segment.height
                    const radius = Math.min(segment.outerRadius, right / 2, bottom / 2)
                    const leftRadius = segment.firstSegment ? radius : 0
                    const rightRadius = segment.lastSegment ? radius : 0
                    let path = `M ${leftRadius} 0 L ${right - rightRadius} 0`
                    path += rightRadius > 0 ? ` Q ${right} 0 ${right} ${rightRadius}` : ` L ${right} 0`
                    path += ` L ${right} ${bottom - rightRadius}`
                    path += rightRadius > 0 ? ` Q ${right} ${bottom} ${right - rightRadius} ${bottom}`
                                           : ` L ${right} ${bottom}`
                    path += ` L ${leftRadius} ${bottom}`
                    path += leftRadius > 0 ? ` Q 0 ${bottom} 0 ${bottom - leftRadius}` : ` L 0 ${bottom}`
                    path += ` L 0 ${leftRadius}`
                    path += leftRadius > 0 ? ` Q 0 0 ${leftRadius} 0` : " L 0 0"
                    return path + " Z"
                }

                objectName: "hnSegment" + index
                text: root.textAt(index)
                font: root.font
                checkable: true
                checked: index === root.currentIndex
                hoverEnabled: true
                padding: 6
                horizontalPadding: 12
                implicitWidth: Math.max(80, implicitContentWidth + leftPadding + rightPadding)
                implicitHeight: Math.max(implicitContentHeight + topPadding + bottomPadding,
                                         HnMetrics.controlHeight(HnControlSize.Normal) - root.topPadding - root.bottomPadding)
                onClicked: root.activate(index)
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.preferredWidth: implicitWidth
                Layout.preferredHeight: implicitHeight
                Accessible.role: Accessible.RadioButton

                contentItem: Text {
                    text: segment.text
                    font: segment.font
                    color: segment.enabled ? HoloniightPalette.textPrimary : HoloniightPalette.textDisabled
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                    textFormat: Text.PlainText
                }
                background: Item {
                    Shape {
                        anchors.fill: parent
                        preferredRendererType: Shape.CurveRenderer
                        ShapePath {
                            fillColor: segment.segmentColor
                            strokeColor: "transparent"
                            PathSvg {
                                path: segment.segmentPath()
                            }
                        }
                    }
                }
            }
        }
    }
    background: Rectangle {
        color: HoloniightPalette.surface
        radius: HnAppearance.roundedRadius(HnSurfaceRole.Control, width, height, HnAppearance.revision)
        border.width: HnMetrics.borderWidth
        border.color: root.visualFocus ? HoloniightPalette.borderFocus : HoloniightPalette.borderPassive
    }
}
