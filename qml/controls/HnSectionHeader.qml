// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import Holonight.Core

Item {
    id: root

    property int sizeRole: HnControlSize.Normal
    property string titleText
    property string descriptionText
    property bool dividerVisible: true
    property bool isCategoryMode: false
    property bool showPrefix: false
    property string prefixText: "// "
    property Component leadingContent
    property Component trailingContent
    readonly property alias leadingItem: leadingLoader.item
    readonly property alias trailingItem: trailingLoader.item
    readonly property int resolvedSizeRole: HnMetrics.normalizedSizeRole(root.sizeRole)
    readonly property real semanticSpacing: HnMetrics.internalSpacing(root.resolvedSizeRole)

    implicitWidth: content.implicitWidth
    implicitHeight: content.implicitHeight
    Accessible.role: Accessible.Grouping
    Accessible.name: root.titleText
    Accessible.description: root.descriptionText

    ColumnLayout {
        id: content

        anchors.fill: parent
        spacing: root.resolvedSizeRole === HnControlSize.Compact ? Math.max(2, root.semanticSpacing / 2) : root.semanticSpacing

        RowLayout {
            spacing: root.resolvedSizeRole === HnControlSize.Compact ? Math.max(2, root.semanticSpacing / 2) : root.semanticSpacing
            visible: root.titleText.length > 0 || root.descriptionText.length > 0
                     || root.leadingContent !== null || root.trailingContent !== null
            Layout.fillWidth: true

            Loader {
                id: leadingLoader

                active: root.leadingContent !== null
                visible: active
                sourceComponent: root.leadingContent
                Layout.alignment: Qt.AlignTop
            }

            ColumnLayout {
                spacing: root.semanticSpacing / 2
                Layout.fillWidth: true

                HnLabel {
                    objectName: "sectionTitleLabel"
                    role: root.isCategoryMode ? HnTypographyRole.MicroHeader
                                              : (root.resolvedSizeRole === HnControlSize.Compact ? HnTypographyRole.Caption : HnTypographyRole.Subheading)
                    rawText: root.isCategoryMode && root.showPrefix ? root.prefixText + root.titleText : root.titleText
                    color: root.isCategoryMode ? HoloniightPalette.textAccent
                                               : (root.resolvedSizeRole === HnControlSize.Compact
                                                  ? HoloniightPalette.textSecondary : HoloniightPalette.textPrimary)
                    font.pointSize: root.isCategoryMode ? HolonightTheme.microSize
                                                        : (root.resolvedSizeRole === HnControlSize.Compact
                                                           ? HolonightTheme.microSize : HolonightTheme.subheadingSize)
                    font.letterSpacing: 0
                    wrapMode: Text.WordWrap
                    visible: rawText.length > 0 || root.titleText.length > 0
                    Layout.fillWidth: true
                }

                HnLabel {
                    role: HnTypographyRole.Caption
                    rawText: root.descriptionText
                    color: HoloniightPalette.textMuted
                    wrapMode: Text.WordWrap
                    visible: rawText.length > 0 || root.descriptionText.length > 0
                    Layout.fillWidth: true
                }
            }

            Loader {
                id: trailingLoader

                active: root.trailingContent !== null
                visible: active
                sourceComponent: root.trailingContent
                Layout.alignment: Qt.AlignTop | Qt.AlignRight
            }
        }

        HnSeparator {
            color: HoloniightPalette.borderPassive
            visible: root.dividerVisible
            Layout.fillWidth: true
            Layout.preferredHeight: visible ? implicitHeight : 0
        }
    }
}
