// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

import QtQuick
import QtQuick.Templates as T
import Holonight

T.TextField {
    id: root

    property bool hasError: false

    implicitWidth: implicitBackgroundWidth + leftInset + rightInset
                   || Math.ceil(Math.max(contentWidth, placeholder.implicitWidth)) + leftPadding + rightPadding
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             contentHeight + topPadding + bottomPadding,
                             placeholder.implicitHeight + topPadding + bottomPadding)

    leftPadding: 8
    rightPadding: 8
    topPadding: 6
    bottomPadding: 6

    color: root.enabled ? HoloniightPalette.textPrimary : HoloniightPalette.textDisabled
    selectionColor: Qt.rgba(HoloniightPalette.primary.r, HoloniightPalette.primary.g,
                            HoloniightPalette.primary.b, 0.3)
    selectedTextColor: HoloniightPalette.onPrimary
    placeholderTextColor: HoloniightPalette.textMuted

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
        implicitWidth: 200
        implicitHeight: 32

        color: HoloniightPalette.surface
        radius: HoloniightPalette.radiusControl

        border.width: (root.activeFocus || root.hasError) ? HoloniightPalette.focusBorderWidth : HoloniightPalette.borderWidth
        border.color: {
            if (root.hasError)       return HoloniightPalette.borderUrgent
            if (root.activeFocus)    return HoloniightPalette.borderFocus
            return HoloniightPalette.borderPassive
        }

        opacity: root.enabled ? 1.0 : 0.5

        Behavior on border.color { ColorAnimation { duration: 80 } }
    }
}
