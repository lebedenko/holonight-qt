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

void expectDerivedAccent(const Holonight::ColorTokens& tok, const QColor& primary) {
  EXPECT_EQ(tok.primary, primary);
  EXPECT_EQ(tok.textAccent, tok.primary);
  EXPECT_EQ(tok.primaryHover, primary.lighter(115));
  EXPECT_EQ(tok.primaryPressed, primary.darker(115));
  EXPECT_EQ(tok.borderFocus, primary);
  EXPECT_EQ(tok.borderActive, primary);
  QColor expected = primary;
  expected.setAlpha(0x55);
  EXPECT_EQ(tok.focusRing, expected);
  expected.setAlpha(0x22);
  EXPECT_EQ(tok.glowCyanSoft, expected);
  expected.setAlpha(0x18);
  EXPECT_EQ(tok.glowBlueSoft, expected);
  expected.setAlpha(0x12);
  EXPECT_EQ(tok.glowVioletSoft, expected);
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
  QFile file = QFile{path};
  const bool opened = file.open(QIODevice::WriteOnly | QIODevice::Text);
  ASSERT_TRUE(opened);
  file.write("[appearance]\nmode=totally_invalid_value\n");
  file.close();

  EnvGuard guard = EnvGuard{"HOLONIGHT_CONFIG_FILE"};
  qputenv("HOLONIGHT_CONFIG_FILE", path.toLocal8Bit());

  const Holonight::ThemeConfig cfg = Holonight::ThemeLoader::load();
  EXPECT_EQ(cfg.appearance_mode, Holonight::AppearanceMode::Dark);
}

TEST(ThemeLoader, ExplicitDarkMode) {
  QTemporaryDir dir;
  ASSERT_TRUE(dir.isValid());
  const QString path = dir.path() + QStringLiteral("/theme.conf");
  QFile file = QFile{path};
  const bool opened = file.open(QIODevice::WriteOnly | QIODevice::Text);
  ASSERT_TRUE(opened);
  file.write("[appearance]\nmode=dark\n");
  file.close();

  EnvGuard guard = EnvGuard{"HOLONIGHT_CONFIG_FILE"};
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
  QFile file = QFile{path};
  const bool opened = file.open(QIODevice::WriteOnly | QIODevice::Text);
  ASSERT_TRUE(opened);
  file.write("[appearance]\nmode=light\n");
  file.close();

  EnvGuard guard = EnvGuard{"HOLONIGHT_CONFIG_FILE"};
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
  QFile file = QFile{path};
  const bool opened = file.open(QIODevice::WriteOnly | QIODevice::Text);
  ASSERT_TRUE(opened);
  file.write("[appearance]\nmode=system\n");
  file.close();

  EnvGuard guard = EnvGuard{"HOLONIGHT_CONFIG_FILE"};
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
  QFile file = QFile{path};
  const bool opened = file.open(QIODevice::WriteOnly | QIODevice::Text);
  ASSERT_TRUE(opened);
  file.write("[appearance]\nmode=dark\nscheme=holonight-storm\naccent=cyan\n[effects]\ntransparency=0.82\n");
  file.close();

  EnvGuard guard = EnvGuard{"HOLONIGHT_CONFIG_FILE"};
  qputenv("HOLONIGHT_CONFIG_FILE", path.toLocal8Bit());

  const Holonight::ThemeConfig cfg = Holonight::ThemeLoader::load();
  EXPECT_EQ(cfg.scheme, QStringLiteral("holonight-storm"));
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

  expectDerivedAccent(tok, QColor{QStringLiteral("#56D7FF")});

  // Non-accent slots must stay at base dark values.
  const Holonight::ColorTokens base = Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightDark);
  EXPECT_EQ(tok.background, base.background);
  EXPECT_EQ(tok.accentCyan, base.accentCyan);
  EXPECT_EQ(tok.accentBlue, base.accentBlue);
}

TEST(ThemeResolver, AccentBlueOverridesCorrectSlots) {
  const Holonight::ColorTokens tok =
      Holonight::ThemeResolver::resolve(makeConfig(Holonight::AppearanceMode::Dark, QStringLiteral("blue")));

  expectDerivedAccent(tok, QColor{QStringLiteral("#5EA2FF")});
}

