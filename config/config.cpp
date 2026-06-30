// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "holonight/config.h"

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
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
constexpr qint64 kMaxConfigFileSize = 64 * 1024;
constexpr int kConfigSchemaVersion = 1;

[[nodiscard]] QString cleanString(const QString& value) { return value.trimmed(); }

[[nodiscard]] QString normalizedString(const QString& value) { return cleanString(value).toLower(); }

[[nodiscard]] QString compactThemeName(const QString& value) {
  QString normalized = normalizedString(value);
  normalized.remove(QLatin1Char(' '));
  normalized.remove(QLatin1Char('-'));
  normalized.remove(QLatin1Char('_'));
  return normalized;
}

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

[[nodiscard]] std::optional<Holonight::ThemeSchemeKind> schemeKindFromString(const QString& value) {
  const QString normalized = normalizedString(value);
  if (normalized == QStringLiteral("holonight-dark")) {
    return Holonight::ThemeSchemeKind::HoloNightDark;
  }
  if (normalized == QStringLiteral("holonight-light")) {
    return Holonight::ThemeSchemeKind::HoloNightLight;
  }
  if (normalized == QStringLiteral("tokyonight-storm")) {
    return Holonight::ThemeSchemeKind::TokyoNightStorm;
  }
  if (normalized == QStringLiteral("tokyonight-day")) {
    return Holonight::ThemeSchemeKind::TokyoNightDay;
  }
  return std::nullopt;
}

[[nodiscard]] std::optional<QString> schemeIdFromKdeColorSchemeName(const QString& value) {
  const QString compact = compactThemeName(value);
  if (compact == QStringLiteral("holonightdark")) {
    return QStringLiteral("holonight-dark");
  }
  if (compact == QStringLiteral("holonightlight")) {
    return QStringLiteral("holonight-light");
  }
  if (compact == QStringLiteral("tokyonightstorm")) {
    return QStringLiteral("tokyonight-storm");
  }
  if (compact == QStringLiteral("tokyonightday")) {
    return QStringLiteral("tokyonight-day");
  }

  // Legacy names shipped before all four catalog variants had dedicated
  // KDE color-scheme files.
  if (compact == QStringLiteral("holonight")) {
    return QStringLiteral("tokyonight-storm");
  }
  if (compact == QStringLiteral("holonightday")) {
    return QStringLiteral("tokyonight-day");
  }

  return std::nullopt;
}

[[nodiscard]] bool isValidAccent(const QString& value) {
  const QString normalized = normalizedString(value);
  return normalized == QStringLiteral("cyan") || normalized == QStringLiteral("blue") ||
         normalized == QStringLiteral("violet") || normalized == QStringLiteral("yellow");
}

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

void readJsonObject(Holonight::ThemeConfig* config, const QJsonObject& root) {
  if (root.contains(QStringLiteral("version")) &&
      root.value(QStringLiteral("version")).toInt(kConfigSchemaVersion) != kConfigSchemaVersion) {
    qWarning() << "Unsupported Holonight config version" << root.value(QStringLiteral("version"));
    return;
  }

  const QJsonObject appearance = root.value(QStringLiteral("appearance")).toObject();
  setAppearanceModeIfValid(&config->appearance_mode, appearance.value(QStringLiteral("mode")).toString());
  setStringIfPresent(&config->scheme, appearance.value(QStringLiteral("scheme")).toString());
  setStringIfPresent(&config->accent, appearance.value(QStringLiteral("accent")).toString());

  const QJsonObject effects = root.value(QStringLiteral("effects")).toObject();
  if (effects.contains(QStringLiteral("transparency"))) {
    setTransparencyIfValid(&config->transparency, effects.value(QStringLiteral("transparency")).toDouble(1.0));
  }

  const QJsonObject icons = root.value(QStringLiteral("icons")).toObject();
  setStringIfPresent(&config->icon_theme, icons.value(QStringLiteral("theme")).toString());
  setStringIfPresent(&config->fallback_icon_theme, icons.value(QStringLiteral("fallback")).toString());

  const QJsonObject fonts = root.value(QStringLiteral("fonts")).toObject();
  setStringIfPresent(&config->ui_font, fonts.value(QStringLiteral("ui")).toString());
  setStringIfPresent(&config->fixed_font, fonts.value(QStringLiteral("fixed")).toString());

  if (fonts.contains(QStringLiteral("baseSize"))) {
    setIntIfValid(&config->base_font_size, fonts.value(QStringLiteral("baseSize")).toInt(config->base_font_size));
  }
  if (root.contains(QStringLiteral("baseFontSize"))) {
    setIntIfValid(&config->base_font_size, root.value(QStringLiteral("baseFontSize")).toInt(config->base_font_size));
  }
  if (root.contains(QStringLiteral("scaleFactor"))) {
    setScaleIfValid(&config->scale_factor, root.value(QStringLiteral("scaleFactor")).toDouble(config->scale_factor));
  }
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

  bool ok = false;
  const int baseSize = settings.value(QStringLiteral("fonts/baseSize")).toInt(&ok);
  if (ok) {
    setIntIfValid(&config->base_font_size, baseSize);
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

void readConfigFile(Holonight::ThemeConfig* config, const QString& path) {
  QFile file = QFile{path};
  if (!file.exists()) {
    return;
  }
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    qWarning() << "Failed to open Holonight config" << path << file.errorString();
    return;
  }
  if (file.size() > kMaxConfigFileSize) {
    qWarning() << "Ignoring oversized Holonight config" << path << file.size();
    return;
  }

  const QByteArray bytes = file.readAll();
  if (file.error() != QFileDevice::NoError) {
    qWarning() << "Failed to read Holonight config" << path << file.errorString();
    return;
  }

  QJsonParseError error = QJsonParseError{};
  const QJsonDocument document = QJsonDocument::fromJson(bytes, &error);
  if (error.error == QJsonParseError::NoError && document.isObject()) {
    readJsonObject(config, document.object());
    return;
  }
  if (QFileInfo{path}.suffix().compare(QStringLiteral("json"), Qt::CaseInsensitive) == 0) {
    qWarning() << "Failed to parse Holonight JSON config" << path << error.errorString();
    return;
  }

  readIniFile(config, path);
}

void applyEnvironment(Holonight::ThemeConfig* config) {
  setAppearanceModeIfValid(&config->appearance_mode, envString("HOLONIGHT_APPEARANCE_MODE"));
  setStringIfPresent(&config->icon_theme, envString("HOLONIGHT_ICON_THEME"));
  setStringIfPresent(&config->fallback_icon_theme, envString("HOLONIGHT_FALLBACK_ICON_THEME"));
  setStringIfPresent(&config->fallback_icon_theme, envString("HOLONIGHT_ICON_FALLBACK_THEME"));
  setStringIfPresent(&config->ui_font, envString("HOLONIGHT_FONT"));
  setStringIfPresent(&config->ui_font, envString("HOLONIGHT_UI_FONT"));
  setStringIfPresent(&config->fixed_font, envString("HOLONIGHT_FIXED_FONT"));

  int intValue = 0;
  if (envInt("HOLONIGHT_FONT_SIZE", &intValue) || envInt("HOLONIGHT_BASE_FONT_SIZE", &intValue)) {
    setIntIfValid(&config->base_font_size, intValue);
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

    QFile file{path};
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

      if (const std::optional<QString> scheme = schemeIdFromKdeColorSchemeName(line.sliced(12).trimmed());
          scheme.has_value()) {
        return scheme;
      }
    }
  }
  return std::nullopt;
}

