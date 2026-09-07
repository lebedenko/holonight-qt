// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

import QtQuick
import QtQuick.Templates as T

// Private implementation contract shared by the style and the icon composite.
QtObject {
    required property T.ComboBox control
    required property T.Popup popup
    required property int maximumVisibleItems
    property int geometryRevision: 0

    readonly property int resolvedMaximumVisibleItems: Math.max(1, maximumVisibleItems)
    readonly property point sceneOrigin: control.mapToItem(null, geometryRevision * 0, 0)
    readonly property point sceneXAxis: control.mapToItem(null, 1 + geometryRevision * 0, 0)
    readonly property point sceneYAxis: control.mapToItem(null, geometryRevision * 0, 1)
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

    readonly property point controlSceneBottom: control.mapToItem(null, geometryRevision * 0,
                                                                   control.height)
    readonly property real controlSceneTop: Math.min(sceneOrigin.y, controlSceneBottom.y)
    readonly property real controlSceneBottomY: Math.max(sceneOrigin.y, controlSceneBottom.y)
    readonly property real sceneSpaceAbove: Math.max(0, controlSceneTop
                                                         - popup.margins - 2 * effectiveScale)
    readonly property real sceneSpaceBelow: control.Window.window
                                                ? Math.max(0, control.Window.window.height
                                                           - controlSceneBottomY
                                                           - popup.margins - 2 * effectiveScale)
                                                : naturalHeight * effectiveScale
    readonly property real sceneAvailableVerticalSpace: Math.max(sceneSpaceAbove, sceneSpaceBelow)
    readonly property real availableVerticalSpace: sceneAvailableVerticalSpace / effectiveScale
    readonly property real naturalHeight: popup.contentItem.implicitHeight + popup.topPadding + popup.bottomPadding
    readonly property real renderedDemand: naturalHeight * effectiveScale
    readonly property real spaceAbove: sceneSpaceAbove / effectiveScale
    readonly property real spaceBelow: control.Window.window
                                           ? sceneSpaceBelow / effectiveScale
                                           : naturalHeight
    readonly property bool opensAbove: sceneSpaceBelow < renderedDemand
                                           && sceneSpaceAbove > sceneSpaceBelow

}
