// SPDX-License-Identifier: GPL-3.0-or-later

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import Holonight.Core

ColumnLayout {
    id: root
    // Ordered {label, value} rows or {heading} section headings; values are caller-formatted.
    property var rows: []
    property real labelWidth: 100
    property real columnSpacing: HnMetrics.internalSpacing(HnControlSize.Compact)
    spacing: HnMetrics.internalSpacing(HnControlSize.Normal)
    Repeater {
        model: root.rows.length
        delegate: RowLayout {
            id: row
            required property int index
            readonly property var modelData: root.rows[index]
            visible: modelData.visible !== false
            spacing: root.columnSpacing
            readonly property bool section: modelData.heading !== undefined
            Layout.fillWidth: true
            Accessible.role: Accessible.StaticText
            Accessible.name: section ? String(modelData.heading) : (modelData.label.length ? String(modelData.label) + ": " : "") + String(modelData.value)
            HnLabel {
                role: HnTypographyRole.Caption
                objectName: row.modelData.headingObjectName ?? ""
                color: row.section ? HoloniightPalette.textSecondary : HoloniightPalette.textMuted
                rawText: row.section ? String(row.modelData.heading) : String(row.modelData.label)
                font.bold: row.section
                wrapMode: Text.Wrap
                Layout.preferredWidth: row.section ? -1 : root.labelWidth
                Layout.minimumWidth: row.section ? 0 : root.labelWidth
                Layout.maximumWidth: row.section ? Infinity : root.labelWidth
                Layout.fillWidth: row.section
                Layout.alignment: Qt.AlignTop
                Accessible.ignored: true
            }
            HnLabel {
                role: HnTypographyRole.Caption
                objectName: row.modelData.objectName ?? ""
                rawText: row.section ? "" : String(row.modelData.value)
                visible: !row.section
                wrapMode: Text.Wrap
                Layout.fillWidth: true
                Layout.minimumWidth: 0
                Layout.preferredWidth: 0
                Layout.alignment: Qt.AlignTop
                Accessible.ignored: true
            }
        }
    }
}
