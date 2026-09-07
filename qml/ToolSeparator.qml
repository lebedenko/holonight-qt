// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

import QtQuick
import QtQuick.Templates as T
import Holonight.Core
import Holonight.impl as Impl

T.ToolSeparator {
    id: root

    Impl.ControlPalette {
        id: colors
        palette: root.palette
        colorGroup: !root.enabled ? Impl.ControlPalette.Disabled : (root.Window.window && !root.Window.window.active ? Impl.ControlPalette.Inactive : Impl.ControlPalette.Active)
        textRole: Impl.ControlPalette.WindowText
        fillRole: Impl.ControlPalette.Button
    }

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    padding: vertical ? 6 : 2
    verticalPadding: vertical ? 2 : 6

    contentItem: Rectangle {
        implicitWidth: root.vertical ? HnMetrics.separatorWidth : 30
        implicitHeight: root.vertical ? 30 : HnMetrics.separatorWidth
        color: colors.colors.borderSubtle
    }
}
