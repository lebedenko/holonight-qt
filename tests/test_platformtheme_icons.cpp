// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "../platformtheme/holonighttheme.h"

#include <QIcon>

#include <gtest/gtest.h>

TEST(PlatformThemeIcons, ThemeNameIsPapirus) {
  HoloniightTheme theme;
  EXPECT_EQ(QIcon::themeName(), QStringLiteral("Papirus-Dark"));
}

TEST(PlatformThemeIcons, FallbackThemeNameIsBreezeDark) {
  HoloniightTheme theme;
  EXPECT_EQ(QIcon::fallbackThemeName(), QStringLiteral("breeze-dark"));
}
