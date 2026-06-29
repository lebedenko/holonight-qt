// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#pragma once

#include "holonight/palette.h"

#include <QString>
#include <QtGlobal>

namespace Holonight {

enum class AppearanceMode {
  Dark,
  Light,
  System,
};

struct ThemeConfig {
  AppearanceMode appearance_mode = AppearanceMode::Dark;
  QString scheme;
  QString accent;
  qreal transparency = 1.0;
  QString icon_theme;
  QString fallback_icon_theme;
  QString ui_font;
  QString fixed_font;
  int base_font_size = 10;
  qreal scale_factor = 1.0;

  [[nodiscard]] int captionSize() const;
  [[nodiscard]] int bodySize() const;
  [[nodiscard]] int titleSize() const;
  [[nodiscard]] int headingSize() const;
  [[nodiscard]] ColorMode resolvedColorMode() const;

  [[nodiscard]] static ThemeConfig defaults();
  [[nodiscard]] static ThemeConfig load();
  [[nodiscard]] static QString configFilePath();
};

}  // namespace Holonight
