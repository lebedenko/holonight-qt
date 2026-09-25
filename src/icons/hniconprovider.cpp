// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "hniconprovider.h"

#include "hniconimageprovider.h"
#include "iconthemeresolver.h"

#include <QCryptographicHash>
#include <QDebug>
#include <QIcon>
#include <QQmlEngine>
#include <QUrlQuery>

#include <algorithm>

namespace {

constexpr auto kProviderRegistrationProperty = "_holonightIconProviderRegistered";
constexpr int kMaximumIconExtent = 1024;

[[nodiscard]] QString colorString(const QColor& color) { return color.name(QColor::HexArgb); }

[[nodiscard]] QString paletteHash(const QColor& highlight, const QColor& positive, const QColor& neutral,
                                  const QColor& negative) {
  const QString colors = colorString(highlight) + colorString(positive) + colorString(neutral) + colorString(negative);
  return QString::number(qHash(colors), 16);
}

}  // namespace

HnIconProvider::HnIconProvider(QObject* parent) : QObject{parent} {}

void HnIconProvider::ensureProviderRegistered() {
  QQmlEngine* engine = qmlEngine(this);
  if (engine == nullptr || engine->property(kProviderRegistrationProperty).toBool()) {
    return;
  }

  engine->addImageProvider(QStringLiteral("hnicons"), new Holonight::HnIconImageProvider{});
  engine->setProperty(kProviderRegistrationProperty, true);
}

QString HnIconProvider::sourceUrl(const QUrl& source, int size, const QColor& color, const QColor& highlight,
                                  const QColor& positive, const QColor& neutral, const QColor& negative,
                                  int palette_revision, bool semantic) {
  const QString source_string = source.toString();
  if (source_string.isEmpty() || source_string.startsWith(QStringLiteral("image://"))) {
    return {};
  }

  ensureProviderRegistered();
  if (qEnvironmentVariableIsSet("HOLONIGHT_RENDER_DIAGNOSTICS")) {
    qInfo() << "HN_ICON" << source_string << "theme" << QIcon::themeName() << "fallback" << QIcon::fallbackThemeName()
            << "paths" << QIcon::themeSearchPaths() << "qtFound" << QIcon::hasThemeIcon(source_string) << "providerPath"
            << Holonight::IconThemeResolver::resolveIconPath(source_string, QSize{size, size});
  }

  QUrlQuery query;
  query.addQueryItem(QStringLiteral("sourceRevision"),
                     QString::fromLatin1(QCryptographicHash::hash(Holonight::IconThemeResolver::readIconBytes(
                                                                      Holonight::IconThemeResolver::resolveIconPath(
                                                                          source_string, QSize{size, size})),
                                                                  QCryptographicHash::Sha256)
                                             .toHex()));
  query.addQueryItem(QStringLiteral("semantic"), semantic ? QStringLiteral("1") : QStringLiteral("0"));
  query.addQueryItem(QStringLiteral("size"), QString::number(std::clamp(size, 1, kMaximumIconExtent)));
  query.addQueryItem(QStringLiteral("color"), colorString(color));
  query.addQueryItem(QStringLiteral("highlight"), colorString(highlight));
  query.addQueryItem(QStringLiteral("positive"), colorString(positive));
  query.addQueryItem(QStringLiteral("neutral"), colorString(neutral));
  query.addQueryItem(QStringLiteral("negative"), colorString(negative));
  query.addQueryItem(QStringLiteral("palette"), paletteHash(highlight, positive, neutral, negative));
  query.addQueryItem(QStringLiteral("revision"), QString::number((std::max)(palette_revision, 0)));
  return QStringLiteral("image://hnicons/%1?%2")
      .arg(QString::fromLatin1(QUrl::toPercentEncoding(source_string)), query.toString(QUrl::FullyEncoded));
}
