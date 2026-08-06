// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "hniconimageprovider.h"

#include "iconrenderer.h"
#include "iconthemeresolver.h"

#include <QMutexLocker>
#include <QUrl>
#include <QUrlQuery>

namespace Holonight {
namespace {

constexpr int kDefaultIconExtent = 24;
constexpr int kMaximumIconExtent = 1024;

[[nodiscard]] QColor queryColor(const QUrlQuery& query, const QString& key, const QColor& fallback) {
  const QColor color{query.queryItemValue(key)};
  return color.isValid() ? color : fallback;
}

[[nodiscard]] QSize logicalSize(const QUrlQuery& query, const QSize& requested_size) {
  if (requested_size.isValid() && !requested_size.isEmpty()) {
    if (requested_size.width() <= kMaximumIconExtent && requested_size.height() <= kMaximumIconExtent) {
      return requested_size;
    }
    return {};
  }
  bool parsed = false;
  const int size = query.queryItemValue(QStringLiteral("size")).toInt(&parsed);
  if (parsed && size > 0 && size <= kMaximumIconExtent) {
    return QSize{size, size};
  }
  return QSize{kDefaultIconExtent, kDefaultIconExtent};
}

}  // namespace

HnIconImageProvider::HnIconImageProvider() : QQuickImageProvider{QQuickImageProvider::Image} {}

QImage HnIconImageProvider::requestImage(const QString& id, QSize* size, const QSize& requested_size) {
  const qsizetype query_start = id.indexOf(QLatin1Char('?'));
  const QString encoded_source = query_start >= 0 ? id.left(query_start) : id;
  const QString encoded_query = query_start >= 0 ? id.mid(query_start + 1) : QString{};
  const QUrlQuery query{encoded_query};
  const QString source = QUrl::fromPercentEncoding(encoded_source.toUtf8());
  const QSize logical_size = logicalSize(query, requested_size);
  if (!logical_size.isValid() || logical_size.isEmpty()) {
    if (size != nullptr) {
      *size = {};
    }
    return {};
  }
  const IconSemanticColors colors{
      .text = queryColor(query, QStringLiteral("color"), QColor{QStringLiteral("#ffffffff")}),
      .highlight = queryColor(query, QStringLiteral("highlight"), QColor{QStringLiteral("#ffffffff")}),
      .positive = queryColor(query, QStringLiteral("positive"), QColor{QStringLiteral("#ffffffff")}),
      .neutral = queryColor(query, QStringLiteral("neutral"), QColor{QStringLiteral("#ffffffff")}),
      .negative = queryColor(query, QStringLiteral("negative"), QColor{QStringLiteral("#ffffffff")}),
  };

  const QString cache_key =
      source + QLatin1Char('|') + QString::number(logical_size.width()) + QLatin1Char('x') +
      QString::number(logical_size.height()) + QLatin1Char('|') + colors.text.name(QColor::HexArgb) + QLatin1Char('|') +
      colors.highlight.name(QColor::HexArgb) + QLatin1Char('|') + colors.positive.name(QColor::HexArgb) +
      QLatin1Char('|') + colors.neutral.name(QColor::HexArgb) + QLatin1Char('|') +
      colors.negative.name(QColor::HexArgb) + QLatin1Char('|') + query.queryItemValue(QStringLiteral("palette"));

  {
    QMutexLocker locker = QMutexLocker{&mutex_};
    while (true) {
      const auto cached = cache_.constFind(cache_key);
      if (cached != cache_.constEnd()) {
        if (size != nullptr) {
          *size = cached->size();
        }
        return *cached;
      }
      if (!in_flight_.contains(cache_key)) {
        in_flight_.insert(cache_key);
        break;
      }
      cache_ready_.wait(&mutex_);
    }
  }

  const auto finish_request = [this, &cache_key](const QImage& image) {
    const QMutexLocker locker{&mutex_};
    in_flight_.remove(cache_key);
    if (!image.isNull()) {
      const qsizetype byte_count = image.sizeInBytes();
      while (!cache_.isEmpty() && cache_cost_bytes_ + byte_count > kMaximumCacheCostBytes) {
        auto oldest = cache_.begin();
        cache_cost_bytes_ -= oldest->sizeInBytes();
        cache_.erase(oldest);
      }
      if (byte_count <= kMaximumCacheCostBytes) {
        cache_.insert(cache_key, image);
        cache_cost_bytes_ += byte_count;
      }
    }
    cache_ready_.wakeAll();
  };

  const QByteArray svg_bytes = IconThemeResolver::resolveSvgBytes(source);
  if (svg_bytes.isEmpty()) {
    finish_request({});
    return {};
  }

  const QImage image = IconRenderer::renderSvg(svg_bytes, logical_size, colors);
  if (size != nullptr) {
    *size = image.size();
  }
  finish_request(image);
  return image;
}

int HnIconImageProvider::cacheSize() const {
  const QMutexLocker locker{&mutex_};
  return cache_.size();
}

}  // namespace Holonight
