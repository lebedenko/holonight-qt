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
            rawText: qsTr("Inputs & Forms")
            Layout.leftMargin: 32
            Layout.topMargin: 24
        }

        HnLabel {
            role: HnTypographyRole.Title
            rawText: qsTr("Search fields")
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

            RowLayout {
                required property var modelData

                spacing: 16
                Layout.fillWidth: true
                Layout.leftMargin: 32
                Layout.rightMargin: 32

                HnLabel {
                    role: HnTypographyRole.Body
                    rawText: parent.modelData.label
                    Layout.preferredWidth: 80
                }

                HnSearchField {
                    sizeRole: parent.modelData.role
                    placeholderText: qsTr("Search applications")
                    Layout.fillWidth: true
                }
            }
        }

        HnSearchField {
            text: qsTr("Filled field with custom content")
            Layout.fillWidth: true
            Layout.leftMargin: 32
            Layout.rightMargin: 32

            leadingContent: HnLabel {
                role: HnTypographyRole.Body
                rawText: qsTr("⌘")
                color: HoloniightPalette.primary
            }

            trailingContent: HnLabel {
                role: HnTypographyRole.Body
                rawText: qsTr("Ctrl K")
                color: HoloniightPalette.textMuted
            }
        }

        HnSearchField {
            placeholderText: qsTr("Validation error")
            hasError: true
            Layout.fillWidth: true
            Layout.leftMargin: 32
            Layout.rightMargin: 32
        }

        HnSearchField {
            text: qsTr("Read-only content remains selectable")
            readOnly: true
            Layout.fillWidth: true
            Layout.leftMargin: 32
            Layout.rightMargin: 32
        }

        HnSearchField {
            text: qsTr("Disabled field")
            enabled: false
            Layout.fillWidth: true
            Layout.leftMargin: 32
            Layout.rightMargin: 32
        }

        HnSearchField {
            text: qsTr("A deliberately long search query that demonstrates horizontal editing and "
                       + "cursor navigation without changing the control contract")
            sizeRole: HnControlSize.Large
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
            rawText: qsTr("Icon combo boxes")
            Layout.leftMargin: 32
        }

        Repeater {
            model: [
                { label: qsTr("Compact"), role: HnControlSize.Compact },
                { label: qsTr("Normal"), role: HnControlSize.Normal },
                { label: qsTr("Large"), role: HnControlSize.Large },
                { label: qsTr("Hero"), role: HnControlSize.Hero }
            ]

            RowLayout {
                required property var modelData

                Layout.fillWidth: true
                Layout.leftMargin: 32
                Layout.rightMargin: 32

                HnLabel {
                    role: HnTypographyRole.Body
                    rawText: parent.modelData.label
                    Layout.preferredWidth: 80
                }

                HnIconComboBox {
                    sizeRole: parent.modelData.role
                    textRole: "label"
                    valueRole: "id"
                    model: [
                        { id: "search", label: qsTr("Search"), icon: "qrc:/qt/qml/Holonight/Controls/assets/search.svg" },
                        { id: "plain", label: qsTr("No icon"), icon: "" },
                        { id: "empty", label: qsTr("Empty icon collapses"), icon: "" }
                    ]
                    Layout.fillWidth: true
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.leftMargin: 32
            Layout.rightMargin: 32

            HnLabel {
                role: HnTypographyRole.Body
                rawText: qsTr("Scrollable")
                Layout.preferredWidth: 80
            }

            HnIconComboBox {
                model: [qsTr("Alpha"), qsTr("Beta"), qsTr("Gamma"), qsTr("Delta"),
                        qsTr("Epsilon"), qsTr("Zeta"), qsTr("Eta"), qsTr("Theta"),
                        qsTr("Iota"), qsTr("Kappa"), qsTr("Lambda"), qsTr("Mu")]
                Layout.fillWidth: true
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.leftMargin: 32
            Layout.rightMargin: 32

            HnIconComboBox {
                editable: true
                model: [qsTr("Editable value"), qsTr("Another value")]
                Layout.fillWidth: true
            }

            HnIconComboBox {
                model: []
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
            rawText: qsTr("Text areas and form structure")
            Layout.leftMargin: 32
        }

        HnTextArea {
            placeholderText: qsTr("Grows from three to five lines, then scrolls")
            minimumVisibleLines: 3
            maximumVisibleLines: 5
            Layout.fillWidth: true
            Layout.leftMargin: 32
            Layout.rightMargin: 32

            footerContent: HnLabel {
                role: HnTypographyRole.Caption
                rawText: qsTr("Optional footer content inside the shared surface")
                color: HoloniightPalette.textMuted
            }
        }

        HnTextArea {
            text: qsTr("A long prefilled value demonstrates bounded overflow.\nLine two.\nLine three.\n"
                       + "Line four.\nLine five.\nLine six remains reachable by scrolling.")
            minimumVisibleLines: 2
            maximumVisibleLines: 4
            hasError: true
            Layout.fillWidth: true
            Layout.leftMargin: 32
            Layout.rightMargin: 32
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.leftMargin: 32
            Layout.rightMargin: 32

            HnTextArea {
                text: qsTr("Read-only selectable text")
                readOnly: true
                Layout.fillWidth: true
            }

            HnTextArea {
                text: qsTr("Disabled multiline editor")
                enabled: false
                Layout.fillWidth: true
            }
        }

        HnFormField {
            labelText: qsTr("Project name")
            helperText: qsTr("Shown when the field has no validation error.")
            required: true
            Layout.fillWidth: true
            Layout.leftMargin: 32
            Layout.rightMargin: 32

            control: Component {
                HnSearchField {
                    placeholderText: qsTr("Enter a project name")
                }
            }
        }

        HnFormField {
            labelText: qsTr("Description")
            helperText: qsTr("Helper text is replaced by the error.")
            errorText: qsTr("A description is required.")
            hasError: true
            required: true
            Layout.fillWidth: true
            Layout.leftMargin: 32
            Layout.rightMargin: 32

            control: Component {
                HnTextArea {
                    minimumVisibleLines: 2
                    maximumVisibleLines: 4
                    hasError: true
                }
            }
        }
    }
}
