// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Templates as T
import Holonight.Core
import Holonight.impl as Impl

T.SpinBox {
    id: root

    readonly property Impl.ControlPalette controlColors: Impl.ControlPalette {
        palette: root.palette
        colorGroup: !root.enabled ? Impl.ControlPalette.Disabled
                                 : (root.Window.window && !root.Window.window.active
                                    ? Impl.ControlPalette.Inactive : Impl.ControlPalette.Active)
        textRole: Impl.ControlPalette.Text
        fillRole: Impl.ControlPalette.Base
    }

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
        color: root.enabled ? root.controlColors.colors.textPrimary : root.controlColors.colors.textDisabled
        selectionColor: root.controlColors.colors.selection
        selectedTextColor: root.controlColors.colors.onPrimary
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
            visible: root.enabled && ((root.HnInputInteraction.hoverAllowed && root.up.hovered) || root.up.pressed)
            color: root.up.pressed ? root.controlColors.colors.surfaceRaised : root.controlColors.colors.buttonHover
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
            color: root.controlColors.colors.borderSubtle

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
            color: (root.enabled && root.value < root.to) ? root.controlColors.colors.buttonText : root.controlColors.colors.stepperDisabledText

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
            visible: root.enabled && ((root.HnInputInteraction.hoverAllowed && root.down.hovered) || root.down.pressed)
            color: root.down.pressed ? root.controlColors.colors.surfaceRaised : root.controlColors.colors.buttonHover
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
            color: root.controlColors.colors.borderSubtle

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
            color: (root.enabled && root.value > root.from) ? root.controlColors.colors.buttonText : root.controlColors.colors.stepperDisabledText

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

        color: root.controlColors.colors.surface
        radius: semanticRadius

        border.width: root.activeFocus ? HnMetrics.focusBorderWidth : HnMetrics.borderWidth
        border.color: root.activeFocus ? root.controlColors.colors.borderFocus : root.controlColors.colors.borderPassive

        opacity: root.enabled || !root.controlColors.defaultDisabled ? 1.0 : 0.5

        Behavior on border.color { ColorAnimation { duration: 80 } }
    }
}
