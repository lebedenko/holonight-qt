// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "../platformtheme/holonighttheme.h"

#include <QFile>
#include <QIcon>
#include <QSaveFile>
#include <QScopeGuard>
#include <QTemporaryDir>
#include <QTest>
#include <QVariant>

#include <gtest/gtest.h>

TEST(PlatformThemeIcons, DoesNotOverrideUserThemeName) {
  const QString user_theme = QIcon::themeName();
  HoloniightTheme theme;
  EXPECT_EQ(QIcon::themeName(), user_theme);
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

TEST(PlatformThemeIcons, LiveIconOnlyChangesRefreshHintsAndPreserveExplicitTheme) {
  QTemporaryDir directory;
  const auto path = directory.filePath("appearance.toml");
  // Use the canonical configuration fixture independently of installed user settings.
  const auto previous = qgetenv("HOLONIGHT_APPEARANCE_FILE");
  const auto previous_theme = QIcon::themeName();
  const auto previous_fallback = QIcon::fallbackThemeName();
  const auto restore = qScopeGuard([&] {
    qputenv("HOLONIGHT_APPEARANCE_FILE", previous);
    QIcon::setThemeName(previous_theme);
    QIcon::setFallbackThemeName(previous_fallback);
  });
  auto write = [&](const QByteArray& theme, const QByteArray& fallback, const QByteArray& typography = QByteArray{}) {
    QSaveFile file(path);
    const auto bytes =
        QByteArray("version = 2\n[icons]\ntheme = \"") + theme + "\"\nfallback = \"" + fallback + "\"\n" + typography;
    return file.open(QIODevice::WriteOnly) && file.write(bytes) == bytes.size() && file.commit();
  };
  ASSERT_TRUE(write("HoloNight", "Papirus"));
  qputenv("HOLONIGHT_APPEARANCE_FILE", path.toUtf8());
  QIcon::setThemeName("ExplicitApplicationTheme");
  HoloniightTheme theme;
  ASSERT_TRUE(write("HoloNight-Dark", "Papirus-Dark"));
  for (int i = 0; i < 100 && theme.themeHint(QPlatformTheme::SystemIconThemeName).toString() != "HoloNight-Dark"; ++i)
    QTest::qWait(20);
  EXPECT_EQ(theme.themeHint(QPlatformTheme::SystemIconThemeName).toString(), "HoloNight-Dark");
  EXPECT_EQ(theme.themeHint(QPlatformTheme::SystemIconFallbackThemeName).toString(), "Papirus-Dark");
  EXPECT_EQ(QIcon::fallbackThemeName(), "Papirus-Dark");
  EXPECT_EQ(QIcon::themeName(), "ExplicitApplicationTheme");
  for (const auto& family : {QByteArray("DejaVu Sans"), QByteArray("Inter")}) {
    ASSERT_TRUE(
        write("HoloNight-Dark", "Papirus-Dark", "[typography]\nui_family = \"" + family + "\"\nui_size = 16\n"));
    ASSERT_TRUE(QTest::qWaitFor([&] { return theme.font(QPlatformTheme::SystemFont)->family() == family; }));
    EXPECT_EQ(theme.font(QPlatformTheme::SystemFont)->pointSize(), 16);
  }
}
