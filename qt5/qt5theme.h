// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include "holonight/appearance.h"

#include <QFont>
#include <qpa/qplatformtheme.h>
class HoloniightQt5Theme : public QPlatformTheme {
 public:
  HoloniightQt5Theme();
  [[nodiscard]] const QPalette* palette(Palette type = SystemPalette) const override;
  [[nodiscard]] QVariant themeHint(ThemeHint hint) const override;
  [[nodiscard]] const QFont* font(Font type = SystemFont) const override;

 private:
  Holonight::ResolvedAppearance config_;
  QPalette palette_;
  QFont system_font_;
  QFont title_font_;
  QFont fixed_font_;
  QFont small_font_;
};
