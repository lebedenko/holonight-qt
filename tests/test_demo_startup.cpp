// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>
#include "startup.h"

#include <QFile>
#include <QTest>

#include <gtest/gtest.h>

TEST(DemoStartup, ThemeArgumentsSupportBothFormsAndNormalizeValues) {
  QByteArray executable{"demo"};
  QByteArray themeOption{"--theme"};
  QByteArray themeValue{" HoloNight-Light "};
  char* longArguments[] = {executable.data(), themeOption.data(), themeValue.data()};
  EXPECT_EQ(Demo::findThemeArgument(longArguments), QStringLiteral("holonight-light"));
  themeOption = "-t";
  char* shortArguments[] = {executable.data(), themeOption.data(), themeValue.data()};
  EXPECT_EQ(Demo::findThemeArgument(shortArguments), QStringLiteral("holonight-light"));
  QByteArray equalsOption{"--theme=holonight-dark"};
  char* equalsArguments[] = {executable.data(), equalsOption.data()};
  EXPECT_EQ(Demo::findThemeArgument(equalsArguments), QStringLiteral("holonight-dark"));
  QByteArray invalid{"--theme=unknown-theme"};
  char* invalidArguments[] = {executable.data(), invalid.data()};
  EXPECT_TRUE(Demo::findThemeArgument(invalidArguments).isEmpty());
}

TEST(DemoStartup, ResolvesSiblingAndForwardsSelectedTheme) {
  EXPECT_EQ(Demo::widgetsExecutable(QStringLiteral("/tmp/demo prefix")),
            QStringLiteral("/tmp/demo prefix/holonight_widgets_demo"));
  EXPECT_TRUE(Demo::widgetsArguments({}).isEmpty());
  EXPECT_EQ(Demo::widgetsArguments(QStringLiteral("holonight-light")),
            (QStringList{QStringLiteral("--theme"), QStringLiteral("holonight-light")}));
}

TEST(DemoStartup, FailedLaunchUpdatesInlineError) {
  QTemporaryDir directory;
  QObject root;
  Demo::WidgetsLauncher launcher(&root, directory.path(), QStringLiteral("holonight-dark"));
  launcher.open();
  const QString error = root.property("widgetsLaunchError").toString();
  EXPECT_TRUE(error.contains(QStringLiteral("Could not open Qt Widgets demo")));
  EXPECT_TRUE(error.contains(Demo::widgetsExecutable(directory.path())));
}

TEST(DemoStartup, DetachedChildReceivesThemeAndOwnsAppearanceConfiguration) {
  QTemporaryDir directory;
  QFile executable{Demo::widgetsExecutable(directory.path())};
  ASSERT_TRUE(executable.open(QIODevice::WriteOnly));
  executable.write("#!/bin/sh\nprintf '%s\\n' \"$@\" \"${HOLONIGHT_APPEARANCE_FILE-unset}\" > \"$0.output\"\n");
  executable.close();
  ASSERT_TRUE(executable.setPermissions(QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner));
  const QByteArray previous = qgetenv("HOLONIGHT_APPEARANCE_FILE");
  const bool wasSet = qEnvironmentVariableIsSet("HOLONIGHT_APPEARANCE_FILE");
  qputenv("HOLONIGHT_APPEARANCE_FILE", "/tmp/parent-owned-appearance.toml");
  const QString error = Demo::launchWidgets(directory.path(), QStringLiteral("holonight-light"));
  if (wasSet)
    qputenv("HOLONIGHT_APPEARANCE_FILE", previous);
  else
    qunsetenv("HOLONIGHT_APPEARANCE_FILE");
  ASSERT_TRUE(error.isEmpty()) << error.toStdString();
  QFile result{executable.fileName() + QStringLiteral(".output")};
  ASSERT_TRUE(QTest::qWaitFor([&] { return result.exists() && result.size() > 0; }, 2000));
  ASSERT_TRUE(result.open(QIODevice::ReadOnly));
  EXPECT_EQ(result.readAll(), QByteArray("--theme\nholonight-light\nunset\n"));
}
