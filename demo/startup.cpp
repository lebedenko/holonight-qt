// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "startup.h"

#include "config.h"

#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QProcess>
#include <QProcessEnvironment>

#include <holonight/config/config.h>
#include <holonight/theme_catalog.h>

namespace Demo {

QString normalizedThemeValue(const QString& value) { return value.trimmed().toLower(); }

bool isThemeValue(const QString& value) { return !Holonight::normalizeSchemeId(value).isEmpty(); }

QString findThemeArgument(std::span<char*> arguments) {
  for (std::size_t i = 1; i < arguments.size(); ++i) {
    const QString arg = QString::fromLocal8Bit(arguments[i]);
    if ((arg == QStringLiteral("--theme") || arg == QStringLiteral("-t")) && i + 1 < arguments.size()) {
      const QString value = normalizedThemeValue(QString::fromLocal8Bit(arguments[i + 1]));
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

bool writeAppearance(const QTemporaryDir& dir, const QString& theme) {
  if (!dir.isValid()) {
    qWarning() << "Error: Could not create temporary theme config directory";
    return false;
  }

  const QString path = dir.filePath(QStringLiteral("appearance.toml"));
  HoloNight::Config::Appearance appearance = HoloNight::Config::defaults();
  appearance.theme.scheme = theme.toStdString();
  appearance.theme.accent = "default";
  if (!HoloNight::Config::writeAtomically(appearance, std::filesystem::path{path.toStdString()})) {
    qWarning() << "Error: Could not write temporary appearance" << path;
    return false;
  }

  qputenv("HOLONIGHT_APPEARANCE_FILE", path.toLocal8Bit());
  return true;
}

void addPluginPaths() {
  const QString executableDir = QFileInfo(QStringLiteral("/proc/self/exe")).canonicalPath();
  const QString plugins =
      executableDir == QStringLiteral(HOLONIGHT_EXECUTABLE_BUILD_DIR)
          ? QStringLiteral(HOLONIGHT_BUILD_PLUGIN_DIR)
          : QDir(executableDir).absoluteFilePath(QStringLiteral("../" HOLONIGHT_INSTALLED_PLUGIN_DIR));
  QCoreApplication::addLibraryPath(plugins);
}

bool parseArguments(QCoreApplication& app) {
  QCommandLineParser parser;
  parser.setApplicationDescription(QStringLiteral("HoloNight Demo"));
  parser.addHelpOption();
  parser.addVersionOption();

  QCommandLineOption themeOption(QStringList() << QStringLiteral("t") << QStringLiteral("theme"),
                                 QStringLiteral("Theme scheme to use."), QStringLiteral("theme"));
  parser.addOption(themeOption);

  parser.process(app);

  if (parser.isSet(themeOption)) {
    const QString selectedTheme = Demo::normalizedThemeValue(parser.value(themeOption));
    if (!Demo::isThemeValue(selectedTheme)) {
      QStringList validThemes;
      for (const Holonight::ThemeVariantCatalogEntry& variant : Holonight::themeVariants()) {
        validThemes.append(variant.id);
      }
      qWarning() << "Error: Invalid theme" << selectedTheme << "(must be one of"
                 << validThemes.join(QStringLiteral(", ")) << ")";
      return false;
    }
  }

  return true;
}

QString widgetsExecutable(const QString& directory) {
  return QDir(directory).absoluteFilePath(QStringLiteral("holonight_widgets_demo"));
}

QStringList widgetsArguments(const QString& theme) {
  return theme.isEmpty() ? QStringList{} : QStringList{QStringLiteral("--theme"), theme};
}

QString launchWidgets(const QString& directory, const QString& theme) {
  QProcess process;
  process.setProgram(widgetsExecutable(directory));
  process.setArguments(widgetsArguments(theme));
  auto environment = QProcessEnvironment::systemEnvironment();
  // The child creates and owns its own temporary appearance configuration.
  if (!theme.isEmpty()) environment.remove(QStringLiteral("HOLONIGHT_APPEARANCE_FILE"));
  process.setProcessEnvironment(environment);
  if (process.startDetached()) return {};
  return QStringLiteral("Could not open Qt Widgets demo: %1 (%2)").arg(process.program(), process.errorString());
}
}  // namespace Demo
