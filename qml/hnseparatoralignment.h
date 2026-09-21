// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#pragma once

#include <QRectF>

#include <cmath>
#include <cstdint>
#include <utility>

namespace Holonight {

enum class SeparatorCrossAlignment : std::uint8_t { Leading, Center, Trailing };

// Round ties toward +infinity, including negative coordinates. Unlike round(), this
// is invariant under integer translation: snap(edge - pixels) == snap(edge) - pixels.
[[nodiscard]] inline qreal snapSeparatorBoundary(qreal physical_coordinate) {
  return std::floor(physical_coordinate + 0.5);
}

[[nodiscard]] inline QRectF separatorRectangle(const QRectF& slot, const QPointF& scene_origin,
                                               const QPointF& signed_scale, qreal dpr, int pixels, bool vertical,
                                               SeparatorCrossAlignment alignment) {
  const qreal scale_x = signed_scale.x() * dpr;
  const qreal scale_y = signed_scale.y() * dpr;
  const qreal origin_x = scene_origin.x() * dpr;
  const qreal origin_y = scene_origin.y() * dpr;
  if (pixels <= 0 || !std::isfinite(dpr) || dpr <= 0 || !std::isfinite(scale_x) || !std::isfinite(scale_y) ||
      scale_x == 0 || scale_y == 0 || !std::isfinite(origin_x) || !std::isfinite(origin_y) ||
      !std::isfinite(slot.width()) || !std::isfinite(slot.height()) || slot.width() <= 0 || slot.height() <= 0) {
    return {};
  }

  const auto axis = [](qreal origin, qreal scale, qreal length) {
    return std::pair{snapSeparatorBoundary(origin), snapSeparatorBoundary(origin + (scale * length))};
  };
  auto [left, right] = axis(origin_x, scale_x, slot.width());
  auto [top, bottom] = axis(origin_y, scale_y, slot.height());
  qreal& start = vertical ? left : top;
  qreal& end = vertical ? right : bottom;
  const qreal origin = vertical ? origin_x : origin_y;
  const qreal scale = vertical ? scale_x : scale_y;
  const qreal length = vertical ? slot.width() : slot.height();
  const qreal signed_pixels = std::copysign(static_cast<qreal>(pixels), scale);
  switch (alignment) {
    case SeparatorCrossAlignment::Leading:
      end = start + signed_pixels;
      break;
    case SeparatorCrossAlignment::Center:
      start = snapSeparatorBoundary(origin + (((scale * length) - signed_pixels) / 2));
      end = start + signed_pixels;
      break;
    case SeparatorCrossAlignment::Trailing:
      start = end - signed_pixels;
      break;
  }
  const QRectF result((left - origin_x) / scale_x, (top - origin_y) / scale_y, (right - left) / scale_x,
                      (bottom - top) / scale_y);
  if (!std::isfinite(result.x()) || !std::isfinite(result.y()) || !std::isfinite(result.width()) ||
      !std::isfinite(result.height()) || result.width() <= 0 || result.height() <= 0) {
    return {};
  }
  return result;
}

}  // namespace Holonight
