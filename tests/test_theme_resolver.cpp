// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "holonight/config.h"
#include "holonight/palette.h"
#include "holonight/theme_catalog.h"
#include "themeloader.h"
#include "themeresolver.h"

#include <QDir>
#include <QFile>
#include <QTemporaryDir>

#include <gtest/gtest.h>

// Guard: if ColorTokens shrinks unexpectedly, operator== silently covers fewer fields.
// Update kExpectedMinSize when intentionally removing fields.
static_assert(sizeof(Holonight::ColorTokens) > 1000,
              "ColorTokens is smaller than expected — verify operator== still covers all fields");

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

Holonight::ThemeConfig makeConfig(Holonight::AppearanceMode mode, const QString& accent = {},
                                  const QString& scheme = {}) {
  Holonight::ThemeConfig cfg;
  cfg.appearance_mode = mode;
  cfg.accent = accent;
  cfg.scheme = scheme;
  return cfg;
}

void expectCyanAccent(const Holonight::ColorTokens& tok) {
  EXPECT_EQ(tok.primary, QColor(QStringLiteral("#7dcfff")));
  EXPECT_EQ(tok.primaryHover, QColor(QStringLiteral("#a3d8ff")));
  EXPECT_EQ(tok.primaryPressed, QColor(QStringLiteral("#5cb8f5")));
  EXPECT_EQ(tok.borderFocus, QColor(QStringLiteral("#7dcfff")));
  EXPECT_EQ(tok.borderActive, QColor(QStringLiteral("#7dcfff")));
  EXPECT_EQ(tok.focusRing, QColor(QStringLiteral("#7dcfff55")));
  EXPECT_EQ(tok.glowCyanSoft, QColor(QStringLiteral("#7dcfff22")));
  EXPECT_EQ(tok.glowBlueSoft, QColor(QStringLiteral("#7dcfff18")));
  EXPECT_EQ(tok.glowVioletSoft, QColor(QStringLiteral("#7dcfff12")));
}

}  // namespace

// ── Mode resolution ───────────────────────────────────────────────────────────

TEST(ThemeLoader, MissingConfigFallsBackToDark) {
  // HOLONIGHT_CONFIG_FILE points to a non-existent file (set by HOLONIGHT_TEST_ENV).
  const Holonight::ThemeConfig cfg = Holonight::ThemeLoader::load();
  EXPECT_EQ(cfg.appearance_mode, Holonight::AppearanceMode::Dark);
}

TEST(ThemeLoader, InvalidModeValueFallsBackToDark) {
  QTemporaryDir dir;
  ASSERT_TRUE(dir.isValid());
  const QString path = dir.path() + QStringLiteral("/theme.conf");
  QFile file{path};
  ASSERT_TRUE(file.open(QIODevice::WriteOnly | QIODevice::Text));
  file.write("[appearance]\nmode=totally_invalid_value\n");
  file.close();

  EnvGuard guard{"HOLONIGHT_CONFIG_FILE"};
  qputenv("HOLONIGHT_CONFIG_FILE", path.toLocal8Bit());

  const Holonight::ThemeConfig cfg = Holonight::ThemeLoader::load();
  EXPECT_EQ(cfg.appearance_mode, Holonight::AppearanceMode::Dark);
}

TEST(ThemeLoader, ExplicitDarkMode) {
  QTemporaryDir dir;
  ASSERT_TRUE(dir.isValid());
  const QString path = dir.path() + QStringLiteral("/theme.conf");
  QFile file{path};
  ASSERT_TRUE(file.open(QIODevice::WriteOnly | QIODevice::Text));
  file.write("[appearance]\nmode=dark\n");
  file.close();

  EnvGuard guard{"HOLONIGHT_CONFIG_FILE"};
  qputenv("HOLONIGHT_CONFIG_FILE", path.toLocal8Bit());

  const Holonight::ThemeConfig cfg = Holonight::ThemeLoader::load();
  EXPECT_EQ(cfg.appearance_mode, Holonight::AppearanceMode::Dark);
  const Holonight::ColorTokens tok = Holonight::ThemeResolver::resolve(cfg);
  const Holonight::ColorTokens base = Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightDark);
  EXPECT_EQ(tok.background, base.background);
  EXPECT_EQ(tok.primary, base.primary);
}

