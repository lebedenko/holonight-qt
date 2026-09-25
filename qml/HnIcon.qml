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

    enum Rendering {
        Original = 0,
        Semantic = 1
    }

    property string name: ""
    property url source: ""

    property int size: 24
    property int iconState: HnIcon.Normal
    property int rendering: HnIcon.Semantic

    property color normalColor: HoloniightPalette.textSecondary
    property color mutedColor: HoloniightPalette.textMuted
    property color disabledColor: HoloniightPalette.textDisabled
    property color activeColor: HoloniightPalette.primary

    readonly property bool _invalidInput: (root.name.length > 0 && String(root.source).length > 0)
                                          || root.name.indexOf("/") >= 0 || root.name.indexOf(":") >= 0
                                          || (root.rendering !== HnIcon.Original && root.rendering !== HnIcon.Semantic)
                                          || (root.rendering === HnIcon.Semantic
                                              && String(root.source).startsWith("image://"))
    readonly property bool hasError: root._invalidInput || iconImage.status === Image.Error
    readonly property color resolvedColor: {
        switch (root.iconState) {
        case HnIcon.Muted: return root.mutedColor
        case HnIcon.Disabled: return root.disabledColor
        case HnIcon.Active: return root.activeColor
        default: return root.normalColor
        }
    }

    readonly property url _renderSource: {
        if (root._invalidInput)
            return ""
        const input = root.name.length > 0 ? root.name : root.source
        if (String(input).length === 0)
            return ""
        if (root.name.length === 0 && root.rendering === HnIcon.Original)
            return root.source
        return HnIconProvider.sourceUrl(input, root.size, root.resolvedColor,
                                        HoloniightPalette.primary, HoloniightPalette.success,
                                        HoloniightPalette.warning, HoloniightPalette.error,
                                        HoloniightPalette.revision,
                                        root.rendering === HnIcon.Semantic)
    }

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
