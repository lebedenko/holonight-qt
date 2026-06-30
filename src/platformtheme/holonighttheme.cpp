// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "holonighttheme.h"

#include "holonight/palette.h"
#include "themeresolver.h"

#include <QIcon>
#include <QStandardPaths>
#include <QVariant>

HoloniightTheme::HoloniightTheme()
    : config_{Holonight::ThemeConfig::load()},
      color_mode_{config_.resolvedColorMode()},
      palette_{Holonight::buildPalette(Holonight::ThemeResolver::resolve(config_))},
      font_system_{config_.ui_font, config_.bodySize()},
      font_title_{config_.ui_font, config_.titleSize()},
      font_fixed_{config_.fixed_font, config_.bodySize()},
      font_small_{config_.ui_font, config_.captionSize()},
      font_mini_{config_.ui_font, config_.captionSize()} {
  QIcon::setThemeName(config_.icon_theme);
  QIcon::setFallbackThemeName(config_.fallback_icon_theme);
}

const QPalette* HoloniightTheme::palette(Palette /*type*/) const { return &palette_; }

QVariant HoloniightTheme::themeHint(ThemeHint hint) const {
  switch (hint) {
    case StyleNames:
      return QStringList{QStringLiteral("Holonight"), QStringLiteral("Fusion")};
    case SystemIconThemeName:
      return config_.icon_theme;
    case SystemIconFallbackThemeName:
      return config_.fallback_icon_theme;
    case IconThemeSearchPaths: {
      static const QStringList kIconThemeSearchPaths = [] {
        QStringList paths;
        const auto dataDirs = QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation);
        for (const QString& dir : dataDirs) {
          paths << dir + QStringLiteral("/icons");
        }
        paths << QStringLiteral(":/icons");
        return paths;
      }();
      return kIconThemeSearchPaths;
    }
    default:
      return QPlatformTheme::themeHint(hint);
  }
}

Qt::ColorScheme HoloniightTheme::colorScheme() const {
  return color_mode_ == Holonight::ColorMode::Light ? Qt::ColorScheme::Light : Qt::ColorScheme::Dark;
}

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
