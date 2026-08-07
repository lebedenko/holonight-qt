// SPDX-License-Identifier: GPL-3.0-or-later

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import Holonight.Core

HnSelectableDelegate {
    id: root

    enum SubtitlePresentation {
        MultiLine,
        SingleLine
    }

    property string title: text
    property string subtitle
    property string metadata
    property int subtitlePresentation: HnListDelegate.MultiLine
    property Component leadingContent
    property int leadingContentAlignment: Qt.AlignTop
    property Component trailingContent
    property bool dividerVisible: false
    readonly property alias leadingItem: leadingLoader.item
    readonly property alias trailingItem: trailingLoader.item

    Accessible.name: root.title
    Accessible.description: root.subtitle

    contentItem: RowLayout {
        spacing: root.semanticSpacing

        Loader {
            id: leadingLoader
            objectName: "hnListDelegateLeadingLoader"
            readonly property int effectiveAlignment: Layout.alignment
            active: root.leadingContent !== null
            visible: active
            sourceComponent: root.leadingContent
            Layout.alignment: root.leadingContentAlignment
        }
        ColumnLayout {
            spacing: Math.max(2, root.semanticSpacing / 2)
            Layout.fillWidth: true
            HnLabel {
                role: HnTypographyRole.Body
                rawText: root.title
                color: root.enabled ? HoloniightPalette.textPrimary : HoloniightPalette.textDisabled
                elide: Text.ElideRight
                Layout.fillWidth: true
            }
            HnLabel {
                objectName: "hnListDelegateSubtitle"
                role: HnTypographyRole.Caption
                rawText: root.subtitle
                color: root.enabled ? HoloniightPalette.textMuted : HoloniightPalette.textDisabled
                wrapMode: root.subtitlePresentation === HnListDelegate.SingleLine ? Text.NoWrap : Text.WordWrap
                elide: root.subtitlePresentation === HnListDelegate.SingleLine ? Text.ElideRight : Text.ElideNone
                maximumLineCount: root.subtitlePresentation === HnListDelegate.SingleLine ? 1 : 0x7fffffff
                visible: text.length > 0
                Layout.fillWidth: true
            }
        }
        HnLabel {
            role: HnTypographyRole.Caption
            rawText: root.metadata
            color: HoloniightPalette.textSecondary
            visible: text.length > 0
            Layout.alignment: Qt.AlignTop
        }
        Loader {
            id: trailingLoader
            active: root.trailingContent !== null
            visible: active
            sourceComponent: root.trailingContent
        }
    }

    Rectangle {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: root.dividerVisible ? HnMetrics.separatorWidth : 0
        color: HoloniightPalette.borderSubtle
        visible: root.dividerVisible
        Accessible.ignored: true
    }
}
