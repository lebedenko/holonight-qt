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
  QString header_font;
  QString display_font;
  int base_font_size = 10;
  int fixed_font_size = 10;
  qreal scale_factor = 1.0;

  [[nodiscard]] int microSize() const;
  [[nodiscard]] int captionSize() const;
  [[nodiscard]] int bodySize() const;
  [[nodiscard]] int subheadingSize() const;
  [[nodiscard]] int titleSize() const;
  [[nodiscard]] int appTitleSize() const;
  [[nodiscard]] int headingSize() const;
  [[nodiscard]] int displaySize() const;
  [[nodiscard]] int fixedFontSize() const;
  [[nodiscard]] QString resolvedHeaderFont() const;
  [[nodiscard]] QString resolvedDisplayFont() const;
  [[nodiscard]] ThemeSchemeKind resolvedThemeScheme() const;
  [[nodiscard]] ColorMode resolvedColorMode() const;
  [[nodiscard]] QString resolvedAccent() const;

  [[nodiscard]] static ThemeConfig defaults();
  [[nodiscard]] static ThemeConfig loadFile();
  [[nodiscard]] static ThemeConfig load();
  [[nodiscard]] static QString configFilePath();
  [[nodiscard]] bool save(QString* error = nullptr) const;
};

[[nodiscard]] ColorMode colorModeForScheme(ThemeSchemeKind scheme);

}  // namespace Holonight
