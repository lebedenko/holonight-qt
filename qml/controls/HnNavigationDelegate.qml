// SPDX-License-Identifier: GPL-3.0-or-later

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import Holonight.Core

HnSelectableDelegate {
    id: root

    property string title: text
    property string badgeText
    property bool showTitleToolTipWhenElided: true
    property Component leadingContent
    property Component trailingContent
    readonly property alias leadingItem: leadingLoader.item
    readonly property alias trailingItem: trailingLoader.item

    selectionStyle: HnSelectableDelegate.AccentEdge
    Accessible.name: root.title

    contentItem: RowLayout {
        spacing: root.semanticSpacing

        Loader {
            id: leadingLoader
            active: root.leadingContent !== null
            visible: active
            sourceComponent: root.leadingContent
        }
        HnLabel {
            id: titleLabel

            objectName: "hnNavigationDelegateTitle"
            role: HnTypographyRole.Body
            rawText: root.title
            color: root.enabled ? HoloniightPalette.textPrimary : HoloniightPalette.textDisabled
            elide: Text.ElideRight
            Layout.fillWidth: true
        }
        HnLabel {
            role: HnTypographyRole.Caption
            rawText: root.badgeText
            color: HoloniightPalette.textSecondary
            visible: text.length > 0
        }
        Loader {
            id: trailingLoader
            active: root.trailingContent !== null
            visible: active
            sourceComponent: root.trailingContent
        }
    }

    ToolTip.text: root.title
    ToolTip.delay: 500
    ToolTip.visible: root.showTitleToolTipWhenElided && root.hovered
                     && root.title.length > 0 && titleLabel.truncated
}
