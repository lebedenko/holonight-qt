// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "iconthemeresolver.h"

#include "holonight/appearance_reader.h"

#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QHash>
#include <QIcon>
#include <QMutex>
#include <QMutexLocker>
#include <QRegularExpression>
#include <QStandardPaths>
#include <QUrl>

namespace Holonight {
namespace {

constexpr qsizetype kMaximumSvgBytes = 1024 * 1024;
constexpr qsizetype kMaximumThemeIconCacheEntries = 256;
QMutex themeIconCacheMutex;
QHash<QString, QByteArray> themeIconCache;

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

[[nodiscard]] QByteArray resolveThemeIcon(const QString& name) {
  const QString icon_name = name.endsWith(QStringLiteral(".svg")) ? name.chopped(4) : name;
  const QStringList roots = iconSearchRoots();
  const QStringList themes = iconThemeNames();
  const QString cache_key =
      icon_name + QLatin1Char('|') + roots.join(QLatin1Char('|')) + QLatin1Char('|') + themes.join(QLatin1Char('|'));
  {
    const QMutexLocker locker{&themeIconCacheMutex};
    const auto cached = themeIconCache.constFind(cache_key);
    if (cached != themeIconCache.constEnd()) {
      return *cached;
    }
  }

  QByteArray bytes;
  for (const QString& root : roots) {
    for (const QString& theme : themes) {
      const QString theme_root = root + QLatin1Char('/') + theme;
      if (!QFileInfo::exists(theme_root)) {
        continue;
      }
      QDirIterator iterator =
          QDirIterator{theme_root, {icon_name + QStringLiteral(".svg")}, QDir::Files, QDirIterator::Subdirectories};
      if (iterator.hasNext()) {
        bytes = readFile(iterator.next());
        break;
      }
    }
    if (!bytes.isEmpty()) {
      break;
    }
  }

  const QMutexLocker locker{&themeIconCacheMutex};
  if (themeIconCache.size() >= kMaximumThemeIconCacheEntries) {
    themeIconCache.clear();
  }
  themeIconCache.insert(cache_key, bytes);
  return bytes;
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
