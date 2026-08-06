// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "holonight/config.h"

#include "holonight/theme_catalog.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSettings>
#include <QStandardPaths>
#include <QtGlobal>

#include <algorithm>
#include <optional>

namespace {

constexpr int kMinFontSize = 6;
constexpr int kMaxFontSize = 48;
constexpr qreal kMinScaleFactor = 0.5;
constexpr qreal kMaxScaleFactor = 3.0;

[[nodiscard]] QString cleanString(const QString& value) { return value.trimmed(); }

[[nodiscard]] QString normalizedString(const QString& value) { return cleanString(value).toLower(); }

void setStringIfPresent(QString* target, const QString& value) {
  const QString cleaned = cleanString(value);
  if (!cleaned.isEmpty()) {
    *target = cleaned;
  }
}

void setIntIfValid(int* target, int value) {
  if (value >= kMinFontSize && value <= kMaxFontSize) {
    *target = value;
  }
}

void setScaleIfValid(qreal* target, qreal value) {
  if (value >= kMinScaleFactor && value <= kMaxScaleFactor) {
    *target = value;
  }
}

void setTransparencyIfValid(qreal* target, qreal value) {
  if (value >= 0.0 && value <= 1.0) {
    *target = value;
  }
}

void setAppearanceModeIfValid(Holonight::AppearanceMode* target, const QString& value) {
  const QString cleaned = normalizedString(value);
  if (cleaned == QStringLiteral("dark")) {
    *target = Holonight::AppearanceMode::Dark;
  } else if (cleaned == QStringLiteral("light")) {
    *target = Holonight::AppearanceMode::Light;
  } else if (cleaned == QStringLiteral("system")) {
    *target = Holonight::AppearanceMode::System;
  }
}

[[nodiscard]] QString envString(const char* name) { return cleanString(QString::fromLocal8Bit(qgetenv(name))); }

[[nodiscard]] bool envInt(const char* name, int* value) {
  bool ok = false;
  const int parsed = envString(name).toInt(&ok);
  if (ok) {
    *value = parsed;
  }
  return ok;
}

[[nodiscard]] bool envReal(const char* name, qreal* value) {
  bool ok = false;
  const qreal parsed = envString(name).toDouble(&ok);
  if (ok) {
    *value = parsed;
  }
  return ok;
}

void readIniFile(Holonight::ThemeConfig* config, const QString& path) {
  QSettings settings = QSettings{path, QSettings::IniFormat};
  setAppearanceModeIfValid(&config->appearance_mode, settings.value(QStringLiteral("appearance/mode")).toString());
  setStringIfPresent(&config->scheme, settings.value(QStringLiteral("appearance/scheme")).toString());
  setStringIfPresent(&config->accent, settings.value(QStringLiteral("appearance/accent")).toString());
  setStringIfPresent(&config->icon_theme, settings.value(QStringLiteral("icons/theme")).toString());
  setStringIfPresent(&config->fallback_icon_theme, settings.value(QStringLiteral("icons/fallback")).toString());
  setStringIfPresent(&config->ui_font, settings.value(QStringLiteral("fonts/ui")).toString());
  setStringIfPresent(&config->fixed_font, settings.value(QStringLiteral("fonts/fixed")).toString());
  setStringIfPresent(&config->header_font, settings.value(QStringLiteral("fonts/header")).toString());
  setStringIfPresent(&config->display_font, settings.value(QStringLiteral("fonts/display")).toString());

  bool ok = false;
  const int baseSize = settings.value(QStringLiteral("fonts/baseSize")).toInt(&ok);
  if (ok) {
    setIntIfValid(&config->base_font_size, baseSize);
  }

  const int fixedSize = settings.value(QStringLiteral("fonts/fixedSize")).toInt(&ok);
  if (ok) {
    setIntIfValid(&config->fixed_font_size, fixedSize);
  }

  const qreal scaleFactor = settings.value(QStringLiteral("scaleFactor")).toDouble(&ok);
  if (ok) {
    setScaleIfValid(&config->scale_factor, scaleFactor);
  }

  const qreal transparency = settings.value(QStringLiteral("effects/transparency")).toDouble(&ok);
  if (ok) {
    setTransparencyIfValid(&config->transparency, transparency);
  }

  if (settings.status() != QSettings::NoError) {
    qWarning() << "Failed to read Holonight config" << path;
  }
}

void applyEnvironment(Holonight::ThemeConfig* config) {
  setAppearanceModeIfValid(&config->appearance_mode, envString("HOLONIGHT_APPEARANCE_MODE"));
  setStringIfPresent(&config->icon_theme, envString("HOLONIGHT_ICON_THEME"));
  setStringIfPresent(&config->fallback_icon_theme, envString("HOLONIGHT_FALLBACK_ICON_THEME"));
  setStringIfPresent(&config->fallback_icon_theme, envString("HOLONIGHT_ICON_FALLBACK_THEME"));
  setStringIfPresent(&config->ui_font, envString("HOLONIGHT_FONT"));
  setStringIfPresent(&config->ui_font, envString("HOLONIGHT_UI_FONT"));
  setStringIfPresent(&config->fixed_font, envString("HOLONIGHT_FIXED_FONT"));
  setStringIfPresent(&config->header_font, envString("HOLONIGHT_HEADER_FONT"));
  setStringIfPresent(&config->display_font, envString("HOLONIGHT_DISPLAY_FONT"));

  int intValue = 0;
  if (envInt("HOLONIGHT_FONT_SIZE", &intValue) || envInt("HOLONIGHT_BASE_FONT_SIZE", &intValue)) {
    setIntIfValid(&config->base_font_size, intValue);
  }
  if (envInt("HOLONIGHT_FIXED_FONT_SIZE", &intValue) || envInt("HOLONIGHT_FIXED_SIZE", &intValue)) {
    setIntIfValid(&config->fixed_font_size, intValue);
  }

  qreal realValue = 0.0;
  if (envReal("HOLONIGHT_SCALE_FACTOR", &realValue)) {
    setScaleIfValid(&config->scale_factor, realValue);
  }
}

[[nodiscard]] std::optional<QString> kdeActiveSchemeId() {
  QStringList configDirs;
  const QString xdgConfigHome = envString("XDG_CONFIG_HOME");
  if (!xdgConfigHome.isEmpty()) {
    configDirs << xdgConfigHome;
  }
  configDirs << QStandardPaths::standardLocations(QStandardPaths::ConfigLocation);
  configDirs.removeDuplicates();

  for (const QString& dir : configDirs) {
    const QString path = dir + QStringLiteral("/kdeglobals");
    if (!QFile::exists(path)) {
      continue;
    }

    QFile file = QFile{path};
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      continue;
    }

    bool inGeneralGroup = false;
    const QStringList lines = QString::fromUtf8(file.readAll()).split(QLatin1Char('\n'));
    for (const QString& rawLine : lines) {
      const QString line = rawLine.trimmed();
      if (line.isEmpty() || line.startsWith(QLatin1Char('#')) || line.startsWith(QLatin1Char(';'))) {
        continue;
      }
      if (line.startsWith(QLatin1Char('[')) && line.endsWith(QLatin1Char(']'))) {
        inGeneralGroup = line.sliced(1, line.size() - 2).trimmed() == QStringLiteral("General");
        continue;
      }
      if (!inGeneralGroup || !line.startsWith(QStringLiteral("ColorScheme="))) {
        continue;
      }

      const QString scheme = Holonight::schemeIdForKdeColorSchemeName(line.sliced(12).trimmed());
      if (!scheme.isEmpty()) {
        return scheme;
      }
    }
  }
  return std::nullopt;
}

