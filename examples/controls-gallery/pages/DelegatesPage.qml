// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts
import Holonight.Core
import Holonight.Controls

Controls.ScrollView {
    id: root
    objectName: "DelegatesPage"

    contentWidth: availableWidth

    component BreadcrumbExample: Item {
        id: example
        property bool editing: false
        implicitHeight: breadcrumbs.implicitHeight

        HnBreadcrumbBar {
            id: breadcrumbs
            objectName: "galleryBreadcrumbBar"
            anchors.fill: parent
            visible: !example.editing
            segments: ["/", "home", "andrii", qsTr("Pictures")]
            onLocationEditRequested: {
                locationEditor.text = segments[0] === "/" ? "/" + segments.slice(1).join("/") : segments.join("/")
                example.editing = true
                locationEditor.forceActiveFocus()
                locationEditor.selectAll()
            }
        }
        Controls.TextField {
            id: locationEditor
            objectName: "galleryLocationEditor"
            anchors.fill: parent
            visible: example.editing
            Accessible.name: qsTr("Edit location")
            onActiveFocusChanged: {
                if (!activeFocus)
                    example.editing = false
            }
            onAccepted: {
                const labels = text.split("/").filter(label => label.length > 0)
                breadcrumbs.segments = text.startsWith("/") ? ["/"].concat(labels) : labels
                example.editing = false
                breadcrumbs.forceActiveFocus()
            }
            Keys.onEscapePressed: {
                example.editing = false
                breadcrumbs.forceActiveFocus()
            }
        }
    }

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
            rawText: "HnAvatar"
            Layout.leftMargin: 32
        }
        RowLayout {
            Layout.leftMargin: 32
            spacing: 24
            HnAvatar {
                source: "qrc:/qt/qml/Holonight/Controls/assets/folder.svg"
            }
            HnAvatar {
                size: 132
                imageInset: 4
                fallbackSource: "qrc:/qt/qml/Holonight/Controls/assets/folder.svg"
            }
            HnAvatar {
                size: 56
            }
        }

        HnLabel {
            role: HnTypographyRole.Title
            rawText: "HnNavigationDelegate"
            Layout.leftMargin: 32
            Layout.topMargin: 8
        }

        ColumnLayout {
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
                HnNavigationDelegate {
                    title: qsTr("Keyboard-focused navigation")
                    focus: true
                    Layout.fillWidth: true
                }
                HnNavigationDelegate {
                    title: qsTr("Disabled navigation")
                    enabled: false
                    Layout.fillWidth: true
                }
                HnLabel { role: HnTypographyRole.Title; rawText: "HnListDelegate" }

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

                        Controls.Label {
                            id: defaultLabel

                            anchors.centerIn: parent
                            text: qsTr("DEFAULT")
                        }
                    }
                    onClicked: checked = !checked
                }
            }

            HnLabel { role: HnTypographyRole.Title; rawText: "HnCardDelegate" }

            HnCardDelegate {
                title: qsTr("Click to toggle selection")
                subtitle: qsTr("Outlined selection on an elevated surface.")
                statusText: qsTr("Ready")
                checked: true
                checkable: true
                objectName: "selectableCard"
                Layout.fillWidth: true
            }
        }

        HnLabel { role: HnTypographyRole.Title; rawText: "HnActionDelegate"; Layout.leftMargin: 32 }

        HnActionDelegate {
            title: qsTr("Open folder")
            description: qsTr("An action with an icon and navigation chevron.")
            iconSource: "qrc:/qt/qml/Holonight/Controls/assets/folder.svg"
            Layout.fillWidth: true
            Layout.leftMargin: 32
            Layout.rightMargin: 32
        }
        HnActionDelegate {
            title: qsTr("Disabled action without chevron")
            description: qsTr("Optional trailing navigation indicator.")
            showChevron: false
            enabled: false
            Layout.fillWidth: true
            Layout.leftMargin: 32
            Layout.rightMargin: 32
        }

        HnLabel { role: HnTypographyRole.Title; rawText: "HnBreadcrumbBar"; Layout.leftMargin: 32 }

        BreadcrumbExample {
            Layout.fillWidth: true
            Layout.leftMargin: 32
            Layout.rightMargin: 32
        }
        BreadcrumbExample {
            Layout.preferredWidth: 200
            Layout.maximumWidth: 200
            Layout.leftMargin: 32
        }
        HnLabel { role: HnTypographyRole.Title; rawText: "HnThumbnailDelegate"; Layout.leftMargin: 32 }

        HnThumbnailDelegate {
            objectName: "selectableThumbnail"
            checkable: true
            caption: qsTr("Click to toggle selection")
            checked: true
            showSelectionMarker: true
            Layout.leftMargin: 32
            Layout.preferredWidth: 180
            Layout.preferredHeight: 160
            thumbnailContent: HnIcon {
                source: "qrc:/qt/qml/Holonight/Controls/assets/folder.svg"
                size: 64
            }
        }

        HnLabel { role: HnTypographyRole.Title; rawText: "HnPreviewFrame"; Layout.leftMargin: 32 }

        Controls.ComboBox {
            id: previewSelector
            objectName: "previewSelector"
            Layout.leftMargin: 32
            model: [qsTr("Content"), qsTr("Loading"), qsTr("Empty"), qsTr("Error")]
            currentIndex: 2
        }
        HnPreviewFrame {
            objectName: "previewFrame"
            Layout.fillWidth: true
            Layout.leftMargin: 32
            Layout.rightMargin: 32
            previewState: previewSelector.currentIndex
            previewContent: HnIcon { source: "qrc:/qt/qml/Holonight/Controls/assets/folder.svg"; size: 64 }
            loadingContent: HnLoadingState { titleText: qsTr("Loading preview") }
            errorContent: HnEmptyState {
                titleText: qsTr("Preview unavailable")
                descriptionText: qsTr("Try another image.")
            }
            emptyContent: HnEmptyState { titleText: qsTr("Select an image") }
        }

        HnLabel { role: HnTypographyRole.Title; rawText: "HnMetadataList"; Layout.leftMargin: 32 }

        HnMetadataList {
            Layout.fillWidth: true
            Layout.leftMargin: 32
            Layout.rightMargin: 32
            rows: [
                {
                    heading: qsTr("Image information")
                },
                {
                    label: qsTr("Dimensions"),
                    value: "1920 × 1080"
                },
                {
                    label: qsTr("Description"),
                    value: qsTr("Caller-formatted metadata wraps in narrow layouts.")
                }
            ]
        }
        HnLabel { role: HnTypographyRole.Title; rawText: "HnLabeledKeyHint"; Layout.leftMargin: 32 }

        HnLabeledKeyHint {
            Layout.leftMargin: 32
            keyGroups: [[Qt.Key_Control, Qt.Key_L]]
            description: qsTr("Edit location")
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
            rawText: "HnSegmentedControl"
            Layout.leftMargin: 32
        }

        HnSegmentedControl {
            model: [
                {
                    text: qsTr("List"),
                    value: "list"
                },
                {
                    text: qsTr("Grid"),
                    value: "grid"
                }
            ]
            Layout.leftMargin: 32
        }

        HnLabel { role: HnTypographyRole.Title; rawText: "HnChoiceCard"; Layout.leftMargin: 32 }

        RowLayout {
            Layout.fillWidth: true
            Layout.leftMargin: 32
            Layout.rightMargin: 32

            Controls.ButtonGroup {
                id: galleryChoices
            }

            HnChoiceCard {
                title: qsTr("Dark")
                description: qsTr("A selected visual choice.")
                checked: true
                Controls.ButtonGroup.group: galleryChoices
                Layout.fillWidth: true
            }
            HnChoiceCard {
                title: qsTr("Light")
                description: qsTr("Composes with a standard Controls.ButtonGroup.")
                Controls.ButtonGroup.group: galleryChoices
                Layout.fillWidth: true
            }
        }
    }
}
