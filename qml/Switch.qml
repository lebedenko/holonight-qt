// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

import QtQuick
import QtQuick.Templates as T
import Holonight.Core

T.Switch {
    id: root

    font.family: HolonightTheme.uiFont
    font.pointSize: HolonightTheme.bodySize

    property int sizeRole: HnControlSize.Normal
    readonly property int resolvedSizeRole: HnMetrics.normalizedSizeRole(root.sizeRole)

    readonly property real indicatorTrackWidth: {
        switch (root.resolvedSizeRole) {
        case HnControlSize.Compact: return 28
        case HnControlSize.Large: return 44
        case HnControlSize.Hero: return 56
        default: return 34
        }
    }
    readonly property real indicatorTrackHeight: {
        switch (root.resolvedSizeRole) {
        case HnControlSize.Compact: return 14
        case HnControlSize.Large: return 22
        case HnControlSize.Hero: return 28
        default: return 16
        }
    }
    readonly property real indicatorThumbSize: {
        switch (root.resolvedSizeRole) {
        case HnControlSize.Compact: return 10
        case HnControlSize.Large: return 16
        case HnControlSize.Hero: return 22
        default: return 12
        }
    }
    readonly property real indicatorThumbMargin: {
        switch (root.resolvedSizeRole) {
        case HnControlSize.Large:
        case HnControlSize.Hero:
            return 3
        default:
            return 2
        }
    }

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    padding: {
        switch (root.resolvedSizeRole) {
        case HnControlSize.Compact: return 3
        case HnControlSize.Large: return 6
        case HnControlSize.Hero: return 8
        default: return 4
        }
    }
    spacing: HnMetrics.internalSpacing(root.resolvedSizeRole)

    indicator: Item {
        implicitWidth: root.indicatorTrackWidth
        implicitHeight: root.indicatorTrackHeight
        x: root.leftPadding
        anchors.verticalCenter: parent.verticalCenter

        // Track
        Rectangle {
            anchors.fill: parent
            radius: height / 2
            color: {
                if (!root.enabled)  return HoloniightPalette.surface
                if (root.checked)   return root.hovered ? HoloniightPalette.primaryHover : HoloniightPalette.primary
                return root.hovered ? HoloniightPalette.textMuted : HoloniightPalette.borderPassive
            }

            Behavior on color { ColorAnimation { duration: 150 } }
        }

        // Thumb
        Rectangle {
            width: root.indicatorThumbSize
            height: root.indicatorThumbSize
            radius: height / 2
            anchors.verticalCenter: parent.verticalCenter
            x: root.checked ? parent.width - width - root.indicatorThumbMargin : root.indicatorThumbMargin

            color: {
                if (!root.enabled) return HoloniightPalette.textDisabled
                return root.checked ? HoloniightPalette.onPrimary : HoloniightPalette.textMuted
            }

            Behavior on x { NumberAnimation { duration: 150; easing.type: Easing.InOutQuad } }
            Behavior on color { ColorAnimation { duration: 150 } }

            // Focus ring
            Rectangle {
                visible: root.visualFocus
                anchors.centerIn: parent
                width: parent.width + 4
                height: parent.height + 4
                radius: (parent.width + 4) / 2
                color: Qt.rgba(0, 0, 0, 0)
                border.color: HoloniightPalette.borderFocus
                border.width: HnMetrics.borderWidth
            }
        }
    }

    contentItem: Text {
        leftPadding: root.indicator.width + root.spacing
        text: root.text
        font: root.font
        color: root.enabled ? HoloniightPalette.textPrimary : HoloniightPalette.textDisabled
        verticalAlignment: Text.AlignVCenter
        textFormat: Text.PlainText
    }
}
