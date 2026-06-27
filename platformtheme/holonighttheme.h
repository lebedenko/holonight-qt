// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#pragma once

#include "holonight/config.h"

#include <QFont>
#include <qpa/qplatformtheme.h>

class HoloniightTheme : public QPlatformTheme {
 public:
  HoloniightTheme();
  Q_DISABLE_COPY_MOVE(HoloniightTheme)

  [[nodiscard]] const QPalette* palette(Palette type = SystemPalette) const override;
  [[nodiscard]] QVariant themeHint(ThemeHint hint) const override;
  [[nodiscard]] Qt::ColorScheme colorScheme() const override;
  [[nodiscard]] const QFont* font(Font type = SystemFont) const override;

 private:
  Holonight::ThemeConfig config_;
  QFont font_system_;
  QFont font_title_;
  QFont font_fixed_;
  QFont font_small_;
  QFont font_mini_;
};
