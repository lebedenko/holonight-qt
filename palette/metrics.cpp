// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "holonight/metrics.h"

namespace Holonight {

const ControlMetrics& MetricTokens::control(ControlSize size) const {
  return controls.at(static_cast<std::size_t>(size));
}

const MetricTokens& metricTokens() {
  static constexpr MetricTokens tokens;
  return tokens;
}

ControlSize normalizedControlSize(int value) {
  if (value < static_cast<int>(ControlSize::Compact) || value > static_cast<int>(ControlSize::Hero)) {
    return ControlSize::Normal;
  }
  return static_cast<ControlSize>(value);
}

}  // namespace Holonight
