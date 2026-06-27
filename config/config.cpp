// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "holonight/config.h"

#include <QDebug>
#include <QFile>
#include <QGuiApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSettings>
#include <QStandardPaths>
#include <QStyleHints>
#include <QtGlobal>

#include <algorithm>

namespace {

constexpr int kMinFontSize = 6;
constexpr int kMaxFontSize = 48;
constexpr qreal kMinScaleFactor = 0.5;
constexpr qreal kMaxScaleFactor = 3.0;

[[nodiscard]] QString cleanString(const QString& value) { return value.trimmed(); }

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

void setAppearanceModeIfValid(Holonight::AppearanceMode* target, const QString& value) {
  const QString cleaned = cleanString(value).toLower();
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

void readJsonObject(Holonight::ThemeConfig* config, const QJsonObject& root) {
  const QJsonObject appearance = root.value(QStringLiteral("appearance")).toObject();
  setAppearanceModeIfValid(&config->appearance_mode, appearance.value(QStringLiteral("mode")).toString());

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

  if (settings.status() != QSettings::NoError) {
    qWarning() << "Failed to read Holonight config" << path;
  }
}

void readConfigFile(Holonight::ThemeConfig* config, const QString& path) {
  QFile file = QFile{path};
  if (!file.exists() || !file.open(QIODevice::ReadOnly | QIODevice::Text)) {
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

}  // namespace

namespace Holonight {

int ThemeConfig::captionSize() const { return (std::max)(kMinFontSize, base_font_size - 1); }

int ThemeConfig::bodySize() const { return base_font_size; }

int ThemeConfig::titleSize() const { return (std::min)(kMaxFontSize, base_font_size + 3); }

int ThemeConfig::headingSize() const { return (std::min)(kMaxFontSize, base_font_size + 6); }

ColorMode ThemeConfig::resolvedColorMode() const {
  switch (appearance_mode) {
    case AppearanceMode::Dark:
      return ColorMode::Dark;
    case AppearanceMode::Light:
      return ColorMode::Light;
    case AppearanceMode::System:
      break;
  }

  const QGuiApplication* application = qobject_cast<const QGuiApplication*>(QGuiApplication::instance());
  if (application == nullptr || application->styleHints() == nullptr) {
    return ColorMode::Dark;
  }

  switch (application->styleHints()->colorScheme()) {
    case Qt::ColorScheme::Light:
      return ColorMode::Light;
    case Qt::ColorScheme::Dark:
    case Qt::ColorScheme::Unknown:
      return ColorMode::Dark;
  }
  return ColorMode::Dark;
}

ThemeConfig ThemeConfig::defaults() {
  return ThemeConfig{.appearance_mode = AppearanceMode::Dark,
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
  const QString jsonPath = base + QStringLiteral("/theme.json");
  if (QFile::exists(jsonPath)) {
    return jsonPath;
  }
  return base + QStringLiteral("/theme.conf");
}

ThemeConfig ThemeConfig::load() {
  ThemeConfig config = defaults();
  readConfigFile(&config, configFilePath());
  applyEnvironment(&config);
  return config;
}

}  // namespace Holonight
