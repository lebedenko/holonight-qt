// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#pragma once

#include <QString>
#include <QtGlobal>

namespace Holonight {

struct ThemeConfig {
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

  [[nodiscard]] static ThemeConfig defaults();
  [[nodiscard]] static ThemeConfig load();
  [[nodiscard]] static QString configFilePath();
};

}  // namespace Holonight
