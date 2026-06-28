// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "../platformtheme/holonighttheme.h"
#include "holonight/palette.h"

#include <QByteArray>
#include <QFont>
#include <QVariant>

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

}  // namespace

TEST(PlatformThemeSmoke, InstantiatesWithoutCrash) {
  HoloniightTheme theme;
  SUCCEED();
}

TEST(PlatformThemeSmoke, PaletteNonNull) {
  HoloniightTheme theme;
  EXPECT_NE(theme.palette(QPlatformTheme::SystemPalette), nullptr);
}

TEST(PlatformThemeSmoke, HighlightMatchesPrimary) {
  HoloniightTheme theme;
  const QPalette* pal = theme.palette(QPlatformTheme::SystemPalette);
  ASSERT_NE(pal, nullptr);
  const Holonight::ColorTokens tokens = Holonight::darkTokens();
  EXPECT_EQ(pal->color(QPalette::Active, QPalette::Highlight), tokens.primary);
}

TEST(PlatformThemeSmoke, StyleNamesContainsHolonight) {
  HoloniightTheme theme;
  const QVariant hint = theme.themeHint(QPlatformTheme::StyleNames);
  ASSERT_TRUE(hint.isValid());
  const QStringList names = hint.toStringList();
  ASSERT_FALSE(names.isEmpty());
  EXPECT_EQ(names.front(), QStringLiteral("Holonight"));
}

TEST(PlatformThemeSmoke, ColorSchemeIsDark) {
  HoloniightTheme theme;
  EXPECT_EQ(theme.colorScheme(), Qt::ColorScheme::Dark);
}

TEST(PlatformThemeSmoke, LightAppearanceUsesLightPaletteAndScheme) {
  EnvGuard guard = EnvGuard{"HOLONIGHT_APPEARANCE_MODE"};
  qputenv("HOLONIGHT_APPEARANCE_MODE", "light");

  HoloniightTheme theme;
  const QPalette* pal = theme.palette(QPlatformTheme::SystemPalette);
  ASSERT_NE(pal, nullptr);
  const Holonight::ColorTokens tokens = Holonight::lightTokens();
  EXPECT_EQ(pal->color(QPalette::Active, QPalette::Window), tokens.background);
  EXPECT_EQ(pal->color(QPalette::Active, QPalette::Highlight), tokens.primary);
  EXPECT_EQ(theme.colorScheme(), Qt::ColorScheme::Light);
}

TEST(PlatformThemeSmoke, FontIsInterTwelvePt) {
  HoloniightTheme theme;
  const QFont* fnt = theme.font(QPlatformTheme::SystemFont);
  ASSERT_NE(fnt, nullptr);
  EXPECT_EQ(fnt->family(), QStringLiteral("Inter"));
  EXPECT_EQ(fnt->pointSize(), 10);
}
