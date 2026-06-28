// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "holonight/config.h"

#include <QDir>
#include <QFile>
#include <QGuiApplication>
#include <QStyleHints>
#include <QTemporaryDir>

#include <gtest/gtest.h>

namespace {

class EnvGuard {
 public:
  explicit EnvGuard(const char* name) : name_{name}, had_value_{qEnvironmentVariableIsSet(name)} {
    if (had_value_) {
      old_value_ = qgetenv(name);
    }
  }
  EnvGuard(const EnvGuard&) = delete;
  EnvGuard& operator=(const EnvGuard&) = delete;
  EnvGuard(EnvGuard&&) = delete;
  EnvGuard& operator=(EnvGuard&&) = delete;

  ~EnvGuard() {
    if (had_value_) {
      qputenv(name_, old_value_);
    } else {
      qunsetenv(name_);
    }
  }

 private:
  const char* name_;
  bool had_value_;
  QByteArray old_value_;
};

void writeFile(const QString& path, const QByteArray& contents) {
  QFile file = QFile{path};
  const bool opened = file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
  ASSERT_TRUE(opened);
  ASSERT_EQ(file.write(contents), contents.size());
}

}  // namespace

TEST(ThemeConfig, DefaultsMatchDocumentedValues) {
  const Holonight::ThemeConfig config = Holonight::ThemeConfig::defaults();
  EXPECT_EQ(config.icon_theme, QStringLiteral("HoloNight"));
  EXPECT_EQ(config.fallback_icon_theme, QStringLiteral("Papirus"));
  EXPECT_EQ(config.ui_font, QStringLiteral("Inter"));
  EXPECT_EQ(config.fixed_font, QStringLiteral("JetBrains Mono"));
  EXPECT_EQ(config.base_font_size, 10);
  EXPECT_EQ(config.captionSize(), 9);
  EXPECT_EQ(config.bodySize(), 10);
  EXPECT_EQ(config.titleSize(), 13);
  EXPECT_EQ(config.headingSize(), 16);
}

TEST(ThemeConfig, LoadsJsonConfigFile) {
  EnvGuard configFileGuard = EnvGuard{"HOLONIGHT_CONFIG_FILE"};
  EnvGuard iconGuard = EnvGuard{"HOLONIGHT_ICON_THEME"};
  EnvGuard fallbackGuard = EnvGuard{"HOLONIGHT_FALLBACK_ICON_THEME"};
  EnvGuard fontGuard = EnvGuard{"HOLONIGHT_FONT"};
  EnvGuard fixedFontGuard = EnvGuard{"HOLONIGHT_FIXED_FONT"};
  EnvGuard sizeGuard = EnvGuard{"HOLONIGHT_FONT_SIZE"};
  EnvGuard scaleGuard = EnvGuard{"HOLONIGHT_SCALE_FACTOR"};
  EnvGuard appearanceGuard = EnvGuard{"HOLONIGHT_APPEARANCE_MODE"};

  qunsetenv("HOLONIGHT_ICON_THEME");
  qunsetenv("HOLONIGHT_FALLBACK_ICON_THEME");
  qunsetenv("HOLONIGHT_FONT");
  qunsetenv("HOLONIGHT_FIXED_FONT");
  qunsetenv("HOLONIGHT_FONT_SIZE");
  qunsetenv("HOLONIGHT_SCALE_FACTOR");
  qunsetenv("HOLONIGHT_APPEARANCE_MODE");

  QTemporaryDir dir;
  ASSERT_TRUE(dir.isValid());
  const QString path = dir.filePath(QStringLiteral("theme.json"));
  writeFile(path, R"({
    "icons": { "theme": "Papirus-Dark", "fallback": "Adwaita" },
    "appearance": { "mode": "light" },
    "fonts": { "ui": "Noto Sans", "fixed": "Iosevka", "baseSize": 11 },
    "scaleFactor": 1.25
  })");
  qputenv("HOLONIGHT_CONFIG_FILE", path.toLocal8Bit());

  const Holonight::ThemeConfig config = Holonight::ThemeConfig::load();
  EXPECT_EQ(config.icon_theme, QStringLiteral("Papirus-Dark"));
  EXPECT_EQ(config.fallback_icon_theme, QStringLiteral("Adwaita"));
  EXPECT_EQ(config.ui_font, QStringLiteral("Noto Sans"));
  EXPECT_EQ(config.fixed_font, QStringLiteral("Iosevka"));
  EXPECT_EQ(config.base_font_size, 11);
  EXPECT_DOUBLE_EQ(config.scale_factor, 1.25);
  EXPECT_EQ(config.appearance_mode, Holonight::AppearanceMode::Light);
  EXPECT_EQ(config.resolvedColorMode(), Holonight::ColorMode::Light);
}

