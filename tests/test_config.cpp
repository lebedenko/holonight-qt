// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "holonight/config.h"
#include "holonight/theme_catalog.h"

#include <QDir>
#include <QFile>
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
  EXPECT_EQ(config.fixed_font_size, 10);
  EXPECT_EQ(config.microSize(), 9);
  EXPECT_EQ(config.captionSize(), 9);
  EXPECT_EQ(config.bodySize(), 10);
  EXPECT_EQ(config.subheadingSize(), 11);
  EXPECT_EQ(config.titleSize(), 13);
  EXPECT_EQ(config.appTitleSize(), 14);
  EXPECT_EQ(config.headingSize(), 16);
  EXPECT_EQ(config.displaySize(), 24);
  EXPECT_EQ(config.fixedFontSize(), 10);
  EXPECT_EQ(config.resolvedHeaderFont(), QStringLiteral("Audiowide"));
  EXPECT_EQ(config.resolvedDisplayFont(), QStringLiteral("Rajdhani"));
}

TEST(ThemeConfig, AppTitleSizeClampsToFontMaximum) {
  Holonight::ThemeConfig config = Holonight::ThemeConfig::defaults();
  config.base_font_size = 45;
  EXPECT_EQ(config.appTitleSize(), 48);
  config.base_font_size = 48;
  EXPECT_EQ(config.appTitleSize(), 48);
}

