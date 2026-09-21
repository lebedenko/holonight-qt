// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#pragma once

#include <QtTypes>

#include <algorithm>
#include <cmath>

namespace Holonight {

struct SeparatorAlignment {
  qreal device_pixel_ratio;
  qreal painted_thickness;
  qreal painted_offset;
};

[[nodiscard]] inline SeparatorAlignment separatorAlignment(qreal requested_thickness, qreal standard_thickness,
                                                           qreal device_pixel_ratio, qreal scene_coordinate,
                                                           qreal minor_axis_scale = 1.0) {
  const qreal effective_dpr = std::isfinite(device_pixel_ratio) ? std::max<qreal>(1.0, device_pixel_ratio) : 1.0;

  qreal painted_thickness = 0.0;
  if (std::isfinite(requested_thickness) && requested_thickness > 0.0 && std::isfinite(standard_thickness) &&
      standard_thickness > 0.0 && std::isfinite(scene_coordinate) && std::isfinite(minor_axis_scale) &&
      minor_axis_scale != 0.0) {
    const qreal physical_pixels = std::max<qreal>(1.0, std::round(requested_thickness / standard_thickness));
    painted_thickness = physical_pixels / effective_dpr / std::abs(minor_axis_scale);
  }

  qreal painted_offset = 0.0;
  if (painted_thickness > 0.0 && std::isfinite(scene_coordinate)) {
    painted_offset =
        ((std::round(scene_coordinate * effective_dpr) / effective_dpr) - scene_coordinate) / minor_axis_scale;
  }

  if (!std::isfinite(painted_thickness) || !std::isfinite(painted_offset)) {
    painted_thickness = 0.0;
    painted_offset = 0.0;
  }

  return {
      .device_pixel_ratio = effective_dpr,
      .painted_thickness = painted_thickness,
      .painted_offset = painted_offset,
  };
}

}  // namespace Holonight
