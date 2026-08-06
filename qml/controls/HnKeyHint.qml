// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Controls.Basic
import Holonight.Core

Control {
    id: root

    property string text

    font.family: HolonightTheme.fixedFont
    font.pointSize: HolonightTheme.fixedFontSize

    padding: 4
    leftPadding: 6
    rightPadding: 6
    Accessible.role: Accessible.StaticText
    Accessible.name: root.text

    contentItem: HnLabel {
        role: HnTypographyRole.Code
        rawText: root.text
        font: root.font
        color: root.enabled ? HoloniightPalette.textSecondary : HoloniightPalette.textDisabled
    }
    background: Rectangle {
        color: HoloniightPalette.surfaceRaised
        radius: HnAppearance.roundedRadius(HnSurfaceRole.Badge, width, height, HnAppearance.revision)
        border.width: HoloniightPalette.borderWidth
        border.color: HoloniightPalette.borderPassive
    }
}
