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
    property url iconSource
    property string titleText
    property string descriptionText
    property Component graphicContent
    property Component actionContent
    readonly property alias graphicItem: graphicLoader.item
    readonly property alias actionItem: actionLoader.item
    readonly property int resolvedSizeRole: HnControlMetrics.normalizedSizeRole(root.sizeRole)
    readonly property real semanticSpacing: HnControlMetrics.internalSpacing(root.resolvedSizeRole)
    readonly property bool hasDefaultIcon: root.graphicContent === null && String(root.iconSource).length > 0
                                           && !defaultIcon.hasError

    implicitWidth: content.implicitWidth
    implicitHeight: content.implicitHeight
    Accessible.role: Accessible.Grouping
    Accessible.name: root.titleText
    Accessible.description: root.descriptionText

    ColumnLayout {
        id: content

        anchors.fill: parent
        spacing: root.semanticSpacing

        Loader {
            id: graphicLoader

            active: root.graphicContent !== null
            visible: active
            sourceComponent: root.graphicContent
            Layout.alignment: Qt.AlignHCenter
        }

        HnIcon {
            id: defaultIcon

            source: root.iconSource
            size: HnControlMetrics.iconSize(root.resolvedSizeRole) * 2
            visible: root.hasDefaultIcon
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: visible ? implicitWidth : 0
            Layout.preferredHeight: visible ? implicitHeight : 0
        }

        HnLabel {
            role: HnTypographyRole.Subheading
            font.bold: true
            rawText: root.titleText
            color: HoloniightPalette.textPrimary
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
            visible: text.length > 0
            Layout.fillWidth: true
        }

        HnLabel {
            role: HnTypographyRole.Caption
            rawText: root.descriptionText
            color: HoloniightPalette.textMuted
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
            visible: text.length > 0
            Layout.fillWidth: true
        }

        Loader {
            id: actionLoader

            active: root.actionContent !== null
            visible: active
            sourceComponent: root.actionContent
            Layout.alignment: Qt.AlignHCenter
        }
    }
}
