// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls.Basic as C
import Holonight as H
import Holonight.Core

FocusScope {
    id: root

    property int sizeRole: HnControlSize.Normal
    property int minimumVisibleLines: 3
    property int maximumVisibleLines: 8
    property bool hasError: false
    property Component footerContent
    property int maximumLength: -1
    property string accessibleName
    property string accessibleDescription

    property alias text: editor.text
    property alias placeholderText: editor.placeholderText
    property alias readOnly: editor.readOnly
    property alias wrapMode: editor.wrapMode
    property alias inputMethodHints: editor.inputMethodHints
    property alias cursorPosition: editor.cursorPosition
    readonly property alias selectionStart: editor.selectionStart
    readonly property alias selectionEnd: editor.selectionEnd
    readonly property alias selectedText: editor.selectedText
    property alias font: editor.font

    readonly property int resolvedSizeRole: HnMetrics.normalizedSizeRole(root.sizeRole)
    readonly property int resolvedMinimumVisibleLines: Math.max(1, root.minimumVisibleLines)
    readonly property int resolvedMaximumVisibleLines: Math.max(root.resolvedMinimumVisibleLines,
                                                                 root.maximumVisibleLines)
    readonly property real fontHeight: editor.font.pixelSize > 0 ? editor.font.pixelSize : (editor.font.pointSize * 1.33)
    readonly property real lineHeight: Math.max(editor.contentHeight / Math.max(1, editor.lineCount),
                                                root.fontHeight * 1.2)
    readonly property real editorVerticalPadding: HnMetrics.internalSpacing(root.resolvedSizeRole) * 2
    readonly property real minimumEditorHeight: root.resolvedMinimumVisibleLines * root.lineHeight
                                                + root.editorVerticalPadding
    readonly property real maximumEditorHeight: root.resolvedMaximumVisibleLines * root.lineHeight
                                                + root.editorVerticalPadding
    readonly property bool overflow: editor.contentHeight + root.editorVerticalPadding
                                     > root.maximumEditorHeight

    implicitWidth: 240
    implicitHeight: editorScroll.implicitHeight + (footerLoader.active
                                                    ? footerLoader.implicitHeight
                                                      + HnMetrics.internalSpacing(root.resolvedSizeRole) * 2
                                                    : 0)
    activeFocusOnTab: true
    onActiveFocusChanged: {
        if (root.activeFocus)
            editor.forceActiveFocus()
    }

    function clear(): void { editor.clear() }
    function select(start: int, end: int): void { editor.select(start, end) }
    function selectAll(): void { editor.selectAll() }
    function deselect(): void { editor.deselect() }
    function insert(position: int, text: string): void { editor.insert(position, text) }
    function remove(start: int, end: int): void { editor.remove(start, end) }
    function undo(): void { editor.undo() }
    function redo(): void { editor.redo() }

    Connections {
        target: editor

        function onTextChanged(): void {
            if (root.maximumLength >= 0 && editor.length > root.maximumLength)
                editor.remove(root.maximumLength, editor.length)
        }
    }

    Rectangle {
        anchors.fill: parent
        color: HoloniightPalette.surface
        radius: HnAppearance.roundedRadius(HnSurfaceRole.Control, width, height,
                                            HnAppearance.revision)
        border.width: (editor.activeFocus || root.hasError)
                      ? HnMetrics.focusBorderWidth
                      : HnMetrics.borderWidth
        border.color: root.hasError ? HoloniightPalette.borderUrgent
                                    : editor.activeFocus ? HoloniightPalette.borderFocus
                                                         : HoloniightPalette.borderPassive
        opacity: root.enabled ? 1.0 : 0.5
    }

    H.ScrollView {
        id: editorScroll

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        contentWidth: editor.wrapMode === TextEdit.NoWrap
                      ? Math.max(availableWidth, editor.contentWidth + editor.leftPadding + editor.rightPadding)
                      : availableWidth
        implicitHeight: Math.max(root.minimumEditorHeight,
                                 Math.min(root.maximumEditorHeight,
                                          editor.contentHeight + root.editorVerticalPadding))
        C.ScrollBar.vertical.policy: root.overflow ? C.ScrollBar.AsNeeded : C.ScrollBar.AlwaysOff
        C.ScrollBar.horizontal.policy: editor.wrapMode === TextEdit.NoWrap
                                       ? C.ScrollBar.AsNeeded : C.ScrollBar.AlwaysOff
        clip: root.overflow || editor.wrapMode === TextEdit.NoWrap

        H.TextArea {
            id: editor

            width: editorScroll.contentWidth
            implicitWidth: 0
            hasError: root.hasError
            enabled: root.enabled
            wrapMode: TextEdit.Wrap
            topPadding: HnMetrics.internalSpacing(root.resolvedSizeRole)
            bottomPadding: HnMetrics.internalSpacing(root.resolvedSizeRole)
            leftPadding: HnMetrics.horizontalPadding(root.resolvedSizeRole)
            rightPadding: HnMetrics.horizontalPadding(root.resolvedSizeRole)
            background: Item {}
            Accessible.name: root.accessibleName
            Accessible.description: root.accessibleDescription
        }
    }

    Loader {
        id: footerLoader

        objectName: "hnTextAreaFooter"
        anchors.left: parent.left
        anchors.leftMargin: HnMetrics.horizontalPadding(root.resolvedSizeRole)
        anchors.right: parent.right
        anchors.rightMargin: HnMetrics.horizontalPadding(root.resolvedSizeRole)
        anchors.top: editorScroll.bottom
        anchors.topMargin: active ? HnMetrics.internalSpacing(root.resolvedSizeRole) : 0
        active: root.footerContent !== null
        sourceComponent: root.footerContent
    }
}
