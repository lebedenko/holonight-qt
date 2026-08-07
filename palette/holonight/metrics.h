// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace Holonight {

enum class ControlSize : std::uint8_t { Compact, Normal, Large, Hero };

struct ControlMetrics {
  int height;
  int horizontal_padding;
  int icon_size;
  int internal_spacing;

  [[nodiscard]] bool operator==(const ControlMetrics&) const = default;
};

struct MetricTokens {
  int border_width = 1;
  int focus_border_width = 1;
  int separator_width = 1;
  int header_height = 56;
  int app_title_icon_size = 32;
  int app_title_icon_spacing = 12;
  int app_title_text_spacing = 8;
  std::array<ControlMetrics, 4> controls{{
      {.height = 28, .horizontal_padding = 8, .icon_size = 16, .internal_spacing = 4},
      {.height = 32, .horizontal_padding = 8, .icon_size = 16, .internal_spacing = 6},
      {.height = 40, .horizontal_padding = 12, .icon_size = 20, .internal_spacing = 8},
      {.height = 52, .horizontal_padding = 16, .icon_size = 24, .internal_spacing = 10},
  }};

  [[nodiscard]] const ControlMetrics& control(ControlSize size) const;
  [[nodiscard]] bool operator==(const MetricTokens&) const = default;
};

[[nodiscard]] const MetricTokens& metricTokens();
[[nodiscard]] ControlSize normalizedControlSize(int value);

}  // namespace Holonight
