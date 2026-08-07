// SPDX-License-Identifier: GPL-3.0-or-later

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import Holonight.Core

HnSelectableDelegate {
    id: root

    property string title: text
    property string subtitle
    property string statusText
    property Component leadingContent
    property Component trailingContent
    readonly property alias leadingItem: leadingLoader.item
    readonly property alias trailingItem: trailingLoader.item

    selectionStyle: HnSelectableDelegate.Outline
    background: Item {
        readonly property real semanticRadius: HnAppearance.roundedRadius(HnSurfaceRole.Card, width, height,
                                                                          HnAppearance.revision)

        Rectangle {
            objectName: "hnCardDelegateBase"
            anchors.fill: parent
            color: root.down ? HoloniightPalette.surfaceElevated
                             : (!root.selected && root.hovered ? HoloniightPalette.surfaceHover
                                                               : HoloniightPalette.surfaceElevated)
            radius: parent.semanticRadius
            Accessible.ignored: true
        }
        Rectangle {
            objectName: "hnCardDelegateSelectedOverlay"
            anchors.fill: parent
            color: root.hovered ? HoloniightPalette.surfaceSelectedHover
                                : HoloniightPalette.surfaceSelected
            radius: parent.semanticRadius
            visible: root.selected && !root.down
            Accessible.ignored: true
        }
        Rectangle {
            anchors.fill: parent
            color: "transparent"
            radius: parent.semanticRadius
            border.width: root.visualFocus || root.selected
                          ? HnMetrics.focusBorderWidth : HnMetrics.borderWidth
            border.color: root.visualFocus ? HoloniightPalette.borderFocus
                                           : (root.selected ? HoloniightPalette.selectionIndicator
                                                            : HoloniightPalette.borderPassive)
            Accessible.ignored: true
        }
    }
    Accessible.name: root.title
    Accessible.description: root.subtitle

    contentItem: RowLayout {
        spacing: root.semanticSpacing
        Loader {
            id: leadingLoader
            active: root.leadingContent !== null
            visible: active
            sourceComponent: root.leadingContent
        }
        ColumnLayout {
            spacing: Math.max(2, root.semanticSpacing / 2)
            Layout.fillWidth: true
            HnLabel {
                role: HnTypographyRole.Body
                font.bold: true
                rawText: root.title
                color: root.enabled ? HoloniightPalette.textPrimary : HoloniightPalette.textDisabled
                Layout.fillWidth: true
            }
            HnLabel {
                role: HnTypographyRole.Caption
                rawText: root.subtitle
                color: HoloniightPalette.textMuted
                wrapMode: Text.WordWrap
                visible: text.length > 0
                Layout.fillWidth: true
            }
            HnLabel {
                role: HnTypographyRole.Caption
                rawText: root.statusText
                color: HoloniightPalette.textSecondary
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
}