void applyKdeColorSchemeFallback(Holonight::ThemeConfig* config) {
  if (schemeKindFromString(config->scheme).has_value() || qEnvironmentVariableIsSet("HOLONIGHT_CONFIG_FILE") ||
      qEnvironmentVariableIsSet("HOLONIGHT_APPEARANCE_MODE")) {
    return;
  }

  if (const std::optional<QString> scheme = kdeActiveSchemeId(); scheme.has_value()) {
    config->scheme = *scheme;
  }
}

}  // namespace

namespace Holonight {

int ThemeConfig::captionSize() const { return (std::max)(kMinFontSize, base_font_size - 1); }

int ThemeConfig::bodySize() const { return base_font_size; }

int ThemeConfig::titleSize() const { return (std::min)(kMaxFontSize, base_font_size + 3); }

int ThemeConfig::headingSize() const { return (std::min)(kMaxFontSize, base_font_size + 6); }

ThemeSchemeKind ThemeConfig::resolvedThemeScheme() const {
  if (const std::optional<ThemeSchemeKind> parsed = schemeKindFromString(scheme); parsed.has_value()) {
    return *parsed;
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

QString ThemeConfig::resolvedAccent() const {
  if (isValidAccent(accent)) {
    return normalizedString(accent);
  }
  return QStringLiteral("cyan");
}

ThemeConfig ThemeConfig::defaults() {
  return ThemeConfig{.appearance_mode = AppearanceMode::Dark,
                     .scheme = {},
                     .accent = {},
                     .transparency = 1.0,
                     .icon_theme = QStringLiteral("HoloNight"),
                     .fallback_icon_theme = QStringLiteral("Papirus"),
                     .ui_font = QStringLiteral("Inter"),
                     .fixed_font = QStringLiteral("JetBrains Mono"),
                     .base_font_size = 10,
                     .scale_factor = 1.0};
}

QString ThemeConfig::configFilePath() {
  const QString overridePath = envString("HOLONIGHT_CONFIG_FILE");
  if (!overridePath.isEmpty()) {
    return overridePath;
  }

  const QString base = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + QStringLiteral("/holonight");
  const QString iniPath = base + QStringLiteral("/theme.conf");
  if (QFile::exists(iniPath)) {
    return iniPath;
  }
  const QString jsonPath = base + QStringLiteral("/theme.json");
  if (QFile::exists(jsonPath)) {
    return jsonPath;
  }
  return iniPath;
}

ThemeConfig ThemeConfig::load() {
  ThemeConfig config = defaults();
  readConfigFile(&config, configFilePath());
  applyEnvironment(&config);
  applyKdeColorSchemeFallback(&config);
  return config;
}

ColorMode colorModeForScheme(ThemeSchemeKind scheme) {
  switch (scheme) {
    case ThemeSchemeKind::HoloNightLight:
    case ThemeSchemeKind::TokyoNightDay:
      return ColorMode::Light;
    case ThemeSchemeKind::HoloNightDark:
    case ThemeSchemeKind::TokyoNightStorm:
      return ColorMode::Dark;
  }
  return ColorMode::Dark;
}

}  // namespace Holonight
