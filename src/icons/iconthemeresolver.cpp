// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "iconthemeresolver.h"

#include "holonight/appearance_reader.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QIcon>
#include <QRegularExpression>
#include <QSet>
#include <QSettings>
#include <QStandardPaths>
#include <QUrl>

namespace Holonight {
namespace {

constexpr qsizetype kMaximumSvgBytes = 1024 * 1024;
[[nodiscard]] QByteArray readFile(const QString& path) {
  QFile file = QFile{path};
  if (!file.open(QIODevice::ReadOnly)) {
    return {};
  }
  const QByteArray bytes = file.read(kMaximumSvgBytes + 1);
  if (file.error() != QFileDevice::NoError || bytes.size() > kMaximumSvgBytes) {
    return {};
  }
  return bytes;
}

[[nodiscard]] QString qrcPathFromUrl(const QUrl& url) {
  QString path = url.path();
  if (!path.startsWith(QLatin1Char('/'))) {
    path.prepend(QLatin1Char('/'));
  }
  return QLatin1Char(':') + path;
}

[[nodiscard]] QByteArray resolveDirectSource(const QString& source) {
  const QUrl url{source};
  if (url.scheme() == QStringLiteral("qrc")) {
    return readFile(qrcPathFromUrl(url));
  }
  if (url.isLocalFile()) {
    return readFile(url.toLocalFile());
  }
  if (source.startsWith(QStringLiteral(":/"))) {
    return readFile(source);
  }
  if (QFileInfo::exists(source)) {
    return readFile(source);
  }
  return {};
}

[[nodiscard]] QStringList iconThemeNames() {
  const AppearanceReader reader;
  const ResolvedAppearance& config = reader.appearance();
  QStringList names;
  if (!config.icon_theme.isEmpty()) {
    names << config.icon_theme;
  }
  if (!QIcon::themeName().isEmpty()) {
    names << QIcon::themeName();
  }
  if (!config.fallback_icon_theme.isEmpty()) {
    names << config.fallback_icon_theme;
  }
  if (!QIcon::fallbackThemeName().isEmpty()) {
    names << QIcon::fallbackThemeName();
  }
  names << QStringLiteral("hicolor");
  names.removeDuplicates();
  return names;
}

[[nodiscard]] QStringList iconSearchRoots() {
  QStringList roots = QIcon::themeSearchPaths();
  roots << QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation)
               .replaceInStrings(QRegularExpression{QStringLiteral("$")}, QStringLiteral("/icons"));
  roots.removeDuplicates();
  return roots;
}

// Search every root for the selected theme before visiting its parents. An index
// may describe a theme split over several roots; inheritance cycles are legal input.
[[nodiscard]] QByteArray resolveInTheme(const QString& name, const QString& theme, const QStringList& roots,
                                        QSet<QString>& visited) {
  if (visited.contains(theme)) return {};
  visited.insert(theme);
  QStringList directories;
  QStringList parents;
  for (const auto& root : roots) {
    QSettings index(root + QLatin1Char('/') + theme + QStringLiteral("/index.theme"), QSettings::IniFormat);
    index.beginGroup(QStringLiteral("Icon Theme"));
    directories.append(index.value(QStringLiteral("Directories")).toStringList());
    directories.append(index.value(QStringLiteral("ScaledDirectories")).toStringList());
    parents.append(index.value(QStringLiteral("Inherits")).toStringList());
  }
  directories.removeDuplicates();
  parents.removeDuplicates();
  for (const auto& root : roots) {
    for (const auto& directory : directories) {
      const auto bytes = readFile(root + QLatin1Char('/') + theme + QLatin1Char('/') + directory + QLatin1Char('/') +
                                  name + QStringLiteral(".svg"));
      if (!bytes.isEmpty()) return bytes;
    }
  }
  for (const auto& parent : parents) {
    const auto bytes = resolveInTheme(name, parent, roots, visited);
    if (!bytes.isEmpty()) return bytes;
  }
  return {};
}

[[nodiscard]] QByteArray resolveThemeIcon(const QString& name) {
  QString icon_name = name.endsWith(QStringLiteral(".svg")) ? name.chopped(4) : name;
  // URLs and filesystem paths are never theme names.
  if (icon_name.contains(QLatin1Char('/')) || icon_name.contains(QLatin1Char(':'))) return {};
  const auto roots = iconSearchRoots();
  const auto themes = iconThemeNames();
  while (!icon_name.isEmpty()) {
    QSet<QString> visited;
    for (const auto& theme : themes) {
      const auto bytes = resolveInTheme(icon_name, theme, roots, visited);
      if (!bytes.isEmpty()) return bytes;
    }
    const auto separator = icon_name.lastIndexOf(QLatin1Char('-'));
    if (separator < 0) break;
    icon_name.truncate(separator);
  }
  return {};
}

}  // namespace

QByteArray IconThemeResolver::resolveSvgBytes(const QString& source) {
  const QByteArray direct = resolveDirectSource(source);
  if (!direct.isEmpty()) {
    return direct;
  }
  return resolveThemeIcon(source);
}

}  // namespace Holonight
