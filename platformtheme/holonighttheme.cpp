// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "holonighttheme.h"

#include "holonight/palette.h"

#include <QIcon>
#include <QVariant>

HoloniightTheme::HoloniightTheme()
    : font_system_{QStringLiteral("Inter"), 12},
      font_title_{QStringLiteral("Inter"), 12},
      font_fixed_{QStringLiteral("JetBrainsMono Nerd Font"), 12},
      font_small_{QStringLiteral("Inter"), 10},
      font_mini_{QStringLiteral("Inter"), 8} {
  QIcon::setThemeName(QStringLiteral("Papirus-Dark"));
  QIcon::setFallbackThemeName(QStringLiteral("breeze-dark"));
}

const QPalette* HoloniightTheme::palette(Palette /*type*/) const {
  static const QPalette kPalette = Holonight::buildPalette(Holonight::darkTokens());
  return &kPalette;
}

QVariant HoloniightTheme::themeHint(ThemeHint hint) const {
  switch (hint) {
    case StyleNames:
      return QStringList{QStringLiteral("Holonight"), QStringLiteral("Fusion")};
    default:
      return QPlatformTheme::themeHint(hint);
  }
}

Qt::ColorScheme HoloniightTheme::colorScheme() const { return Qt::ColorScheme::Dark; }

const QFont* HoloniightTheme::font(Font type) const {
  switch (type) {
    case SystemFont:
      return &font_system_;
    case TitleBarFont:
      return &font_title_;
    case FixedFont:
      return &font_fixed_;
    case SmallFont:
      return &font_small_;
    case MiniFont:
      return &font_mini_;
    default:
      return QPlatformTheme::font(type);
  }
}
