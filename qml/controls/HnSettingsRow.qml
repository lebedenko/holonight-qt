// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import Holonight.Core

FocusScope {
    id: root

    default property Component control
    property int sizeRole: HnControlSize.Normal
    property string titleText
    property string descriptionText
    property bool stacked: false
    property bool dividerVisible: false
    property real contentHorizontalPadding: 0
    property Component leadingContent
    property Component trailingContent
    readonly property alias leadingItem: leadingLoader.item
    readonly property alias controlItem: controlLoader.item
    readonly property alias trailingItem: trailingLoader.item
    readonly property int resolvedSizeRole: HnControlMetrics.normalizedSizeRole(root.sizeRole)
    readonly property real semanticSpacing: HnControlMetrics.internalSpacing(root.resolvedSizeRole)

    implicitWidth: content.implicitWidth
    implicitHeight: Math.max(content.implicitHeight + root.semanticSpacing * 2,
                             HnControlMetrics.controlHeight(root.resolvedSizeRole))
    activeFocusOnTab: controlLoader.status === Loader.Ready && controlLoader.loadedControl.activeFocusOnTab
    onActiveFocusChanged: {
        if (root.activeFocus && controlLoader.status === Loader.Ready
                && controlLoader.loadedControl.activeFocusOnTab)
            controlLoader.loadedControl.forceActiveFocus()
    }

    GridLayout {
        id: content

        anchors.fill: parent
        anchors.topMargin: root.semanticSpacing
        anchors.bottomMargin: root.semanticSpacing
        anchors.leftMargin: Math.max(0, root.contentHorizontalPadding)
        anchors.rightMargin: Math.max(0, root.contentHorizontalPadding)
        columns: root.stacked ? 2 : 4
        columnSpacing: root.semanticSpacing
        rowSpacing: root.semanticSpacing

        Loader {
            id: leadingLoader

            active: root.leadingContent !== null
            visible: active
            sourceComponent: root.leadingContent
            Layout.alignment: Qt.AlignTop
            Layout.rowSpan: 1
        }

        ColumnLayout {
            spacing: root.semanticSpacing / 2
            Layout.fillWidth: true
            Layout.columnSpan: root.stacked && !leadingLoader.active ? 2 : 1

            HnLabel {
                objectName: "hnSettingsRowTitle"
                role: HnTypographyRole.Body
                rawText: root.titleText
                color: HoloniightPalette.textPrimary
                wrapMode: Text.WordWrap
                visible: text.length > 0
                Layout.fillWidth: true
            }

            HnLabel {
                objectName: "hnSettingsRowDescription"
                role: HnTypographyRole.Caption
                rawText: root.descriptionText
                color: HoloniightPalette.textMuted
                wrapMode: Text.WordWrap
                visible: text.length > 0
                Layout.fillWidth: true
            }
        }

        Loader {
            id: controlLoader

            readonly property Item loadedControl: item as Item

            active: root.control !== null
            visible: active
            sourceComponent: root.control
            Layout.alignment: root.stacked ? Qt.AlignLeft : Qt.AlignRight | Qt.AlignVCenter
            Layout.column: root.stacked ? 0 : 2
            Layout.columnSpan: root.stacked ? 2 : 1
            Layout.row: root.stacked ? 1 : 0
            Layout.fillWidth: root.stacked
        }

        Loader {
            id: trailingLoader

            active: root.trailingContent !== null
            visible: active
            sourceComponent: root.trailingContent
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            Layout.column: root.stacked ? 0 : 3
            Layout.columnSpan: root.stacked ? 2 : 1
            Layout.row: root.stacked ? 2 : 0
        }
    }

    HnSeparator {
        objectName: "settingsRowDivider"
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        color: HoloniightPalette.borderPassive
        height: root.dividerVisible ? implicitHeight : 0
        visible: root.dividerVisible
    }
}
