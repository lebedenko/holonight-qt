// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Templates as T
import Holonight.Core
import Holonight.impl as Impl

T.ItemDelegate {
    id: root

    readonly property Impl.ControlPalette controlColors: Impl.ControlPalette {
        palette: root.palette
        colorGroup: !root.enabled ? Impl.ControlPalette.Disabled
                                 : (root.Window.window && !root.Window.window.active
                                    ? Impl.ControlPalette.Inactive : Impl.ControlPalette.Active)
        textRole: Impl.ControlPalette.WindowText
        fillRole: Impl.ControlPalette.Base
    }

    property real topLeftRadius: -1
    property real topRightRadius: -1
    property real bottomLeftRadius: -1
    property real bottomRightRadius: -1

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
        color: root.enabled ? root.controlColors.colors.textPrimary : root.controlColors.colors.textDisabled
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
            color: root.down ? root.controlColors.colors.surfaceElevated
                             : (!parent.isSelected && root.hovered ? root.controlColors.colors.surfaceHover : "transparent")
            topLeftRadius: root.topLeftRadius >= 0 ? root.topLeftRadius : parent.semanticRadius
            topRightRadius: root.topRightRadius >= 0 ? root.topRightRadius : parent.semanticRadius
            bottomLeftRadius: root.bottomLeftRadius >= 0 ? root.bottomLeftRadius : parent.semanticRadius
            bottomRightRadius: root.bottomRightRadius >= 0 ? root.bottomRightRadius : parent.semanticRadius
            Accessible.ignored: true
        }
        Rectangle {
            objectName: "hnItemDelegateSelectedOverlay"
            anchors.fill: parent
            color: root.hovered ? root.controlColors.colors.surfaceSelectedHover
                                : root.controlColors.colors.surfaceSelected
            topLeftRadius: root.topLeftRadius >= 0 ? root.topLeftRadius : parent.semanticRadius
            topRightRadius: root.topRightRadius >= 0 ? root.topRightRadius : parent.semanticRadius
            bottomLeftRadius: root.bottomLeftRadius >= 0 ? root.bottomLeftRadius : parent.semanticRadius
            bottomRightRadius: root.bottomRightRadius >= 0 ? root.bottomRightRadius : parent.semanticRadius
            visible: parent.isSelected && !root.down
            Accessible.ignored: true
        }
        Rectangle {
            anchors.fill: parent
            color: "transparent"
            topLeftRadius: root.topLeftRadius >= 0 ? root.topLeftRadius : parent.semanticRadius
            topRightRadius: root.topRightRadius >= 0 ? root.topRightRadius : parent.semanticRadius
            bottomLeftRadius: root.bottomLeftRadius >= 0 ? root.bottomLeftRadius : parent.semanticRadius
            bottomRightRadius: root.bottomRightRadius >= 0 ? root.bottomRightRadius : parent.semanticRadius
            border.width: root.visualFocus ? HnMetrics.focusBorderWidth : 0
            border.color: root.controlColors.colors.borderFocus
            Accessible.ignored: true
        }
    }
}
