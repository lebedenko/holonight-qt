// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls.Basic
import Holonight.Core

Label {
    id: root

    property int role: HnTypographyRole.Body
    property bool showPrefix: false
    property string prefixText: "// "
    property string rawText: ""

    textFormat: (showPrefix && role === HnTypographyRole.MicroHeader) ? Text.StyledText : Text.PlainText
    text: {
        const content = rawText.length > 0 ? rawText : ""
        if (showPrefix && role === HnTypographyRole.MicroHeader && content.length > 0) {
            return "<font color='" + HoloniightPalette.brandForeground + "'>" + prefixText + "</font>" + content
        }
        return content
    }

    font.family: {
        switch (role) {
        case HnTypographyRole.MicroHeader: return HolonightTheme.headerFont
        case HnTypographyRole.Display: return HolonightTheme.displayFont
        case HnTypographyRole.Code: return HolonightTheme.fixedFont
        default: return HolonightTheme.uiFont
        }
    }

    font.pointSize: {
        switch (role) {
        case HnTypographyRole.Display: return HolonightTheme.displaySize
        case HnTypographyRole.Heading: return HolonightTheme.headingSize
        case HnTypographyRole.Title: return HolonightTheme.titleSize
        case HnTypographyRole.Subheading: return HolonightTheme.subheadingSize
        case HnTypographyRole.Body: return HolonightTheme.bodySize
        case HnTypographyRole.Caption:
        case HnTypographyRole.MicroHeader: return HolonightTheme.microSize
        case HnTypographyRole.Code: return HolonightTheme.fixedFontSize
        default: return HolonightTheme.bodySize
        }
    }

    font.weight: {
        switch (role) {
        case HnTypographyRole.Display:
        case HnTypographyRole.Heading:
        case HnTypographyRole.Title: return Font.Bold
        case HnTypographyRole.Subheading:
        case HnTypographyRole.MicroHeader: return Font.DemiBold
        default: return Font.Normal
        }
    }

    font.letterSpacing: role === HnTypographyRole.MicroHeader ? 1.5 : 0
    font.capitalization: role === HnTypographyRole.MicroHeader ? Font.AllUppercase : Font.MixedCase

    color: {
        switch (role) {
        case HnTypographyRole.MicroHeader: return HoloniightPalette.textMuted
        case HnTypographyRole.Caption: return HoloniightPalette.textSecondary
        default: return HoloniightPalette.textPrimary
        }
    }
}
