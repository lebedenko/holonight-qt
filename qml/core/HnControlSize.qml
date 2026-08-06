// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

pragma Singleton

import QtQml

QtObject {
    enum Role {
        Compact = 0,
        Normal = 1,
        Large = 2,
        Hero = 3
    }
}
