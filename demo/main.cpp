// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "config.h"
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>

int main(int argc, char *argv[]) {
  // Prepend paths relative to the binary so the installed demo finds
  // ~/.local/lib/qt6/plugins/{styles,platformthemes} without overriding
  // system Qt plugin paths (which supply the xcb/wayland platform plugins).
  const QString installRoot =
      QDir::cleanPath(QFileInfo(QStringLiteral("/proc/self/exe")).canonicalPath() + QStringLiteral("/.."));
  QCoreApplication::addLibraryPath(installRoot + QStringLiteral("/lib/qt6/plugins"));

  QGuiApplication app(argc, argv);
  QQuickStyle::setStyle(QStringLiteral("Holonight"));
  QQmlApplicationEngine engine;
  engine.addImportPath(installRoot + QStringLiteral("/lib/qt6/qml"));
  engine.addImportPath(QStringLiteral(HOLONIGHT_QML_IMPORT_PATH));
  engine.loadFromModule(QStringLiteral("HoloniightDemo"), QStringLiteral("Main"));
  if (engine.rootObjects().isEmpty()) { return 1; }
  return QGuiApplication::exec();
}
