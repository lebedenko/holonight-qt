// SPDX-License-Identifier: GPL-3.0-or-later

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Templates as T
import Holonight.Core

T.AbstractButton {
    id: root

    property int sizeRole: HnControlSize.Normal
    property int iconState: root.enabled ? (root.down ? HnIcon.Active : HnIcon.Normal) : HnIcon.Disabled
    readonly property int resolvedSizeRole: HnMetrics.normalizedSizeRole(root.sizeRole)

    implicitWidth: HnMetrics.controlHeight(root.resolvedSizeRole)
    implicitHeight: HnMetrics.controlHeight(root.resolvedSizeRole)
    hoverEnabled: true
    Accessible.role: Accessible.Button

    contentItem: Item {
        HnIcon {
            objectName: "hnIconButtonIcon"
            anchors.centerIn: parent
            source: root.icon.source
            size: HnMetrics.iconSize(root.resolvedSizeRole)
            iconState: root.iconState
            normalColor: root.icon.color.a > 0 ? root.icon.color : HoloniightPalette.textSecondary
            mutedColor: root.icon.color.a > 0 ? root.icon.color : HoloniightPalette.textMuted
            disabledColor: HoloniightPalette.textDisabled
            activeColor: root.icon.color.a > 0 ? root.icon.color : HoloniightPalette.primary
        }
    }

    background: Rectangle {
        readonly property real semanticRadius: HnAppearance.roundedRadius(HnSurfaceRole.Control, width, height,
                                                                          HnAppearance.revision)

        color: {
            if (!root.enabled)
                return "transparent"
            if (root.down)
                return HoloniightPalette.surface
            if (root.hovered)
                return HoloniightPalette.surfaceHover
            return "transparent"
        }
        radius: semanticRadius
        border.color: root.visualFocus ? HoloniightPalette.borderFocus : "transparent"
        border.width: root.visualFocus ? HnMetrics.focusBorderWidth : 0

    }
}