TEST(ThemeConfig, EnvironmentOverridesConfigFile) {
  EnvGuard configFileGuard = EnvGuard{"HOLONIGHT_CONFIG_FILE"};
  EnvGuard iconGuard = EnvGuard{"HOLONIGHT_ICON_THEME"};
  EnvGuard fallbackGuard = EnvGuard{"HOLONIGHT_ICON_FALLBACK_THEME"};
  EnvGuard fontGuard = EnvGuard{"HOLONIGHT_FONT"};
  EnvGuard fixedFontGuard = EnvGuard{"HOLONIGHT_FIXED_FONT"};
  EnvGuard sizeGuard = EnvGuard{"HOLONIGHT_FONT_SIZE"};
  EnvGuard scaleGuard = EnvGuard{"HOLONIGHT_SCALE_FACTOR"};
  EnvGuard appearanceGuard = EnvGuard{"HOLONIGHT_APPEARANCE_MODE"};

  QTemporaryDir dir;
  ASSERT_TRUE(dir.isValid());
  const QString path = dir.filePath(QStringLiteral("theme.json"));
  writeFile(path, R"({
    "icons": { "theme": "ConfigIcons", "fallback": "ConfigFallback" },
    "appearance": { "mode": "dark" },
    "fonts": { "ui": "Config Sans", "fixed": "Config Mono", "baseSize": 10 },
    "scaleFactor": 1.0
  })");

  qputenv("HOLONIGHT_CONFIG_FILE", path.toLocal8Bit());
  qputenv("HOLONIGHT_ICON_THEME", "EnvIcons");
  qputenv("HOLONIGHT_ICON_FALLBACK_THEME", "EnvFallback");
  qputenv("HOLONIGHT_FONT", "Env Sans");
  qputenv("HOLONIGHT_FIXED_FONT", "Env Mono");
  qputenv("HOLONIGHT_FONT_SIZE", "12");
  qputenv("HOLONIGHT_SCALE_FACTOR", "1.5");
  qputenv("HOLONIGHT_APPEARANCE_MODE", "light");

  const Holonight::ThemeConfig config = Holonight::ThemeConfig::load();
  EXPECT_EQ(config.icon_theme, QStringLiteral("EnvIcons"));
  EXPECT_EQ(config.fallback_icon_theme, QStringLiteral("EnvFallback"));
  EXPECT_EQ(config.ui_font, QStringLiteral("Env Sans"));
  EXPECT_EQ(config.fixed_font, QStringLiteral("Env Mono"));
  EXPECT_EQ(config.base_font_size, 12);
  EXPECT_DOUBLE_EQ(config.scale_factor, 1.5);
  EXPECT_EQ(config.appearance_mode, Holonight::AppearanceMode::Light);
  EXPECT_EQ(config.resolvedColorMode(), Holonight::ColorMode::Light);
}

TEST(ThemeConfig, LoadsIniAppearanceMode) {
  EnvGuard configFileGuard = EnvGuard{"HOLONIGHT_CONFIG_FILE"};
  EnvGuard appearanceGuard = EnvGuard{"HOLONIGHT_APPEARANCE_MODE"};
  qunsetenv("HOLONIGHT_APPEARANCE_MODE");

  QTemporaryDir dir;
  ASSERT_TRUE(dir.isValid());
  const QString path = dir.filePath(QStringLiteral("theme.conf"));
  writeFile(path, R"(
[appearance]
mode=light
)");
  qputenv("HOLONIGHT_CONFIG_FILE", path.toLocal8Bit());

  const Holonight::ThemeConfig config = Holonight::ThemeConfig::load();
  EXPECT_EQ(config.appearance_mode, Holonight::AppearanceMode::Light);
  EXPECT_EQ(config.resolvedColorMode(), Holonight::ColorMode::Light);
}

TEST(ThemeConfig, ConfigFilePathPrefersIniOverLegacyJson) {
  EnvGuard configFileGuard = EnvGuard{"HOLONIGHT_CONFIG_FILE"};
  EnvGuard xdgGuard = EnvGuard{"XDG_CONFIG_HOME"};
  qunsetenv("HOLONIGHT_CONFIG_FILE");

  QTemporaryDir dir;
  ASSERT_TRUE(dir.isValid());
  qputenv("XDG_CONFIG_HOME", dir.path().toLocal8Bit());
  const QString config_dir = dir.filePath(QStringLiteral("holonight"));
  ASSERT_TRUE(QDir{}.mkpath(config_dir));
  const QString ini_path = config_dir + QStringLiteral("/theme.conf");
  const QString json_path = config_dir + QStringLiteral("/theme.json");
  writeFile(ini_path, "[appearance]\nmode=light\n");
  writeFile(json_path, R"({ "appearance": { "mode": "dark" } })");

  EXPECT_EQ(Holonight::ThemeConfig::configFilePath(), ini_path);
  const Holonight::ThemeConfig config = Holonight::ThemeConfig::load();
  EXPECT_EQ(config.appearance_mode, Holonight::AppearanceMode::Light);
}

