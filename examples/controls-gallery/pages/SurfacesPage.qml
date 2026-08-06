// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Holonight.Core
import Holonight.Controls
import ".."

ScrollView {
    id: root

    contentWidth: availableWidth

    ColumnLayout {
        width: parent.width
        spacing: 16

        HnLabel {
            role: HnTypographyRole.Heading
            rawText: qsTr("Surfaces & Elevation")
            Layout.leftMargin: 32
            Layout.topMargin: 24
        }

        HnLabel {
            role: HnTypographyRole.Caption
            rawText: qsTr("Nest surfaces from the window plane toward transient content. The frame role selects semantic geometry; its fill may use the color token appropriate to that plane.")
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
            Layout.leftMargin: 32
            Layout.rightMargin: 32
        }

        SurfaceHierarchyExample {
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
            rawText: qsTr("Header & Title Controls")
            Layout.leftMargin: 32
        }

        RowLayout {
            spacing: 24
            Layout.fillWidth: true
            Layout.leftMargin: 32
            Layout.rightMargin: 32

            HnAppTitle {
                applicationName: qsTr("Controls Gallery")
            }

            HnAppTitle {
                applicationName: qsTr("Controls Gallery")
                iconSource: "qrc:/qt/qml/Holonight/Controls/assets/folder.svg"
                iconTinted: true
                Layout.fillWidth: true
            }
        }

        HnHeaderBar {
            dividerInset: 12
            content: HnAppTitle {
                applicationName: qsTr("Header region")
                iconSource: "qrc:/qt/qml/Holonight/Controls/assets/folder.svg"
                iconTinted: true
            }
            Layout.fillWidth: true
            Layout.leftMargin: 32
            Layout.rightMargin: 32
        }

        HnPanelHeader {
            title: qsTr("Selection and composition")
            description: qsTr("Reusable navigation, list, card, status, choice, and action patterns.")
            Layout.fillWidth: true
            Layout.leftMargin: 32
            Layout.rightMargin: 32
            trailingContent: HnKeyHint { text: qsTr("Ctrl K") }
        }

        HnSectionHeader {
            titleText: qsTr("CATEGORY HEADER")
            isCategoryMode: true
            showPrefix: true
            Layout.fillWidth: true
            Layout.leftMargin: 32
            Layout.rightMargin: 32
        }

        HnSectionHeader {
            titleText: qsTr("Standard Section Header")
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
            titleText: qsTr("Compact Section Header")
            descriptionText: qsTr("Compact density role for dense settings surfaces.")
            sizeRole: HnControlSize.Compact
            Layout.fillWidth: true
            Layout.leftMargin: 32
            Layout.rightMargin: 32
        }
    }
}
