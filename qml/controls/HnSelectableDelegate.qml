// SPDX-License-Identifier: GPL-3.0-or-later

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Templates as T
import Holonight.Core

T.ItemDelegate {
    id: root

    enum SelectionStyle {
        Fill,
        AccentEdge,
        Outline
    }

    property int sizeRole: HnControlSize.Normal
    property int selectionStyle: HnSelectableDelegate.Fill
    readonly property int resolvedSizeRole: HnControlMetrics.normalizedSizeRole(root.sizeRole)
    readonly property bool selected: root.highlighted || root.checked
                                     || (root.ListView ? root.ListView.isCurrentItem : false)
    readonly property real semanticSpacing: HnControlMetrics.internalSpacing(root.resolvedSizeRole)

    implicitWidth: Math.max(160, implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(HnControlMetrics.controlHeight(root.resolvedSizeRole),
                             implicitContentHeight + topPadding + bottomPadding)
    padding: HnControlMetrics.horizontalPadding(root.resolvedSizeRole)
    spacing: root.semanticSpacing
    hoverEnabled: true

    background: Item {
        readonly property real semanticRadius: HnAppearance.roundedRadius(HnSurfaceRole.Control, width, height,
                                                                          HnAppearance.revision)

        Rectangle {
            objectName: "hnSelectableDelegateBase"
            anchors.fill: parent
            color: root.down ? HoloniightPalette.surfaceElevated
                             : (!root.selected && root.hovered ? HoloniightPalette.surfaceHover : "transparent")
            radius: parent.semanticRadius
            Accessible.ignored: true
        }
        Rectangle {
            id: selectedOverlay

            objectName: "hnSelectableDelegateSelectedOverlay"
            anchors.fill: parent
            color: root.hovered ? HoloniightPalette.surfaceSelectedHover
                                : HoloniightPalette.surfaceSelected
            gradient: root.selectionStyle === HnSelectableDelegate.AccentEdge ? selectionGradient : null
            radius: parent.semanticRadius
            visible: root.selected && !root.down
            Accessible.ignored: true
            readonly property real accentWidth: Math.max(2, HoloniightPalette.borderWidth * 2)
            readonly property real accentStop: width > 0 ? Math.min(1, accentWidth / width) : 0
            readonly property real hardStopOffset: 0.25 / Math.max(1, width)

            Gradient {
                id: selectionGradient

                objectName: "hnSelectableDelegateSelectionGradient"
                orientation: Gradient.Horizontal

                GradientStop {
                    objectName: "hnSelectableDelegateAccentStartStop"
                    position: 0
                    color: HoloniightPalette.selectionIndicator
                }
                GradientStop {
                    objectName: "hnSelectableDelegateAccentEndStop"
                    position: selectedOverlay.accentStop
                    color: HoloniightPalette.selectionIndicator
                }
                GradientStop {
                    objectName: "hnSelectableDelegateSurfaceStartStop"
                    position: Math.min(1, selectedOverlay.accentStop + selectedOverlay.hardStopOffset)
                    color: selectedOverlay.color
                }
                GradientStop {
                    objectName: "hnSelectableDelegateSurfaceEndStop"
                    position: 1
                    color: selectedOverlay.color
                }
            }
        }
        Rectangle {
            anchors.fill: parent
            color: "transparent"
            radius: parent.semanticRadius
            border.width: root.visualFocus
                          || (root.selected && root.selectionStyle === HnSelectableDelegate.Outline)
                          ? HoloniightPalette.focusBorderWidth : 0
            border.color: root.visualFocus ? HoloniightPalette.borderFocus
                                           : HoloniightPalette.selectionIndicator
            Accessible.ignored: true
        }
    }
}
