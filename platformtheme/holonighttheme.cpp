// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "holonighttheme.h"

#include "holonight/palette.h"

#include <QVariant>

HoloniightTheme::HoloniightTheme() : font_{QStringLiteral("Inter"), 10} {}

const QPalette* HoloniightTheme::palette(Palette /*type*/) const {
  static const QPalette kPalette = Holonight::buildPalette(Holonight::darkTokens());
  return &kPalette;
}

QVariant HoloniightTheme::themeHint(ThemeHint hint) const {
  switch (hint) {
    case StyleNames:
      return QStringList{QStringLiteral("holonight"), QStringLiteral("Fusion")};
    case SystemIconThemeName:
      return QStringLiteral("papirus");
    case SystemIconFallbackThemeName:
      return QStringLiteral("hicolor");
    default:
      return QPlatformTheme::themeHint(hint);
  }
}

Qt::ColorScheme HoloniightTheme::colorScheme() const { return Qt::ColorScheme::Dark; }

const QFont* HoloniightTheme::font(Font /*type*/) const { return &font_; }
