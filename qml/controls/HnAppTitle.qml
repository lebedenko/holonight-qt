// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import Holonight.Core

Item {
    id: root

    required property string applicationName
    property url iconSource
    property bool iconTinted: false
    property bool skipBranding: false

    implicitWidth: content.implicitWidth
    implicitHeight: content.implicitHeight

    Accessible.role: Accessible.StaticText
    Accessible.name: root.skipBranding ? root.applicationName : ("HoloNight " + root.applicationName)

    RowLayout {
        id: content

        anchors.fill: parent
        spacing: 0

        Loader {
            id: iconLoader

            readonly property HnIcon loadedIcon: item as HnIcon
            readonly property bool iconAvailable: status === Loader.Ready
                                                  && loadedIcon !== null
                                                  && !loadedIcon.hasError

            active: String(root.iconSource).length > 0
            visible: iconAvailable
            sourceComponent: iconComponent
            Layout.alignment: Qt.AlignVCenter
            Layout.preferredWidth: visible ? HnControlMetrics.appTitleIconSize : 0
            Layout.preferredHeight: visible ? HnControlMetrics.appTitleIconSize : 0
        }

        Item {
            visible: iconLoader.iconAvailable
            Layout.preferredWidth: visible ? HnControlMetrics.appTitleIconSpacing : 0
            Layout.preferredHeight: 1
        }

        HnLabel {
            objectName: "brandLabel"
            role: HnTypographyRole.Title
            rawText: "HoloNight"
            color: HoloniightPalette.brandForeground
            font.pointSize: HolonightTheme.appTitleSize
            font.weight: Font.DemiBold
            Accessible.ignored: true
            visible: !root.skipBranding
            Layout.alignment: Qt.AlignVCenter
        }

        Item {
            visible: !root.skipBranding
            Layout.preferredWidth: visible ? HnControlMetrics.appTitleTextSpacing : 0
            Layout.preferredHeight: 1
        }

        HnLabel {
            objectName: "applicationLabel"
            role: HnTypographyRole.Title
            rawText: root.applicationName
            color: HoloniightPalette.textPrimary
            elide: Text.ElideRight
            font.pointSize: HolonightTheme.appTitleSize
            font.weight: Font.DemiBold
            Accessible.ignored: true
            Layout.alignment: Qt.AlignVCenter
            Layout.fillWidth: true
        }
    }

    Component {
        id: iconComponent

        HnIcon {
            objectName: "appTitleIcon"
            readonly property bool accessibilityIgnored: Accessible.ignored
            source: root.iconSource
            size: HnControlMetrics.appTitleIconSize
            tinted: root.iconTinted
            Accessible.ignored: true
        }
    }
}
