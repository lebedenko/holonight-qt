// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#pragma once

#include <QFont>
#include <qpa/qplatformtheme.h>

class HoloniightTheme : public QPlatformTheme {
 public:
  HoloniightTheme();

  [[nodiscard]] const QPalette* palette(Palette type = SystemPalette) const override;
  [[nodiscard]] QVariant themeHint(ThemeHint hint) const override;
  [[nodiscard]] Qt::ColorScheme colorScheme() const override;
  [[nodiscard]] const QFont* font(Font type = SystemFont) const override;

 private:
  QFont font_;
};
