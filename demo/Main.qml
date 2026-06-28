// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Holonight as H

ApplicationWindow {
    id: root
    visible: true
    width: 840
    height: 700
    title: "holonight demo"
    color: palette.window

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // ── Header ────────────────────────────────────────────────────
        Item { implicitHeight: 32 }
        Label {
            text: "holonight"
            font.pixelSize: 24
            font.bold: true
            Layout.leftMargin: 40
        }
        Label {
            text: "Dark Qt6 theme for tiling window managers"
            font.pixelSize: 13
            color: palette.placeholderText
            Layout.leftMargin: 40
            Layout.bottomMargin: 16
        }

        // ── Tab bar ───────────────────────────────────────────────────
        TabBar {
            id: tabBar
            Layout.fillWidth: true
            TabButton { text: "Input" }
            TabButton { text: "Text" }
            TabButton { text: "Navigation" }
            TabButton { text: "Feedback" }
            TabButton { text: "Containers" }
        }

        // ── Tab pages ─────────────────────────────────────────────────
        StackLayout {
            currentIndex: tabBar.currentIndex
            Layout.fillWidth: true
            Layout.fillHeight: true

            // ── Tab 1: Input ──────────────────────────────────────────
            ScrollView {
                contentWidth: availableWidth
                contentHeight: inputCol.implicitHeight

                ColumnLayout {
                    id: inputCol
                    width: parent.availableWidth
                    spacing: 0

                    Item { implicitHeight: 24 }
                    Label { text: "BUTTONS"; font.pixelSize: 11; font.letterSpacing: 1; color: palette.placeholderText; Layout.leftMargin: 40; Layout.bottomMargin: 10 }
                    RowLayout {
                        spacing: 8
                        Layout.leftMargin: 40
                        Layout.bottomMargin: 8
                        Button { text: "Default" }
                        Button { text: "Primary"; highlighted: true }
                    }
                    RowLayout {
                        spacing: 8
                        Layout.leftMargin: 40
                        Layout.bottomMargin: 32
                        Button { text: "Disabled"; enabled: false }
                        Button { text: "Disabled Primary"; highlighted: true; enabled: false }
                    }

                    Label { text: "CHECKBOXES"; font.pixelSize: 11; font.letterSpacing: 1; color: palette.placeholderText; Layout.leftMargin: 40; Layout.bottomMargin: 10 }
                    RowLayout {
                        spacing: 48
                        Layout.leftMargin: 40
                        Layout.bottomMargin: 32
                        ColumnLayout {
                            spacing: 4
                            CheckBox { text: "Unchecked" }
                            CheckBox { text: "Checked"; checked: true }
                            CheckBox { text: "Partial"; checkState: Qt.PartiallyChecked; tristate: true }
                        }
                        ColumnLayout {
                            spacing: 4
                            CheckBox { text: "Disabled unchecked"; enabled: false }
                            CheckBox { text: "Disabled checked"; checked: true; enabled: false }
                            CheckBox { text: "Disabled partial"; checkState: Qt.PartiallyChecked; tristate: true; enabled: false }
                        }
                    }

                    Label { text: "RADIO BUTTONS"; font.pixelSize: 11; font.letterSpacing: 1; color: palette.placeholderText; Layout.leftMargin: 40; Layout.bottomMargin: 10 }
                    RowLayout {
                        spacing: 48
                        Layout.leftMargin: 40
                        Layout.bottomMargin: 32
                        ColumnLayout {
                            spacing: 4
                            ButtonGroup { id: radioGroup }
                            RadioButton { text: "Option A"; ButtonGroup.group: radioGroup; checked: true }
                            RadioButton { text: "Option B"; ButtonGroup.group: radioGroup }
                            RadioButton { text: "Option C"; ButtonGroup.group: radioGroup }
                        }
                        ColumnLayout {
                            spacing: 4
                            RadioButton { text: "Disabled"; enabled: false; checked: true }
                            RadioButton { text: "Disabled"; enabled: false }
                        }
                    }

                    Label { text: "SWITCHES"; font.pixelSize: 11; font.letterSpacing: 1; color: palette.placeholderText; Layout.leftMargin: 40; Layout.bottomMargin: 10 }
                    RowLayout {
                        spacing: 24
                        Layout.leftMargin: 40
                        Layout.bottomMargin: 32
                        Switch { text: "Off" }
                        Switch { text: "On"; checked: true }
                        Switch { text: "Disabled off"; enabled: false }
                        Switch { text: "Disabled on"; checked: true; enabled: false }
                    }

                    Label { text: "SLIDERS"; font.pixelSize: 11; font.letterSpacing: 1; color: palette.placeholderText; Layout.leftMargin: 40; Layout.bottomMargin: 10 }
                    ColumnLayout {
                        spacing: 8
                        Layout.fillWidth: true
                        Layout.leftMargin: 40
                        Layout.rightMargin: 40
                        Layout.bottomMargin: 32
                        Slider { Layout.fillWidth: true; value: 0.4 }
                        Slider { Layout.fillWidth: true; value: 0.7; enabled: false }
                    }

                    Label { text: "RANGE SLIDER"; font.pixelSize: 11; font.letterSpacing: 1; color: palette.placeholderText; Layout.leftMargin: 40; Layout.bottomMargin: 10 }
                    ColumnLayout {
                        spacing: 8
                        Layout.fillWidth: true
                        Layout.leftMargin: 40
                        Layout.rightMargin: 40
                        Layout.bottomMargin: 32
                        RangeSlider { Layout.fillWidth: true; first.value: 0.3; second.value: 0.7 }
                        RangeSlider { Layout.fillWidth: true; first.value: 0.2; second.value: 0.6; enabled: false }
                    }

                    Label { text: "DIAL"; font.pixelSize: 11; font.letterSpacing: 1; color: palette.placeholderText; Layout.leftMargin: 40; Layout.bottomMargin: 10 }
                    RowLayout {
                        spacing: 24
                        Layout.leftMargin: 40
                        Layout.bottomMargin: 32
                        Dial { value: 0.4 }
                        Dial { value: 0.4; enabled: false }
                    }

                    Item { implicitHeight: 24 }
                }
            }

            // ── Tab 2: Text ───────────────────────────────────────────
            ScrollView {
                contentWidth: availableWidth
                contentHeight: textCol.implicitHeight

                ColumnLayout {
                    id: textCol
                    width: parent.availableWidth
                    spacing: 0

                    Item { implicitHeight: 24 }
                    Label { text: "TEXT FIELD"; font.pixelSize: 11; font.letterSpacing: 1; color: palette.placeholderText; Layout.leftMargin: 40; Layout.bottomMargin: 10 }
                    ColumnLayout {
                        spacing: 8
                        Layout.fillWidth: true
                        Layout.leftMargin: 40
                        Layout.rightMargin: 40
                        Layout.bottomMargin: 32
                        TextField { Layout.fillWidth: true; placeholderText: "Placeholder text" }
                        TextField { Layout.fillWidth: true; text: "Filled input value" }
                        H.TextField { Layout.fillWidth: true; placeholderText: "Error state"; hasError: true }
                        TextField { Layout.fillWidth: true; placeholderText: "Disabled"; enabled: false }
                    }

                    Label { text: "TEXT AREA"; font.pixelSize: 11; font.letterSpacing: 1; color: palette.placeholderText; Layout.leftMargin: 40; Layout.bottomMargin: 10 }
                    ColumnLayout {
                        spacing: 8
                        Layout.fillWidth: true
                        Layout.leftMargin: 40
                        Layout.rightMargin: 40
                        Layout.bottomMargin: 32
                        TextArea {
                            implicitHeight: 80
                            text: "Multi-line\ntext area\nwith content"
                            wrapMode: TextArea.Wrap
                            Layout.fillWidth: true
                        }
                        TextArea {
                            implicitHeight: 80
                            text: "Disabled text area"
                            wrapMode: TextArea.Wrap
                            enabled: false
                            Layout.fillWidth: true
                        }
                    }

                    Label { text: "SPINBOX"; font.pixelSize: 11; font.letterSpacing: 1; color: palette.placeholderText; Layout.leftMargin: 40; Layout.bottomMargin: 10 }
                    RowLayout {
                        spacing: 16
                        Layout.leftMargin: 40
                        Layout.bottomMargin: 32
                        SpinBox { value: 42; from: 0; to: 100 }
                        SpinBox { value: 42; from: 0; to: 100; enabled: false }
                    }

                    Label { text: "COMBOBOX"; font.pixelSize: 11; font.letterSpacing: 1; color: palette.placeholderText; Layout.leftMargin: 40; Layout.bottomMargin: 10 }
                    RowLayout {
                        spacing: 16
                        Layout.leftMargin: 40
                        Layout.bottomMargin: 32
                        ComboBox { model: ["Hyprland", "sway", "niri", "river"] }
                        ComboBox { model: ["Hyprland", "sway", "niri", "river"]; enabled: false }
                    }

                    Item { implicitHeight: 24 }
                }
            }

            // ── Tab 3: Navigation ─────────────────────────────────────
            ScrollView {
                contentWidth: availableWidth
                contentHeight: navCol.implicitHeight

                ColumnLayout {
                    id: navCol
                    width: parent.availableWidth
                    spacing: 0

                    Item { implicitHeight: 24 }
                    Label { text: "TAB BAR"; font.pixelSize: 11; font.letterSpacing: 1; color: palette.placeholderText; Layout.leftMargin: 40; Layout.bottomMargin: 10 }
                    ColumnLayout {
                        spacing: 0
                        Layout.fillWidth: true
                        Layout.leftMargin: 40
                        Layout.rightMargin: 40
                        Layout.bottomMargin: 32
                        TabBar {
                            id: innerTabBar
                            Layout.fillWidth: true
                            TabButton { text: "Alpha" }
                            TabButton { text: "Beta" }
                            TabButton { text: "Gamma" }
                        }
                        StackLayout {
                            currentIndex: innerTabBar.currentIndex
                            implicitHeight: 60
                            Layout.fillWidth: true
                            Item {
                                Rectangle { anchors.fill: parent; color: palette.base; radius: 4
                                    Label { anchors.centerIn: parent; text: "Alpha content"; color: palette.placeholderText } }
                            }
                            Item {
                                Rectangle { anchors.fill: parent; color: palette.base; radius: 4
                                    Label { anchors.centerIn: parent; text: "Beta content"; color: palette.placeholderText } }
                            }
                            Item {
                                Rectangle { anchors.fill: parent; color: palette.base; radius: 4
                                    Label { anchors.centerIn: parent; text: "Gamma content"; color: palette.placeholderText } }
                            }
                        }
                    }

                    Label { text: "MENU"; font.pixelSize: 11; font.letterSpacing: 1; color: palette.placeholderText; Layout.leftMargin: 40; Layout.bottomMargin: 10 }
                    RowLayout {
                        Layout.leftMargin: 40
                        Layout.bottomMargin: 32
                        Button {
                            text: "Open Menu"
                            onClicked: contextMenu.open()
                            Menu {
                                id: contextMenu
                                MenuItem { text: "Action One" }
                                MenuItem { text: "Action Two" }
                                MenuItem { text: "Disabled Action"; enabled: false }
                            }
                        }
                    }

                    Label { text: "TOOLBAR"; font.pixelSize: 11; font.letterSpacing: 1; color: palette.placeholderText; Layout.leftMargin: 40; Layout.bottomMargin: 10 }
                    ToolBar {
                        Layout.fillWidth: true
                        Layout.bottomMargin: 32
                        RowLayout {
                            anchors.fill: parent
                            spacing: 4
                            ToolButton { text: "File" }
                            ToolButton { text: "Edit" }
                            ToolButton { text: "View" }
                            ToolButton { text: "Disabled"; enabled: false }
                            Item { Layout.fillWidth: true }
                        }
                    }

                    Label { text: "SCROLLBAR"; font.pixelSize: 11; font.letterSpacing: 1; color: palette.placeholderText; Layout.leftMargin: 40; Layout.bottomMargin: 10 }
                    Rectangle {
                        implicitHeight: 180
                        color: palette.base
                        radius: 4
                        border.color: palette.mid
                        border.width: 1
                        Layout.fillWidth: true
                        Layout.leftMargin: 40
                        Layout.rightMargin: 40
                        Layout.bottomMargin: 32
                        ListView {
                            anchors.fill: parent
                            anchors.margins: 1
                            model: [
                                "Button", "CheckBox", "ComboBox", "Dial", "Frame", "GroupBox",
                                "Label", "Menu", "ProgressBar", "RadioButton", "RangeSlider",
                                "ScrollBar", "ScrollView", "Slider", "SpinBox", "Switch",
                                "TabBar", "TextField", "TextArea", "ToolBar", "ToolButton"
                            ]
                            delegate: navItemDelegate
                            ScrollBar.vertical: ScrollBar { policy: ScrollBar.AlwaysOn }

                            Component {
                                id: navItemDelegate

                                ItemDelegate {
                                    required property int index
                                    required property string modelData

                                    width: ListView.view ? ListView.view.width : 0
                                    text: modelData
                                    highlighted: ListView.isCurrentItem
                                    onClicked: {
                                        if (ListView.view) {
                                            ListView.view.currentIndex = index
                                        }
                                    }
                                }
                            }
                        }
                    }

                    Item { implicitHeight: 24 }
                }
            }

            // ── Tab 4: Feedback ───────────────────────────────────────
            ScrollView {
                contentWidth: availableWidth
                contentHeight: feedbackCol.implicitHeight

                ColumnLayout {
                    id: feedbackCol
                    width: parent.availableWidth
                    spacing: 0

                    Item { implicitHeight: 24 }
                    Label { text: "PROGRESS BAR"; font.pixelSize: 11; font.letterSpacing: 1; color: palette.placeholderText; Layout.leftMargin: 40; Layout.bottomMargin: 10 }
                    ColumnLayout {
                        spacing: 8
                        Layout.fillWidth: true
                        Layout.leftMargin: 40
                        Layout.rightMargin: 40
                        Layout.bottomMargin: 32
                        ProgressBar { Layout.fillWidth: true; indeterminate: true }
                        ProgressBar { Layout.fillWidth: true; value: 0.65 }
                        ProgressBar { Layout.fillWidth: true; value: 0.4; enabled: false }
                    }

                    Label { text: "TOOLTIP"; font.pixelSize: 11; font.letterSpacing: 1; color: palette.placeholderText; Layout.leftMargin: 40; Layout.bottomMargin: 10 }
                    RowLayout {
                        spacing: 16
                        Layout.leftMargin: 40
                        Layout.bottomMargin: 32
                        Button {
                            id: immediateTooltipButton

                            text: "Hover me"
                            ToolTip {
                                visible: immediateTooltipButton.hovered
                                text: "This is a tooltip"
                                delay: 0
                            }
                        }
                        Button {
                            id: delayedTooltipButton

                            text: "Delayed tooltip"
                            ToolTip {
                                visible: delayedTooltipButton.hovered
                                text: "Tooltip with 500ms delay"
                                delay: 500
                            }
                        }
                    }

                    Label { text: "BUSY INDICATOR"; font.pixelSize: 11; font.letterSpacing: 1; color: palette.placeholderText; Layout.leftMargin: 40; Layout.bottomMargin: 10 }
                    RowLayout {
                        spacing: 32
                        Layout.leftMargin: 40
                        Layout.bottomMargin: 32
                        BusyIndicator { running: true }
                        BusyIndicator { running: false; enabled: false }
                    }

                    Item { implicitHeight: 24 }
                }
            }

            // ── Tab 5: Containers ─────────────────────────────────────
            ScrollView {
                contentWidth: availableWidth
                contentHeight: containersCol.implicitHeight

                ColumnLayout {
                    id: containersCol
                    width: parent.availableWidth
                    spacing: 0

                    Item { implicitHeight: 24 }
                    Label { text: "FRAME"; font.pixelSize: 11; font.letterSpacing: 1; color: palette.placeholderText; Layout.leftMargin: 40; Layout.bottomMargin: 10 }
                    RowLayout {
                        spacing: 16
                        Layout.leftMargin: 40
                        Layout.rightMargin: 40
                        Layout.bottomMargin: 32
                        Frame {
                            implicitWidth: 180
                            implicitHeight: 80
                            Label { anchors.centerIn: parent; text: "Frame content" }
                        }
                        Frame {
                            implicitWidth: 180
                            implicitHeight: 80
                            enabled: false
                            Label { anchors.centerIn: parent; text: "Disabled frame" }
                        }
                    }

                    Label { text: "GROUP BOX"; font.pixelSize: 11; font.letterSpacing: 1; color: palette.placeholderText; Layout.leftMargin: 40; Layout.bottomMargin: 10 }
                    RowLayout {
                        spacing: 16
                        Layout.leftMargin: 40
                        Layout.rightMargin: 40
                        Layout.bottomMargin: 32
                        GroupBox {
                            title: "Group"
                            implicitWidth: 180
                            ColumnLayout {
                                spacing: 4
                                Label { text: "Item one" }
                                Label { text: "Item two" }
                            }
                        }
                        GroupBox {
                            title: "Disabled"
                            implicitWidth: 180
                            enabled: false
                            ColumnLayout {
                                spacing: 4
                                Label { text: "Item one" }
                                Label { text: "Item two" }
                            }
                        }
                    }

                    Label { text: "SCROLL VIEW"; font.pixelSize: 11; font.letterSpacing: 1; color: palette.placeholderText; Layout.leftMargin: 40; Layout.bottomMargin: 10 }
                    ScrollView {
                        implicitHeight: 120
                        clip: true
                        Layout.fillWidth: true
                        Layout.leftMargin: 40
                        Layout.rightMargin: 40
                        Layout.bottomMargin: 32
                        ColumnLayout {
                            width: parent.availableWidth
                            spacing: 4
                            Repeater {
                                model: 12
                                Label {
                                    required property int index

                                    text: "Scrollable row " + (index + 1)
                                    leftPadding: 8
                                }
                            }
                        }
                    }

                    Label { text: "LIST VIEW"; font.pixelSize: 11; font.letterSpacing: 1; color: palette.placeholderText; Layout.leftMargin: 40; Layout.bottomMargin: 10 }
                    Rectangle {
                        implicitHeight: 180
                        color: palette.base
                        radius: 4
                        border.color: palette.mid
                        border.width: 1
                        Layout.fillWidth: true
                        Layout.leftMargin: 40
                        Layout.rightMargin: 40
                        Layout.bottomMargin: 32
                        ListView {
                            anchors.fill: parent
                            anchors.margins: 1
                            model: [
                                "ItemDelegate One", "ItemDelegate Two", "ItemDelegate Three",
                                "ItemDelegate Four", "ItemDelegate Five", "ItemDelegate Six",
                                "ItemDelegate Seven", "ItemDelegate Eight"
                            ]
                            delegate: containerItemDelegate
                            ScrollBar.vertical: ScrollBar { policy: ScrollBar.AlwaysOn }

                            Component {
                                id: containerItemDelegate

                                ItemDelegate {
                                    required property int index
                                    required property string modelData

                                    width: ListView.view ? ListView.view.width : 0
                                    text: modelData
                                    highlighted: ListView.isCurrentItem
                                    onClicked: {
                                        if (ListView.view) {
                                            ListView.view.currentIndex = index
                                        }
                                    }
                                }
                            }
                        }
                    }

                    Item { implicitHeight: 24 }
                }
            }
        }
    }
}
