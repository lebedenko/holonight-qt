// SPDX-License-Identifier: GPL-3.0-or-later
#include "qt5theme.h"

#include "holonight/palette.h"
#include "qt5probeappearance.h"
#include "themeresolver.h"

#include <QIcon>
#include <QStandardPaths>
#include <QVariant>
HoloniightQt5Theme::HoloniightQt5Theme()
    : config_{Holonight::qt5ProbeAppearance()},
      palette_{Holonight::buildPalette(Holonight::ThemeResolver::resolve(config_))},
      system_font_{config_.ui_font, config_.bodySize()},
      title_font_{config_.title_font, config_.title_font_size},
      fixed_font_{config_.monospace_font, config_.monospace_font_size},
      small_font_{config_.ui_font, config_.captionSize()} {
  QIcon::setThemeName(config_.icon_theme);
  QIcon::setFallbackThemeName(config_.fallback_icon_theme);
}
const QPalette* HoloniightQt5Theme::palette(Palette) const { return &palette_; }
QVariant HoloniightQt5Theme::themeHint(ThemeHint hint) const {
  switch (hint) {
    case StyleNames:
      return QStringList{QStringLiteral("Holonight"), QStringLiteral("Fusion")};
    case SystemIconThemeName:
      return config_.icon_theme;
    case SystemIconFallbackThemeName:
      return config_.fallback_icon_theme;
    case IconThemeSearchPaths: {
      QStringList paths;
      for (const QString& directory : QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation))
        paths << directory + QStringLiteral("/icons");
      return paths;
    }
    default:
      return QPlatformTheme::themeHint(hint);
  }
}
const QFont* HoloniightQt5Theme::font(Font type) const {
  switch (type) {
    case SystemFont:
      return &system_font_;
    case TitleBarFont:
      return &title_font_;
    case FixedFont:
      return &fixed_font_;
    case SmallFont:
    case MiniFont:
      return &small_font_;
    default:
      return QPlatformTheme::font(type);
  }
}
