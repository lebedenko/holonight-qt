// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as C
import Holonight.Core

C.Button {
    id: root
    display: C.AbstractButton.IconOnly
    icon.source: "assets/close.svg"
    icon.width: HnMetrics.iconSize(HnControlSize.Normal)
    icon.height: HnMetrics.iconSize(HnControlSize.Normal)
    implicitWidth: HnMetrics.controlHeight(HnControlSize.Normal)
    implicitHeight: HnMetrics.controlHeight(HnControlSize.Normal)
    background: HnButtonSurface {
        pressed: root.down
        hovered: root.hovered
        focusVisible: root.visualFocus
    }
}