TEST(ThemeConfig, InvalidAppearanceFallsBackToDark) {
  EnvGuard configFileGuard = EnvGuard{"HOLONIGHT_CONFIG_FILE"};
  EnvGuard appearanceGuard = EnvGuard{"HOLONIGHT_APPEARANCE_MODE"};

  QTemporaryDir dir;
  ASSERT_TRUE(dir.isValid());
  const QString path = dir.filePath(QStringLiteral("theme.json"));
  writeFile(path, R"({ "appearance": { "mode": "invalid" } })");
  qputenv("HOLONIGHT_CONFIG_FILE", path.toLocal8Bit());
  qputenv("HOLONIGHT_APPEARANCE_MODE", "also-invalid");

  const Holonight::ThemeConfig config = Holonight::ThemeConfig::load();
  EXPECT_EQ(config.appearance_mode, Holonight::AppearanceMode::Dark);
  EXPECT_EQ(config.resolvedColorMode(), Holonight::ColorMode::Dark);
}

TEST(ThemeConfig, MalformedJsonDoesNotFallBackToIni) {
  EnvGuard configFileGuard = EnvGuard{"HOLONIGHT_CONFIG_FILE"};

  QTemporaryDir dir;
  ASSERT_TRUE(dir.isValid());
  const QString path = dir.filePath(QStringLiteral("theme.json"));
  writeFile(path, R"(
[appearance]
mode=light
)");
  qputenv("HOLONIGHT_CONFIG_FILE", path.toLocal8Bit());

  const Holonight::ThemeConfig config = Holonight::ThemeConfig::load();
  EXPECT_EQ(config.appearance_mode, Holonight::AppearanceMode::Dark);
  EXPECT_EQ(config.resolvedColorMode(), Holonight::ColorMode::Dark);
}

TEST(ThemeConfig, OversizedConfigFallsBackToDefaults) {
  EnvGuard configFileGuard = EnvGuard{"HOLONIGHT_CONFIG_FILE"};

  QTemporaryDir dir;
  ASSERT_TRUE(dir.isValid());
  const QString path = dir.filePath(QStringLiteral("theme.json"));
  writeFile(path, QByteArray(70 * 1024, '{'));
  qputenv("HOLONIGHT_CONFIG_FILE", path.toLocal8Bit());

  const Holonight::ThemeConfig config = Holonight::ThemeConfig::load();
  EXPECT_EQ(config.icon_theme, Holonight::ThemeConfig::defaults().icon_theme);
  EXPECT_EQ(config.appearance_mode, Holonight::AppearanceMode::Dark);
}

TEST(ThemeConfig, UnsupportedConfigVersionFallsBackToDefaults) {
  EnvGuard configFileGuard = EnvGuard{"HOLONIGHT_CONFIG_FILE"};

  QTemporaryDir dir;
  ASSERT_TRUE(dir.isValid());
  const QString path = dir.filePath(QStringLiteral("theme.json"));
  writeFile(path, R"({
    "version": 2,
    "appearance": { "mode": "light" }
  })");
  qputenv("HOLONIGHT_CONFIG_FILE", path.toLocal8Bit());

  const Holonight::ThemeConfig config = Holonight::ThemeConfig::load();
  EXPECT_EQ(config.appearance_mode, Holonight::AppearanceMode::Dark);
  EXPECT_EQ(config.resolvedColorMode(), Holonight::ColorMode::Dark);
}

TEST(ThemeConfig, DefaultsToDarkAppearance) {
  const Holonight::ThemeConfig config = Holonight::ThemeConfig::defaults();
  EXPECT_EQ(config.appearance_mode, Holonight::AppearanceMode::Dark);
  EXPECT_EQ(config.resolvedColorMode(), Holonight::ColorMode::Dark);
}

TEST(ThemeConfig, SystemAppearanceWithoutPreferenceFallsBackToDark) {
  Holonight::ThemeConfig config = Holonight::ThemeConfig::defaults();
  config.appearance_mode = Holonight::AppearanceMode::System;
  const QGuiApplication* application = qobject_cast<const QGuiApplication*>(QGuiApplication::instance());
  if (application != nullptr && application->styleHints()->colorScheme() == Qt::ColorScheme::Light) {
    EXPECT_EQ(config.resolvedColorMode(), Holonight::ColorMode::Light);
  } else {
    EXPECT_EQ(config.resolvedColorMode(), Holonight::ColorMode::Dark);
  }
}
