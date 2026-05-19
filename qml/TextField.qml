// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

import QtQuick
import QtQuick.Templates as T
import Holonight

T.TextField {
    id: control

    implicitWidth: implicitBackgroundWidth + leftInset + rightInset
                   || Math.ceil(Math.max(contentWidth, placeholder.implicitWidth)) + leftPadding + rightPadding
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             contentHeight + topPadding + bottomPadding,
                             placeholder.implicitHeight + topPadding + bottomPadding)

    leftPadding: 8
    rightPadding: 8
    topPadding: 6
    bottomPadding: 6

    property bool hasError: false

    color: HoloniightPalette.onSurface
    selectionColor: Qt.rgba(HoloniightPalette.primary.r, HoloniightPalette.primary.g,
                            HoloniightPalette.primary.b, 0.3)
    selectedTextColor: HoloniightPalette.onPrimary
    placeholderTextColor: HoloniightPalette.onSurfaceVariant

    Text {
        id: placeholder
        x: control.leftPadding
        y: control.topPadding
        width: control.width - (control.leftPadding + control.rightPadding)
        height: control.height - (control.topPadding + control.bottomPadding)

        text: control.placeholderText
        font: control.font
        color: control.placeholderTextColor
        verticalAlignment: control.verticalAlignment
        visible: !control.length && !control.preeditText && (!control.activeFocus || control.horizontalAlignment !== Qt.AlignHCenter)
        elide: Text.ElideRight
        renderType: control.renderType
    }

    background: Rectangle {
        implicitWidth: 200
        implicitHeight: 32

        color: HoloniightPalette.surface
        radius: 4

        border.width: (control.activeFocus || control.hasError) ? 2 : 1
        border.color: {
            if (control.hasError)       return HoloniightPalette.borderUrgent
            if (control.activeFocus)    return HoloniightPalette.borderFocus
            return HoloniightPalette.borderPassive
        }

        opacity: control.enabled ? 1.0 : 0.5

        Behavior on border.color { ColorAnimation { duration: 80 } }
    }
}
