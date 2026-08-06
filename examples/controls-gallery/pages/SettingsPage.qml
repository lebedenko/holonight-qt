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
            rawText: qsTr("Settings & Feedback")
            Layout.leftMargin: 32
            Layout.topMargin: 24
        }

        HnLabel {
            role: HnTypographyRole.Title
            rawText: qsTr("Settings & State Controls")
            Layout.leftMargin: 32
            Layout.topMargin: 8
        }

        Repeater {
            model: [
                { label: qsTr("Compact"), role: HnControlSize.Compact },
                { label: qsTr("Normal"), role: HnControlSize.Normal },
                { label: qsTr("Large"), role: HnControlSize.Large },
                { label: qsTr("Hero"), role: HnControlSize.Hero }
            ]

            HnSettingsRow {
                required property var modelData

                sizeRole: modelData.role
                titleText: modelData.label
                descriptionText: qsTr("Inline settings row with semantic spacing.")
                Layout.fillWidth: true
                Layout.leftMargin: 32
                Layout.rightMargin: 32

                control: Component {
                    Switch {
                        sizeRole: modelData.role
                        text: qsTr("Enabled")
                        checked: true
                    }
                }
            }
        }

        Repeater {
            model: [
                {
                    title: qsTr("Shape scale"),
                    description: qsTr("Compact stacked controls begin at the shared content origin."),
                    role: HnControlSize.Compact,
                    enabled: true,
                    value: qsTr("8 px")
                },
                {
                    title: qsTr("Override the base chamfer used by every application surface"),
                    description: qsTr("Long titles wrap without shifting the full-width control composition below."),
                    role: HnControlSize.Normal,
                    enabled: false,
                    value: qsTr("12 px")
                }
            ]

            HnSettingsRow {
                id: settingsRow

                required property var modelData

                titleText: modelData.title
                descriptionText: modelData.description
                sizeRole: modelData.role
                enabled: modelData.enabled
                stacked: true
                Layout.fillWidth: true
                Layout.leftMargin: 32
                Layout.rightMargin: 32

                control: Component {
                    RowLayout {
                        Switch {
                            sizeRole: settingsRow.modelData.role
                            checked: true
                            Layout.alignment: Qt.AlignVCenter
                        }
                        Slider {
                            value: 0.5
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignVCenter
                        }
                        HnLabel {
                            role: HnTypographyRole.Body
                            rawText: settingsRow.modelData.value
                            horizontalAlignment: Text.AlignRight
                            Layout.alignment: Qt.AlignVCenter
                            Layout.preferredWidth: 52
                        }
                    }
                }
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
            rawText: qsTr("Color pickers")
            Layout.leftMargin: 32
        }

        HnColorPicker {
            id: standaloneColorPicker

            selectedColor: HoloniightPalette.accentViolet
            Layout.leftMargin: 32
        }

        HnSettingsRow {
            titleText: qsTr("Accent color")
            descriptionText: qsTr("Embedded in a settings row; selection defaults to none.")
            Layout.fillWidth: true
            Layout.leftMargin: 32
            Layout.rightMargin: 32

            control: Component {
                HnColorPicker {}
            }
        }

        HnSettingsRow {
            titleText: qsTr("Accent color (disabled)")
            descriptionText: qsTr("Disabled state dims every swatch and blocks selection.")
            enabled: false
            Layout.fillWidth: true
            Layout.leftMargin: 32
            Layout.rightMargin: 32

            control: Component {
                HnColorPicker {
                    selectedColor: HoloniightPalette.success
                }
            }
        }

        HnSectionHeader {
            titleText: qsTr("ACCOUNT & SECURITY")
            isCategoryMode: true
            showPrefix: true
            Layout.fillWidth: true
            Layout.leftMargin: 32
            Layout.rightMargin: 32
        }

        HnSectionHeader {
            titleText: qsTr("Account")
            descriptionText: qsTr("Optional leading and trailing content preserve the action's natural focus.")
            Layout.fillWidth: true
            Layout.leftMargin: 32
            Layout.rightMargin: 32

            leadingContent: HnLabel {
                role: HnTypographyRole.Body
                rawText: qsTr("●")
                color: HoloniightPalette.primary
            }
            trailingContent: Button { text: qsTr("Reset") }
        }

        HnSectionHeader {
            titleText: qsTr("Compact Section")
            descriptionText: qsTr("Compact size role for dense panel layouts.")
            sizeRole: HnControlSize.Compact
            Layout.fillWidth: true
            Layout.leftMargin: 32
            Layout.rightMargin: 32
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
            rawText: qsTr("Status indicators & Separators")
            Layout.leftMargin: 32
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.leftMargin: 32
            Layout.rightMargin: 32
            HnStatusIndicator { status: HnStatusIndicator.Neutral; text: qsTr("Neutral") }
            HnStatusIndicator { status: HnStatusIndicator.Info; text: qsTr("Info") }
            HnStatusIndicator { status: HnStatusIndicator.Success; text: qsTr("Success") }
            HnStatusIndicator { status: HnStatusIndicator.Warning; text: qsTr("Warning") }
            HnStatusIndicator { status: HnStatusIndicator.Error; text: qsTr("Error") }
        }

        Repeater {
            model: [
                { label: qsTr("Solid"), mode: HnSeparator.Solid },
                { label: qsTr("Fade both"), mode: HnSeparator.FadeBoth },
                { label: qsTr("Fade start"), mode: HnSeparator.FadeStart },
                { label: qsTr("Fade end"), mode: HnSeparator.FadeEnd }
            ]

            ColumnLayout {
                required property var modelData

                spacing: 4
                Layout.fillWidth: true
                Layout.leftMargin: 32
                Layout.rightMargin: 32

                HnLabel {
                    role: HnTypographyRole.Caption
                    rawText: parent.modelData.label
                    color: HoloniightPalette.textMuted
                }

                HnSeparator {
                    fadeMode: parent.modelData.mode
                    Layout.fillWidth: true
                }
            }
        }

        RowLayout {
            spacing: 24
            Layout.fillWidth: true
            Layout.leftMargin: 32
            Layout.rightMargin: 32
            Layout.preferredHeight: 72

            HnSeparator {
                orientation: Qt.Vertical
                Layout.fillHeight: true
            }

            HnSeparator {
                orientation: Qt.Vertical
                color: HoloniightPalette.borderPassive
                thickness: HoloniightPalette.separatorWidth * 2
                Layout.fillHeight: true
            }

            HnLabel {
                role: HnTypographyRole.Caption
                rawText: qsTr("Vertical, passive, and increased-thickness variants")
                color: HoloniightPalette.textMuted
                Layout.fillWidth: true
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
            rawText: qsTr("Empty & Loading States")
            Layout.leftMargin: 32
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.leftMargin: 32
            Layout.rightMargin: 32

            HnEmptyState {
                iconSource: "qrc:/qt/qml/Holonight/Controls/assets/search.svg"
                titleText: qsTr("No results")
                descriptionText: qsTr("Try a different search term.")
                Layout.fillWidth: true
                actionContent: Button { text: qsTr("Clear search") }
            }

            HnEmptyState {
                titleText: qsTr("Custom graphic")
                descriptionText: qsTr("Graphic content replaces the icon.")
                enabled: false
                Layout.fillWidth: true
                graphicContent: HnLabel {
                    role: HnTypographyRole.Display
                    rawText: qsTr("◇")
                    font.pointSize: HolonightTheme.displaySize
                }
                actionContent: Button { text: qsTr("Disabled action") }
            }

            HnEmptyState {
                titleText: qsTr("No graphic")
                descriptionText: qsTr("Absent graphics reserve no space.")
                Layout.fillWidth: true
            }
        }

        HnEmptyState {
            titleText: qsTr("Long empty-state content")
            descriptionText: qsTr("This deliberately long message demonstrates centered wrapping at fractional "
                                  + "scale without asking the component to inspect a model or decide visibility.")
            Layout.fillWidth: true
            Layout.leftMargin: 32
            Layout.rightMargin: 32
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.leftMargin: 32
            Layout.rightMargin: 32

            HnLoadingState {
                titleText: qsTr("Indeterminate")
                descriptionText: qsTr("Waiting for progress.")
                Layout.fillWidth: true
                actionContent: Button { text: qsTr("Cancel") }
            }

            HnLoadingState {
                titleText: qsTr("Determinate")
                descriptionText: qsTr("65 percent complete.")
                progress: 0.65
                Layout.fillWidth: true
            }

            HnLoadingState {
                titleText: qsTr("Stopped")
                descriptionText: qsTr("Messages and actions remain composed.")
                running: false
                Layout.fillWidth: true
                actionContent: Button { text: qsTr("Retry") }
            }
        }

        HnLabel {
            role: HnTypographyRole.Caption
            rawText: qsTr("Tab into a field, type text, press Escape to clear it, and activate the clear button to verify focus restoration.")
            color: HoloniightPalette.textMuted
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
            Layout.leftMargin: 32
            Layout.rightMargin: 32
            Layout.bottomMargin: 28
        }
    }
}
