// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

import QtQuick
import QtQuick.Templates as T
import Holonight

T.ComboBox {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    leftPadding: 8
    rightPadding: 28
    topPadding: 6
    bottomPadding: 6

    delegate: T.ItemDelegate {
        width: control.popup.width
        height: 28
        text: control.textRole ? (Array.isArray(control.model) ? modelData[control.textRole] : model[control.textRole]) : modelData

        contentItem: Text {
            text: parent.text
            color: ListView.isCurrentItem ? HoloniightPalette.onPrimary : HoloniightPalette.onSurface
            font: control.font
            verticalAlignment: Text.AlignVCenter
            leftPadding: 8
        }

        background: Rectangle {
            color: parent.highlighted ? HoloniightPalette.surfaceHover : (ListView.isCurrentItem ? HoloniightPalette.primary : "transparent")
            radius: 4
        }

        highlighted: control.highlightedIndex === index
    }

    indicator: Canvas {
        id: arrow
        x: control.width - width - 8
        y: (control.height - height) / 2
        width: 12
        height: 8
        contextType: "2d"

        Connections {
            target: control
            function onPressedChanged() { arrow.requestPaint() }
        }

        onPaint: {
            var ctx = getContext("2d")
            ctx.reset()
            ctx.fillStyle = HoloniightPalette.onSurface
            ctx.beginPath()
            ctx.moveTo(0, 0)
            ctx.lineTo(width, 0)
            ctx.lineTo(width / 2, height)
            ctx.closePath()
            ctx.fill()
        }
    }

    contentItem: Text {
        leftPadding: 0
        rightPadding: control.indicator.width + 4
        text: control.displayText
        font: control.font
        color: control.enabled ? HoloniightPalette.onSurface : HoloniightPalette.onSurfaceDisabled
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    background: Rectangle {
        implicitWidth: 120
        implicitHeight: 32
        radius: 4
        color: control.hovered ? HoloniightPalette.surfaceHover : HoloniightPalette.surface
        border.color: control.visualFocus || control.popup.visible ? HoloniightPalette.focusRing : (control.enabled ? HoloniightPalette.outline : HoloniightPalette.outlineVariant)
        border.width: (control.visualFocus || control.popup.visible) ? 2 : 1
    }

    popup: T.Popup {
        y: control.height
        width: control.width
        implicitHeight: contentItem.implicitHeight
        padding: 1

        contentItem: ListView {
            clip: true
            implicitHeight: contentHeight
            model: control.delegateModel
            currentIndex: control.highlightedIndex
            ScrollBar.vertical: T.ScrollBar {}
        }

        background: Rectangle {
            color: HoloniightPalette.surface
            border.color: HoloniightPalette.outline
            border.width: 1
            radius: 4
        }
    }
}
