// SPDX-License-Identifier: GPL-3.0-or-later

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import Holonight.Core

Control {
    id: root

    enum Status {
        Neutral,
        Info,
        Success,
        Warning,
        Error
    }

    property int status: HnStatusIndicator.Neutral
    property string text
    property url iconSource
    property bool dotVisible: iconSource.toString().length === 0
    readonly property color statusColor: {
        switch (root.status) {
        case HnStatusIndicator.Info: return HoloniightPalette.accentCyan
        case HnStatusIndicator.Success: return HoloniightPalette.success
        case HnStatusIndicator.Warning: return HoloniightPalette.warning
        case HnStatusIndicator.Error: return HoloniightPalette.error
        default: return HoloniightPalette.textMuted
        }
    }

    padding: 4
    Accessible.role: Accessible.StaticText
    Accessible.name: root.text

    contentItem: RowLayout {
        spacing: 4
        Rectangle {
            color: root.statusColor
            radius: width / 2
            visible: root.dotVisible
            Layout.preferredWidth: 7
            Layout.preferredHeight: 7
            Accessible.ignored: true
        }
        HnIcon {
            source: root.iconSource
            size: 14
            normalColor: root.statusColor
            visible: !root.dotVisible
        }
        HnLabel {
            objectName: "hnStatusIndicatorText"
            role: HnTypographyRole.Caption
            rawText: root.text
            color: root.enabled ? root.statusColor : HoloniightPalette.textDisabled
            visible: text.length > 0
        }
    }
}
