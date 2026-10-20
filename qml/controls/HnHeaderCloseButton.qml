// SPDX-License-Identifier: GPL-3.0-or-later

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as C
import Holonight.Core

C.Button {
    id: root
    property real cornerRadius: HnMetrics.internalSpacing(HnControlSize.Compact)
    display: C.AbstractButton.IconOnly
    padding: 2
    horizontalPadding: 2
    icon.name: "window-close"
    icon.width: 24
    icon.height: 24
    icon.color: !enabled ? HoloniightPalette.textDisabled : hovered || down ? HoloniightPalette.onError : HoloniightPalette.textPrimary
    implicitWidth: HnMetrics.controlHeight(HnControlSize.Compact)
    implicitHeight: HnMetrics.controlHeight(HnControlSize.Compact)
    Accessible.name: qsTr("Close")
    Keys.onPressed: event => {
        if ((event.key === Qt.Key_Return || event.key === Qt.Key_Enter) && !event.isAutoRepeat) {
            root.click();
            event.accepted = true;
        }
    }
    background: HnButtonSurface {
        pressed: root.down
        hovered: root.hovered
        focusVisible: root.visualFocus
        cornerRadius: root.cornerRadius
        color: root.enabled && (pressed || hovered) ? HoloniightPalette.error : "transparent"
    }
}
