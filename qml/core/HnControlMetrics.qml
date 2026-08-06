// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

pragma Singleton

import QtQml

QtObject {
    readonly property int headerHeight: 56
    readonly property int appTitleIconSize: 32
    readonly property int appTitleIconSpacing: 12
    readonly property int appTitleTextSpacing: 8

    function normalizedSizeRole(sizeRole: int): int {
        switch (sizeRole) {
        case HnControlSize.Compact:
        case HnControlSize.Normal:
        case HnControlSize.Large:
        case HnControlSize.Hero:
            return sizeRole
        default:
            return HnControlSize.Normal
        }
    }

    function controlHeight(sizeRole: int): int {
        switch (normalizedSizeRole(sizeRole)) {
        case HnControlSize.Compact: return 28
        case HnControlSize.Large: return 40
        case HnControlSize.Hero: return 52
        default: return 32
        }
    }

    function horizontalPadding(sizeRole: int): int {
        switch (normalizedSizeRole(sizeRole)) {
        case HnControlSize.Large: return 12
        case HnControlSize.Hero: return 16
        default: return 8
        }
    }

    function iconSize(sizeRole: int): int {
        switch (normalizedSizeRole(sizeRole)) {
        case HnControlSize.Large: return 20
        case HnControlSize.Hero: return 24
        default: return 16
        }
    }

    function internalSpacing(sizeRole: int): int {
        switch (normalizedSizeRole(sizeRole)) {
        case HnControlSize.Compact: return 4
        case HnControlSize.Large: return 8
        case HnControlSize.Hero: return 10
        default: return 6
        }
    }
}
