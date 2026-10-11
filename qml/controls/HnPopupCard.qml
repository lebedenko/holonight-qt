// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as C
import Holonight.Core

C.Popup {
    id: root
    property real maximumWidth: 480
    property real viewportMargin: 12
    property real dimmerOpacity: 0.22
    property string dimmerObjectName: ""

    parent: C.Overlay.overlay
    anchors.centerIn: parent
    width: Math.max(0, Math.min(root.maximumWidth, (parent ? parent.width : 0) - 2 * root.viewportMargin))
    height: Math.max(0, Math.min(implicitHeight, (parent ? parent.height : 0) - 2 * root.viewportMargin))
    padding: 16
    modal: true
    focus: true
    closePolicy: C.Popup.CloseOnEscape | C.Popup.CloseOnPressOutside
    C.Overlay.modal: Rectangle {
        objectName: root.dimmerObjectName
        color: Qt.alpha(HoloniightPalette.shadow, root.dimmerOpacity)
    }
    background: Rectangle {
        color: HoloniightPalette.surfaceRaised
        border.width: HnMetrics.borderWidth
        border.color: HoloniightPalette.borderPassive
        radius: HnMetrics.internalSpacing(HnControlSize.Compact)
        Accessible.ignored: true
    }
}
