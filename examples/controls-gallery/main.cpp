// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "config.h"

#include <QCoreApplication>
#include <QDir>
#include <QGuiApplication>
#include <QQmlApplicationEngine>

int main(int argc, char* argv[]) {
  QGuiApplication app(argc, argv);

  QQmlApplicationEngine engine;
  const QString executableDir = QCoreApplication::applicationDirPath();
  if (executableDir == QStringLiteral(HOLONIGHT_EXECUTABLE_BUILD_DIR)) {
    engine.addImportPath(QStringLiteral(HOLONIGHT_QML_IMPORT_PATH));
  } else {
    engine.addImportPath(QDir(executableDir).absoluteFilePath(QStringLiteral("../" HOLONIGHT_INSTALLED_QML_DIR)));
  }
  engine.loadFromModule(QStringLiteral("HolonightControlsGallery"), QStringLiteral("Main"));
  if (engine.rootObjects().isEmpty()) {
    return 1;
  }
  return QGuiApplication::exec();
}
