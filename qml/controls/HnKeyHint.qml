// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Controls as C
import Holonight.Core

C.Control {
    id: root

    property string text
    property var keyGroups: []
    property bool wrap: false
    readonly property string accessibleText: sequence.accessibleText

    font.family: HolonightTheme.monospaceFont
    font.pointSize: HolonightTheme.monospaceFontSize

    // Scale compact padding with the resolved line height (22 px at the reference font).
    padding: metrics.height * (2 / 22)
    leftPadding: metrics.height * (6 / 22)
    rightPadding: leftPadding
    // Use single-line metrics, never the width-dependent wrapped height.
    implicitWidth: Math.max(implicitContentWidth + leftPadding + rightPadding,
                            1.2 * (metrics.height + topPadding + bottomPadding))
    Accessible.role: Accessible.StaticText
    Accessible.name: accessibleText

    FontMetrics {
        id: metrics
        font: root.font
    }

    contentItem: Item {
        implicitWidth: sequence.implicitWidth
        implicitHeight: sequence.implicitHeight
        Accessible.ignored: true

        HnKeySequenceLabel {
            id: sequence
            x: (parent.width - width) / 2
            y: (parent.height - height) / 2
            width: Math.min(parent.width, implicitWidth)
            keyGroups: root.keyGroups
            text: root.text
            font: root.font
            wrap: root.wrap
            Accessible.ignored: true
        }
    }

    background: Rectangle {
        color: HoloniightPalette.surfaceRaised
        radius: Math.min(2, HnAppearance.roundedRadius(HnSurfaceRole.Control, width, height, HnAppearance.revision))
        border.width: HnMetrics.borderWidth
        border.color: HoloniightPalette.borderPassive
    }
}
