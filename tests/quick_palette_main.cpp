// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include <QApplication>
#include <QGuiApplication>
#include <QTemporaryDir>

#include <gtest/gtest.h>
#include <holonight/config/config.h>
#include <memory>

int main(int argc, char** argv) {
  qInstallMessageHandler([](QtMsgType type, const QMessageLogContext&, const QString& message) {
    if (message.contains("Unable to assign") || message.contains("Binding loop") ||
        message.contains("non-existent property") || message.contains("Cannot assign") ||
        message.contains("ReferenceError") || message.contains("TypeError") || message.contains("only supports"))
      ADD_FAILURE() << message.toStdString();
    if (type == QtWarningMsg || type == QtCriticalMsg) fprintf(stderr, "%s\n", qPrintable(message));
  });
  ::testing::InitGoogleTest(&argc, argv);
  QTemporaryDir directory;
  if (!directory.isValid()) return 1;
  auto appearance = HoloNight::Config::defaults();
  const auto scheme = qgetenv("UQC_SCHEME");
  if (!scheme.isEmpty()) appearance.theme.scheme = scheme.toStdString();
  const auto path = directory.filePath("appearance.toml");
  if (!HoloNight::Config::writeAtomically(appearance, std::filesystem::path(path.toStdString()))) return 1;
  qputenv("HOLONIGHT_APPEARANCE_FILE", path.toUtf8());
  std::unique_ptr<QGuiApplication> app;
  if (qEnvironmentVariableIsSet("UQC_HYBRID")) {
    app = std::make_unique<QApplication>(argc, argv);
    if (!QApplication::setStyle("holonight")) return 1;
  } else
    app = std::make_unique<QGuiApplication>(argc, argv);
  return RUN_ALL_TESTS();
}
