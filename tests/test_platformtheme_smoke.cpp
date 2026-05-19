// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "../platformtheme/holonighttheme.h"
#include "holonight/palette.h"

#include <QFont>
#include <QVariant>

#include <gtest/gtest.h>

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
  EXPECT_TRUE(names.contains(QStringLiteral("holonight")));
}

TEST(PlatformThemeSmoke, IconThemeIsPapirus) {
  HoloniightTheme theme;
  const QVariant iconTheme = theme.themeHint(QPlatformTheme::SystemIconThemeName);
  EXPECT_EQ(iconTheme.toString(), QStringLiteral("papirus"));
}

TEST(PlatformThemeSmoke, ColorSchemeIsDark) {
  HoloniightTheme theme;
  EXPECT_EQ(theme.colorScheme(), Qt::ColorScheme::Dark);
}

TEST(PlatformThemeSmoke, FontIsInterTenPt) {
  HoloniightTheme theme;
  const QFont* fnt = theme.font(QPlatformTheme::SystemFont);
  ASSERT_NE(fnt, nullptr);
  EXPECT_EQ(fnt->family(), QStringLiteral("Inter"));
  EXPECT_EQ(fnt->pointSize(), 10);
}
