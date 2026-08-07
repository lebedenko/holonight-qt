// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Holonight.Core
import Holonight.Controls

ScrollView {
    id: root

    property int baseFontSizeOverride: HolonightTheme.bodySize
    readonly property real scaleFactor: baseFontSizeOverride / Math.max(1, HolonightTheme.bodySize)

    contentWidth: availableWidth

    ColumnLayout {
        width: parent.width
        spacing: 16

        HnLabel {
            role: HnTypographyRole.Heading
            rawText: qsTr("Typography & Scale Tokens")
            Layout.leftMargin: 32
            Layout.topMargin: 24
        }

        HnLabel {
            role: HnTypographyRole.Caption
            rawText: qsTr("HoloNight typography system uses strict point size (pt) scale tokens and semantic role components.")
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
            Layout.leftMargin: 32
            Layout.rightMargin: 32
        }

        HnSurfaceFrame {
            surfaceRole: HnSurfaceRole.Card
            Layout.fillWidth: true
            Layout.leftMargin: 32
            Layout.rightMargin: 32
            implicitHeight: cardLayout.implicitHeight + 32

            ColumnLayout {
                id: cardLayout

                anchors.fill: parent
                anchors.margins: 16
                spacing: 12

                HnLabel {
                    role: HnTypographyRole.Title
                    rawText: qsTr("Live Font Size Control")
                }

                HnLabel {
                    role: HnTypographyRole.Caption
                    rawText: qsTr("Adjust base font size for visual testing. Changes remain in memory and are not persisted.")
                    color: HoloniightPalette.textMuted
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }

                RowLayout {
                    spacing: 16
                    Layout.fillWidth: true

                    HnLabel {
                        role: HnTypographyRole.Body
                        rawText: qsTr("Base Size:")
                    }

                    SpinBox {
                        id: fontSizeSpinBox

                        from: 6
                        to: 32
                        value: root.baseFontSizeOverride
                        editable: true
                        onValueChanged: root.baseFontSizeOverride = value
                    }

                    HnLabel {
                        role: HnTypographyRole.Body
                        rawText: qsTr("%1 pt (%2%)").arg(root.baseFontSizeOverride).arg(Math.round(root.scaleFactor * 100))
                        color: HoloniightPalette.textMuted
                    }

                    Item { Layout.fillWidth: true }

                    Button {
                        text: qsTr("Small (8 pt)")
                        onClicked: root.baseFontSizeOverride = 8
                    }

                    Button {
                        text: qsTr("Default (10 pt)")
                        onClicked: root.baseFontSizeOverride = 10
                    }

                    Button {
                        text: qsTr("Large (14 pt)")
                        onClicked: root.baseFontSizeOverride = 14
                    }

                    Button {
                        text: qsTr("Hero (18 pt)")
                        onClicked: root.baseFontSizeOverride = 18
                    }
                }
            }
        }

        HnLabel {
            role: HnTypographyRole.Title
            rawText: qsTr("Typography Roles Preview")
            Layout.leftMargin: 32
            Layout.topMargin: 8
        }

        ColumnLayout {
            spacing: 12
            Layout.fillWidth: true
            Layout.leftMargin: 32
            Layout.rightMargin: 32

            HnLabel {
                role: HnTypographyRole.Display
                rawText: "Display • " + Math.round(HolonightTheme.displayFontSize * root.scaleFactor) + " pt (14°C)"
                font.pointSize: HolonightTheme.displayFontSize * root.scaleFactor
            }

            HnLabel {
                role: HnTypographyRole.Heading
                rawText: "Heading • " + Math.round(HolonightTheme.headingSize * root.scaleFactor) + " pt (Appearance)"
                font.pointSize: HolonightTheme.headingSize * root.scaleFactor
            }

            HnLabel {
                role: HnTypographyRole.Title
                rawText: "Title • " + Math.round(HolonightTheme.titleFontSize * root.scaleFactor) + " pt (Theme Settings)"
                font.pointSize: HolonightTheme.titleFontSize * root.scaleFactor
            }

            HnLabel {
                role: HnTypographyRole.Subheading
                rawText: "Subheading • " + Math.round(HolonightTheme.subheadingSize * root.scaleFactor) + " pt (Interface font)"
                font.pointSize: HolonightTheme.subheadingSize * root.scaleFactor
            }

            HnLabel {
                role: HnTypographyRole.Body
                rawText: "Body • " + Math.round(HolonightTheme.bodySize * root.scaleFactor) + " pt (Standard interface body text)"
                font.pointSize: HolonightTheme.bodySize * root.scaleFactor
            }

            HnLabel {
                role: HnTypographyRole.Caption
                rawText: "Caption • " + Math.round(HolonightTheme.microSize * root.scaleFactor) + " pt (Muted subtext, metadata, timestamps)"
                font.pointSize: HolonightTheme.microSize * root.scaleFactor
            }

            HnLabel {
                role: HnTypographyRole.MicroHeader
                rawText: "CALENDAR"
                showPrefix: true
                font.pointSize: HolonightTheme.microSize * root.scaleFactor
            }

            HnLabel {
                role: HnTypographyRole.Code
                rawText: "Code • ~/.config/holonight/config.toml"
                font.pointSize: HolonightTheme.monospaceFontSize * root.scaleFactor
            }
        }

        HnSeparator {
            Layout.fillWidth: true
            Layout.leftMargin: 32
            Layout.rightMargin: 32
            Layout.topMargin: 8
            Layout.bottomMargin: 8
        }

        HnLabel {
            role: HnTypographyRole.Caption
            rawText: qsTr("Run with QT_SCALE_FACTOR=1.25 to inspect fractional scaling. Set HOLONIGHT_APPEARANCE_FILE to inspect each installed palette variant.")
            color: HoloniightPalette.textMuted
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
            Layout.leftMargin: 32
            Layout.rightMargin: 32
            Layout.bottomMargin: 16
        }
    }
}