TEST(ThemeResolver, AccentVioletOverridesCorrectSlots) {
  const Holonight::ColorTokens tok =
      Holonight::ThemeResolver::resolve(makeConfig(Holonight::AppearanceMode::Dark, QStringLiteral("violet")));

  expectDerivedAccent(tok, QColor{QStringLiteral("#9A8CFF")});
}

TEST(ThemeResolver, AccentYellowOverridesCorrectSlots) {
  const Holonight::ColorTokens tok =
      Holonight::ThemeResolver::resolve(makeConfig(Holonight::AppearanceMode::Dark, QStringLiteral("yellow")));

  expectDerivedAccent(tok, QColor{QStringLiteral("#F2C46B")});
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
  const Holonight::ColorTokens holonightStorm = Holonight::ThemeResolver::resolve(
      makeConfig(Holonight::AppearanceMode::Light, QStringLiteral("cyan"), QStringLiteral("holonight-storm")));
  const Holonight::ColorTokens holonightLight = Holonight::ThemeResolver::resolve(
      makeConfig(Holonight::AppearanceMode::Dark, QStringLiteral("cyan"), QStringLiteral("holonight-light")));
  const Holonight::ColorTokens holonightMocha = Holonight::ThemeResolver::resolve(
      makeConfig(Holonight::AppearanceMode::Light, QStringLiteral("cyan"), QStringLiteral("holonight-mocha")));
  const Holonight::ColorTokens holonightLatte = Holonight::ThemeResolver::resolve(
      makeConfig(Holonight::AppearanceMode::Dark, QStringLiteral("cyan"), QStringLiteral("holonight-latte")));
  const Holonight::ColorTokens holonightDay = Holonight::ThemeResolver::resolve(
      makeConfig(Holonight::AppearanceMode::Dark, QStringLiteral("cyan"), QStringLiteral("holonight-day")));
  const Holonight::ColorTokens holonightEmber = Holonight::ThemeResolver::resolve(
      makeConfig(Holonight::AppearanceMode::Light, QStringLiteral("cyan"), QStringLiteral("holonight-ember")));
  const Holonight::ColorTokens holonightSol = Holonight::ThemeResolver::resolve(
      makeConfig(Holonight::AppearanceMode::Dark, QStringLiteral("cyan"), QStringLiteral("holonight-sol")));
  const Holonight::ColorTokens holonightCyberD = Holonight::ThemeResolver::resolve(
      makeConfig(Holonight::AppearanceMode::Light, QStringLiteral("cyan"), QStringLiteral("holonight-cyber-d")));
  const Holonight::ColorTokens holonightCyberL = Holonight::ThemeResolver::resolve(
      makeConfig(Holonight::AppearanceMode::Dark, QStringLiteral("cyan"), QStringLiteral("holonight-cyber-l")));

  EXPECT_EQ(holonightDark.background, Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightDark).background);
  EXPECT_EQ(holonightStorm.background,
            Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightStorm).background);
  EXPECT_EQ(holonightLight.background,
            Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightLight).background);
  EXPECT_EQ(holonightMocha.background,
            Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightMocha).background);
  EXPECT_EQ(holonightLatte.background,
            Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightLatte).background);
  EXPECT_EQ(holonightDay.background, Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightDay).background);
  EXPECT_EQ(holonightEmber.background,
            Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightEmber).background);
  EXPECT_EQ(holonightSol.background, Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightSol).background);
  EXPECT_EQ(holonightCyberD.background,
            Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightCyberD).background);
  EXPECT_EQ(holonightCyberL.background,
            Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightCyberL).background);
  EXPECT_NE(holonightDark.background, holonightStorm.background);
  EXPECT_NE(holonightDark.background, holonightMocha.background);
  EXPECT_NE(holonightStorm.background, holonightMocha.background);
  EXPECT_NE(holonightLight.background, holonightDay.background);
  EXPECT_NE(holonightLight.background, holonightLatte.background);
  EXPECT_NE(holonightDay.background, holonightLatte.background);
  EXPECT_NE(holonightDark.background, holonightCyberD.background);
  EXPECT_NE(holonightLight.background, holonightCyberL.background);
}

TEST(ThemeResolver, EveryCatalogSchemeResolvesToItsBaseTokens) {
  for (const Holonight::ThemeVariantCatalogEntry& variant : Holonight::themeVariants()) {
    const Holonight::ColorTokens resolved = Holonight::ThemeResolver::resolve(
        makeConfig(Holonight::AppearanceMode::Dark, QStringLiteral("default"), variant.id));
    EXPECT_EQ(resolved, Holonight::tokensForScheme(variant.scheme)) << variant.id.toStdString();
  }
}

