// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Templates as T
import Holonight

T.ScrollView {
    id: root

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    clip: contentWidth > availableWidth || contentHeight > availableHeight

    ScrollBar.vertical: ScrollBar {
        x: root.mirrored ? 0 : root.width - width
        y: root.topPadding
        height: root.availableHeight
        active: root.ScrollBar.horizontal.active
        parent: root
    }

    ScrollBar.horizontal: ScrollBar {
        x: root.leftPadding
        y: root.height - height
        width: root.availableWidth
        active: root.ScrollBar.vertical.active
        parent: root
    }
}
