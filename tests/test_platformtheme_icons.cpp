// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "../platformtheme/holonighttheme.h"

#include <QIcon>

#include <gtest/gtest.h>

TEST(PlatformThemeIcons, ThemeNameIsHoloNight) {
  HoloniightTheme theme;
  EXPECT_EQ(QIcon::themeName(), QStringLiteral("HoloNight"));
}

TEST(PlatformThemeIcons, FallbackThemeNameIsPapirus) {
  HoloniightTheme theme;
  EXPECT_EQ(QIcon::fallbackThemeName(), QStringLiteral("Papirus-Dark"));
}
