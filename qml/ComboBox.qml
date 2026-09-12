// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as C
import QtQuick.Shapes
import QtQuick.Templates as T
import Holonight
import Holonight.Core
import Holonight.impl as Impl

T.ComboBox {
    id: root

    readonly property Impl.ControlPalette controlColors: Impl.ControlPalette {
        palette: root.palette
        colorGroup: !root.enabled ? Impl.ControlPalette.Disabled
                                 : (root.Window.window && !root.Window.window.active
                                    ? Impl.ControlPalette.Inactive : Impl.ControlPalette.Active)
        textRole: Impl.ControlPalette.ButtonText
        fillRole: Impl.ControlPalette.Base
    }

    font.family: HolonightTheme.uiFont
    font.pointSize: HolonightTheme.bodySize

    property int maximumVisibleItems: 8
    property real delegateHeight: 28

    readonly property int resolvedMaximumVisibleItems: popupGeometry.resolvedMaximumVisibleItems
    readonly property point sceneOrigin: popupGeometry.sceneOrigin
    readonly property point sceneXAxis: popupGeometry.sceneXAxis
    readonly property point sceneYAxis: popupGeometry.sceneYAxis
    readonly property real sceneScaleX: popupGeometry.sceneScaleX
    readonly property real sceneScaleY: popupGeometry.sceneScaleY
    readonly property real sceneAxisDot: popupGeometry.sceneAxisDot
    readonly property bool popupTransformSupported: popupGeometry.popupTransformSupported
    readonly property real effectiveScale: popupGeometry.effectiveScale

    Impl.ComboBoxPopupGeometry {
        id: popupGeometry
        control: root
        maximumVisibleItems: root.maximumVisibleItems
        popup: root.popup
    }

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
            strokeColor: root.enabled ? root.controlColors.colors.textPrimary : root.controlColors.colors.textDisabled
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
        color: root.enabled ? root.controlColors.colors.textPrimary : root.controlColors.colors.textDisabled
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
        color: (root.HnInputInteraction.hoverAllowed && root.hovered) ? root.controlColors.colors.surfaceHover : root.controlColors.colors.surface
        border.color: root.visualFocus || root.popup.visible ? root.controlColors.colors.borderFocus : (root.enabled ? root.controlColors.colors.borderPassive : root.controlColors.colors.borderPassive)
        border.width: (root.visualFocus || root.popup.visible) ? HnMetrics.focusBorderWidth : HnMetrics.borderWidth
    }

    popup: T.Popup {
        id: popup
        readonly property Impl.ControlPalette controlColors: Impl.ControlPalette {
            palette: popup.palette
            inheritFrom: root.palette
            colorGroup: !popup.enabled ? Impl.ControlPalette.Disabled
                : (popup.contentItem.Window.window && !popup.contentItem.Window.window.active
                   ? Impl.ControlPalette.Inactive : Impl.ControlPalette.Active)
        }

        property alias geometryRevision: popupGeometry.geometryRevision

        readonly property point controlSceneBottom: popupGeometry.controlSceneBottom
        readonly property real controlSceneTop: popupGeometry.controlSceneTop
        readonly property real controlSceneBottomY: popupGeometry.controlSceneBottomY
        readonly property real sceneSpaceAbove: popupGeometry.sceneSpaceAbove
        readonly property real sceneSpaceBelow: popupGeometry.sceneSpaceBelow
        readonly property real sceneAvailableVerticalSpace: popupGeometry.sceneAvailableVerticalSpace
        readonly property real availableVerticalSpace: popupGeometry.availableVerticalSpace
        readonly property real naturalHeight: popupGeometry.naturalHeight
        readonly property real renderedDemand: popupGeometry.renderedDemand
        readonly property real spaceAbove: popupGeometry.spaceAbove
        readonly property real spaceBelow: popupGeometry.spaceBelow
        readonly property bool opensAbove: popupGeometry.opensAbove

        objectName: "holonightComboBoxPopup"
        parent: C.Overlay.overlay
        popupType: T.Popup.Item
        closePolicy: T.Popup.CloseOnEscape | T.Popup.CloseOnPressOutside
        modal: true
        dim: false
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

            // Demand must not query the viewport while its height is being evaluated.
            // All owned delegates use the control's configurable delegateHeight.
            readonly property real itemLimitedHeight: Math.min(count, root.resolvedMaximumVisibleItems)
                                                        * root.delegateHeight

            objectName: "holonightComboBoxPopupList"
            clip: contentHeight > height
            implicitHeight: itemLimitedHeight
            interactive: contentHeight > height
            // A composite may replace this popup; two views must not share its delegates.
            model: root.popup === popup ? root.delegateModel : null
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

            color: popup.controlColors.colors.surface
            border.color: popup.controlColors.colors.borderPassive
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
