// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Templates as T
import Holonight

T.ComboBox {
    id: root

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    leftPadding: 8
    rightPadding: 28
    topPadding: 6
    bottomPadding: 6

    delegate: comboBoxDelegate

    indicator: Text {
        x: root.width - width - 8
        y: (root.height - height) / 2
        width: 12
        height: 8
        text: "▾"
        color: HoloniightPalette.textPrimary
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        textFormat: Text.PlainText
    }

    contentItem: Text {
        leftPadding: 0
        rightPadding: root.indicator.width + 4
        text: root.displayText
        font: root.font
        color: root.enabled ? HoloniightPalette.textPrimary : HoloniightPalette.textDisabled
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
        textFormat: Text.PlainText
    }

    background: Rectangle {
        implicitWidth: 120
        implicitHeight: 32
        radius: HoloniightPalette.radiusControl
        color: root.hovered ? HoloniightPalette.surfaceHover : HoloniightPalette.surface
        border.color: root.visualFocus || root.popup.visible ? HoloniightPalette.borderFocus : (root.enabled ? HoloniightPalette.borderPassive : HoloniightPalette.borderPassive)
        border.width: (root.visualFocus || root.popup.visible) ? HoloniightPalette.focusBorderWidth : HoloniightPalette.borderWidth
    }

    popup: T.Popup {
        y: root.height
        width: root.width
        implicitHeight: Math.min(contentItem.implicitHeight, Window.window ? Math.max(0, Window.window.height - y - 8) : contentItem.implicitHeight)
        padding: 1

        contentItem: ListView {
            clip: true
            implicitHeight: Math.min(contentHeight, Window.window ? Math.max(0, Window.window.height - root.popup.y - 8) : contentHeight)
            model: root.delegateModel
            currentIndex: root.highlightedIndex
            ScrollBar.vertical: T.ScrollBar {}
        }

        background: Rectangle {
            color: HoloniightPalette.surface
            border.color: HoloniightPalette.borderPassive
            border.width: HoloniightPalette.borderWidth
            radius: HoloniightPalette.radiusPopup
        }
    }

    Component {
        id: comboBoxDelegate

        T.ItemDelegate {
            id: delegateRoot

            required property int index

            width: root.popup.width
            height: 28
            text: root.textAt(index)
            highlighted: root.highlightedIndex === index

            contentItem: Text {
                text: delegateRoot.text
                color: ListView.isCurrentItem ? HoloniightPalette.onPrimary : HoloniightPalette.textPrimary
                font: root.font
                verticalAlignment: Text.AlignVCenter
                textFormat: Text.PlainText
                leftPadding: 8
            }

            background: Rectangle {
                visible: delegateRoot.highlighted || ListView.isCurrentItem
                color: delegateRoot.highlighted ? HoloniightPalette.surfaceHover : HoloniightPalette.primary
                radius: HoloniightPalette.radiusControl
            }
        }
    }
}
