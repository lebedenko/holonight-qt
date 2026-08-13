// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as C
import QtQuick.Shapes
import QtQuick.Templates as T
import Holonight
import Holonight.Core

T.ComboBox {
    id: root

    font.family: HolonightTheme.uiFont
    font.pointSize: HolonightTheme.bodySize

    property int maximumVisibleItems: 8
    property real delegateHeight: 28

    readonly property int resolvedMaximumVisibleItems: Math.max(1, maximumVisibleItems)
    readonly property point sceneOrigin: mapToItem(null, popup.geometryRevision * 0, 0)
    readonly property point sceneXAxis: mapToItem(null, 1 + popup.geometryRevision * 0, 0)
    readonly property point sceneYAxis: mapToItem(null, popup.geometryRevision * 0, 1)
    readonly property real sceneScaleX: Math.hypot(sceneXAxis.x - sceneOrigin.x,
                                                   sceneXAxis.y - sceneOrigin.y)
    readonly property real sceneScaleY: Math.hypot(sceneYAxis.x - sceneOrigin.x,
                                                   sceneYAxis.y - sceneOrigin.y)
    readonly property real sceneAxisDot: (sceneXAxis.x - sceneOrigin.x) * (sceneYAxis.x - sceneOrigin.x)
                                         + (sceneXAxis.y - sceneOrigin.y) * (sceneYAxis.y - sceneOrigin.y)
    readonly property bool popupTransformSupported: sceneScaleX > 0
                                                    && sceneScaleY > 0
                                                    && Math.abs(sceneScaleX - sceneScaleY) < 0.001
                                                    && Math.abs(sceneAxisDot) < 0.001
                                                    && Math.abs(sceneXAxis.y - sceneOrigin.y) < 0.001
                                                    && Math.abs(sceneYAxis.x - sceneOrigin.x) < 0.001
    readonly property real effectiveScale: popupTransformSupported ? sceneScaleX : 1

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    leftPadding: 8
    rightPadding: 28
    topPadding: 6
    bottomPadding: 6

    delegate: comboBoxDelegate

    indicator: Shape {
        x: root.width - width - 8
        y: (root.height - height) / 2
        width: 12
        height: 12
        preferredRendererType: Shape.CurveRenderer

        ShapePath {
            strokeColor: root.enabled ? HoloniightPalette.textPrimary : HoloniightPalette.textDisabled
            strokeWidth: 1.5
            fillColor: "transparent"
            capStyle: ShapePath.RoundCap
            joinStyle: ShapePath.RoundJoin

            PathPolyline {
                path: [
                    Qt.point(2, 4),
                    Qt.point(6, 8),
                    Qt.point(10, 4)
                ]
            }
        }
    }

    contentItem: Text {
        leftPadding: 0
        rightPadding: root.indicator.width + 4
        text: root.displayText
        font: root.font
        color: root.enabled ? HoloniightPalette.textPrimary : HoloniightPalette.textDisabled
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
        textFormat: Text.PlainText
    }

    background: Rectangle {
        readonly property real semanticRadius: HnAppearance.roundedRadius(HnSurfaceRole.Control,
                                                                          width, height,
                                                                          HnAppearance.revision)

        implicitWidth: 120
        implicitHeight: 32
        radius: semanticRadius
        color: root.hovered ? HoloniightPalette.surfaceHover : HoloniightPalette.surface
        border.color: root.visualFocus || root.popup.visible ? HoloniightPalette.borderFocus : (root.enabled ? HoloniightPalette.borderPassive : HoloniightPalette.borderPassive)
        border.width: (root.visualFocus || root.popup.visible) ? HnMetrics.focusBorderWidth : HnMetrics.borderWidth
    }

    popup: T.Popup {
        id: popup

        property int geometryRevision: 0

        readonly property point controlSceneBottom: root.mapToItem(null, geometryRevision * 0,
                                                                   root.height)
        readonly property real controlSceneTop: Math.min(root.sceneOrigin.y, controlSceneBottom.y)
        readonly property real controlSceneBottomY: Math.max(root.sceneOrigin.y, controlSceneBottom.y)
        readonly property real sceneSpaceAbove: Math.max(0, controlSceneTop
                                                         - margins - 2 * root.effectiveScale)
        readonly property real sceneSpaceBelow: root.Window.window
                                                ? Math.max(0, root.Window.window.height
                                                           - controlSceneBottomY
                                                           - margins - 2 * root.effectiveScale)
                                                : naturalHeight * root.effectiveScale
        readonly property real sceneAvailableVerticalSpace: Math.max(sceneSpaceAbove, sceneSpaceBelow)
        readonly property real availableVerticalSpace: sceneAvailableVerticalSpace / root.effectiveScale
        readonly property real naturalHeight: contentItem.implicitHeight + topPadding + bottomPadding
        readonly property real renderedDemand: naturalHeight * root.effectiveScale
        readonly property real spaceAbove: sceneSpaceAbove / root.effectiveScale
        readonly property real spaceBelow: root.Window.window
                                           ? sceneSpaceBelow / root.effectiveScale
                                           : naturalHeight
        readonly property bool opensAbove: sceneSpaceBelow < renderedDemand
                                           && sceneSpaceAbove > sceneSpaceBelow

        objectName: "holonightComboBoxPopup"
        parent: C.Overlay.overlay
        popupType: T.Popup.Item
        x: root.sceneOrigin.x
        y: opensAbove ? controlSceneTop - 2 * root.effectiveScale - implicitHeight
                      : controlSceneBottomY + 2 * root.effectiveScale
        width: root.width
        implicitHeight: Math.min(naturalHeight, opensAbove ? spaceAbove : spaceBelow)
        margins: 4 * root.effectiveScale
        padding: 4
        scale: root.effectiveScale
        transformOrigin: opensAbove ? Item.BottomLeft : Item.TopLeft

        onAboutToShow: geometryRevision++

        contentItem: ListView {
            id: popupList

            readonly property real averageItemHeight: count > 0 ? contentHeight / count : 0
            readonly property real itemLimitedHeight: Math.min(contentHeight,
                                                               averageItemHeight
                                                               * root.resolvedMaximumVisibleItems)

            objectName: "holonightComboBoxPopupList"
            clip: contentHeight > height
            implicitHeight: itemLimitedHeight
            interactive: contentHeight > height
            model: root.delegateModel
            currentIndex: root.highlightedIndex
            onCurrentIndexChanged: positionViewAtIndex(currentIndex, ListView.Contain)
            ScrollBar.vertical: ScrollBar {
                policy: popupList.contentHeight > popupList.height
                        ? ScrollBar.AlwaysOn
                        : ScrollBar.AlwaysOff
            }
        }

        onOpened: popupList.positionViewAtIndex(root.highlightedIndex, ListView.Contain)

        background: Rectangle {
            readonly property real semanticRadius: HnAppearance.roundedRadius(HnSurfaceRole.Control,
                                                                              width, height,
                                                                              HnAppearance.revision)

            color: HoloniightPalette.surface
            border.color: HoloniightPalette.borderPassive
            border.width: HnMetrics.borderWidth
            radius: semanticRadius
        }
    }

    Component {
        id: comboBoxDelegate

        ItemDelegate {
            required property int index

            readonly property real popupInnerRadius: Math.max(0,
                                                              root.popup.background.semanticRadius
                                                              - root.popup.padding)

            width: root.popup.availableWidth
            height: root.delegateHeight
            text: root.textAt(index)
            highlighted: root.highlightedIndex === index
            topLeftRadius: index === 0 ? popupInnerRadius : 0
            topRightRadius: index === 0 ? popupInnerRadius : 0
            bottomLeftRadius: index === root.count - 1 ? popupInnerRadius : 0
            bottomRightRadius: index === root.count - 1 ? popupInnerRadius : 0
        }
    }
}
