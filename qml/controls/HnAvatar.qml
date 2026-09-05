// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Effects
import Holonight.Core

Item {
    id: root

    property url source
    property real size: 56
    property url fallbackSource
    property color backgroundColor: HoloniightPalette.background
    property color ringColor: HoloniightPalette.borderActive
    property real ringWidth: HnMetrics.borderWidth
    // Measured from the outer edge, independently of the ring width.
    property real imageInset: 3

    implicitWidth: size
    implicitHeight: size
    Accessible.ignored: true

    Rectangle {
        id: circle
        anchors.centerIn: parent
        width: Math.min(root.width, root.height)
        height: width
        radius: width / 2
        color: root.backgroundColor
        antialiasing: true
    }

    Item {
        id: imageArea
        readonly property real pixelRatio: root.Window.window
                                           ? root.Window.window.devicePixelRatio : root.Screen.devicePixelRatio
        anchors.centerIn: parent
        width: Math.max(0, circle.width - 2 * Math.max(0, root.imageInset))
        height: width

        Image {
            id: avatar
            objectName: "hnAvatarImage"
            anchors.fill: parent
            source: root.source
            sourceSize: Qt.size(Math.ceil(width * imageArea.pixelRatio),
                                Math.ceil(height * imageArea.pixelRatio))
            fillMode: Image.PreserveAspectCrop
            asynchronous: true
            retainWhileLoading: false
            smooth: true
            visible: false
            layer.enabled: true
        }
        Image {
            id: fallback
            objectName: "hnAvatarFallback"
            anchors.fill: parent
            source: root.fallbackSource
            sourceSize: avatar.sourceSize
            fillMode: Image.PreserveAspectCrop
            asynchronous: true
            retainWhileLoading: false
            smooth: true
            visible: false
            layer.enabled: true
        }
        Rectangle {
            id: mask
            anchors.fill: parent
            radius: width / 2
            color: "white"
            antialiasing: true
            visible: false
            layer.enabled: true
        }
        MultiEffect {
            objectName: "hnAvatarEffect"
            anchors.fill: parent
            source: avatar.status === Image.Ready ? avatar : fallback
            visible: avatar.status === Image.Ready || fallback.status === Image.Ready
            maskEnabled: true
            maskSource: mask
            maskThresholdMin: 0.5
            maskSpreadAtMin: 1
        }
    }

    Rectangle {
        anchors.fill: circle
        radius: width / 2
        color: "transparent"
        border.color: root.ringColor
        border.width: Math.max(0, root.ringWidth)
        antialiasing: true
    }
}
