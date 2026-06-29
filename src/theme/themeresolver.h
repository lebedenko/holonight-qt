// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#pragma once

#include "holonight/config.h"
#include "holonight/palette.h"

namespace Holonight {

class ThemeResolver {
public:
  ThemeResolver() = delete;
  [[nodiscard]] static ColorTokens resolve(const ThemeConfig& config);

private:
  [[nodiscard]] static ColorTokens resolveBase(ColorMode mode);
  static void applyAccent(ColorTokens& tok, const QString& accent);
};

}  // namespace Holonight
