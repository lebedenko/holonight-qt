// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "hnmetrics.h"

int HnMetrics::normalizedSizeRole(int size_role) const {
  return static_cast<int>(Holonight::normalizedControlSize(size_role));
}

int HnMetrics::controlHeight(int size_role) const {
  return tokens().control(Holonight::normalizedControlSize(size_role)).height;
}

int HnMetrics::horizontalPadding(int size_role) const {
  return tokens().control(Holonight::normalizedControlSize(size_role)).horizontal_padding;
}

int HnMetrics::iconSize(int size_role) const {
  return tokens().control(Holonight::normalizedControlSize(size_role)).icon_size;
}

int HnMetrics::internalSpacing(int size_role) const {
  return tokens().control(Holonight::normalizedControlSize(size_role)).internal_spacing;
}
