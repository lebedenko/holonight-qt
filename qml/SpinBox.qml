// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Templates as T
import Holonight.Core

T.SpinBox {
    id: root

    font.family: HolonightTheme.uiFont
    font.pointSize: HolonightTheme.bodySize

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            contentItem.implicitWidth + (up.indicator ? up.indicator.implicitWidth : 0)
                                                      + (down.indicator ? down.indicator.implicitWidth : 0)
                                                      + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             contentItem.implicitHeight + topPadding + bottomPadding,
                             up.indicator ? up.indicator.implicitHeight : 0,
                             down.indicator ? down.indicator.implicitHeight : 0)

    leftPadding: (down.indicator ? down.indicator.width : 0) + 4
    rightPadding: (up.indicator ? up.indicator.width : 0) + 4
    topPadding: 4
    bottomPadding: 4

    validator: doubleValidator
    DoubleValidator {
        id: doubleValidator
        bottom: Math.min(root.from, root.to)
        top: Math.max(root.from, root.to)
    }

    contentItem: TextInput {
        z: 2
        text: root.displayText
        font: root.font
        color: root.enabled ? HoloniightPalette.textPrimary : HoloniightPalette.textDisabled
        selectionColor: Qt.rgba(HoloniightPalette.primary.r, HoloniightPalette.primary.g,
                                HoloniightPalette.primary.b, 0.3)
        selectedTextColor: HoloniightPalette.onPrimary
        horizontalAlignment: Qt.AlignHCenter
        verticalAlignment: Qt.AlignVCenter
        readOnly: !root.editable
        validator: root.validator
        inputMethodHints: root.inputMethodHints
    }

    up.indicator: Item {
        x: root.mirrored ? 0 : root.width - width
        height: root.height
        implicitWidth: 32
        implicitHeight: 32

        readonly property real semanticRadius: HnAppearance.roundedRadius(HnSurfaceRole.Control,
                                                                          root.width, root.height,
                                                                          HnAppearance.revision)

        Rectangle {
            visible: root.enabled && (root.up.hovered || root.up.pressed)
            color: root.up.pressed ? HoloniightPalette.surfaceRaised : HoloniightPalette.surfaceHover
            radius: Math.max(0, parent.semanticRadius - 1)

            anchors {
                fill: parent
                margins: root.activeFocus ? HnMetrics.focusBorderWidth : HnMetrics.borderWidth
            }

            // Flatten left corners so the inner edge against text area is straight
            Rectangle {
                width: parent.radius
                color: parent.color

                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    left: parent.left
                }
            }
        }

        // Vertical separator line
        Rectangle {
            width: 1
            color: HoloniightPalette.borderSubtle

            anchors {
                left: parent.left
                top: parent.top
                bottom: parent.bottom
                topMargin: 4
                bottomMargin: 4
            }
        }

        Text {
            anchors.centerIn: parent
            text: "+"
            textFormat: Text.PlainText
            color: (root.enabled && root.value < root.to) ? HoloniightPalette.textPrimary : HoloniightPalette.textDisabled

            font {
                pointSize: HolonightTheme.bodySize
                bold: true
                preferShaping: false
            }
        }
    }

    down.indicator: Item {
        x: root.mirrored ? root.width - width : 0
        height: root.height
        implicitWidth: 32
        implicitHeight: 32

        readonly property real semanticRadius: HnAppearance.roundedRadius(HnSurfaceRole.Control,
                                                                          root.width, root.height,
                                                                          HnAppearance.revision)

        Rectangle {
            visible: root.enabled && (root.down.hovered || root.down.pressed)
            color: root.down.pressed ? HoloniightPalette.surfaceRaised : HoloniightPalette.surfaceHover
            radius: Math.max(0, parent.semanticRadius - 1)

            anchors {
                fill: parent
                margins: root.activeFocus ? HnMetrics.focusBorderWidth : HnMetrics.borderWidth
            }

            // Flatten right corners so the inner edge against text area is straight
            Rectangle {
                width: parent.radius
                color: parent.color

                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    right: parent.right
                }
            }
        }

        // Vertical separator line
        Rectangle {
            width: 1
            color: HoloniightPalette.borderSubtle

            anchors {
                right: parent.right
                top: parent.top
                bottom: parent.bottom
                topMargin: 4
                bottomMargin: 4
            }
        }

        Text {
            anchors.centerIn: parent
            text: "-"
            textFormat: Text.PlainText
            color: (root.enabled && root.value > root.from) ? HoloniightPalette.textPrimary : HoloniightPalette.textDisabled

            font {
                pointSize: HolonightTheme.bodySize
                bold: true
                preferShaping: false
            }
        }
    }

    background: Rectangle {
        readonly property real semanticRadius: HnAppearance.roundedRadius(HnSurfaceRole.Control,
                                                                          width, height,
                                                                          HnAppearance.revision)

        implicitWidth: 140
        implicitHeight: 32

        color: HoloniightPalette.surface
        radius: semanticRadius

        border.width: root.activeFocus ? HnMetrics.focusBorderWidth : HnMetrics.borderWidth
        border.color: root.activeFocus ? HoloniightPalette.borderFocus : HoloniightPalette.borderPassive

        opacity: root.enabled ? 1.0 : 0.5

        Behavior on border.color { ColorAnimation { duration: 80 } }
    }
}
