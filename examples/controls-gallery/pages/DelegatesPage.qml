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
            rawText: qsTr("Lists & Selection")
            Layout.leftMargin: 32
            Layout.topMargin: 24
        }

        HnLabel {
            role: HnTypographyRole.Title
            rawText: qsTr("Navigation & List Delegates")
            Layout.leftMargin: 32
            Layout.topMargin: 8
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.leftMargin: 32
            Layout.rightMargin: 32

            ColumnLayout {
                Layout.fillWidth: true

                HnNavigationDelegate {
                    title: qsTr("A selected navigation title long enough to elide and show its tooltip")
                    badgeText: qsTr("4")
                    checked: true
                    Layout.fillWidth: true
                }
                HnNavigationDelegate { title: qsTr("Keyboard-focused navigation"); focus: true; Layout.fillWidth: true }
                HnNavigationDelegate { title: qsTr("Disabled navigation"); enabled: false; Layout.fillWidth: true }
                HnListDelegate {
                    title: qsTr("Descriptive list row")
                    subtitle: qsTr("Long secondary text wraps while metadata and trailing content remain optional.")
                    metadata: qsTr("Now")
                    dividerVisible: true
                    Layout.fillWidth: true
                }
                HnListDelegate {
                    id: singleLineDelegate

                    title: qsTr("Fixed-height list row")
                    subtitle: qsTr("alsa_output.pci-0000_00_1f.3.analog-stereo-with-a-long-device-identifier")
                    subtitlePresentation: HnListDelegate.SingleLine
                    checked: true
                    Layout.fillWidth: true
                    Layout.preferredHeight: 64
                    trailingContent: Rectangle {
                        implicitWidth: defaultLabel.implicitWidth + 16
                        implicitHeight: defaultLabel.implicitHeight + 4
                        radius: height / 2
                        color: HoloniightPalette.surfaceSelected
                        visible: singleLineDelegate.checked

                        Label {
                            id: defaultLabel

                            anchors.centerIn: parent
                            text: qsTr("DEFAULT")
                        }
                    }
                    onClicked: checked = !checked
                }
            }

            HnCardDelegate {
                title: qsTr("Selected card")
                subtitle: qsTr("Outlined selection on an elevated surface.")
                statusText: qsTr("Ready")
                checked: true
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
            rawText: qsTr("Segmented Controls & Choice Cards")
            Layout.leftMargin: 32
        }

        HnSegmentedControl {
            model: [{ text: qsTr("List"), value: "list" }, { text: qsTr("Grid"), value: "grid" }]
            Layout.leftMargin: 32
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.leftMargin: 32
            Layout.rightMargin: 32

            ButtonGroup { id: galleryChoices }

            HnChoiceCard {
                title: qsTr("Dark")
                description: qsTr("A selected visual choice.")
                checked: true
                ButtonGroup.group: galleryChoices
                Layout.fillWidth: true
            }
            HnChoiceCard {
                title: qsTr("Light")
                description: qsTr("Composes with a standard ButtonGroup.")
                ButtonGroup.group: galleryChoices
                Layout.fillWidth: true
            }
        }
    }
}
