// SPDX-License-Identifier: GPL-3.0-or-later

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as C
import QtQuick.Layouts
import Holonight.Core

C.Control {
    id: root
    property alias text: hint.text
    property alias keyGroups: hint.keyGroups
    property alias wrap: hint.wrap
    property alias lowercaseLetters: hint.lowercaseLetters
    property string description
    property real hintSpacing: HnMetrics.internalSpacing(HnControlSize.Compact)
    readonly property alias hintItem: hint
    readonly property alias descriptionItem: descriptionLabel
    readonly property string accessibleText: hint.accessibleText + (description.length ? " " + description : "")
    implicitWidth: implicitContentWidth + leftPadding + rightPadding
    implicitHeight: implicitContentHeight + topPadding + bottomPadding
    spacing: hintSpacing
    Accessible.role: Accessible.StaticText
    Accessible.name: accessibleText
    contentItem: RowLayout {
        spacing: root.hintSpacing
        HnKeyHint {
            id: hint
            font: {
                const result = Qt.font(descriptionLabel.font);
                result.family = HolonightTheme.monospaceFont;
                return result;
            }
            objectName: root.objectName + "Keycap"
            Accessible.ignored: true
        }
        HnLabel {
            id: descriptionLabel
            objectName: root.objectName + "Label"
            role: HnTypographyRole.Caption
            rawText: root.description
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
            Accessible.ignored: true
        }
    }
}
