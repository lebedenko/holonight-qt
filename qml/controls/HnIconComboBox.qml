// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Templates as T
import QtQuick.Controls as C
import Holonight.Core
import Holonight.impl as Impl

C.ComboBox {
    id: root
    palette: compositePalette.appearancePalette

    readonly property Impl.ControlPalette compositePalette: Impl.ControlPalette {}

    readonly property Impl.ControlPalette frameColors: Impl.ControlPalette {
        palette: root.palette
        colorGroup: !root.enabled ? Impl.ControlPalette.Disabled
                                 : (root.Window.window && !root.Window.window.active
                                    ? Impl.ControlPalette.Inactive : Impl.ControlPalette.Active)
        textRole: Impl.ControlPalette.ButtonText
        fillRole: Impl.ControlPalette.Base
    }

    font.family: HolonightTheme.uiFont
    font.pointSize: HolonightTheme.bodySize

    property int sizeRole: HnControlSize.Normal
    property string iconRole: "icon"

    readonly property int resolvedSizeRole: HnMetrics.normalizedSizeRole(root.sizeRole)
    readonly property url currentIconSource: root.roleValue(root.currentIndex, root.iconRole)

    property int maximumVisibleItems: 8
    property real delegateHeight: HnMetrics.controlHeight(root.resolvedSizeRole)

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

    function roleValue(index: int, role: string): var {
        if (index < 0 || !root.model || !role)
            return ""

        if (typeof root.model.get === "function") {
            const row = root.model.get(index)
            return row && row[role] !== undefined ? row[role] : ""
        }

        const row = root.model[index]
        if (row && typeof row === "object" && row[role] !== undefined)
            return row[role]
        return ""
    }

    implicitHeight: HnMetrics.controlHeight(root.resolvedSizeRole)
    leftPadding: HnMetrics.horizontalPadding(root.resolvedSizeRole)
    rightPadding: HnMetrics.horizontalPadding(root.resolvedSizeRole) + root.indicator.width
    topPadding: 0
    bottomPadding: 0

    delegate: iconDelegate

    contentItem: T.TextField {
        rightPadding: HnMetrics.internalSpacing(root.resolvedSizeRole)
        text: root.editable ? root.editText : root.displayText
        enabled: root.editable
        autoScroll: root.editable
        font: root.font
        color: root.enabled ? HoloniightPalette.textPrimary : HoloniightPalette.textDisabled
        selectionColor: HoloniightPalette.primary
        selectedTextColor: HoloniightPalette.onPrimary
        verticalAlignment: Text.AlignVCenter

        HnIcon {
            id: selectedIcon

            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            source: root.currentIconSource
            size: HnMetrics.iconSize(root.resolvedSizeRole)
            iconState: root.enabled ? HnIcon.Normal : HnIcon.Disabled
            visible: source.toString().length > 0 && !selectedIcon.hasError
        }

        leftPadding: selectedIcon.visible
                     ? selectedIcon.width + HnMetrics.internalSpacing(root.resolvedSizeRole)
                     : 0
    }

    background: Rectangle {
        readonly property real semanticRadius: HnAppearance.roundedRadius(HnSurfaceRole.Control,
                                                                          width, height,
                                                                          HnAppearance.revision)

        implicitWidth: 120
        implicitHeight: 32
        radius: semanticRadius
        color: root.hovered ? root.frameColors.colors.surfaceHover : root.frameColors.colors.surface
        border.color: root.visualFocus || root.popup.visible ? root.frameColors.colors.borderFocus : (root.enabled ? root.frameColors.colors.borderPassive : root.frameColors.colors.borderPassive)
        border.width: (root.visualFocus || root.popup.visible) ? HnMetrics.focusBorderWidth : HnMetrics.borderWidth
    }

    popup: C.Popup {
        id: popup
        palette: compositePalette.appearancePalette

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
        popupType: C.Popup.Item
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
            C.ScrollBar.vertical: C.ScrollBar {
                policy: popupList.contentHeight > popupList.height
                        ? C.ScrollBar.AlwaysOn
                        : C.ScrollBar.AlwaysOff
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
        id: iconDelegate

        C.ItemDelegate {
            id: delegateRoot
            palette: compositePalette.appearancePalette

            required property int index
            required property var model
            readonly property var rowModel: model
            readonly property url iconSource: rowModel && rowModel[root.iconRole] !== undefined
                                                ? rowModel[root.iconRole]
                                                : root.roleValue(index, root.iconRole)
            readonly property real popupInnerRadius: Math.max(0,
                                                              HnAppearance.roundedRadius(HnSurfaceRole.Control,
                                                                  root.popup.width, root.popup.height,
                                                                  HnAppearance.revision)
                                                              - root.popup.padding)

            width: root.popup.availableWidth
            height: root.delegateHeight
            text: root.textAt(index)
            highlighted: root.highlightedIndex === index
            readonly property real topLeftRadius: index === 0 ? popupInnerRadius : 0
            readonly property real topRightRadius: index === 0 ? popupInnerRadius : 0
            readonly property real bottomLeftRadius: index === root.count - 1 ? popupInnerRadius : 0
            readonly property real bottomRightRadius: index === root.count - 1 ? popupInnerRadius : 0
            leftPadding: HnMetrics.horizontalPadding(root.resolvedSizeRole)
                         + (delegateIcon.visible
                            ? delegateIcon.width + HnMetrics.internalSpacing(root.resolvedSizeRole)
                            : 0)

            background: Item {
                readonly property bool isSelected: delegateRoot.highlighted || delegateRoot.checked || (delegateRoot.ListView ? delegateRoot.ListView.isCurrentItem : false)

                implicitWidth: 100
                implicitHeight: 32
                visible: delegateRoot.down || delegateRoot.hovered || isSelected || delegateRoot.visualFocus
                Rectangle {
                    objectName: "hnItemDelegateBase"
                    anchors.fill: parent
                    color: delegateRoot.down ? HoloniightPalette.surfaceElevated
                                     : (!parent.isSelected && delegateRoot.hovered ? HoloniightPalette.surfaceHover : "transparent")
                    topLeftRadius: delegateRoot.topLeftRadius
                    topRightRadius: delegateRoot.topRightRadius
                    bottomLeftRadius: delegateRoot.bottomLeftRadius
                    bottomRightRadius: delegateRoot.bottomRightRadius
                    Accessible.ignored: true
                }
                Rectangle {
                    objectName: "hnItemDelegateSelectedOverlay"
                    anchors.fill: parent
                    color: delegateRoot.hovered ? HoloniightPalette.surfaceSelectedHover
                                        : HoloniightPalette.surfaceSelected
                    topLeftRadius: delegateRoot.topLeftRadius
                    topRightRadius: delegateRoot.topRightRadius
                    bottomLeftRadius: delegateRoot.bottomLeftRadius
                    bottomRightRadius: delegateRoot.bottomRightRadius
                    visible: parent.isSelected && !delegateRoot.down
                    Accessible.ignored: true
                }
                Rectangle {
                    anchors.fill: parent
                    color: "transparent"
                    topLeftRadius: delegateRoot.topLeftRadius
                    topRightRadius: delegateRoot.topRightRadius
                    bottomLeftRadius: delegateRoot.bottomLeftRadius
                    bottomRightRadius: delegateRoot.bottomRightRadius
                    border.width: delegateRoot.visualFocus ? HnMetrics.focusBorderWidth : 0
                    border.color: HoloniightPalette.borderFocus
                    Accessible.ignored: true
                }
            }

            HnIcon {
                id: delegateIcon

                anchors.left: parent.left
                anchors.leftMargin: HnMetrics.horizontalPadding(root.resolvedSizeRole)
                anchors.verticalCenter: parent.verticalCenter
                source: delegateRoot.iconSource
                size: HnMetrics.iconSize(root.resolvedSizeRole)
                iconState: delegateRoot.enabled ? HnIcon.Normal : HnIcon.Disabled
                visible: source.toString().length > 0 && !delegateIcon.hasError
            }
        }
    }
}
