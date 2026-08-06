// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import Holonight.Core

Window {
    id: root

    property bool applicationHeaderVisible: false
    property real applicationHeaderHeight: 40
    property real contentPadding: 0
    property color applicationHeaderColor: HoloniightPalette.surface

    readonly property real effectiveApplicationHeaderHeight: root.applicationHeaderVisible
                                                              ? Math.max(0, root.applicationHeaderHeight) : 0
    readonly property real effectiveContentPadding: Math.max(0, root.contentPadding)
    readonly property alias applicationHeader: applicationHeader
    readonly property alias clientContentItem: clientContentItem

    property alias applicationHeaderLeadingData: leadingItem.data
    property alias applicationHeaderCenterData: centerItem.data
    property alias applicationHeaderTrailingData: trailingItem.data
    default property alias contentData: clientContentItem.data

    color: HoloniightPalette.background

    Item {
        id: applicationHeader

        height: root.effectiveApplicationHeaderHeight
        visible: root.applicationHeaderVisible
        clip: applicationHeader.height > 0

        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }

        Rectangle {
            anchors.fill: parent
            color: root.applicationHeaderColor
            Accessible.ignored: true
        }

        RowLayout {
            spacing: 8

            anchors {
                fill: parent
                leftMargin: root.effectiveContentPadding
                rightMargin: root.effectiveContentPadding
            }

            Item {
                id: leadingItem

                Layout.preferredWidth: childrenRect.width
                Layout.fillHeight: true
            }

            Item {
                id: centerItem

                Layout.fillWidth: true
                Layout.fillHeight: true

                HnLabel {
                    objectName: "hnApplicationWindowHeaderTitle"
                    anchors.verticalCenter: parent.verticalCenter
                    width: parent.width
                    visible: centerItem.children.length === 1
                    role: HnTypographyRole.Subheading
                    font.bold: true
                    rawText: root.title
                    color: HoloniightPalette.textPrimary
                    elide: Text.ElideRight
                }
            }

            Item {
                id: trailingItem

                Layout.preferredWidth: childrenRect.width
                Layout.fillHeight: true
            }
        }
    }

    Item {
        id: clientContentItem

        anchors {
            fill: parent
            margins: root.effectiveContentPadding
            topMargin: root.effectiveApplicationHeaderHeight + root.effectiveContentPadding
        }
    }
}
