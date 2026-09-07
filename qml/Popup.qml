// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

import QtQuick
import QtQuick.Templates as T
import Holonight.Core
import Holonight.impl as Impl

T.Popup {
    id: root

    Impl.ControlPalette {
        id: colors
        palette: root.palette
        colorGroup: !root.enabled ? Impl.ControlPalette.Disabled
                                 : (root.contentItem.Window.window && !root.contentItem.Window.window.active
                                    ? Impl.ControlPalette.Inactive : Impl.ControlPalette.Active)
        textRole: Impl.ControlPalette.WindowText
        fillRole: Impl.ControlPalette.Button
    }

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    font.family: HolonightTheme.uiFont
    font.pointSize: HolonightTheme.bodySize
    padding: 12

    background: Rectangle {
        color: colors.colors.surfaceRaised
        border.color: colors.colors.borderPassive
        border.width: HnMetrics.borderWidth
        radius: HnAppearance.roundedRadius(HnSurfaceRole.Menu, width, height, HnAppearance.revision)
    }

    T.Overlay.modal: Rectangle { color: colors.colors.modalOverlay }
    T.Overlay.modeless: Rectangle { color: colors.colors.modelessOverlay }
}
