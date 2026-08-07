// SPDX-License-Identifier: GPL-3.0-or-later

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls.Basic
import Holonight.Core

HnListDelegate {
    id: root

    property string description
    property url iconSource
    property bool showChevron: true
    subtitle: description
    leadingContent: iconSource.toString().length > 0 ? iconComponent : null
    trailingContent: showChevron ? chevronComponent : null

    Component {
        id: iconComponent
        HnIcon {
            source: root.iconSource
            size: HnMetrics.iconSize(root.resolvedSizeRole)
        }
    }
    Component {
        id: chevronComponent
        HnLabel {
            role: HnTypographyRole.Body
            rawText: qsTr("›", "action navigation chevron")
            color: HoloniightPalette.textMuted
            Accessible.ignored: true
        }
    }
}
