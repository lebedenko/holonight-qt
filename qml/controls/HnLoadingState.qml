// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import Holonight as H
import Holonight.Core

Item {
    id: root

    property int sizeRole: HnControlSize.Normal
    property string titleText
    property string descriptionText
    property bool running: true
    property real progress: -1
    property Component actionContent
    readonly property alias actionItem: actionLoader.item
    readonly property int resolvedSizeRole: HnMetrics.normalizedSizeRole(root.sizeRole)
    readonly property real normalizedProgress: root.progress < 0 ? -1 : Math.min(1, Math.max(0, root.progress))
    readonly property real semanticSpacing: HnMetrics.internalSpacing(root.resolvedSizeRole)

    implicitWidth: content.implicitWidth
    implicitHeight: content.implicitHeight
    Accessible.role: Accessible.Grouping
    Accessible.name: root.titleText
    Accessible.description: root.descriptionText

    ColumnLayout {
        id: content

        anchors.fill: parent
        spacing: root.semanticSpacing

        H.ProgressBar {
            objectName: "hnLoadingProgress"
            value: root.normalizedProgress < 0 ? 0 : root.normalizedProgress
            indeterminate: root.running && root.visible && root.normalizedProgress < 0
            visible: root.running
            Accessible.name: root.titleText
            Layout.fillWidth: true
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