void applyKdeColorSchemeFallback(Holonight::ThemeConfig* config) {
  if (!Holonight::normalizeSchemeId(config->scheme).isEmpty() || qEnvironmentVariableIsSet("HOLONIGHT_CONFIG_FILE") ||
      qEnvironmentVariableIsSet("HOLONIGHT_APPEARANCE_MODE")) {
    return;
  }

  if (const std::optional<QString> scheme = kdeActiveSchemeId(); scheme.has_value()) {
    config->scheme = *scheme;
  }
}

}  // namespace

namespace Holonight {

int ThemeConfig::microSize() const { return (std::max)(kMinFontSize, base_font_size - 1); }

int ThemeConfig::captionSize() const { return (std::max)(kMinFontSize, base_font_size - 1); }

int ThemeConfig::bodySize() const { return base_font_size; }

int ThemeConfig::subheadingSize() const { return (std::min)(kMaxFontSize, base_font_size + 1); }

int ThemeConfig::titleSize() const { return (std::min)(kMaxFontSize, base_font_size + 3); }

int ThemeConfig::appTitleSize() const { return (std::min)(kMaxFontSize, base_font_size + 4); }

int ThemeConfig::headingSize() const { return (std::min)(kMaxFontSize, base_font_size + 6); }

int ThemeConfig::displaySize() const { return (std::min)(kMaxFontSize, base_font_size + 14); }

int ThemeConfig::fixedFontSize() const { return (std::clamp)(fixed_font_size, kMinFontSize, kMaxFontSize); }

QString ThemeConfig::resolvedHeaderFont() const { return header_font.isEmpty() ? ui_font : header_font; }

QString ThemeConfig::resolvedDisplayFont() const { return display_font.isEmpty() ? ui_font : display_font; }

ThemeSchemeKind ThemeConfig::resolvedThemeScheme() const {
  const QString normalized_scheme = normalizeSchemeId(scheme);
  if (!normalized_scheme.isEmpty()) {
    return schemeKindForSchemeId(normalized_scheme);
  }

  switch (appearance_mode) {
    case AppearanceMode::Dark:
      return ThemeSchemeKind::HoloNightDark;
    case AppearanceMode::Light:
      return ThemeSchemeKind::HoloNightLight;
    case AppearanceMode::System:
      return ThemeSchemeKind::HoloNightDark;
  }
  return ThemeSchemeKind::HoloNightDark;
}

ColorMode ThemeConfig::resolvedColorMode() const { return colorModeForScheme(resolvedThemeScheme()); }

QString ThemeConfig::resolvedAccent() const { return normalizeAccentId(accent); }

ThemeConfig ThemeConfig::defaults() {
  return ThemeConfig{.appearance_mode = AppearanceMode::Dark,
                     .scheme = {},
                     .accent = {},
                     .transparency = 1.0,
                     .icon_theme = QStringLiteral("HoloNight"),
                     .fallback_icon_theme = QStringLiteral("Papirus"),
                     .ui_font = QStringLiteral("Inter"),
                     .fixed_font = QStringLiteral("JetBrains Mono"),
                     .header_font = QStringLiteral("Audiowide"),
                     .display_font = QStringLiteral("Rajdhani"),
                     .base_font_size = 10,
                     .fixed_font_size = 10,
                     .scale_factor = 1.0};
}

QString ThemeConfig::configFilePath() {
  const QString overridePath = envString("HOLONIGHT_CONFIG_FILE");
  if (!overridePath.isEmpty()) {
    return overridePath;
  }

  const QString base = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + QStringLiteral("/holonight");
  return base + QStringLiteral("/theme.conf");
}

ThemeConfig ThemeConfig::loadFile() {
  ThemeConfig config = defaults();
  readIniFile(&config, configFilePath());
  return config;
}

ThemeConfig ThemeConfig::load() {
  ThemeConfig config = loadFile();
  applyEnvironment(&config);
  applyKdeColorSchemeFallback(&config);
  return config;
}

bool ThemeConfig::save(QString* error) const {
  const QString path = configFilePath();
  QDir directory = QFileInfo{path}.dir();
  if (!directory.exists() && !directory.mkpath(QStringLiteral("."))) {
    if (error != nullptr) {
      *error = QStringLiteral("Failed to create %1").arg(directory.absolutePath());
    }
    return false;
  }

  QSettings settings{path, QSettings::IniFormat};
  settings.setValue(QStringLiteral("appearance/mode"),
                    resolvedColorMode() == ColorMode::Light ? QStringLiteral("light") : QStringLiteral("dark"));
  settings.setValue(QStringLiteral("appearance/scheme"), scheme);
  settings.setValue(QStringLiteral("appearance/accent"), resolvedAccent());
  settings.setValue(QStringLiteral("fonts/ui"), ui_font);
  settings.setValue(QStringLiteral("fonts/fixed"), fixed_font);
  settings.setValue(QStringLiteral("fonts/baseSize"), base_font_size);
  settings.setValue(QStringLiteral("fonts/fixedSize"), fixed_font_size);
  settings.sync();
  if (settings.status() == QSettings::NoError) {
    return true;
  }
  if (error != nullptr) {
    *error = QStringLiteral("Failed to write %1").arg(path);
  }
  return false;
}

ColorMode colorModeForScheme(ThemeSchemeKind scheme) {
  const ThemeVariantCatalogEntry* variant = themeVariantForSchemeId(schemeIdForKind(scheme));
  if (variant != nullptr) {
    return variant->mode;
  }
  return ColorMode::Dark;
}

}  // namespace Holonight
