// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "../platformtheme/holonighttheme.h"

#include <QIcon>
#include <QVariant>

#include <gtest/gtest.h>

TEST(PlatformThemeIcons, ThemeNameIsHoloNight) {
  HoloniightTheme theme;
  EXPECT_EQ(QIcon::themeName(), QStringLiteral("HoloNight"));
}

TEST(PlatformThemeIcons, FallbackThemeNameIsPapirus) {
  HoloniightTheme theme;
  EXPECT_EQ(QIcon::fallbackThemeName(), QStringLiteral("Papirus"));
}

TEST(PlatformThemeIcons, ThemeHintSystemIconThemeName) {
  HoloniightTheme theme;
  const QVariant hint = theme.themeHint(QPlatformTheme::SystemIconThemeName);
  ASSERT_TRUE(hint.isValid());
  EXPECT_EQ(hint.toString(), QStringLiteral("HoloNight"));
}

TEST(PlatformThemeIcons, ThemeHintFallbackThemeName) {
  HoloniightTheme theme;
  const QVariant hint = theme.themeHint(QPlatformTheme::SystemIconFallbackThemeName);
  ASSERT_TRUE(hint.isValid());
  EXPECT_EQ(hint.toString(), QStringLiteral("Papirus"));
}

TEST(PlatformThemeIcons, IconSearchPathsContainsQrcIcons) {
  HoloniightTheme theme;
  const QVariant hint = theme.themeHint(QPlatformTheme::IconThemeSearchPaths);
  ASSERT_TRUE(hint.isValid());
  const QStringList paths = hint.toStringList();
  ASSERT_FALSE(paths.isEmpty());
  EXPECT_TRUE(paths.contains(QStringLiteral(":/icons")));
}

TEST(PlatformThemeIcons, IconSearchPathsAllEndWithIcons) {
  HoloniightTheme theme;
  const QStringList paths = theme.themeHint(QPlatformTheme::IconThemeSearchPaths).toStringList();
  for (const QString& path : paths) {
    EXPECT_TRUE(path.endsWith(QStringLiteral("/icons"))) << path.toStdString();
  }
}
