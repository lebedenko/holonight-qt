// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic
import Holonight.Core

FocusScope {
    id: root

    default property Component control
    property string labelText
    property string helperText
    property string errorText
    property bool required: false
    property bool hasError: false
    readonly property alias controlItem: controlLoader.item

    implicitWidth: content.implicitWidth
    implicitHeight: content.implicitHeight
    activeFocusOnTab: controlLoader.item !== null
    onActiveFocusChanged: {
        if (root.activeFocus && controlLoader.status === Loader.Ready)
            controlLoader.loadedControl.forceActiveFocus()
    }

    ColumnLayout {
        id: content

        anchors.fill: parent
        spacing: 6

        Row {
            spacing: 3
            visible: root.labelText.length > 0 || root.required
            Layout.fillWidth: true

            HnLabel {
                role: HnTypographyRole.Body
                rawText: root.labelText
                color: HoloniightPalette.textPrimary
            }

            HnLabel {
                role: HnTypographyRole.Body
                rawText: qsTr("*", "required form field marker")
                color: HoloniightPalette.borderUrgent
                visible: root.required
                Accessible.ignored: true
            }
        }

        Loader {
            id: controlLoader

            readonly property Item loadedControl: item as Item

            sourceComponent: root.control
            Layout.fillWidth: true
        }

        HnLabel {
            readonly property string supportingText: root.hasError ? root.errorText : root.helperText

            role: HnTypographyRole.Caption
            rawText: supportingText
            color: root.hasError ? HoloniightPalette.borderUrgent : HoloniightPalette.textMuted
            wrapMode: Text.WordWrap
            visible: supportingText.length > 0
            Layout.fillWidth: true
        }
    }
}
