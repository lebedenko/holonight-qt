// SPDX-License-Identifier: GPL-3.0-or-later

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as C
import QtQuick.Shapes
import Holonight.Core
import Holonight.impl

C.Control {
    id: root

    property string text
    property var keyGroups: []
    property bool wrap: false
    readonly property string accessibleText: keyGroups.length > 0
        ? keyGroups.map(group => group.map(key => d.keyName(key)).join(qsTr(" plus "))).join(qsTr(" or "))
        : text

    font.family: HolonightTheme.monospaceFont
    font.pointSize: HolonightTheme.monospaceFontSize

    // Calibrated to the original 4/6 px padding at the default 12 pt JetBrains Mono font (~22 px line height).
    padding: metrics.height * (4 / 22)
    leftPadding: metrics.height * (6 / 22)
    rightPadding: leftPadding
    Accessible.role: Accessible.StaticText
    Accessible.name: accessibleText

    FontMetrics {
        id: metrics
        font: root.font
    }

    QtObject {
        id: d

        readonly property color ink: root.enabled ? HoloniightPalette.textSecondary : HoloniightPalette.textDisabled
        readonly property real symbolWidth: metrics.capitalHeight * 1.35
        readonly property var groups: root.keyGroups.map(group => group.map(key => ({
            label: keyName(key), path: symbolPath(key)
        })))
        readonly property real naturalWidth: {
            // advanceWidth() is invokable: explicitly track font changes even for text-only groups.
            metrics.font
            return groups.reduce((total, group, index) => total
                + group.reduce((width, key, keyIndex) => width + keyWidth(key)
                    + (keyIndex > 0 ? metrics.advanceWidth("+") : 0), 0)
                + (index > 0 ? metrics.advanceWidth(" / ") : 0), 0)
        }
        readonly property var layout: arrange()

        function keyName(key: int): string {
            switch (key) {
            case Qt.Key_Shift: return qsTr("Shift")
            case Qt.Key_Control: return qsTr("Ctrl")
            case Qt.Key_Alt: return qsTr("Alt")
            case Qt.Key_Meta: return qsTr("Meta")
            case Qt.Key_Tab: return qsTr("Tab")
            case Qt.Key_Backtab: return qsTr("Backtab")
            case Qt.Key_Return: return qsTr("Return")
            case Qt.Key_Enter: return qsTr("Enter")
            case Qt.Key_Backspace: return qsTr("Backspace")
            case Qt.Key_Space: return qsTr("Space")
            case Qt.Key_Delete: return qsTr("Delete")
            case Qt.Key_Escape: return qsTr("Esc")
            case Qt.Key_Up: return qsTr("Up")
            case Qt.Key_Down: return qsTr("Down")
            case Qt.Key_Left: return qsTr("Left")
            case Qt.Key_Right: return qsTr("Right")
            case Qt.Key_Home: return qsTr("Home")
            case Qt.Key_End: return qsTr("End")
            case Qt.Key_PageUp: return qsTr("Page Up")
            case Qt.Key_PageDown: return qsTr("Page Down")
            case Qt.Key_Insert: return qsTr("Insert")
            case Qt.Key_CapsLock: return qsTr("Caps Lock")
            case Qt.Key_NumLock: return qsTr("Num Lock")
            case Qt.Key_ScrollLock: return qsTr("Scroll Lock")
            case Qt.Key_Menu: return qsTr("Menu")
            case Qt.Key_Pause: return qsTr("Pause")
            case Qt.Key_Print: return qsTr("Print")
            case Qt.Key_Plus: return "+"
            case Qt.Key_Slash: return "/"
            }
            return KeyHintNames.keyName(key)
        }

        // Common 16 x 12 grid; 1-unit stroke remains within the capital region.
        function symbolPath(key: int): string {
            switch (key) {
            case Qt.Key_Shift: return "M 8,.5 L 14,6.5 H 10.5 V 11.5 H 5.5 V 6.5 H 2 Z"
            case Qt.Key_Tab: return "M 1,6 H 12 M 7,1 L 12,6 L 7,11 M 15,.5 V 11.5"
            case Qt.Key_Backtab: return "M 15,6 H 4 M 9,1 L 4,6 L 9,11 M 1,.5 V 11.5"
            case Qt.Key_Return:
            case Qt.Key_Enter: return "M 14,.5 V 7 H 1 M 5,3 L 1,7 L 5,11"
            case Qt.Key_Backspace: return "M .5,6 L 5,.5 H 15.5 V 11.5 H 5 Z M 7,3 L 12,9 M 12,3 L 7,9"
            case Qt.Key_Delete: return "M 15.5,6 L 11,.5 H .5 V 11.5 H 11 Z M 4,3 L 9,9 M 9,3 L 4,9"
            case Qt.Key_Space: return "M 1,4 V 9 H 15 V 4"
            case Qt.Key_Up: return "M 8,11.5 V .5 M 2,6.5 L 8,.5 L 14,6.5"
            case Qt.Key_Down: return "M 8,.5 V 11.5 M 2,5.5 L 8,11.5 L 14,5.5"
            case Qt.Key_Left: return "M 15,6 H 1 M 6,1 L 1,6 L 6,11"
            case Qt.Key_Right: return "M 1,6 H 15 M 10,1 L 15,6 L 10,11"
            }
            return ""
        }

        function keyWidth(key): real {
            return key.path.length > 0 ? symbolWidth : metrics.advanceWidth(key.label)
        }

        function arrange() {
            const tokens = []
            const limit = root.wrap ? Math.max(1, root.availableWidth) : Infinity
            const lineHeight = metrics.height
            let x = 0
            let y = 0
            for (let g = 0; g < groups.length; ++g) {
                const group = groups[g]
                const groupWidth = group.reduce((width, key, i) => width + keyWidth(key)
                    + (i > 0 ? metrics.advanceWidth("+") : 0), 0)
                const separatorWidth = g > 0 ? metrics.advanceWidth(" / ") : 0
                // Prefer moving a complete alternative before splitting its keys.
                if (x > 0 && x + separatorWidth + groupWidth > limit) {
                    x = 0
                    y += lineHeight
                }
                for (let k = 0; k < group.length; ++k) {
                    const separator = k > 0 ? "+" : (g > 0 ? " / " : "")
                    const prefixWidth = metrics.advanceWidth(separator)
                    const key = group[k]
                    const width = keyWidth(key)
                    if (x > 0 && x + prefixWidth + width > limit) {
                        x = 0
                        y += lineHeight
                    }
                    tokens.push({ x: x, y: y, prefix: separator, prefixWidth: prefixWidth,
                                  label: key.label, path: key.path, width: width })
                    x += prefixWidth + width
                }
            }
            return { tokens: tokens, height: tokens.length > 0 ? y + lineHeight : 0 }
        }
    }

    contentItem: Item {
        implicitWidth: root.keyGroups.length > 0 ? d.naturalWidth : literal.implicitWidth
        implicitHeight: root.keyGroups.length > 0 ? d.layout.height : literal.implicitHeight
        Accessible.ignored: true

        Text {
            id: literal
            visible: root.keyGroups.length === 0
            width: parent.width
            text: root.text
            textFormat: Text.PlainText
            font: root.font
            color: d.ink
            wrapMode: root.wrap ? Text.Wrap : Text.NoWrap
            Accessible.ignored: true
        }

        Repeater {
            model: d.layout.tokens
            delegate: Item {
                id: token
                required property var modelData
                x: modelData.x
                y: modelData.y
                width: modelData.prefixWidth + modelData.width
                height: metrics.height
                Accessible.ignored: true

                Text {
                    text: token.modelData.prefix
                    font: root.font
                    color: d.ink
                    textFormat: Text.PlainText
                    Accessible.ignored: true
                }
                Text {
                    id: keyLabel
                    x: token.modelData.prefixWidth
                    visible: token.modelData.path.length === 0
                    text: token.modelData.label
                    font: root.font
                    color: d.ink
                    textFormat: Text.PlainText
                    Accessible.ignored: true
                }
                Shape {
                    visible: token.modelData.path.length > 0
                    x: token.modelData.prefixWidth
                    y: keyLabel.baselineOffset - metrics.capitalHeight
                    width: 16
                    height: 12
                    preferredRendererType: Shape.CurveRenderer
                    Accessible.ignored: true
                    transform: Scale { xScale: d.symbolWidth / 16; yScale: metrics.capitalHeight / 12 }
                    ShapePath {
                        strokeColor: d.ink
                        strokeWidth: 1
                        fillColor: "transparent"
                        capStyle: ShapePath.RoundCap
                        joinStyle: ShapePath.RoundJoin
                        PathSvg { path: token.modelData.path }
                    }
                }
            }
        }
    }
    background: Rectangle {
        color: HoloniightPalette.surfaceRaised
        radius: HnAppearance.roundedRadius(HnSurfaceRole.Control, width, height, HnAppearance.revision)
        border.width: HnMetrics.borderWidth
        border.color: HoloniightPalette.borderPassive
    }
}
