// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Holonight as H
import Holonight.Core as Core
import Holonight.Controls as Controls

Controls.HnApplicationWindow {
    id: root
    visible: true
    width: 840
    height: 700
    title: "holonight demo"

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // ── Header ────────────────────────────────────────────────────
        Item { implicitHeight: 32 }
        Core.HnLabel {
            role: Core.HnTypographyRole.Heading
            rawText: "holonight"
            Layout.leftMargin: 40
        }
        Core.HnLabel {
            role: Core.HnTypographyRole.Subheading
            rawText: "Qt6 theme variants for tiling window managers"
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
                    Core.HnLabel { role: Core.HnTypographyRole.MicroHeader; rawText: "BUTTONS"; showPrefix: true; Layout.leftMargin: 40; Layout.bottomMargin: 10 }
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

                    Core.HnLabel { role: Core.HnTypographyRole.MicroHeader; rawText: "CHECKBOXES"; Layout.leftMargin: 40; Layout.bottomMargin: 10 }
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

                    Core.HnLabel { role: Core.HnTypographyRole.MicroHeader; rawText: "RADIO BUTTONS"; Layout.leftMargin: 40; Layout.bottomMargin: 10 }
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

                    Core.HnLabel { role: Core.HnTypographyRole.MicroHeader; rawText: "SWITCHES"; Layout.leftMargin: 40; Layout.bottomMargin: 10 }
                    RowLayout {
                        spacing: 24
                        Layout.leftMargin: 40
                        Layout.bottomMargin: 32
                        Switch { text: "Off" }
                        Switch { text: "On"; checked: true }
                        Switch { text: "Disabled off"; enabled: false }
                        Switch { text: "Disabled on"; checked: true; enabled: false }
                    }

                    Core.HnLabel { role: Core.HnTypographyRole.MicroHeader; rawText: "SLIDERS"; Layout.leftMargin: 40; Layout.bottomMargin: 10 }
                    ColumnLayout {
                        spacing: 8
                        Layout.fillWidth: true
                        Layout.leftMargin: 40
                        Layout.rightMargin: 40
                        Layout.bottomMargin: 32
                        Slider { Layout.fillWidth: true; value: 0.4 }
                        Slider { Layout.fillWidth: true; value: 0.7; enabled: false }
                    }

                    Core.HnLabel { role: Core.HnTypographyRole.MicroHeader; rawText: "RANGE SLIDER"; Layout.leftMargin: 40; Layout.bottomMargin: 10 }
                    ColumnLayout {
                        spacing: 8
                        Layout.fillWidth: true
                        Layout.leftMargin: 40
                        Layout.rightMargin: 40
                        Layout.bottomMargin: 32
                        RangeSlider { Layout.fillWidth: true; first.value: 0.3; second.value: 0.7 }
                        RangeSlider { Layout.fillWidth: true; first.value: 0.2; second.value: 0.6; enabled: false }
                    }

                    Core.HnLabel { role: Core.HnTypographyRole.MicroHeader; rawText: "DIAL"; Layout.leftMargin: 40; Layout.bottomMargin: 10 }
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
                    Core.HnLabel { role: Core.HnTypographyRole.MicroHeader; rawText: "TEXT FIELD"; Layout.leftMargin: 40; Layout.bottomMargin: 10 }
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

                    Core.HnLabel { role: Core.HnTypographyRole.MicroHeader; rawText: "TEXT AREA"; Layout.leftMargin: 40; Layout.bottomMargin: 10 }
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
                        H.TextArea {
                            implicitHeight: 80
                            placeholderText: "Error state"
                            hasError: true
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

                    Core.HnLabel { role: Core.HnTypographyRole.MicroHeader; rawText: "SPINBOX"; Layout.leftMargin: 40; Layout.bottomMargin: 10 }
                    RowLayout {
                        spacing: 16
                        Layout.leftMargin: 40
                        Layout.bottomMargin: 32
                        SpinBox { value: 42; from: 0; to: 100 }
                        SpinBox { value: 42; from: 0; to: 100; enabled: false }
                    }

                    Core.HnLabel { role: Core.HnTypographyRole.MicroHeader; rawText: "COMBOBOX"; Layout.leftMargin: 40; Layout.bottomMargin: 10 }
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
                    Core.HnLabel { role: Core.HnTypographyRole.MicroHeader; rawText: "TAB BAR"; Layout.leftMargin: 40; Layout.bottomMargin: 10 }
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

                    Core.HnLabel { role: Core.HnTypographyRole.MicroHeader; rawText: "MENU"; Layout.leftMargin: 40; Layout.bottomMargin: 10 }
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

                    Core.HnLabel { role: Core.HnTypographyRole.MicroHeader; rawText: "TOOLBAR"; Layout.leftMargin: 40; Layout.bottomMargin: 10 }
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

                    Core.HnLabel { role: Core.HnTypographyRole.MicroHeader; rawText: "SCROLLBAR"; Layout.leftMargin: 40; Layout.bottomMargin: 10 }
                    Rectangle {
                        implicitHeight: 180
                        color: Core.HoloniightPalette.surface
                        radius: Core.HnAppearance.roundedRadius(Core.HnSurfaceRole.Control, width, height, Core.HnAppearance.revision)
                        border.color: Core.HoloniightPalette.borderPassive
                        border.width: Core.HnMetrics.borderWidth
                        Layout.fillWidth: true
                        Layout.leftMargin: 40
                        Layout.rightMargin: 40
                        Layout.bottomMargin: 32
                        ListView {
                            anchors.fill: parent
                            anchors.margins: 4
                            clip: contentHeight > height
                            model: [
                                "Button", "CheckBox", "ComboBox", "Dial", "Frame", "GroupBox",
                                "Label", "Menu", "ProgressBar", "RadioButton", "RangeSlider",
                                "ScrollBar", "ScrollView", "Slider", "SpinBox", "Switch",
                                "TabBar", "TextField", "TextArea", "ToolBar", "ToolButton"
                            ]
                            delegate: navItemDelegate
                            ScrollBar.vertical: H.ScrollBar { policy: ScrollBar.AlwaysOn }

                            Component {
                                id: navItemDelegate

                                H.ItemDelegate {
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
                    Core.HnLabel { role: Core.HnTypographyRole.MicroHeader; rawText: "PROGRESS BAR"; Layout.leftMargin: 40; Layout.bottomMargin: 10 }
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

                    Core.HnLabel { role: Core.HnTypographyRole.MicroHeader; rawText: "TOOLTIP"; Layout.leftMargin: 40; Layout.bottomMargin: 10 }
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

                    Core.HnLabel { role: Core.HnTypographyRole.MicroHeader; rawText: "BUSY INDICATOR"; Layout.leftMargin: 40; Layout.bottomMargin: 10 }
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
                    Core.HnLabel { role: Core.HnTypographyRole.MicroHeader; rawText: "FRAME"; Layout.leftMargin: 40; Layout.bottomMargin: 10 }
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

                    Core.HnLabel { role: Core.HnTypographyRole.MicroHeader; rawText: "PER-CORNER SURFACES"; Layout.leftMargin: 40; Layout.bottomMargin: 10 }
                    RowLayout {
                        spacing: 12
                        Layout.leftMargin: 40
                        Layout.rightMargin: 40
                        Layout.bottomMargin: 32

                        Controls.HnSurfaceFrame {
                            surfaceRole: Core.HnSurfaceRole.Panel
                            chamferedCornersOverride: Core.HnCornerMask.TopRight | Core.HnCornerMask.BottomRight

                            Layout.preferredWidth: 150
                            Layout.preferredHeight: 84

                            Label { anchors.centerIn: parent; text: "LEFT DOCK" }
                        }
                        Controls.HnSurfaceFrame {
                            surfaceRole: Core.HnSurfaceRole.Panel
                            chamferedCornersOverride: Core.HnCornerMask.TopLeft | Core.HnCornerMask.BottomLeft

                            Layout.preferredWidth: 150
                            Layout.preferredHeight: 84

                            Label { anchors.centerIn: parent; text: "RIGHT DOCK" }
                        }
                        Controls.HnSurfaceFrame {
                            surfaceRole: Core.HnSurfaceRole.Card
                            chamferedCornersOverride: Core.HnCornerMask.TopRight

                            Layout.preferredWidth: 150
                            Layout.preferredHeight: 84

                            Label { anchors.centerIn: parent; text: "3 ROUND / 1 CUT" }
                        }
                        Controls.HnSurfaceFrame {
                            surfaceRole: Core.HnSurfaceRole.Card
                            chamferedCornersOverride: Core.HnCornerMask.All & ~Core.HnCornerMask.BottomLeft

                            Layout.preferredWidth: 150
                            Layout.preferredHeight: 84

                            Label { anchors.centerIn: parent; text: "3 CUT / 1 ROUND" }
                        }
                    }

                    Core.HnLabel { role: Core.HnTypographyRole.MicroHeader; rawText: "GROUP BOX"; Layout.leftMargin: 40; Layout.bottomMargin: 10 }
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

                    Core.HnLabel { role: Core.HnTypographyRole.MicroHeader; rawText: "SCROLL VIEW"; Layout.leftMargin: 40; Layout.bottomMargin: 10 }
                    ScrollView {
                        implicitHeight: 120
                        clip: contentHeight > availableHeight
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

                    Core.HnLabel { role: Core.HnTypographyRole.MicroHeader; rawText: "LIST VIEW"; Layout.leftMargin: 40; Layout.bottomMargin: 10 }
                    Rectangle {
                        implicitHeight: 180
                        color: Core.HoloniightPalette.surface
                        radius: Core.HnAppearance.roundedRadius(Core.HnSurfaceRole.Control, width, height, Core.HnAppearance.revision)
                        border.color: Core.HoloniightPalette.borderPassive
                        border.width: Core.HnMetrics.borderWidth
                        Layout.fillWidth: true
                        Layout.leftMargin: 40
                        Layout.rightMargin: 40
                        Layout.bottomMargin: 32
                        ListView {
                            anchors.fill: parent
                            anchors.margins: 4
                            clip: contentHeight > height
                            model: [
                                "ItemDelegate One", "ItemDelegate Two", "ItemDelegate Three",
                                "ItemDelegate Four", "ItemDelegate Five", "ItemDelegate Six",
                                "ItemDelegate Seven", "ItemDelegate Eight"
                            ]
                            delegate: containerItemDelegate
                            ScrollBar.vertical: H.ScrollBar { policy: ScrollBar.AlwaysOn }

                            Component {
                                id: containerItemDelegate

                                H.ItemDelegate {
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