TEST(ThemeConfig, LoadsConfigFile) {
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
  const QString path = dir.filePath(QStringLiteral("theme.conf"));
  writeFile(path,
            "scaleFactor=1.25\n[icons]\ntheme=Papirus-Dark\nfallback=Adwaita\n[appearance]\nscheme=holonight-day\n"
            "accent=violet\nmode=light\n[fonts]\nui=Noto Sans\nfixed=Iosevka\nbaseSize=11\n");
  qputenv("HOLONIGHT_CONFIG_FILE", path.toLocal8Bit());

  const Holonight::ThemeConfig config = Holonight::ThemeConfig::load();
  EXPECT_EQ(config.icon_theme, QStringLiteral("Papirus-Dark"));
  EXPECT_EQ(config.fallback_icon_theme, QStringLiteral("Adwaita"));
  EXPECT_EQ(config.ui_font, QStringLiteral("Noto Sans"));
  EXPECT_EQ(config.fixed_font, QStringLiteral("Iosevka"));
  EXPECT_EQ(config.base_font_size, 11);
  EXPECT_DOUBLE_EQ(config.scale_factor, 1.25);
  EXPECT_EQ(config.appearance_mode, Holonight::AppearanceMode::Light);
  EXPECT_EQ(config.scheme, QStringLiteral("holonight-day"));
  EXPECT_EQ(config.accent, QStringLiteral("violet"));
  EXPECT_EQ(config.resolvedThemeScheme(), Holonight::ThemeSchemeKind::HoloNightDay);
  EXPECT_EQ(config.resolvedColorMode(), Holonight::ColorMode::Light);
  EXPECT_EQ(config.resolvedAccent(), QStringLiteral("violet"));
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
  const QString path = dir.filePath(QStringLiteral("theme.conf"));
  writeFile(path,
            "[icons]\ntheme=ConfigIcons\nfallback=ConfigFallback\n[appearance]\nmode=dark\n"
            "[fonts]\nui=Config Sans\nfixed=Config Mono\nbaseSize=10\nscaleFactor=1.0\n");

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

TEST(ThemeConfig, LoadsSpecialHeaderAndDisplayFontsAndFixedSize) {
  EnvGuard configFileGuard = EnvGuard{"HOLONIGHT_CONFIG_FILE"};
  EnvGuard headerGuard = EnvGuard{"HOLONIGHT_HEADER_FONT"};
  EnvGuard displayGuard = EnvGuard{"HOLONIGHT_DISPLAY_FONT"};
  EnvGuard fixedSizeGuard = EnvGuard{"HOLONIGHT_FIXED_FONT_SIZE"};

  qunsetenv("HOLONIGHT_HEADER_FONT");
  qunsetenv("HOLONIGHT_DISPLAY_FONT");
  qunsetenv("HOLONIGHT_FIXED_FONT_SIZE");

  QTemporaryDir dir;
  ASSERT_TRUE(dir.isValid());
  const QString path = dir.filePath(QStringLiteral("theme.conf"));
  writeFile(path, "[fonts]\nui=Inter\nfixed=JetBrains Mono\nheader=Space Grotesk\ndisplay=Outfit\nfixedSize=14\n");

  qputenv("HOLONIGHT_CONFIG_FILE", path.toLocal8Bit());

  Holonight::ThemeConfig config = Holonight::ThemeConfig::load();
  EXPECT_EQ(config.resolvedHeaderFont(), QStringLiteral("Space Grotesk"));
  EXPECT_EQ(config.resolvedDisplayFont(), QStringLiteral("Outfit"));
  EXPECT_EQ(config.fixedFontSize(), 14);

  qputenv("HOLONIGHT_HEADER_FONT", "Env Header");
  qputenv("HOLONIGHT_DISPLAY_FONT", "Env Display");
  qputenv("HOLONIGHT_FIXED_FONT_SIZE", "16");

  config = Holonight::ThemeConfig::load();
  EXPECT_EQ(config.resolvedHeaderFont(), QStringLiteral("Env Header"));
  EXPECT_EQ(config.resolvedDisplayFont(), QStringLiteral("Env Display"));
  EXPECT_EQ(config.fixedFontSize(), 16);
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

TEST(ThemeConfig, LoadsIniSchemeAndAccent) {
  EnvGuard configFileGuard = EnvGuard{"HOLONIGHT_CONFIG_FILE"};
  EnvGuard appearanceGuard = EnvGuard{"HOLONIGHT_APPEARANCE_MODE"};
  qunsetenv("HOLONIGHT_APPEARANCE_MODE");

  QTemporaryDir dir;
  ASSERT_TRUE(dir.isValid());
  const QString path = dir.filePath(QStringLiteral("theme.conf"));
  writeFile(path, R"(
[appearance]
scheme=  HoloNight-Storm
accent=  Blue
mode=light
)");
  qputenv("HOLONIGHT_CONFIG_FILE", path.toLocal8Bit());

  const Holonight::ThemeConfig config = Holonight::ThemeConfig::load();
  EXPECT_EQ(config.scheme, QStringLiteral("HoloNight-Storm"));
  EXPECT_EQ(config.accent, QStringLiteral("Blue"));
  EXPECT_EQ(config.resolvedThemeScheme(), Holonight::ThemeSchemeKind::HoloNightStorm);
  EXPECT_EQ(config.resolvedColorMode(), Holonight::ColorMode::Dark);
  EXPECT_EQ(config.resolvedAccent(), QStringLiteral("blue"));
}

TEST(ThemeConfig, ValidSchemeOverridesStaleMode) {
  Holonight::ThemeConfig config = Holonight::ThemeConfig::defaults();
  config.scheme = QStringLiteral("holonight-light");
  config.appearance_mode = Holonight::AppearanceMode::Dark;

  EXPECT_EQ(config.resolvedThemeScheme(), Holonight::ThemeSchemeKind::HoloNightLight);
  EXPECT_EQ(config.resolvedColorMode(), Holonight::ColorMode::Light);

  config.scheme = QStringLiteral("holonight-mocha");
  config.appearance_mode = Holonight::AppearanceMode::Light;
  EXPECT_EQ(config.resolvedThemeScheme(), Holonight::ThemeSchemeKind::HoloNightMocha);
  EXPECT_EQ(config.resolvedColorMode(), Holonight::ColorMode::Dark);

  config.scheme = QStringLiteral("holonight-latte");
  config.appearance_mode = Holonight::AppearanceMode::Dark;
  EXPECT_EQ(config.resolvedThemeScheme(), Holonight::ThemeSchemeKind::HoloNightLatte);
  EXPECT_EQ(config.resolvedColorMode(), Holonight::ColorMode::Light);
}

TEST(ThemeConfig, ConfigFilePathAlwaysResolvesThemeConf) {
  EnvGuard configFileGuard = EnvGuard{"HOLONIGHT_CONFIG_FILE"};
  EnvGuard xdgGuard = EnvGuard{"XDG_CONFIG_HOME"};
  qunsetenv("HOLONIGHT_CONFIG_FILE");

  QTemporaryDir dir;
  ASSERT_TRUE(dir.isValid());
  qputenv("XDG_CONFIG_HOME", dir.path().toLocal8Bit());
  const QString config_dir = dir.filePath(QStringLiteral("holonight"));
  ASSERT_TRUE(QDir{}.mkpath(config_dir));
  const QString ini_path = config_dir + QStringLiteral("/theme.conf");
  writeFile(ini_path, "[appearance]\nmode=light\n");

  EXPECT_EQ(Holonight::ThemeConfig::configFilePath(), ini_path);
  const Holonight::ThemeConfig config = Holonight::ThemeConfig::load();
  EXPECT_EQ(config.appearance_mode, Holonight::AppearanceMode::Light);
}

TEST(ThemeConfig, InvalidAppearanceFallsBackToDark) {
  EnvGuard configFileGuard = EnvGuard{"HOLONIGHT_CONFIG_FILE"};
  EnvGuard appearanceGuard = EnvGuard{"HOLONIGHT_APPEARANCE_MODE"};

  QTemporaryDir dir;
  ASSERT_TRUE(dir.isValid());
  const QString path = dir.filePath(QStringLiteral("theme.conf"));
  writeFile(path, "[appearance]\nmode=invalid\n");
  qputenv("HOLONIGHT_CONFIG_FILE", path.toLocal8Bit());
  qputenv("HOLONIGHT_APPEARANCE_MODE", "also-invalid");

  const Holonight::ThemeConfig config = Holonight::ThemeConfig::load();
  EXPECT_EQ(config.appearance_mode, Holonight::AppearanceMode::Dark);
  EXPECT_EQ(config.resolvedColorMode(), Holonight::ColorMode::Dark);
}

TEST(ThemeConfig, DefaultsToDarkAppearance) {
  const Holonight::ThemeConfig config = Holonight::ThemeConfig::defaults();
  EXPECT_EQ(config.appearance_mode, Holonight::AppearanceMode::Dark);
  EXPECT_EQ(config.resolvedThemeScheme(), Holonight::ThemeSchemeKind::HoloNightDark);
  EXPECT_EQ(config.resolvedColorMode(), Holonight::ColorMode::Dark);
  EXPECT_EQ(config.resolvedAccent(), QStringLiteral("default"));
}

TEST(ThemeConfig, KdeActiveColorSchemeSuppliesMissingScheme) {
  EnvGuard configFileGuard = EnvGuard{"HOLONIGHT_CONFIG_FILE"};
  EnvGuard appearanceGuard = EnvGuard{"HOLONIGHT_APPEARANCE_MODE"};
  EnvGuard xdgConfigGuard = EnvGuard{"XDG_CONFIG_HOME"};
  qunsetenv("HOLONIGHT_CONFIG_FILE");
  qunsetenv("HOLONIGHT_APPEARANCE_MODE");

  QTemporaryDir dir;
  ASSERT_TRUE(dir.isValid());
  qputenv("XDG_CONFIG_HOME", dir.path().toLocal8Bit());
  writeFile(dir.filePath(QStringLiteral("kdeglobals")), "[General]\nColorScheme=HoloNight Storm\n");

  const Holonight::ThemeConfig config = Holonight::ThemeConfig::load();
  EXPECT_EQ(config.scheme, QStringLiteral("holonight-storm"));
  EXPECT_EQ(config.resolvedThemeScheme(), Holonight::ThemeSchemeKind::HoloNightStorm);
  EXPECT_EQ(config.resolvedColorMode(), Holonight::ColorMode::Dark);
}

TEST(ThemeConfig, KdeActiveColorSchemeRecognizesCatppuccinNames) {
  EnvGuard configFileGuard = EnvGuard{"HOLONIGHT_CONFIG_FILE"};
  EnvGuard appearanceGuard = EnvGuard{"HOLONIGHT_APPEARANCE_MODE"};
  EnvGuard xdgConfigGuard = EnvGuard{"XDG_CONFIG_HOME"};
  qunsetenv("HOLONIGHT_CONFIG_FILE");
  qunsetenv("HOLONIGHT_APPEARANCE_MODE");

  QTemporaryDir dir;
  ASSERT_TRUE(dir.isValid());
  qputenv("XDG_CONFIG_HOME", dir.path().toLocal8Bit());
  writeFile(dir.filePath(QStringLiteral("kdeglobals")), "[General]\nColorScheme=HoloNight Mocha\n");

  Holonight::ThemeConfig config = Holonight::ThemeConfig::load();
  EXPECT_EQ(config.scheme, QStringLiteral("holonight-mocha"));
  EXPECT_EQ(config.resolvedThemeScheme(), Holonight::ThemeSchemeKind::HoloNightMocha);
  EXPECT_EQ(config.resolvedColorMode(), Holonight::ColorMode::Dark);

  writeFile(dir.filePath(QStringLiteral("kdeglobals")), "[General]\nColorScheme=HoloNight Latte\n");
  config = Holonight::ThemeConfig::load();
  EXPECT_EQ(config.scheme, QStringLiteral("holonight-latte"));
  EXPECT_EQ(config.resolvedThemeScheme(), Holonight::ThemeSchemeKind::HoloNightLatte);
  EXPECT_EQ(config.resolvedColorMode(), Holonight::ColorMode::Light);
}

TEST(ThemeConfig, KdeActiveColorSchemeRecognizesDraculaNames) {
  EnvGuard configFileGuard = EnvGuard{"HOLONIGHT_CONFIG_FILE"};
  EnvGuard appearanceGuard = EnvGuard{"HOLONIGHT_APPEARANCE_MODE"};
  EnvGuard xdgConfigGuard = EnvGuard{"XDG_CONFIG_HOME"};
  qunsetenv("HOLONIGHT_CONFIG_FILE");
  qunsetenv("HOLONIGHT_APPEARANCE_MODE");

  QTemporaryDir dir;
  ASSERT_TRUE(dir.isValid());
  qputenv("XDG_CONFIG_HOME", dir.path().toLocal8Bit());
  writeFile(dir.filePath(QStringLiteral("kdeglobals")), "[General]\nColorScheme=HoloNight Dracula\n");

  Holonight::ThemeConfig config = Holonight::ThemeConfig::load();
  EXPECT_EQ(config.scheme, QStringLiteral("holonight-dracula"));
  EXPECT_EQ(config.resolvedThemeScheme(), Holonight::ThemeSchemeKind::HoloNightDracula);
  EXPECT_EQ(config.resolvedColorMode(), Holonight::ColorMode::Dark);

  writeFile(dir.filePath(QStringLiteral("kdeglobals")), "[General]\nColorScheme=HoloNight Alucard\n");
  config = Holonight::ThemeConfig::load();
  EXPECT_EQ(config.scheme, QStringLiteral("holonight-alucard"));
  EXPECT_EQ(config.resolvedThemeScheme(), Holonight::ThemeSchemeKind::HoloNightAlucard);
  EXPECT_EQ(config.resolvedColorMode(), Holonight::ColorMode::Light);
}

TEST(ThemeConfig, KdeActiveColorSchemeDoesNotOverrideExplicitConfigScheme) {
  EnvGuard configFileGuard = EnvGuard{"HOLONIGHT_CONFIG_FILE"};
  EnvGuard appearanceGuard = EnvGuard{"HOLONIGHT_APPEARANCE_MODE"};
  EnvGuard xdgConfigGuard = EnvGuard{"XDG_CONFIG_HOME"};
  qunsetenv("HOLONIGHT_CONFIG_FILE");
  qunsetenv("HOLONIGHT_APPEARANCE_MODE");

  QTemporaryDir dir;
  ASSERT_TRUE(dir.isValid());
  qputenv("XDG_CONFIG_HOME", dir.path().toLocal8Bit());
  writeFile(dir.filePath(QStringLiteral("kdeglobals")), "[General]\nColorScheme=HoloNight Storm\n");

  const QString configDir = dir.filePath(QStringLiteral("holonight"));
  ASSERT_TRUE(QDir().mkpath(configDir));
  writeFile(configDir + QStringLiteral("/theme.conf"), "[appearance]\nscheme=holonight-light\nmode=dark\n");

  const Holonight::ThemeConfig config = Holonight::ThemeConfig::load();
  EXPECT_EQ(config.scheme, QStringLiteral("holonight-light"));
  EXPECT_EQ(config.resolvedThemeScheme(), Holonight::ThemeSchemeKind::HoloNightLight);
  EXPECT_EQ(config.resolvedColorMode(), Holonight::ColorMode::Light);
}

TEST(ThemeConfig, AppearanceEnvironmentSuppressesKdeFallback) {
  EnvGuard configFileGuard = EnvGuard{"HOLONIGHT_CONFIG_FILE"};
  EnvGuard appearanceGuard = EnvGuard{"HOLONIGHT_APPEARANCE_MODE"};
  EnvGuard xdgConfigGuard = EnvGuard{"XDG_CONFIG_HOME"};
  qunsetenv("HOLONIGHT_CONFIG_FILE");

  QTemporaryDir dir;
  ASSERT_TRUE(dir.isValid());
  qputenv("XDG_CONFIG_HOME", dir.path().toLocal8Bit());
  qputenv("HOLONIGHT_APPEARANCE_MODE", "light");
  writeFile(dir.filePath(QStringLiteral("kdeglobals")), "[General]\nColorScheme=HoloNight Storm\n");

  const Holonight::ThemeConfig config = Holonight::ThemeConfig::load();
  EXPECT_TRUE(config.scheme.isEmpty());
  EXPECT_EQ(config.resolvedThemeScheme(), Holonight::ThemeSchemeKind::HoloNightLight);
  EXPECT_EQ(config.resolvedColorMode(), Holonight::ColorMode::Light);
}

TEST(ThemeConfig, InvalidOrMissingSchemeFallsBackFromLegacyMode) {
  Holonight::ThemeConfig config = Holonight::ThemeConfig::defaults();

  config.scheme = QStringLiteral("missing");
  config.appearance_mode = Holonight::AppearanceMode::Light;
  EXPECT_EQ(config.resolvedThemeScheme(), Holonight::ThemeSchemeKind::HoloNightLight);
  EXPECT_EQ(config.resolvedColorMode(), Holonight::ColorMode::Light);

  config.scheme.clear();
  config.appearance_mode = Holonight::AppearanceMode::Dark;
  EXPECT_EQ(config.resolvedThemeScheme(), Holonight::ThemeSchemeKind::HoloNightDark);
  EXPECT_EQ(config.resolvedColorMode(), Holonight::ColorMode::Dark);

  config.appearance_mode = Holonight::AppearanceMode::System;
  EXPECT_EQ(config.resolvedThemeScheme(), Holonight::ThemeSchemeKind::HoloNightDark);
  EXPECT_EQ(config.resolvedColorMode(), Holonight::ColorMode::Dark);
}

TEST(ThemeConfig, SystemAppearanceFallsBackToDark) {
  Holonight::ThemeConfig config = Holonight::ThemeConfig::defaults();
  config.appearance_mode = Holonight::AppearanceMode::System;
  EXPECT_EQ(config.resolvedThemeScheme(), Holonight::ThemeSchemeKind::HoloNightDark);
  EXPECT_EQ(config.resolvedColorMode(), Holonight::ColorMode::Dark);
}

TEST(ThemeConfig, InvalidAccentResolvesToDefault) {
  Holonight::ThemeConfig config = Holonight::ThemeConfig::defaults();

  config.accent.clear();
  EXPECT_EQ(config.resolvedAccent(), QStringLiteral("default"));

  config.accent = QStringLiteral("magenta");
  EXPECT_EQ(config.resolvedAccent(), QStringLiteral("default"));
}

TEST(ThemeCatalog, ListsFamiliesVariantsAndDefaultAccentOptions) {
  ASSERT_EQ(Holonight::themeFamilies().size(), 6);
  ASSERT_EQ(Holonight::themeVariants().size(), 12);

  for (const Holonight::ThemeFamilyCatalogEntry& family : Holonight::themeFamilies()) {
    EXPECT_FALSE(family.id.isEmpty());
    EXPECT_FALSE(family.name.isEmpty());
    EXPECT_EQ(family.variant_ids.size(), 2);
  }

  for (const Holonight::ThemeVariantCatalogEntry& variant : Holonight::themeVariants()) {
    EXPECT_FALSE(variant.id.isEmpty());
    EXPECT_FALSE(variant.name.isEmpty());
    EXPECT_FALSE(variant.family_id.isEmpty());
    EXPECT_TRUE(variant.default_accent_color.isValid());

    const QVector<Holonight::AccentCatalogEntry> accents = Holonight::accentOptionsForScheme(variant.id);
    ASSERT_EQ(accents.size(), 4);
    EXPECT_EQ(accents.at(0).id, QStringLiteral("cyan"));
    EXPECT_EQ(accents.at(1).id, QStringLiteral("blue"));
    EXPECT_EQ(accents.at(2).id, QStringLiteral("violet"));
    EXPECT_EQ(accents.at(3).id, QStringLiteral("yellow"));
  }
}
