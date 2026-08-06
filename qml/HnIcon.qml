pragma ComponentBehavior: Bound

import QtQuick
import Holonight.Core

Item {
    id: root

    enum IconState {
        Normal = 0,
        Muted = 1,
        Disabled = 2,
        Active = 3
    }

    required property url source

    property int size: 24
    property int iconState: HnIcon.Normal
    property bool tinted: true

    property color normalColor: HoloniightPalette.textSecondary
    property color mutedColor: HoloniightPalette.textMuted
    property color disabledColor: HoloniightPalette.textDisabled
    property color activeColor: HoloniightPalette.primary

    readonly property bool hasError: iconImage.status === Image.Error
    readonly property color resolvedColor: {
        switch (root.iconState) {
        case HnIcon.Muted: return root.mutedColor
        case HnIcon.Disabled: return root.disabledColor
        case HnIcon.Active: return root.activeColor
        default: return root.normalColor
        }
    }

    readonly property string _sourceString: String(root.source)
    readonly property bool _usesShellIconProvider: root._sourceString.indexOf("image://icon/") === 0
    readonly property url _renderSource: root.tinted && !root._usesShellIconProvider
                                      ? HnIconProvider.sourceUrl(root.source, root.size,
                                                                 root.resolvedColor,
                                                                 HoloniightPalette.primary,
                                                                 HoloniightPalette.success,
                                                                 HoloniightPalette.warning,
                                                                 HoloniightPalette.error,
                                                                 HoloniightPalette.revision)
                                      : root.source

    implicitWidth: root.size
    implicitHeight: root.size

    Accessible.ignored: true

    Image {
        id: iconImage

        anchors.fill: parent
        source: root._renderSource
        sourceSize.width: root.size
        sourceSize.height: root.size
        smooth: true
        visible: !root.hasError
    }
}
