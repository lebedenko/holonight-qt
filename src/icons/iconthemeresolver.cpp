// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "iconthemeresolver.h"

#include <QFile>
#include <QFileInfo>
#include <QIcon>
#include <QSet>
#include <QSettings>
#include <QUrl>

#include <limits>

namespace Holonight {
namespace {

constexpr qsizetype kMaximumSvgBytes = 1024 * 1024;
[[nodiscard]] QByteArray readFile(const QString& path, qsizetype limit = kMaximumSvgBytes) {
  QFile file = QFile{path};
  if (!file.open(QIODevice::ReadOnly)) {
    return {};
  }
  const QByteArray bytes = file.read(limit + 1);
  if (file.error() != QFileDevice::NoError || bytes.size() > limit) {
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

[[nodiscard]] QString directPath(const QString& source) {
  const QUrl url{source};
  if (url.scheme() == QStringLiteral("qrc")) return qrcPathFromUrl(url);
  if (url.isLocalFile()) return url.toLocalFile();
  if (source.startsWith(QStringLiteral(":/")) || QFileInfo::exists(source)) return source;
  return {};
}

[[nodiscard]] QStringList iconThemeNames() {
  QStringList names;
  if (!QIcon::themeName().isEmpty()) {
    names << QIcon::themeName();
  }
  if (!QIcon::fallbackThemeName().isEmpty()) {
    names << QIcon::fallbackThemeName();
  }
  names << QStringLiteral("hicolor");
  names.removeDuplicates();
  return names;
}

[[nodiscard]] QStringList iconSearchRoots() { return QIcon::themeSearchPaths(); }

[[nodiscard]] int directoryDistance(const QSettings& index, const QString& directory, int requested,
                                    qreal requested_scale) {
  const QString group = directory.endsWith(QStringLiteral("/.")) ? directory.chopped(2) : directory;
  const int size = index.value(group + QStringLiteral("/Size"), 0).toInt();
  if (size <= 0) return (std::numeric_limits<int>::max)();
  const int scale = (std::max)(1, index.value(group + QStringLiteral("/Scale"), 1).toInt());
  const QString type = index.value(group + QStringLiteral("/Type"), QStringLiteral("Threshold")).toString();
  int minimum = size;
  int maximum = size;
  if (type == QStringLiteral("Scalable")) {
    minimum = index.value(group + QStringLiteral("/MinSize"), size).toInt();
    maximum = index.value(group + QStringLiteral("/MaxSize"), size).toInt();
  } else if (type == QStringLiteral("Threshold")) {
    const int threshold = index.value(group + QStringLiteral("/Threshold"), 2).toInt();
    minimum -= threshold;
    maximum += threshold;
  }
  int distance = 0;
  if (requested < minimum) distance = minimum - requested;
  if (requested > maximum) distance = requested - maximum;
  return distance * 1000 + qRound(qAbs(scale - requested_scale) * 100);
}

// Search every root of a theme before walking its inherited themes.
[[nodiscard]] QString resolvePathInTheme(const QString& name, const QString& theme, const QStringList& roots,
                                         int requested, qreal scale, QSet<QString>& visited) {
  if (visited.contains(theme)) return {};
  visited.insert(theme);
  struct Candidate {
    QString path;
    int distance = (std::numeric_limits<int>::max)();
  } best;
  QStringList parents;
  for (const auto& root : roots) {
    const QString theme_path = root + QLatin1Char('/') + theme;
    if (!QFileInfo::exists(theme_path + QStringLiteral("/index.theme"))) continue;
    QSettings index(theme_path + QStringLiteral("/index.theme"), QSettings::IniFormat);
    // QSettings parses INI sections lazily. Icon directories contain slashes in their section names;
    // enumerate them before looking up nested keys so those sections are available on first access.
    index.allKeys();
    index.beginGroup(QStringLiteral("Icon Theme"));
    QStringList directories = index.value(QStringLiteral("Directories")).toStringList();
    directories.append(index.value(QStringLiteral("ScaledDirectories")).toStringList());
    parents.append(index.value(QStringLiteral("Inherits")).toStringList());
    index.endGroup();
    for (const auto& directory : directories) {
      const int distance = directoryDistance(index, directory, requested, scale);
      if (distance > best.distance) continue;
      for (const auto& extension : {QStringLiteral(".svg"), QStringLiteral(".png"), QStringLiteral(".xpm")}) {
        const QString path = theme_path + QLatin1Char('/') + directory + QLatin1Char('/') + name + extension;
        if (QFileInfo{path}.isFile() && distance < best.distance) {
          best = {path, distance};
        }
      }
    }
  }
  if (!best.path.isEmpty()) return best.path;
  parents.removeDuplicates();
  for (const auto& parent : parents) {
    const QString path = resolvePathInTheme(name, parent, roots, requested, scale, visited);
    if (!path.isEmpty()) return path;
  }
  return {};
}

}  // namespace

QString IconThemeResolver::resolveIconPath(const QString& source, QSize size, qreal scale) {
  const QString direct = directPath(source);
  if (!direct.isEmpty()) return QFileInfo{direct}.isFile() ? direct : QString{};
  if (source.contains(QLatin1Char('/')) || source.contains(QLatin1Char(':'))) return {};
  QString name = source.endsWith(QStringLiteral(".svg")) ? source.chopped(4) : source;
  const QStringList roots = iconSearchRoots();
  const QStringList themes = iconThemeNames();
  const int requested = (std::max)(size.width(), size.height());
  while (!name.isEmpty()) {
    for (const auto& theme : themes) {
      QSet<QString> visited;
      const QString path = resolvePathInTheme(name, theme, roots, requested, scale, visited);
      if (!path.isEmpty()) return path;
    }
    if (name.endsWith(QStringLiteral("-symbolic"))) break;
    const int separator = name.lastIndexOf(QLatin1Char('-'));
    if (separator < 0) break;
    name.truncate(separator);
  }
  return {};
}

QByteArray IconThemeResolver::readIconBytes(const QString& path) {
  return readFile(path,
                  path.endsWith(QStringLiteral(".svg"), Qt::CaseInsensitive) ? kMaximumSvgBytes : 8 * 1024 * 1024);
}

}  // namespace Holonight
