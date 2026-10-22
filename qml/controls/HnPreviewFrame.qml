// SPDX-License-Identifier: GPL-3.0-or-later

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as C
import Holonight.Core

C.Control {
    id: root
    enum State {
        Content,
        Loading,
        Empty,
        Error
    }
    property int previewState: HnPreviewFrame.Empty
    property Component previewContent
    property Component loadingContent
    property Component emptyContent
    property Component errorContent
    readonly property alias previewItem: presentation.item

    // Deliberately independent of loaded content, including state transitions.
    implicitWidth: 240
    implicitHeight: 180
    padding: 0
    contentItem: Loader {
        id: presentation
        sourceComponent: root.previewState === HnPreviewFrame.Loading ? root.loadingContent : root.previewState === HnPreviewFrame.Error ? root.errorContent : root.previewState === HnPreviewFrame.Empty ? root.emptyContent : root.previewContent
    }
    background: null
}
