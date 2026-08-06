// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#pragma once

#include "holonight/config.h"

namespace Holonight {

class ThemeLoader {
public:
  ThemeLoader() = delete;
  [[nodiscard]] static ThemeConfig load();
};

}  // namespace Holonight
