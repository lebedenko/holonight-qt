// SPDX-License-Identifier: GPL-3.0-or-later

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Templates as T
import QtQuick.Controls.Basic
import QtQuick.Layouts
import Holonight.Core

T.CheckDelegate {
    id: root

    property string title: text
    property string description
    property Component thumbnailContent
    readonly property alias thumbnailItem: thumbnailLoader.item

    implicitWidth: Math.max(180, implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: implicitContentHeight + topPadding + bottomPadding
    padding: 10
    spacing: 8
    hoverEnabled: true
    Accessible.name: root.title
    Accessible.description: root.description

    contentItem: ColumnLayout {
        spacing: root.spacing
        Loader {
            id: thumbnailLoader
            active: root.thumbnailContent !== null
            visible: active
            sourceComponent: root.thumbnailContent
            Layout.fillWidth: true
        }
        HnLabel {
            role: HnTypographyRole.Body
            font.bold: true
            rawText: root.title
            color: root.enabled ? HoloniightPalette.textPrimary : HoloniightPalette.textDisabled
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
    background: Item {
        Rectangle {
            objectName: "hnChoiceCardBase"
            anchors.fill: parent
            color: root.down ? HoloniightPalette.surfaceElevated
                             : (!root.checked && root.hovered ? HoloniightPalette.surfaceHover
                                                              : HoloniightPalette.surfaceElevated)
            radius: HnAppearance.roundedRadius(HnSurfaceRole.Control, width, height, HnAppearance.revision)
            Accessible.ignored: true
        }
        Rectangle {
            objectName: "hnChoiceCardSelectedOverlay"
            anchors.fill: parent
            color: HoloniightPalette.surfaceSelected
            radius: HnAppearance.roundedRadius(HnSurfaceRole.Control, width, height, HnAppearance.revision)
            visible: root.checked && !root.down
            Accessible.ignored: true
        }
        Rectangle {
            anchors.fill: parent
            color: "transparent"
            radius: HnAppearance.roundedRadius(HnSurfaceRole.Control, width, height, HnAppearance.revision)
            border.width: root.visualFocus || root.checked
                          ? HnMetrics.focusBorderWidth : HnMetrics.borderWidth
            border.color: root.visualFocus ? HoloniightPalette.borderFocus
                                           : (root.checked ? HoloniightPalette.selectionIndicator
                                                           : HoloniightPalette.borderPassive)
            Accessible.ignored: true
        }
    }
}
