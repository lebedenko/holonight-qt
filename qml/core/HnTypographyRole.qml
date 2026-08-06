// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

pragma Singleton

import QtQuick

QtObject {
    enum Role {
        Display,
        Heading,
        Title,
        Subheading,
        Body,
        Caption,
        MicroHeader,
        Code
    }
}