TEST(ThemeLoader, ExplicitLightMode) {
  QTemporaryDir dir;
  ASSERT_TRUE(dir.isValid());
  const QString path = dir.path() + QStringLiteral("/theme.conf");
  QFile file{path};
  ASSERT_TRUE(file.open(QIODevice::WriteOnly | QIODevice::Text));
  file.write("[appearance]\nmode=light\n");
  file.close();

  EnvGuard guard{"HOLONIGHT_CONFIG_FILE"};
  qputenv("HOLONIGHT_CONFIG_FILE", path.toLocal8Bit());

  const Holonight::ThemeConfig cfg = Holonight::ThemeLoader::load();
  EXPECT_EQ(cfg.appearance_mode, Holonight::AppearanceMode::Light);
  const Holonight::ColorTokens tok = Holonight::ThemeResolver::resolve(cfg);
  const Holonight::ColorTokens base = Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightLight);
  EXPECT_EQ(tok.background, base.background);
  EXPECT_EQ(tok.primary, base.primary);
}

TEST(ThemeLoader, SystemModeFallsBackToDark) {
  QTemporaryDir dir;
  ASSERT_TRUE(dir.isValid());
  const QString path = dir.path() + QStringLiteral("/theme.conf");
  QFile file{path};
  ASSERT_TRUE(file.open(QIODevice::WriteOnly | QIODevice::Text));
  file.write("[appearance]\nmode=system\n");
  file.close();

  EnvGuard guard{"HOLONIGHT_CONFIG_FILE"};
  qputenv("HOLONIGHT_CONFIG_FILE", path.toLocal8Bit());

  const Holonight::ThemeConfig cfg = Holonight::ThemeLoader::load();
  EXPECT_EQ(cfg.appearance_mode, Holonight::AppearanceMode::System);
  const Holonight::ColorTokens tok = Holonight::ThemeResolver::resolve(cfg);
  const Holonight::ColorTokens base = Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightDark);
  EXPECT_EQ(tok.background, base.background);
  EXPECT_EQ(tok.primary, base.primary);
}

TEST(ThemeLoader, NewFieldsAreParsedFromIni) {
  QTemporaryDir dir;
  ASSERT_TRUE(dir.isValid());
  const QString path = dir.path() + QStringLiteral("/theme.conf");
  QFile file{path};
  ASSERT_TRUE(file.open(QIODevice::WriteOnly | QIODevice::Text));
  file.write("[appearance]\nmode=dark\nscheme=tokyonight-storm\naccent=cyan\n[effects]\ntransparency=0.82\n");
  file.close();

  EnvGuard guard{"HOLONIGHT_CONFIG_FILE"};
  qputenv("HOLONIGHT_CONFIG_FILE", path.toLocal8Bit());

  const Holonight::ThemeConfig cfg = Holonight::ThemeLoader::load();
  EXPECT_EQ(cfg.scheme, QStringLiteral("tokyonight-storm"));
  EXPECT_EQ(cfg.accent, QStringLiteral("cyan"));
  EXPECT_DOUBLE_EQ(cfg.transparency, 0.82);
}

// ── Accent override ───────────────────────────────────────────────────────────

TEST(ThemeResolver, EmptyAccentKeepsNativeSchemePrimary) {
  const Holonight::ColorTokens tok = Holonight::ThemeResolver::resolve(makeConfig(Holonight::AppearanceMode::Dark));
  const Holonight::ColorTokens base = Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightDark);
  EXPECT_EQ(tok.background, base.background);
  EXPECT_EQ(tok.primary, base.primary);
  EXPECT_EQ(tok.primaryHover, base.primaryHover);
  EXPECT_EQ(tok.primaryPressed, base.primaryPressed);
}

TEST(ThemeResolver, InvalidAccentKeepsNativeSchemePrimary) {
  const Holonight::ColorTokens tok =
      Holonight::ThemeResolver::resolve(makeConfig(Holonight::AppearanceMode::Dark, QStringLiteral("magenta")));
  const Holonight::ColorTokens base = Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightDark);
  EXPECT_EQ(tok.background, base.background);
  EXPECT_EQ(tok.primary, base.primary);
}