TEST(ThemeResolver, EveryCatalogAccentMatchesTheResolvedPrimaryColor) {
  for (const Holonight::ThemeVariantCatalogEntry& variant : Holonight::themeVariants()) {
    for (const Holonight::AccentCatalogEntry& accent : Holonight::accentOptionsForScheme(variant.id)) {
      const Holonight::ColorTokens resolved =
          Holonight::ThemeResolver::resolve(makeConfig(Holonight::AppearanceMode::Dark, accent.id, variant.id));
      EXPECT_EQ(resolved.primary, accent.color) << variant.id.toStdString() << '/' << accent.id.toStdString();
    }
  }
}

TEST(ThemeResolver, DraculaFamilyAccentsUseSchemeNativeColors) {
  const Holonight::ColorTokens dracula = Holonight::ThemeResolver::resolve(
      makeConfig(Holonight::AppearanceMode::Light, QStringLiteral("cyan"), QStringLiteral("holonight-dracula")));
  EXPECT_EQ(dracula.primary, QColor(QStringLiteral("#8BE9FD")));
  EXPECT_EQ(dracula.primaryHover, QColor(QStringLiteral("#A4FFFF")));
  EXPECT_EQ(dracula.primaryPressed, QColor(QStringLiteral("#6FD3E7")));

  const Holonight::ColorTokens dracula_blue = Holonight::ThemeResolver::resolve(
      makeConfig(Holonight::AppearanceMode::Light, QStringLiteral("blue"), QStringLiteral("holonight-dracula")));
  EXPECT_EQ(dracula_blue.primary, QColor(QStringLiteral("#6272A4")));
  EXPECT_EQ(dracula_blue.primaryHover, QColor(QStringLiteral("#8292C4")));
  EXPECT_EQ(dracula_blue.primaryPressed, QColor(QStringLiteral("#4C567A")));

  const Holonight::ColorTokens alucard = Holonight::ThemeResolver::resolve(
      makeConfig(Holonight::AppearanceMode::Dark, QStringLiteral("cyan"), QStringLiteral("holonight-alucard")));
  EXPECT_EQ(alucard.primary, QColor(QStringLiteral("#036A96")));
  EXPECT_EQ(alucard.primaryHover, QColor(QStringLiteral("#167FAB")));
  EXPECT_EQ(alucard.primaryPressed, QColor(QStringLiteral("#025477")));
}

TEST(ThemeResolver, CyberAccentCyanUsesSchemeNativeColors) {
  const Holonight::ColorTokens cyberD = Holonight::ThemeResolver::resolve(
      makeConfig(Holonight::AppearanceMode::Light, QStringLiteral("cyan"), QStringLiteral("holonight-cyber-d")));
  EXPECT_EQ(cyberD.primary, QColor(QStringLiteral("#39D5FF")));
  EXPECT_EQ(cyberD.primaryHover, QColor(QStringLiteral("#6BE4FF")));
  EXPECT_EQ(cyberD.primaryPressed, QColor(QStringLiteral("#1FAEDB")));

  const Holonight::ColorTokens cyberL = Holonight::ThemeResolver::resolve(
      makeConfig(Holonight::AppearanceMode::Dark, QStringLiteral("cyan"), QStringLiteral("holonight-cyber-l")));
  EXPECT_EQ(cyberL.primary, QColor(QStringLiteral("#0E9BD6")));
  EXPECT_EQ(cyberL.primaryHover, QColor(QStringLiteral("#12B4F2")));
  EXPECT_EQ(cyberL.primaryPressed, QColor(QStringLiteral("#0A7AAA")));
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
      makeConfig(Holonight::AppearanceMode::Dark, QStringLiteral("cyan"), QStringLiteral("holonight-day")));
  EXPECT_EQ(tok.background, Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightDay).background);
}

TEST(ThemeResolver, MetricsRemainStableAcrossSchemes) {
  const Holonight::ColorTokens dark = Holonight::ThemeResolver::resolve(
      makeConfig(Holonight::AppearanceMode::Dark, QStringLiteral("cyan"), QStringLiteral("holonight-dark")));
  const Holonight::ColorTokens light = Holonight::ThemeResolver::resolve(
      makeConfig(Holonight::AppearanceMode::Dark, QStringLiteral("cyan"), QStringLiteral("holonight-light")));

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
