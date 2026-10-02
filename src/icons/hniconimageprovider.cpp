// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "hniconimageprovider.h"

#include "iconrenderer.h"
#include "iconthemeresolver.h"

#include <QCryptographicHash>
#include <QMutexLocker>
#include <QPainter>
#include <QSvgRenderer>
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

QImage renderImage(const QString& path, const QByteArray& source_bytes, QSize logical_size,
                   const IconSemanticColors& colors, bool semantic, bool symbolic) {
  QImage image;
  if (path.endsWith(QStringLiteral(".svg"), Qt::CaseInsensitive)) {
    if (semantic) {
      image = IconRenderer::renderSvg(source_bytes, logical_size, colors, symbolic);
    } else {
      // Original rendering must retain the source stylesheet as authored.
      QSvgRenderer renderer{source_bytes};
      if (renderer.isValid()) {
        image = QImage{logical_size, QImage::Format_ARGB32_Premultiplied};
        image.fill(Qt::transparent);
        QPainter painter{&image};
        renderer.render(&painter);
      }
    }
  } else {
    image = QImage::fromData(source_bytes).scaled(logical_size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    if (semantic && symbolic && !image.isNull()) {
      QPainter painter{&image};
      painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
      painter.fillRect(image.rect(), colors.text);
    }
  }
  return image;
}

}  // namespace

HnIconImageProvider::HnIconImageProvider() : QQuickImageProvider{QQuickImageProvider::Image} {}

QImage HnIconImageProvider::requestImage(const QString& identifier, QSize* size, const QSize& requested_size) {
  const qsizetype query_start = identifier.indexOf(QLatin1Char('?'));
  const QString encoded_source = query_start >= 0 ? identifier.left(query_start) : identifier;
  const QString encoded_query = query_start >= 0 ? identifier.mid(query_start + 1) : QString{};
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
      .accent = queryColor(query, QStringLiteral("accent"), {}),
      .background = queryColor(query, QStringLiteral("background"), {}),
      .highlightedText = queryColor(query, QStringLiteral("highlightedText"), {}),
  };

  // Resolve before cache lookup: the same name may now select another theme or
  // changed file. Cache rendered pixels by content, never stale source names.
  qreal dpr = query.queryItemValue(QStringLiteral("dpr")).toDouble();
  if (!qIsFinite(dpr) || dpr < 1 || dpr > 8) {
    dpr = 1;
  }
  const int extent = query.queryItemValue(QStringLiteral("size")).toInt();
  const QSize asset_size = query.hasQueryItem(QStringLiteral("dpr")) && extent > 0 && extent <= kMaximumIconExtent
                               ? QSize{extent, extent}
                               : logical_size;
  const QString path = IconThemeResolver::resolveIconPath(source, asset_size, dpr);
  const QByteArray source_bytes = IconThemeResolver::readIconBytes(path);
  if (source_bytes.isEmpty()) {
    if (size != nullptr) {
      *size = {};
    }
    return {};
  }
  const auto source_hash =
      QString::fromLatin1(QCryptographicHash::hash(source_bytes, QCryptographicHash::Sha256).toHex());
  const bool semantic = query.queryItemValue(QStringLiteral("semantic")) != QStringLiteral("0");
  const bool symbolic = source.endsWith(QStringLiteral("-symbolic")) ||
                        (semantic && (source.contains(QLatin1Char('/')) || source.contains(QLatin1Char(':'))));
  const QString cache_key =
      QString::number(static_cast<int>(colors.accent.isValid())) +
      QString::number(static_cast<int>(colors.background.isValid())) +
      QString::number(static_cast<int>(colors.highlightedText.isValid())) + QLatin1Char('|') +
      colors.accent.name(QColor::HexArgb) + QLatin1Char('|') + colors.background.name(QColor::HexArgb) +
      QLatin1Char('|') + colors.highlightedText.name(QColor::HexArgb) + QLatin1Char('|') +
      query.queryItemValue(QStringLiteral("state")) + QLatin1Char('|') + QString::number(dpr) + QLatin1Char('|') +
      source_hash + QLatin1Char('|') + QString::number(logical_size.width()) + QLatin1Char('x') +
      QString::number(logical_size.height()) + QLatin1Char('|') + colors.text.name(QColor::HexArgb) + QLatin1Char('|') +
      colors.highlight.name(QColor::HexArgb) + QLatin1Char('|') + colors.positive.name(QColor::HexArgb) +
      QLatin1Char('|') + colors.neutral.name(QColor::HexArgb) + QLatin1Char('|') +
      colors.negative.name(QColor::HexArgb) + QLatin1Char('|') + query.queryItemValue(QStringLiteral("palette")) +
      QLatin1Char('|') + (semantic ? QLatin1Char('1') : QLatin1Char('0')) + QLatin1Char('|') +
      (symbolic ? QLatin1Char('1') : QLatin1Char('0'));

  if (const QImage cached = beginRequest(cache_key, size); !cached.isNull()) {
    return cached;
  }

  const QImage image = renderImage(path, source_bytes, logical_size, colors, semantic, symbolic);
  if (size != nullptr) {
    *size = image.size();
  }
  finishRequest(cache_key, image);
  return image;
}

QImage HnIconImageProvider::beginRequest(const QString& cache_key, QSize* size) {
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
  return {};
}

void HnIconImageProvider::finishRequest(const QString& cache_key, const QImage& image) {
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
}

int HnIconImageProvider::cacheSize() const {
  const QMutexLocker locker{&mutex_};
  return static_cast<int>(cache_.size());
}

}  // namespace Holonight