TEST(ThemeResolver, DefaultAccentKeepsNativeSchemePrimary) {
  const Holonight::ColorTokens tok = Holonight::ThemeResolver::resolve(
      makeConfig(Holonight::AppearanceMode::Dark, QStringLiteral("default"), QStringLiteral("holonight-mocha")));
  const Holonight::ColorTokens base = Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightMocha);
  EXPECT_EQ(tok.primary, base.primary);
  EXPECT_EQ(tok.primaryHover, base.primaryHover);
  EXPECT_EQ(tok.primaryPressed, base.primaryPressed);
}

TEST(ThemeResolver, AccentCyanOverridesCorrectSlots) {
  const Holonight::ColorTokens tok =
      Holonight::ThemeResolver::resolve(makeConfig(Holonight::AppearanceMode::Dark, QStringLiteral("cyan")));

  expectCyanAccent(tok);

  // Non-accent slots must stay at base dark values.
  const Holonight::ColorTokens base = Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightDark);
  EXPECT_EQ(tok.background, base.background);
  EXPECT_EQ(tok.accentCyan, base.accentCyan);
  EXPECT_EQ(tok.accentBlue, base.accentBlue);
}

TEST(ThemeResolver, AccentBlueOverridesCorrectSlots) {
  const Holonight::ColorTokens tok =
      Holonight::ThemeResolver::resolve(makeConfig(Holonight::AppearanceMode::Dark, QStringLiteral("blue")));

  EXPECT_EQ(tok.primary, QColor(QStringLiteral("#7aa2f7")));
  EXPECT_EQ(tok.primaryHover, QColor(QStringLiteral("#9ab5ff")));
  EXPECT_EQ(tok.primaryPressed, QColor(QStringLiteral("#5f88e8")));
  EXPECT_EQ(tok.borderFocus, QColor(QStringLiteral("#7aa2f7")));
  EXPECT_EQ(tok.borderActive, QColor(QStringLiteral("#7aa2f7")));
  EXPECT_EQ(tok.focusRing, QColor(QStringLiteral("#7aa2f755")));
  EXPECT_EQ(tok.glowCyanSoft, QColor(QStringLiteral("#7aa2f722")));
  EXPECT_EQ(tok.glowBlueSoft, QColor(QStringLiteral("#7aa2f718")));
  EXPECT_EQ(tok.glowVioletSoft, QColor(QStringLiteral("#7aa2f712")));
}

TEST(ThemeResolver, AccentVioletOverridesCorrectSlots) {
  const Holonight::ColorTokens tok =
      Holonight::ThemeResolver::resolve(makeConfig(Holonight::AppearanceMode::Dark, QStringLiteral("violet")));

  EXPECT_EQ(tok.primary, QColor(QStringLiteral("#bb9af7")));
  EXPECT_EQ(tok.primaryHover, QColor(QStringLiteral("#cdb4ff")));
  EXPECT_EQ(tok.primaryPressed, QColor(QStringLiteral("#a07fdf")));
  EXPECT_EQ(tok.borderFocus, QColor(QStringLiteral("#bb9af7")));
  EXPECT_EQ(tok.borderActive, QColor(QStringLiteral("#bb9af7")));
  EXPECT_EQ(tok.focusRing, QColor(QStringLiteral("#bb9af755")));
  EXPECT_EQ(tok.glowCyanSoft, QColor(QStringLiteral("#bb9af722")));
  EXPECT_EQ(tok.glowBlueSoft, QColor(QStringLiteral("#bb9af718")));
  EXPECT_EQ(tok.glowVioletSoft, QColor(QStringLiteral("#bb9af712")));
}

