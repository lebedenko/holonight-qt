// SPDX-License-Identifier: GPL-3.0-or-later
pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls as C
import Holonight.Core

C.Control {
    id: root
    property string label: "NORMAL"
    property color fill: HoloniightPalette.accentBlue
    property var supportedLabels: ["NORMAL", "VISUAL", "V-MARK", "SEARCH", "INSERT"]
    implicitWidth: Math.ceil(metric.advanceWidth) + leftPadding + rightPadding
    topPadding: 2
    bottomPadding: 2
    leftPadding: 8
    rightPadding: 8
    focusPolicy: Qt.NoFocus
    activeFocusOnTab: false
    Accessible.role: Accessible.StaticText
    Accessible.name: qsTr("%1 mode").arg(label)
    TextMetrics {
        id: metric
        font: caption.font
        text: root.supportedLabels.reduce((longest, candidate) => candidate.length > longest.length ? candidate : longest, "")
    }
    contentItem: HnLabel {
        id: caption
        objectName: "modeBadgeLabel"
        role: HnTypographyRole.Code
        color: HoloniightPalette.background
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        rawText: root.label
    }
    background: Rectangle {
        color: root.fill
        radius: HnAppearance.roundedRadius(HnSurfaceRole.Control, width, height, HnAppearance.revision)
    }
}
