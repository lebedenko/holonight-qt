// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

import QtQuick
import QtQuick.Templates as T
import Holonight.Core
import Holonight.impl as Impl

T.ToolTip {
    id: root

    readonly property Impl.ControlPalette controlColors: Impl.ControlPalette {
        palette: root.palette
        colorGroup: !root.enabled ? Impl.ControlPalette.Disabled
                                 : (root.contentItem.Window.window && !root.contentItem.Window.window.active
                                    ? Impl.ControlPalette.Inactive : Impl.ControlPalette.Active)
        textRole: Impl.ControlPalette.WindowText
        fillRole: Impl.ControlPalette.Base
    }

    font.family: HolonightTheme.uiFont
    font.pointSize: HolonightTheme.captionSize

    x: parent ? (parent.width - implicitWidth) / 2 : 0
    y: -implicitHeight - 4

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    margins: 8
    padding: 8

    delay: 500
    timeout: 5000

    contentItem: Text {
        text: root.text
        font: root.font
        color: root.controlColors.colors.tooltipText
        wrapMode: Text.Wrap
        textFormat: Text.PlainText
    }

    background: Rectangle {
        readonly property real semanticRadius: HnAppearance.roundedRadius(HnSurfaceRole.Tooltip,
                                                                          width, height,
                                                                          HnAppearance.revision)

        color: root.controlColors.colors.tooltipBase
        border.color: root.controlColors.colors.borderPassive
        border.width: HnMetrics.borderWidth
        radius: semanticRadius
    }
}
