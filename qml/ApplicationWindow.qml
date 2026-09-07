// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

import QtQuick
import QtQuick.Templates as T
import Holonight.Core
import Holonight.impl as Impl

T.ApplicationWindow {
    id: root

    Impl.ControlPalette {
        id: colors
        palette: root.palette
        colorGroup: root.active ? Impl.ControlPalette.Active : Impl.ControlPalette.Inactive
        textRole: Impl.ControlPalette.WindowText
        fillRole: Impl.ControlPalette.Button
    }

    font.family: HolonightTheme.uiFont
    font.pointSize: HolonightTheme.bodySize

    color: colors.colors.background
}
