// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#pragma once

#include <QHash>
#include <QImage>
#include <QMutex>
#include <QQuickImageProvider>
#include <QSet>
#include <QWaitCondition>

namespace Holonight {

class HnIconImageProvider final : public QQuickImageProvider {
 public:
  HnIconImageProvider();

  [[nodiscard]] QImage requestImage(const QString& id, QSize* size, const QSize& requested_size) override;
  [[nodiscard]] int cacheSize() const;

 private:
  static constexpr qsizetype kMaximumCacheCostBytes = 16 * 1024 * 1024;

  mutable QMutex mutex_;
  QWaitCondition cache_ready_;
  QHash<QString, QImage> cache_;
  QSet<QString> in_flight_;
  qsizetype cache_cost_bytes_ = 0;
};

}  // namespace Holonight
