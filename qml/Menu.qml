// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

import QtQuick
import QtQuick.Templates as T
import Holonight
import Holonight.Core
import Holonight.impl as Impl

T.Menu {
    id: root

    readonly property Impl.ControlPalette controlColors: Impl.ControlPalette {
        palette: root.palette
        colorGroup: !root.enabled ? Impl.ControlPalette.Disabled
                                 : (root.contentItem.Window.window && !root.contentItem.Window.window.active
                                    ? Impl.ControlPalette.Inactive : Impl.ControlPalette.Active)
        textRole: Impl.ControlPalette.WindowText
        fillRole: Impl.ControlPalette.Button
    }

    font.family: HolonightTheme.uiFont
    font.pointSize: HolonightTheme.bodySize

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    margins: 0
    padding: 4

    readonly property bool hasIcons: {
        for (let index = 0; index < root.count; ++index) {
            const item = root.itemAt(index) as T.MenuItem
            if (item && item.visible
                    && (item.icon.name.length > 0 || item.icon.source.toString().length > 0))
                return true
        }
        return false
    }

    delegate: MenuItem {}

    contentItem: ListView {
        implicitHeight: contentHeight
        model: root.contentModel
        interactive: contentHeight > implicitHeight
        clip: contentHeight > height
        currentIndex: root.currentIndex
    }

    background: Rectangle {
        readonly property real semanticRadius: HnAppearance.roundedRadius(HnSurfaceRole.Menu,
                                                                          width, height,
                                                                          HnAppearance.revision)

        implicitWidth: 180
        color: root.controlColors.colors.surfaceRaised
        border.color: root.controlColors.colors.borderPassive
        border.width: HnMetrics.borderWidth
        radius: semanticRadius
    }
}
