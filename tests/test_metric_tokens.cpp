// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "holonight/metrics.h"

#include <gtest/gtest.h>

TEST(MetricTokens, ScalarsMatchAcceptedContract) {
  const Holonight::MetricTokens& tokens = Holonight::metricTokens();
  EXPECT_EQ(tokens.border_width, 1);
  EXPECT_EQ(tokens.focus_border_width, 1);
  EXPECT_EQ(tokens.separator_width, 1);
  EXPECT_EQ(tokens.header_height, 56);
  EXPECT_EQ(tokens.app_title_icon_size, 32);
  EXPECT_EQ(tokens.app_title_icon_spacing, 12);
  EXPECT_EQ(tokens.app_title_text_spacing, 8);
}

TEST(MetricTokens, ControlRolesMatchAcceptedContract) {
  const Holonight::MetricTokens& tokens = Holonight::metricTokens();
  EXPECT_EQ(tokens.control(Holonight::ControlSize::Compact), (Holonight::ControlMetrics{28, 8, 16, 4}));
  EXPECT_EQ(tokens.control(Holonight::ControlSize::Normal), (Holonight::ControlMetrics{32, 8, 16, 6}));
  EXPECT_EQ(tokens.control(Holonight::ControlSize::Large), (Holonight::ControlMetrics{40, 12, 20, 8}));
  EXPECT_EQ(tokens.control(Holonight::ControlSize::Hero), (Holonight::ControlMetrics{52, 16, 24, 10}));
}

TEST(MetricTokens, InvalidControlRoleNormalizesToNormal) {
  EXPECT_EQ(Holonight::normalizedControlSize(-1), Holonight::ControlSize::Normal);
  EXPECT_EQ(Holonight::normalizedControlSize(999), Holonight::ControlSize::Normal);
}
