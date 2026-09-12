// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

import QtQuick
import QtQuick.Templates as T
import QtQuick.Layouts
import Holonight.Core
import Holonight.impl as Impl

T.MenuItem {
    id: root

    readonly property Impl.ControlPalette controlColors: Impl.ControlPalette {
        palette: root.palette
        colorGroup: !root.enabled ? Impl.ControlPalette.Disabled
                                 : (root.Window.window && !root.Window.window.active
                                    ? Impl.ControlPalette.Inactive : Impl.ControlPalette.Active)
        textRole: Impl.ControlPalette.ButtonText
        fillRole: Impl.ControlPalette.Button
    }

    font.family: HolonightTheme.uiFont
    font.pointSize: HolonightTheme.bodySize

    readonly property color foregroundColor: {
        if (!root.enabled)
            return root.controlColors.colors.textDisabled
        if (root.highlighted)
            return root.controlColors.colors.onPrimary
        return root.controlColors.colors.textPrimary
    }

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    padding: 6
    leftPadding: 12
    rightPadding: 12 + (root.subMenu ? 16 + root.spacing : 0)
    spacing: 6
    hoverEnabled: true

    contentItem: RowLayout {
        spacing: root.spacing
        LayoutMirroring.enabled: root.mirrored

        Item {
            Layout.preferredWidth: HnMetrics.iconSize(HnControlSize.Compact)
            Layout.preferredHeight: HnMetrics.iconSize(HnControlSize.Compact)
            Layout.alignment: Qt.AlignVCenter

            Text {
                anchors.centerIn: parent
                text: root.checkable && root.checked ? qsTr("✓", "checked menu item indicator") : ""
                color: root.highlighted ? root.foregroundColor : root.controlColors.colors.primary
                font.pointSize: HolonightTheme.captionSize
                textFormat: Text.PlainText
                Accessible.ignored: true
            }
        }

        Item {
            Layout.preferredWidth: HnMetrics.iconSize(HnControlSize.Compact)
            Layout.preferredHeight: HnMetrics.iconSize(HnControlSize.Compact)
            Layout.alignment: Qt.AlignVCenter

            HnIcon {
                objectName: "hnMenuItemIcon"
                anchors.centerIn: parent
                source: root.icon.source
                size: HnMetrics.iconSize(HnControlSize.Compact)
                iconState: root.enabled ? HnIcon.Normal : HnIcon.Disabled
                normalColor: root.foregroundColor
                disabledColor: root.foregroundColor
                visible: root.icon.source.toString().length > 0
            }
        }

        Text {
            objectName: "hnMenuItemLabel"
            text: root.text
            font: root.font
            color: root.foregroundColor
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
            textFormat: Text.PlainText
            Layout.fillWidth: true
        }
    }

    arrow: Text {
        anchors.left: root.mirrored ? parent.left : undefined
        anchors.leftMargin: root.mirrored ? 8 : 0
        anchors.right: root.mirrored ? undefined : parent.right
        anchors.rightMargin: root.mirrored ? 0 : 8
        anchors.verticalCenter: parent.verticalCenter
        visible: root.subMenu
        text: root.mirrored ? qsTr("‹", "submenu arrow") : qsTr("›", "submenu arrow")
        color: root.foregroundColor
        font.pointSize: HolonightTheme.bodySize
        textFormat: Text.PlainText
        Accessible.ignored: true
    }

    background: Rectangle {
        objectName: "hnMenuItemBackground"
        readonly property real semanticRadius: HnAppearance.roundedRadius(HnSurfaceRole.Menu,
                                                                          width, height,
                                                                          HnAppearance.revision)

        implicitWidth: 180
        implicitHeight: 28
        visible: root.enabled && (root.down || root.highlighted || (root.HnInputInteraction.hoverAllowed && root.hovered))
        radius: semanticRadius
        color: {
            if (root.down && root.highlighted)
                return root.controlColors.colors.primaryPressed
            if (root.highlighted)
                return root.controlColors.colors.primary
            if (root.down)
                return root.controlColors.colors.buttonPressed
            if ((root.HnInputInteraction.hoverAllowed && root.hovered))
                return root.controlColors.colors.surfaceHover
            return "transparent"
        }

        Behavior on color {
            ColorAnimation { duration: 80 }
        }
    }
}
