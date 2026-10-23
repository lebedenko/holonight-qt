// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "config.h"
#include "startup.h"

#include <QDir>
#include <QGuiApplication>
#include <QQmlApplicationEngine>

int main(int argc, char* argv[]) {
  QTemporaryDir themeConfigDir;
  const QString theme = Demo::findThemeArgument({argv, static_cast<std::size_t>(argc)});
  Demo::addPluginPaths();
  if (!theme.isEmpty() && !Demo::writeAppearance(themeConfigDir, theme)) return 1;

  QGuiApplication app(argc, argv);

  if (!Demo::parseArguments(app)) return 1;

  QQmlApplicationEngine engine;
  const QString executableDir = QCoreApplication::applicationDirPath();
  if (executableDir == QStringLiteral(HOLONIGHT_EXECUTABLE_BUILD_DIR)) {
    engine.addImportPath(QStringLiteral(HOLONIGHT_QML_IMPORT_PATH));
  } else {
    engine.addImportPath(QDir(executableDir).absoluteFilePath(QStringLiteral("../" HOLONIGHT_INSTALLED_QML_DIR)));
  }
  engine.loadFromModule(QStringLiteral("HoloniightDemo"), QStringLiteral("Main"));
  if (engine.rootObjects().isEmpty()) {
    return 1;
  }
  QObject* root = engine.rootObjects().constFirst();
  // A QML signal keeps the gallery independently loadable by coverage tests.
  QObject::connect(&engine, &QQmlApplicationEngine::quit, &app, &QCoreApplication::quit);
  Demo::WidgetsLauncher launcher(root, executableDir, theme);
  QObject::connect(root, SIGNAL(openWidgetsDemoRequested()), &launcher, SLOT(open()));
  return QGuiApplication::exec();
}