TEST(ThemeResolver, AccentYellowOverridesCorrectSlots) {
  const Holonight::ColorTokens tok =
      Holonight::ThemeResolver::resolve(makeConfig(Holonight::AppearanceMode::Dark, QStringLiteral("yellow")));

  EXPECT_EQ(tok.primary, QColor(QStringLiteral("#e0af68")));
  EXPECT_EQ(tok.primaryHover, QColor(QStringLiteral("#f0c98a")));
  EXPECT_EQ(tok.primaryPressed, QColor(QStringLiteral("#c99050")));
  EXPECT_EQ(tok.borderFocus, QColor(QStringLiteral("#e0af68")));
  EXPECT_EQ(tok.borderActive, QColor(QStringLiteral("#e0af68")));
  EXPECT_EQ(tok.focusRing, QColor(QStringLiteral("#e0af6855")));
  EXPECT_EQ(tok.glowCyanSoft, QColor(QStringLiteral("#e0af6822")));
  EXPECT_EQ(tok.glowBlueSoft, QColor(QStringLiteral("#e0af6818")));
  EXPECT_EQ(tok.glowVioletSoft, QColor(QStringLiteral("#e0af6812")));
}

TEST(ThemeResolver, CatppuccinAccentCyanUsesSchemeNativeColors) {
  const Holonight::ColorTokens mocha = Holonight::ThemeResolver::resolve(
      makeConfig(Holonight::AppearanceMode::Light, QStringLiteral("cyan"), QStringLiteral("holonight-mocha")));
  EXPECT_EQ(mocha.primary, QColor(QStringLiteral("#89DCEB")));
  EXPECT_EQ(mocha.primaryHover, QColor(QStringLiteral("#94E2D5")));
  EXPECT_EQ(mocha.primaryPressed, QColor(QStringLiteral("#74C7EC")));
  EXPECT_EQ(mocha.borderFocus, QColor(QStringLiteral("#89DCEB")));
  EXPECT_EQ(mocha.borderActive, QColor(QStringLiteral("#89DCEB")));
  EXPECT_EQ(mocha.focusRing.rgb(), QColor(QStringLiteral("#89DCEB")).rgb());
  EXPECT_EQ(mocha.focusRing.alpha(), 0x55);

  const Holonight::ColorTokens latte = Holonight::ThemeResolver::resolve(
      makeConfig(Holonight::AppearanceMode::Dark, QStringLiteral("cyan"), QStringLiteral("holonight-latte")));
  EXPECT_EQ(latte.primary, QColor(QStringLiteral("#04A5E5")));
  EXPECT_EQ(latte.primaryHover, QColor(QStringLiteral("#179299")));
  EXPECT_EQ(latte.primaryPressed, QColor(QStringLiteral("#209FB5")));
  EXPECT_EQ(latte.borderFocus, QColor(QStringLiteral("#04A5E5")));
  EXPECT_EQ(latte.borderActive, QColor(QStringLiteral("#04A5E5")));
  EXPECT_EQ(latte.focusRing.rgb(), QColor(QStringLiteral("#04A5E5")).rgb());
  EXPECT_EQ(latte.focusRing.alpha(), 0x55);
}

TEST(ThemeResolver, CatppuccinAccentOverridesUseAdjacentCompanions) {
  const Holonight::ColorTokens mochaViolet = Holonight::ThemeResolver::resolve(
      makeConfig(Holonight::AppearanceMode::Dark, QStringLiteral("violet"), QStringLiteral("holonight-mocha")));
  EXPECT_EQ(mochaViolet.primary, QColor(QStringLiteral("#CBA6F7")));
  EXPECT_EQ(mochaViolet.primaryHover, QColor(QStringLiteral("#F5C2E7")));
  EXPECT_EQ(mochaViolet.primaryPressed, QColor(QStringLiteral("#B4BEFE")));

  const Holonight::ColorTokens latteYellow = Holonight::ThemeResolver::resolve(
      makeConfig(Holonight::AppearanceMode::Dark, QStringLiteral("yellow"), QStringLiteral("holonight-latte")));
  EXPECT_EQ(latteYellow.primary, QColor(QStringLiteral("#DF8E1D")));
  EXPECT_EQ(latteYellow.primaryHover, QColor(QStringLiteral("#FE640B")));
  EXPECT_EQ(latteYellow.primaryPressed, QColor(QStringLiteral("#E64553")));
}

