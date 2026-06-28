// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "config.h"

#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>

namespace {

bool isColorModeValue(const QString& value) {
  return value == QStringLiteral("light") || value == QStringLiteral("dark");
}

}  // namespace

int main(int argc, char* argv[]) {
  // Prepend paths relative to the binary so the installed demo finds
  // ~/.local/lib/qt6/plugins/{styles,platformthemes} without overriding
  // system Qt plugin paths (which supply the xcb/wayland platform plugins).
  const QString installRoot =
      QDir::cleanPath(QFileInfo(QStringLiteral("/proc/self/exe")).canonicalPath() + QStringLiteral("/.."));
  QCoreApplication::addLibraryPath(installRoot + QStringLiteral("/lib/qt6/plugins"));

  // Check for the --mode argument before creating QGuiApplication, as the
  // platform theme plugin is initialized in its constructor.
  for (int i = 1; i < argc; ++i) {
    const QString arg = QString::fromLocal8Bit(argv[i]);
    if ((arg == QStringLiteral("--mode") || arg == QStringLiteral("-m")) && i + 1 < argc) {
      const QString value = QString::fromLocal8Bit(argv[i + 1]).toLower();
      if (isColorModeValue(value)) {
        qputenv("HOLONIGHT_APPEARANCE_MODE", value.toLocal8Bit());
      }
    } else if (arg.startsWith(QStringLiteral("--mode="))) {
      const QString value = arg.mid(7).toLower();
      if (isColorModeValue(value)) {
        qputenv("HOLONIGHT_APPEARANCE_MODE", value.toLocal8Bit());
      }
    }
  }

  QGuiApplication app(argc, argv);

  QCommandLineParser parser;
  parser.setApplicationDescription(QStringLiteral("HoloNight Theme Demo App"));
  parser.addHelpOption();
  parser.addVersionOption();

  QCommandLineOption modeOption(QStringList() << QStringLiteral("m") << QStringLiteral("mode"),
                                QStringLiteral("Color mode to use (light or dark)."), QStringLiteral("mode"));
  parser.addOption(modeOption);

  parser.process(app);

  if (parser.isSet(modeOption)) {
    const QString mode = parser.value(modeOption).toLower();
    if (!isColorModeValue(mode)) {
      qWarning() << "Error: Invalid mode" << mode << "(must be 'light' or 'dark')";
      return 1;
    }
  }

  QQuickStyle::setStyle(QStringLiteral("Holonight"));
  QQmlApplicationEngine engine;
  engine.addImportPath(installRoot + QStringLiteral("/lib/qt6/qml"));
  engine.addImportPath(QStringLiteral(HOLONIGHT_QML_IMPORT_PATH));
  engine.loadFromModule(QStringLiteral("HoloniightDemo"), QStringLiteral("Main"));
  if (engine.rootObjects().isEmpty()) {
    return 1;
  }
  return QGuiApplication::exec();
}
