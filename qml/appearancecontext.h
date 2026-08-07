// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#pragma once

class QQmlEngine;

namespace Holonight {

class AppearanceReader;

[[nodiscard]] AppearanceReader* appearanceReaderForEngine(QQmlEngine* engine);

}  // namespace Holonight