TEST(ThemeResolver, AccentIsCaseInsensitive) {
  const Holonight::ColorTokens lower =
      Holonight::ThemeResolver::resolve(makeConfig(Holonight::AppearanceMode::Dark, QStringLiteral("cyan")));
  const Holonight::ColorTokens upper =
      Holonight::ThemeResolver::resolve(makeConfig(Holonight::AppearanceMode::Dark, QStringLiteral("CYAN")));
  EXPECT_EQ(lower, upper);
}

TEST(ThemeResolver, SchemesResolveToConcreteCatalogEntries) {
  const Holonight::ColorTokens holonightDark = Holonight::ThemeResolver::resolve(
      makeConfig(Holonight::AppearanceMode::Light, QStringLiteral("cyan"), QStringLiteral("holonight-dark")));
  const Holonight::ColorTokens tokyoStorm = Holonight::ThemeResolver::resolve(
      makeConfig(Holonight::AppearanceMode::Light, QStringLiteral("cyan"), QStringLiteral("tokyonight-storm")));
  const Holonight::ColorTokens holonightLight = Holonight::ThemeResolver::resolve(
      makeConfig(Holonight::AppearanceMode::Dark, QStringLiteral("cyan"), QStringLiteral("holonight-light")));
  const Holonight::ColorTokens holonightMocha = Holonight::ThemeResolver::resolve(
      makeConfig(Holonight::AppearanceMode::Light, QStringLiteral("cyan"), QStringLiteral("holonight-mocha")));
  const Holonight::ColorTokens holonightLatte = Holonight::ThemeResolver::resolve(
      makeConfig(Holonight::AppearanceMode::Dark, QStringLiteral("cyan"), QStringLiteral("holonight-latte")));
  const Holonight::ColorTokens tokyoDay = Holonight::ThemeResolver::resolve(
      makeConfig(Holonight::AppearanceMode::Dark, QStringLiteral("cyan"), QStringLiteral("tokyonight-day")));
  const Holonight::ColorTokens holonightEmber = Holonight::ThemeResolver::resolve(
      makeConfig(Holonight::AppearanceMode::Light, QStringLiteral("cyan"), QStringLiteral("holonight-ember")));
  const Holonight::ColorTokens holonightSol = Holonight::ThemeResolver::resolve(
      makeConfig(Holonight::AppearanceMode::Dark, QStringLiteral("cyan"), QStringLiteral("holonight-sol")));

  EXPECT_EQ(holonightDark.background, Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightDark).background);
  EXPECT_EQ(tokyoStorm.background, Holonight::tokensForScheme(Holonight::ThemeSchemeKind::TokyoNightStorm).background);
  EXPECT_EQ(holonightLight.background,
            Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightLight).background);
  EXPECT_EQ(holonightMocha.background,
            Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightMocha).background);
  EXPECT_EQ(holonightLatte.background,
            Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightLatte).background);
  EXPECT_EQ(tokyoDay.background, Holonight::tokensForScheme(Holonight::ThemeSchemeKind::TokyoNightDay).background);
  EXPECT_EQ(holonightEmber.background,
            Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightEmber).background);
  EXPECT_EQ(holonightSol.background, Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightSol).background);
  EXPECT_NE(holonightDark.background, tokyoStorm.background);
  EXPECT_NE(holonightDark.background, holonightMocha.background);
  EXPECT_NE(tokyoStorm.background, holonightMocha.background);
  EXPECT_NE(holonightLight.background, tokyoDay.background);
  EXPECT_NE(holonightLight.background, holonightLatte.background);
  EXPECT_NE(tokyoDay.background, holonightLatte.background);
}

