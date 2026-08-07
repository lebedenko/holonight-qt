// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Shapes
import QtQuick.Templates as T
import Holonight
import Holonight.Core

T.ComboBox {
    id: root

    font.family: HolonightTheme.uiFont
    font.pointSize: HolonightTheme.bodySize

    property int maximumVisibleItems: 8

    readonly property int resolvedMaximumVisibleItems: Math.max(1, maximumVisibleItems)

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

        readonly property real controlWindowY: root.mapToItem(null, 0, 0).y
        readonly property real spaceAbove: Math.max(0, controlWindowY - margins - 2)
        readonly property real spaceBelow: root.Window.window
                                                   ? Math.max(0, root.Window.window.height
                                                              - controlWindowY - root.height
                                                              - margins - 2)
                                                   : contentItem.implicitHeight + topPadding + bottomPadding
        readonly property real availableVerticalSpace: Math.max(spaceAbove, spaceBelow)
        readonly property bool opensAbove: spaceBelow < contentItem.implicitHeight + topPadding + bottomPadding
                                           && spaceAbove > spaceBelow

        objectName: "holonightComboBoxPopup"
        y: opensAbove ? -implicitHeight - 2 : root.height + 2
        width: root.width
        implicitHeight: Math.min(contentItem.implicitHeight + topPadding + bottomPadding,
                                 availableVerticalSpace)
        margins: 4
        padding: 4

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
            height: 28
            text: root.textAt(index)
            highlighted: root.highlightedIndex === index
            topLeftRadius: index === 0 ? popupInnerRadius : 0
            topRightRadius: index === 0 ? popupInnerRadius : 0
            bottomLeftRadius: index === root.count - 1 ? popupInnerRadius : 0
            bottomRightRadius: index === root.count - 1 ? popupInnerRadius : 0
        }
    }
}
