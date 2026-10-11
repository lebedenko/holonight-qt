// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

pragma ComponentBehavior: Bound

import QtQuick
import Holonight.Core

Rectangle {
    id: root
    property bool pressed: false
    property bool hovered: false
    property bool focusVisible: false
    property bool floating: false
    property real cornerRadius: HnMetrics.internalSpacing(HnControlSize.Compact)

    color: root.pressed ? HoloniightPalette.surface : root.hovered ? HoloniightPalette.surfaceHover : root.floating ? Qt.alpha(HoloniightPalette.surface, 0.85) : "transparent"
    border.width: root.focusVisible ? HnMetrics.focusBorderWidth : root.floating ? HnMetrics.borderWidth : 0
    border.color: root.focusVisible ? HoloniightPalette.borderFocus : root.floating ? HoloniightPalette.borderPassive : "transparent"
    radius: root.cornerRadius
    Accessible.ignored: true
}
