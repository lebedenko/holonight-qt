// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

import QtQuick
import QtQuick.Templates as T
import Holonight.Core
import Holonight.impl as Impl

T.TextField {
    id: root

    readonly property Impl.ControlPalette controlColors: Impl.ControlPalette {
        palette: root.palette
        colorGroup: !root.enabled ? Impl.ControlPalette.Disabled
                                 : (root.Window.window && !root.Window.window.active
                                    ? Impl.ControlPalette.Inactive : Impl.ControlPalette.Active)
        textRole: Impl.ControlPalette.Text
        fillRole: Impl.ControlPalette.Base
    }

    font.family: HolonightTheme.uiFont
    font.pointSize: HolonightTheme.bodySize

    property bool hasError: false

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            Math.ceil(Math.max(contentWidth, placeholder.implicitWidth))
                            + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             contentHeight + topPadding + bottomPadding,
                             placeholder.implicitHeight + topPadding + bottomPadding)

    leftPadding: 8
    rightPadding: 8
    topPadding: 6
    bottomPadding: 6
    verticalAlignment: TextInput.AlignVCenter

    color: root.enabled ? root.controlColors.colors.textPrimary : root.controlColors.colors.textDisabled
    selectionColor: root.controlColors.colors.selection
    selectedTextColor: root.controlColors.colors.onPrimary
    placeholderTextColor: root.controlColors.colors.textMuted

    Text {
        id: placeholder
        x: root.leftPadding
        y: root.topPadding
        width: root.width - (root.leftPadding + root.rightPadding)
        height: root.height - (root.topPadding + root.bottomPadding)

        text: root.placeholderText
        font: root.font
        color: root.placeholderTextColor
        verticalAlignment: root.verticalAlignment
        visible: root.length < 1 && root.preeditText.length < 1
                 && (root.horizontalAlignment === Qt.AlignHCenter ? root.activeFocus === false : true)
        elide: Text.ElideRight
        renderType: root.renderType
        textFormat: Text.PlainText
    }

    background: Rectangle {
        readonly property real semanticRadius: HnAppearance.roundedRadius(HnSurfaceRole.Control,
                                                                          width, height,
                                                                          HnAppearance.revision)

        implicitWidth: 200
        implicitHeight: 32

        color: root.controlColors.colors.surface
        radius: semanticRadius

        border.width: (root.activeFocus || root.hasError) ? HnMetrics.focusBorderWidth : HnMetrics.borderWidth
        border.color: {
            if (root.hasError)       return root.controlColors.colors.borderUrgent
            if (root.activeFocus)    return root.controlColors.colors.borderFocus
            return root.controlColors.colors.borderPassive
        }

        opacity: root.enabled || !root.controlColors.defaultDisabled ? 1.0 : 0.5

        Behavior on border.color { ColorAnimation { duration: 80 } }
    }
}
