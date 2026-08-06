// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "hniconprovider.h"

#include "hniconimageprovider.h"
#include "iconthemeresolver.h"

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

[[nodiscard]] bool hasSemanticColorClasses(const QByteArray& svg_bytes) {
  return svg_bytes.contains("ColorScheme-Text") || svg_bytes.contains("ColorScheme-Highlight") ||
         svg_bytes.contains("ColorScheme-PositiveText") || svg_bytes.contains("ColorScheme-NeutralText") ||
         svg_bytes.contains("ColorScheme-NegativeText");
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
                                  int palette_revision) {
  const QString source_string = source.toString();
  if (source_string.isEmpty()) {
    return {};
  }

  ensureProviderRegistered();

  QUrlQuery query;
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

bool HnIconProvider::supportsSemanticColors(const QUrl& source) const {
  const QString source_string = source.toString();
  if (source_string.isEmpty()) {
    return false;
  }
  return hasSemanticColorClasses(Holonight::IconThemeResolver::resolveSvgBytes(source_string));
}
