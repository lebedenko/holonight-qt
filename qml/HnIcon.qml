pragma ComponentBehavior: Bound

import QtQuick
import Holonight.Core

Item {
    id: root

    enum IconState {
        Normal = 0,
        Muted = 1,
        Disabled = 2,
        Active = 3,
        Selected = 4
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

    property var paletteContext: null
    property color accentColor: iconPalette.colors.Accent ?? HoloniightPalette.primary
    property color backgroundColor: iconPalette.colors.Background ?? HoloniightPalette.background
    property color highlightColor: iconPalette.colors.Highlight ?? HoloniightPalette.primary
    property color highlightedTextColor: iconPalette.colors.HighlightedText ?? HoloniightPalette.onPrimary
    property color positiveColor: HoloniightPalette.success
    property color neutralColor: HoloniightPalette.warning
    property color negativeColor: HoloniightPalette.error

    property var disabledAccentColor: iconPalette.colors.disabledExplicitAccent ? iconPalette.colors.disabledAccent : undefined
    property color disabledBackgroundColor: iconPalette.colors.disabledBackground ?? backgroundColor
    property color disabledHighlightColor: iconPalette.colors.disabledHighlight ?? highlightColor
    property color disabledHighlightedTextColor: iconPalette.colors.disabledHighlightedText ?? highlightedTextColor
    property var disabledPositiveColor
    property var disabledNeutralColor
    property var disabledNegativeColor

    HnIconPalette {
        id: iconPalette
        palette: root.paletteContext
    }

    readonly property bool hasContextForeground: iconPalette.colors.ExplicitText ?? false
    readonly property color contextForeground: iconPalette.colors.Text ?? HoloniightPalette.textSecondary

    property color normalColor: root.hasContextForeground ? root.contextForeground : HoloniightPalette.textSecondary
    property color mutedColor: iconPalette.colors.ExplicitText ? iconPalette.colors.Text : HoloniightPalette.textMuted
    property color disabledColor: iconPalette.colors.disabledExplicitText ? iconPalette.colors.disabledText : HoloniightPalette.textDisabled
    property color activeColor: iconPalette.colors.ExplicitText ? iconPalette.colors.Text : HoloniightPalette.primary

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
        return HnIconProvider.sourceUrlWithOptions(input, root.size, {
            color: root.resolvedColor, highlight: root.highlightColor,
            positive: root.positiveColor, neutral: root.neutralColor, negative: root.negativeColor,
            accent: root.accentColor, background: root.backgroundColor,
            highlightedText: root.highlightedTextColor,
            disabledColor: root.disabledColor,
            disabledAccent: root.disabledAccentColor,
            disabledBackground: root.disabledBackgroundColor,
            disabledHighlight: root.disabledHighlightColor,
            disabledHighlightedText: root.disabledHighlightedTextColor,
            disabledPositive: root.disabledPositiveColor, disabledNeutral: root.disabledNeutralColor,
            disabledNegative: root.disabledNegativeColor,
            state: root.iconState, revision: HoloniightPalette.revision,
            dpr: root.Screen.devicePixelRatio,
            semantic: root.rendering === HnIcon.Semantic
        })
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
