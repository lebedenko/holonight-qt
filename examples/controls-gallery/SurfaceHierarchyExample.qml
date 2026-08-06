// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import Holonight.Core
import Holonight.Controls

HnSurfaceFrame {
    id: windowSurface

    implicitHeight: 420
    surfaceRole: HnSurfaceRole.Window

    Label {
        id: windowLabel

        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 28
        anchors.topMargin: 24
        text: qsTr("Window · background")
        color: HoloniightPalette.textPrimary
        font.bold: true
    }

    HnSurfaceFrame {
        id: baseSurface

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: windowLabel.bottom
        anchors.bottom: parent.bottom
        anchors.leftMargin: 28
        anchors.rightMargin: 28
        anchors.topMargin: 16
        anchors.bottomMargin: 28
        surfaceRole: HnSurfaceRole.Panel
        fillColor: HoloniightPalette.surface

        Label {
            id: baseLabel

            anchors.left: parent.left
            anchors.top: parent.top
            anchors.leftMargin: 24
            anchors.topMargin: 20
            text: qsTr("Panel · surface")
            color: HoloniightPalette.textPrimary
            font.bold: true
        }

        HnSurfaceFrame {
            id: cardSurface

            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: baseLabel.bottom
            anchors.bottom: parent.bottom
            anchors.leftMargin: 24
            anchors.rightMargin: 24
            anchors.topMargin: 14
            anchors.bottomMargin: 24
            surfaceRole: HnSurfaceRole.Card

            Label {
                id: cardLabel

                anchors.left: parent.left
                anchors.top: parent.top
                anchors.leftMargin: 20
                anchors.topMargin: 18
                text: qsTr("Card · surfaceElevated")
                color: HoloniightPalette.textPrimary
                font.bold: true
            }

            HnSurfaceFrame {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: cardLabel.bottom
                anchors.bottom: parent.bottom
                anchors.leftMargin: 20
                anchors.rightMargin: 20
                anchors.topMargin: 12
                anchors.bottomMargin: 20
                surfaceRole: HnSurfaceRole.Popup

                Label {
                    anchors.centerIn: parent
                    text: qsTr("Popup · surfaceRaised")
                    color: HoloniightPalette.textPrimary
                    font.bold: true
                }
            }
        }
    }
}
