// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Templates as T
import Holonight.Core

T.ItemDelegate {
    id: root

    font.family: HolonightTheme.uiFont
    font.pointSize: HolonightTheme.bodySize

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    padding: 8
    spacing: 8
    hoverEnabled: true

    contentItem: Text {
        readonly property bool isSelected: root.highlighted || root.checked || (root.ListView ? root.ListView.isCurrentItem : false)

        text: root.text
        font: root.font
        color: root.enabled ? HoloniightPalette.textPrimary : HoloniightPalette.textDisabled
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
        textFormat: Text.PlainText
    }

    background: Item {
        readonly property real semanticRadius: HnAppearance.roundedRadius(HnSurfaceRole.Control,
                                                                          width, height,
                                                                          HnAppearance.revision)
        readonly property bool isSelected: root.highlighted || root.checked || (root.ListView ? root.ListView.isCurrentItem : false)

        implicitWidth: 100
        implicitHeight: 32
        visible: root.down || root.hovered || isSelected || root.visualFocus
        Rectangle {
            objectName: "hnItemDelegateBase"
            anchors.fill: parent
            color: root.down ? HoloniightPalette.surfaceElevated
                             : (!parent.isSelected && root.hovered ? HoloniightPalette.surfaceHover : "transparent")
            radius: parent.semanticRadius
            Accessible.ignored: true
        }
        Rectangle {
            objectName: "hnItemDelegateSelectedOverlay"
            anchors.fill: parent
            color: root.hovered ? HoloniightPalette.surfaceSelectedHover
                                : HoloniightPalette.surfaceSelected
            radius: parent.semanticRadius
            visible: parent.isSelected && !root.down
            Accessible.ignored: true
        }
        Rectangle {
            anchors.fill: parent
            color: "transparent"
            radius: parent.semanticRadius
            border.width: root.visualFocus ? HoloniightPalette.focusBorderWidth : 0
            border.color: HoloniightPalette.borderFocus
            Accessible.ignored: true
        }
    }
}
