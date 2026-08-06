// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Holonight.Core
import Holonight.Controls
import "pages"

HnApplicationWindow {
    id: root

    property int currentPageIndex: 0

    visible: true
    width: 1000
    height: 760
    title: qsTr("HoloNight Controls Gallery")

    RowLayout {
        anchors.fill: parent
        spacing: 0

        // Left Navigation Bar Sidebar
        HnSurfaceFrame {
            surfaceRole: HnSurfaceRole.Card
            Layout.fillHeight: true
            Layout.preferredWidth: 240
            Layout.minimumWidth: 200
            Layout.maximumWidth: 300

            ColumnLayout {
                spacing: 8
                anchors.fill: parent

                HnAppTitle {
                    applicationName: qsTr("Controls Gallery")
                    iconSource: "qrc:/qt/qml/Holonight/Controls/assets/folder.svg"
                    iconTinted: true
                    Layout.fillWidth: true
                    Layout.leftMargin: 12
                    Layout.rightMargin: 12
                    Layout.topMargin: 16
                    Layout.bottomMargin: 8
                }

                HnSeparator {
                    Layout.fillWidth: true
                    Layout.leftMargin: 12
                    Layout.rightMargin: 12
                }

                ListView {
                    id: navListView

                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true
                    currentIndex: root.currentPageIndex

                    model: ListModel {
                        ListElement { titleToken: "Surfaces & Elevation"; iconToken: "qrc:/qt/qml/Holonight/Controls/assets/folder.svg" }
                        ListElement { titleToken: "Typography & Scale"; iconToken: "qrc:/qt/qml/Holonight/Controls/assets/edit.svg" }
                        ListElement { titleToken: "Inputs & Forms"; iconToken: "qrc:/qt/qml/Holonight/Controls/assets/search.svg" }
                        ListElement { titleToken: "Buttons & Actions"; iconToken: "qrc:/qt/qml/Holonight/Controls/assets/more-vertical.svg" }
                        ListElement { titleToken: "Lists & Selection"; iconToken: "qrc:/qt/qml/Holonight/Controls/assets/folder.svg" }
                        ListElement { titleToken: "Settings & Feedback"; iconToken: "qrc:/qt/qml/Holonight/Controls/assets/edit.svg" }
                    }

                    delegate: HnNavigationDelegate {
                        required property int index
                        required property string titleToken
                        required property string iconToken

                        width: ListView.view.width
                        title: qsTr(titleToken)
                        icon.source: iconToken
                        checked: navListView.currentIndex === index
                        onClicked: {
                            navListView.currentIndex = index
                            root.currentPageIndex = index
                        }
                    }
                }
            }
        }

        HnSeparator {
            orientation: Qt.Vertical
            Layout.fillHeight: true
        }

        // Main Content Area Pages
        StackLayout {
            currentIndex: root.currentPageIndex
            Layout.fillWidth: true
            Layout.fillHeight: true

            SurfacesPage {}
            TypographyPage {}
            InputsPage {}
            ButtonsPage {}
            DelegatesPage {}
            SettingsPage {}
        }
    }
}
