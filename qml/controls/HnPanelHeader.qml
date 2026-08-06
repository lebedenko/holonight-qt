// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import Holonight.Core

Item {
    id: root

    property string title
    property string description
    property Component leadingContent
    property Component trailingContent
    property bool dividerVisible: true
    readonly property alias leadingItem: leadingLoader.item
    readonly property alias trailingItem: trailingLoader.item

    implicitWidth: content.implicitWidth
    implicitHeight: content.implicitHeight
    Accessible.role: Accessible.Grouping
    Accessible.name: root.title
    Accessible.description: root.description

    ColumnLayout {
        id: content
        anchors.fill: parent
        spacing: 8
        RowLayout {
            spacing: 8
            Layout.fillWidth: true
            Loader {
                id: leadingLoader
                active: root.leadingContent !== null
                visible: active
                sourceComponent: root.leadingContent
            }
            ColumnLayout {
                spacing: 2
                Layout.fillWidth: true
                HnLabel {
                    role: HnTypographyRole.Title
                    font.bold: true
                    rawText: root.title
                    color: HoloniightPalette.textPrimary
                    visible: text.length > 0
                    Layout.fillWidth: true
                }
                HnLabel {
                    role: HnTypographyRole.Caption
                    rawText: root.description
                    color: HoloniightPalette.textMuted
                    wrapMode: Text.WordWrap
                    visible: text.length > 0
                    Layout.fillWidth: true
                }
            }
            Loader {
                id: trailingLoader
                active: root.trailingContent !== null
                visible: active
                sourceComponent: root.trailingContent
            }
        }
        Rectangle {
            color: HoloniightPalette.borderPassive
            visible: root.dividerVisible
            Layout.fillWidth: true
            Layout.preferredHeight: visible ? HoloniightPalette.separatorWidth : 0
        }
    }
}
