// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "../platformtheme/holonighttheme.h"

#include <QFont>

#include <gtest/gtest.h>

TEST(PlatformThemeFonts, SystemFontNonNull) {
  HoloniightTheme theme;
  EXPECT_NE(theme.font(QPlatformTheme::SystemFont), nullptr);
}

TEST(PlatformThemeFonts, SystemFontProperties) {
  HoloniightTheme theme;
  const QFont* fnt = theme.font(QPlatformTheme::SystemFont);
  ASSERT_NE(fnt, nullptr);
  EXPECT_EQ(fnt->family(), QStringLiteral("Inter"));
  EXPECT_EQ(fnt->weight(), QFont::Normal);
  EXPECT_EQ(fnt->pointSize(), 10);
}

TEST(PlatformThemeFonts, TitleBarFontNonNull) {
  HoloniightTheme theme;
  EXPECT_NE(theme.font(QPlatformTheme::TitleBarFont), nullptr);
}

TEST(PlatformThemeFonts, TitleBarFontProperties) {
  HoloniightTheme theme;
  const QFont* fnt = theme.font(QPlatformTheme::TitleBarFont);
  ASSERT_NE(fnt, nullptr);
  EXPECT_EQ(fnt->family(), QStringLiteral("Inter"));
  EXPECT_EQ(fnt->weight(), QFont::Normal);
  EXPECT_EQ(fnt->pointSize(), 13);
}

TEST(PlatformThemeFonts, FixedFontNonNull) {
  HoloniightTheme theme;
  EXPECT_NE(theme.font(QPlatformTheme::FixedFont), nullptr);
}

TEST(PlatformThemeFonts, FixedFontProperties) {
  HoloniightTheme theme;
  const QFont* fnt = theme.font(QPlatformTheme::FixedFont);
  ASSERT_NE(fnt, nullptr);
  EXPECT_EQ(fnt->family(), QStringLiteral("JetBrains Mono"));
  EXPECT_EQ(fnt->weight(), QFont::Normal);
  EXPECT_EQ(fnt->pointSize(), 10);
}

TEST(PlatformThemeFonts, SmallFontNonNull) {
  HoloniightTheme theme;
  EXPECT_NE(theme.font(QPlatformTheme::SmallFont), nullptr);
}

TEST(PlatformThemeFonts, SmallFontProperties) {
  HoloniightTheme theme;
  const QFont* fnt = theme.font(QPlatformTheme::SmallFont);
  ASSERT_NE(fnt, nullptr);
  EXPECT_EQ(fnt->family(), QStringLiteral("Inter"));
  EXPECT_EQ(fnt->weight(), QFont::Normal);
  EXPECT_EQ(fnt->pointSize(), 9);
}

TEST(PlatformThemeFonts, MiniFontNonNull) {
  HoloniightTheme theme;
  EXPECT_NE(theme.font(QPlatformTheme::MiniFont), nullptr);
}

TEST(PlatformThemeFonts, MiniFontProperties) {
  HoloniightTheme theme;
  const QFont* fnt = theme.font(QPlatformTheme::MiniFont);
  ASSERT_NE(fnt, nullptr);
  EXPECT_EQ(fnt->family(), QStringLiteral("Inter"));
  EXPECT_EQ(fnt->weight(), QFont::Normal);
  EXPECT_EQ(fnt->pointSize(), 9);
}

TEST(PlatformThemeFonts, MenuBarFontReturnsNull) {
  HoloniightTheme theme;
  EXPECT_EQ(theme.font(QPlatformTheme::MenuBarFont), nullptr);
}

TEST(PlatformThemeFonts, PushButtonFontReturnsNull) {
  HoloniightTheme theme;
  EXPECT_EQ(theme.font(QPlatformTheme::PushButtonFont), nullptr);
}

TEST(PlatformThemeFonts, LabelFontReturnsNull) {
  HoloniightTheme theme;
  EXPECT_EQ(theme.font(QPlatformTheme::LabelFont), nullptr);
}
