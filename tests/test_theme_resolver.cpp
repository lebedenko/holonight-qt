// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "themeloader.h"
#include "themeresolver.h"

#include "holonight/config.h"
#include "holonight/palette.h"

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

Holonight::ThemeConfig makeConfig(Holonight::AppearanceMode mode, const QString& accent = {}) {
  Holonight::ThemeConfig cfg;
  cfg.appearance_mode = mode;
  cfg.accent = accent;
  return cfg;
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
  EXPECT_EQ(Holonight::ThemeResolver::resolve(cfg), Holonight::darkTokens());
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
  EXPECT_EQ(Holonight::ThemeResolver::resolve(cfg), Holonight::lightTokens());
}

TEST(ThemeLoader, SystemModeInHeadlessEnvironmentFallsBackToDark) {
  // In offscreen/headless Qt, QGuiApplication::styleHints()->colorScheme() returns Unknown.
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
  // system + Unknown colorScheme → dark
  EXPECT_EQ(Holonight::ThemeResolver::resolve(cfg), Holonight::darkTokens());
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

TEST(ThemeResolver, EmptyAccentProducesBaseTokens) {
  const Holonight::ColorTokens tok = Holonight::ThemeResolver::resolve(makeConfig(Holonight::AppearanceMode::Dark));
  EXPECT_EQ(tok, Holonight::darkTokens());
}

TEST(ThemeResolver, InvalidAccentProducesBaseTokens) {
  const Holonight::ColorTokens tok =
      Holonight::ThemeResolver::resolve(makeConfig(Holonight::AppearanceMode::Dark, QStringLiteral("magenta")));
  EXPECT_EQ(tok, Holonight::darkTokens());
}

TEST(ThemeResolver, AccentCyanOverridesCorrectSlots) {
  const Holonight::ColorTokens tok =
      Holonight::ThemeResolver::resolve(makeConfig(Holonight::AppearanceMode::Dark, QStringLiteral("cyan")));

  EXPECT_EQ(tok.primary, QColor(QStringLiteral("#7dcfff")));
  EXPECT_EQ(tok.primaryHover, QColor(QStringLiteral("#a3d8ff")));
  EXPECT_EQ(tok.primaryPressed, QColor(QStringLiteral("#5cb8f5")));
  EXPECT_EQ(tok.borderFocus, QColor(QStringLiteral("#7dcfff")));
  EXPECT_EQ(tok.borderActive, QColor(QStringLiteral("#7dcfff")));
  EXPECT_EQ(tok.focusRing, QColor(QStringLiteral("#7dcfff55")));
  EXPECT_EQ(tok.glowCyanSoft, QColor(QStringLiteral("#7dcfff22")));
  EXPECT_EQ(tok.glowBlueSoft, QColor(QStringLiteral("#7dcfff18")));
  EXPECT_EQ(tok.glowVioletSoft, QColor(QStringLiteral("#7dcfff12")));

  // Non-accent slots must stay at base dark values.
  const Holonight::ColorTokens base = Holonight::darkTokens();
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

TEST(ThemeResolver, AccentIsCaseInsensitive) {
  const Holonight::ColorTokens lower =
      Holonight::ThemeResolver::resolve(makeConfig(Holonight::AppearanceMode::Dark, QStringLiteral("cyan")));
  const Holonight::ColorTokens upper =
      Holonight::ThemeResolver::resolve(makeConfig(Holonight::AppearanceMode::Dark, QStringLiteral("CYAN")));
  EXPECT_EQ(lower, upper);
}

// ── reload() suppression (tested at resolver level) ──────────────────────────

TEST(ThemeResolver, SameConfigProducesEqualTokens) {
  const Holonight::ThemeConfig cfg = makeConfig(Holonight::AppearanceMode::Dark);
  EXPECT_EQ(Holonight::ThemeResolver::resolve(cfg), Holonight::ThemeResolver::resolve(cfg));
}

TEST(ThemeResolver, DifferentModeProducesUnequalTokens) {
  const Holonight::ColorTokens dark = Holonight::ThemeResolver::resolve(makeConfig(Holonight::AppearanceMode::Dark));
  const Holonight::ColorTokens light = Holonight::ThemeResolver::resolve(makeConfig(Holonight::AppearanceMode::Light));
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
