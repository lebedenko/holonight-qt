// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Holonight.Core
import Holonight.Controls

ScrollView {
    id: root

    contentWidth: availableWidth

    ColumnLayout {
        width: parent.width
        spacing: 16

        HnLabel {
            role: HnTypographyRole.Heading
            rawText: qsTr("Buttons & Actions")
            Layout.leftMargin: 32
            Layout.topMargin: 24
        }

        HnLabel {
            role: HnTypographyRole.Title
            rawText: qsTr("Icon actions and menus")
            Layout.leftMargin: 32
            Layout.topMargin: 8
        }

        RowLayout {
            spacing: 12
            Layout.fillWidth: true
            Layout.leftMargin: 32
            Layout.rightMargin: 32

            HnIconButton {
                sizeRole: HnControlSize.Compact
                icon.source: "qrc:/qt/qml/Holonight/Controls/assets/more-vertical.svg"
                Accessible.name: qsTr("Compact actions")
            }
            HnIconButton {
                icon.source: "qrc:/qt/qml/Holonight/Controls/assets/edit.svg"
                Accessible.name: qsTr("Edit")
            }
            HnIconButton {
                sizeRole: HnControlSize.Large
                icon.source: "qrc:/qt/qml/Holonight/Controls/assets/folder.svg"
                Accessible.name: qsTr("Open folder")
            }
            HnIconButton {
                icon.source: "qrc:/qt/qml/Holonight/Controls/assets/delete.svg"
                icon.color: HoloniightPalette.error
                Accessible.name: qsTr("Delete")
            }
            HnIconButton {
                enabled: false
                icon.source: "qrc:/qt/qml/Holonight/Controls/assets/more-vertical.svg"
                Accessible.name: qsTr("Disabled actions")
            }
            HnIconButton {
                focus: true
                icon.source: "qrc:/qt/qml/Holonight/Controls/assets/more-vertical.svg"
                Accessible.name: qsTr("Keyboard-focused actions")
            }
        }

        HnSeparator {
            Layout.fillWidth: true
            Layout.leftMargin: 32
            Layout.rightMargin: 32
            Layout.topMargin: 8
            Layout.bottomMargin: 8
        }

        HnLabel {
            role: HnTypographyRole.Title
            rawText: qsTr("Standard button icon sizing")
            Layout.leftMargin: 32
        }

        RowLayout {
            spacing: 12
            Layout.fillWidth: true
            Layout.leftMargin: 32
            Layout.rightMargin: 32

            Button {
                implicitWidth: implicitHeight
                display: AbstractButton.IconOnly
                icon.source: "qrc:/qt/qml/Holonight/Controls/assets/edit.svg"
                Accessible.name: qsTr("Edit")
            }
            Button {
                text: qsTr("Enabled")
                icon.source: "qrc:/qt/qml/Holonight/Controls/assets/folder.svg"
            }
            Button {
                text: qsTr("Disabled")
                enabled: false
                icon.source: "qrc:/qt/qml/Holonight/Controls/assets/folder.svg"
            }
            Button {
                text: qsTr("Highlighted")
                highlighted: true
                icon.source: "qrc:/qt/qml/Holonight/Controls/assets/folder.svg"
            }
        }

        HnSeparator {
            Layout.fillWidth: true
            Layout.leftMargin: 32
            Layout.rightMargin: 32
            Layout.topMargin: 8
            Layout.bottomMargin: 8
        }

        HnLabel {
            role: HnTypographyRole.Title
            rawText: qsTr("Popup Menus")
            Layout.leftMargin: 32
        }

        Menu {
            id: iconMenu

            MenuItem { text: qsTr("Plain action") }
            MenuItem {
                text: qsTr("Open folder")
                icon.source: "qrc:/qt/qml/Holonight/Controls/assets/folder.svg"
            }
            MenuItem {
                text: qsTr("Checked icon action")
                checkable: true
                checked: true
                icon.source: "qrc:/qt/qml/Holonight/Controls/assets/edit.svg"
            }
            Menu {
                title: qsTr("Submenu with a deliberately long translated label")
                MenuItem { text: qsTr("Nested action") }
            }
        }

        Button {
            id: iconMenuButton

            text: qsTr("Open icon menu")
            Layout.leftMargin: 32
            onClicked: iconMenu.popup(iconMenuButton, 0, iconMenuButton.height)
        }

        HnSeparator {
            Layout.fillWidth: true
            Layout.leftMargin: 32
            Layout.rightMargin: 32
            Layout.topMargin: 8
            Layout.bottomMargin: 8
        }

        HnLabel {
            role: HnTypographyRole.Title
            rawText: qsTr("Action bar")
            Layout.leftMargin: 32
        }

        HnActionBar {
            dividerVisible: false
            Layout.fillWidth: true
            Layout.leftMargin: 32
            Layout.rightMargin: 32
            Layout.preferredHeight: 40

            leadingContent: RowLayout {
                HnLabel {
                    role: HnTypographyRole.Body
                    rawText: qsTr("Shell is running")
                }
                HnLabel {
                    role: HnTypographyRole.Body
                    rawText: qsTr("v0.1.0")
                    color: HoloniightPalette.textMuted
                }
            }
            trailingContent: RowLayout {
                Button { text: qsTr("Discard Changes") }
                Button { text: qsTr("Apply") }
                Button { text: qsTr("Save & Apply"); highlighted: true }
            }
        }
    }
}
