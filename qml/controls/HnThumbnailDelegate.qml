// SPDX-License-Identifier: GPL-3.0-or-later

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as C
import QtQuick.Layouts
import Holonight.Core

HnSelectableDelegate {
    id: root
    property Component thumbnailContent
    property string caption: text
    property int captionElide: Text.ElideRight
    readonly property alias captionItem: captionLabel
    property string tooltip: caption
    property bool showSelectionMarker: false
    readonly property alias thumbnailItem: thumbnail.item
    signal activated
    signal contextRequested

    // Grid current-item focus is independent of explicit application selection.
    readonly property bool explicitlySelected: checked || highlighted
    currentItemSelects: false
    selectionStyle: HnSelectableDelegate.Outline
    Accessible.name: caption
    onClicked: activated()
    onPressAndHold: contextRequested()
    TapHandler {
        acceptedButtons: Qt.RightButton
        onTapped: root.contextRequested()
    }
    Keys.onReturnPressed: activated()
    Keys.onEnterPressed: activated()
    Keys.onMenuPressed: contextRequested()
    C.ToolTip.visible: hovered && tooltip.length > 0
    C.ToolTip.text: tooltip

    contentItem: ColumnLayout {
        spacing: root.semanticSpacing
        Accessible.ignored: true
        Loader {
            id: thumbnail
            sourceComponent: root.thumbnailContent
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
        HnLabel {
            id: captionLabel
            objectName: "thumbnailCaption"
            rawText: root.caption
            visible: text.length > 0
            elide: root.captionElide
            maximumLineCount: 1
            horizontalAlignment: Text.AlignHCenter
            Layout.fillWidth: true
            Accessible.ignored: true
        }
    }
    Rectangle {
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: root.padding
        width: HnMetrics.internalSpacing(root.resolvedSizeRole)
        height: width
        radius: width / 2
        visible: root.showSelectionMarker && root.explicitlySelected
        color: HoloniightPalette.selectionIndicator
        Accessible.ignored: true
    }
}
