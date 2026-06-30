// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "config.h"

#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QTemporaryDir>

namespace {

QString normalizedThemeValue(const QString& value) { return value.trimmed().toLower(); }

bool isThemeValue(const QString& value) {
  const QString theme = normalizedThemeValue(value);
  return theme == QStringLiteral("holonight-dark") || theme == QStringLiteral("holonight-light") ||
         theme == QStringLiteral("tokyonight-storm") || theme == QStringLiteral("tokyonight-day");
}

QString legacyModeForTheme(const QString& value) {
  const QString theme = normalizedThemeValue(value);
  return theme == QStringLiteral("holonight-light") || theme == QStringLiteral("tokyonight-day")
             ? QStringLiteral("light")
             : QStringLiteral("dark");
}

QString findThemeArgument(int argc, char* argv[]) {
  for (int i = 1; i < argc; ++i) {
    const QString arg = QString::fromLocal8Bit(argv[i]);
    if ((arg == QStringLiteral("--theme") || arg == QStringLiteral("-t")) && i + 1 < argc) {
      const QString value = normalizedThemeValue(QString::fromLocal8Bit(argv[i + 1]));
      if (isThemeValue(value)) {
        return value;
      }
    } else if (arg.startsWith(QStringLiteral("--theme="))) {
      const QString value = normalizedThemeValue(arg.mid(8));
      if (isThemeValue(value)) {
        return value;
      }
    }
  }
  return {};
}

bool writeThemeConfig(const QTemporaryDir& dir, const QString& theme) {
  if (!dir.isValid()) {
    qWarning() << "Error: Could not create temporary theme config directory";
    return false;
  }

  const QString path = dir.filePath(QStringLiteral("theme.conf"));
  QFile file{path};
  if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
    qWarning() << "Error: Could not write temporary theme config" << path << file.errorString();
    return false;
  }

  const QByteArray contents =
      QStringLiteral("[appearance]\nscheme=%1\naccent=cyan\nmode=%2\n").arg(theme, legacyModeForTheme(theme)).toUtf8();
  if (file.write(contents) != contents.size()) {
    qWarning() << "Error: Could not write complete temporary theme config" << path << file.errorString();
    return false;
  }

  qputenv("HOLONIGHT_CONFIG_FILE", path.toLocal8Bit());
  return true;
}

}  // namespace

int main(int argc, char* argv[]) {
  // Prepend paths relative to the binary so the installed demo finds
  // ~/.local/lib/qt6/plugins/{styles,platformthemes} without overriding
  // system Qt plugin paths (which supply the xcb/wayland platform plugins).
  const QString installRoot =
      QDir::cleanPath(QFileInfo(QStringLiteral("/proc/self/exe")).canonicalPath() + QStringLiteral("/.."));
  QCoreApplication::addLibraryPath(installRoot + QStringLiteral("/lib/qt6/plugins"));

  // Check for the --theme argument before creating QGuiApplication, as the
  // platform theme plugin is initialized in its constructor.
  QTemporaryDir themeConfigDir;
  const QString theme = findThemeArgument(argc, argv);
  if (!theme.isEmpty() && !writeThemeConfig(themeConfigDir, theme)) {
    return 1;
  }

  QGuiApplication app(argc, argv);

  QCommandLineParser parser;
  parser.setApplicationDescription(QStringLiteral("HoloNight Theme Demo App"));
  parser.addHelpOption();
  parser.addVersionOption();

  QCommandLineOption themeOption(QStringList() << QStringLiteral("t") << QStringLiteral("theme"),
                                 QStringLiteral("Theme scheme to use."), QStringLiteral("theme"));
  parser.addOption(themeOption);

  parser.process(app);

  if (parser.isSet(themeOption)) {
    const QString selectedTheme = normalizedThemeValue(parser.value(themeOption));
    if (!isThemeValue(selectedTheme)) {
      qWarning() << "Error: Invalid theme" << selectedTheme
                 << "(must be 'holonight-dark', 'holonight-light', 'tokyonight-storm', or 'tokyonight-day')";
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