TEST(ThemeResolver, GruvboxAccentCyanUsesSchemeNativeColors) {
  const Holonight::ColorTokens ember = Holonight::ThemeResolver::resolve(
      makeConfig(Holonight::AppearanceMode::Light, QStringLiteral("cyan"), QStringLiteral("holonight-ember")));
  EXPECT_EQ(ember.primary, QColor(QStringLiteral("#8ec07c")));
  EXPECT_EQ(ember.primaryHover, QColor(QStringLiteral("#a8d3c5")));
  EXPECT_EQ(ember.primaryPressed, QColor(QStringLiteral("#83a598")));

  const Holonight::ColorTokens sol = Holonight::ThemeResolver::resolve(
      makeConfig(Holonight::AppearanceMode::Dark, QStringLiteral("cyan"), QStringLiteral("holonight-sol")));
  EXPECT_EQ(sol.primary, QColor(QStringLiteral("#427b58")));
  EXPECT_EQ(sol.primaryHover, QColor(QStringLiteral("#689d6a")));
  EXPECT_EQ(sol.primaryPressed, QColor(QStringLiteral("#2d5c3f")));
}

TEST(ThemeResolver, ValidSchemeWinsWhenModeDisagrees) {
  const Holonight::ColorTokens tok = Holonight::ThemeResolver::resolve(
      makeConfig(Holonight::AppearanceMode::Dark, QStringLiteral("cyan"), QStringLiteral("tokyonight-day")));
  EXPECT_EQ(tok.background, Holonight::tokensForScheme(Holonight::ThemeSchemeKind::TokyoNightDay).background);
}

TEST(ThemeResolver, RadiusAndMetricsRemainStableAcrossSchemes) {
  const Holonight::ColorTokens dark = Holonight::ThemeResolver::resolve(
      makeConfig(Holonight::AppearanceMode::Dark, QStringLiteral("cyan"), QStringLiteral("holonight-dark")));
  const Holonight::ColorTokens light = Holonight::ThemeResolver::resolve(
      makeConfig(Holonight::AppearanceMode::Dark, QStringLiteral("cyan"), QStringLiteral("holonight-light")));

  EXPECT_EQ(dark.radiusTight, light.radiusTight);
  EXPECT_EQ(dark.radiusControl, light.radiusControl);
  EXPECT_EQ(dark.radiusPopup, light.radiusPopup);
  EXPECT_EQ(dark.radiusPill, light.radiusPill);
  EXPECT_EQ(dark.borderWidth, light.borderWidth);
  EXPECT_EQ(dark.focusBorderWidth, light.focusBorderWidth);
  EXPECT_EQ(dark.separatorWidth, light.separatorWidth);
  EXPECT_EQ(dark.controlHeight, light.controlHeight);
  EXPECT_EQ(dark.controlPadding, light.controlPadding);
}

// ── reload() suppression (tested at resolver level) ──────────────────────────

TEST(ThemeResolver, SameConfigProducesEqualTokens) {
  const Holonight::ThemeConfig cfg = makeConfig(Holonight::AppearanceMode::Dark);
  EXPECT_EQ(Holonight::ThemeResolver::resolve(cfg), Holonight::ThemeResolver::resolve(cfg));
}

TEST(ThemeResolver, DifferentModeProducesUnequalTokens) {
  const Holonight::ColorTokens dark =
      Holonight::ThemeResolver::resolve(makeConfig(Holonight::AppearanceMode::Dark, QStringLiteral("cyan")));
  const Holonight::ColorTokens light =
      Holonight::ThemeResolver::resolve(makeConfig(Holonight::AppearanceMode::Light, QStringLiteral("cyan")));
  EXPECT_FALSE(dark == light);
}

TEST(ThemeResolver, DifferentAccentProducesUnequalTokens) {
  const Holonight::ColorTokens cyan =
      Holonight::ThemeResolver::resolve(makeConfig(Holonight::AppearanceMode::Dark, QStringLiteral("cyan")));
  const Holonight::ColorTokens blue =
      Holonight::ThemeResolver::resolve(makeConfig(Holonight::AppearanceMode::Dark, QStringLiteral("blue")));
  EXPECT_FALSE(cyan == blue);
}

TEST(ThemeResolver, SameAccentProducesEqualTokens) {
  const Holonight::ColorTokens tok1 =
      Holonight::ThemeResolver::resolve(makeConfig(Holonight::AppearanceMode::Dark, QStringLiteral("violet")));
  const Holonight::ColorTokens tok2 =
      Holonight::ThemeResolver::resolve(makeConfig(Holonight::AppearanceMode::Dark, QStringLiteral("violet")));
  EXPECT_EQ(tok1, tok2);
}
