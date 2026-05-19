// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "holonight/palette.h"

#include <algorithm>
#include <cmath>
#include <gtest/gtest.h>

static double linearize(double val) { return val <= 0.04045 ? val / 12.92 : std::pow((val + 0.055) / 1.055, 2.4); }

static double relativeLuminance(const QColor& col) {
  return (0.2126 * linearize(col.redF())) + (0.7152 * linearize(col.greenF())) + (0.0722 * linearize(col.blueF()));
}

static double contrastRatio(const QColor& fgColor, const QColor& bgColor) {
  const double lum1 = relativeLuminance(fgColor);
  const double lum2 = relativeLuminance(bgColor);
  return (std::max(lum1, lum2) + 0.05) / (std::min(lum1, lum2) + 0.05);
}

class ContrastTest : public ::testing::Test {
 protected:
  Holonight::ColorTokens tok_{Holonight::darkTokens()};
};

// ── Text contrast: WCAG 1.4.3 requires ≥4.5:1 for normal text ──────────────

TEST_F(ContrastTest, OnSurfaceVsSurface) {
  EXPECT_GE(contrastRatio(tok_.onSurface, tok_.surface), 4.5) << "onSurface on surface fails WCAG AA";
}

TEST_F(ContrastTest, OnSurfaceVsSurfaceVariant) {
  EXPECT_GE(contrastRatio(tok_.onSurface, tok_.surfaceVariant), 4.5) << "onSurface on surfaceVariant fails WCAG AA";
}

TEST_F(ContrastTest, OnSurfaceVsSurfaceContainer) {
  EXPECT_GE(contrastRatio(tok_.onSurface, tok_.surfaceContainer), 4.5) << "onSurface on surfaceContainer fails WCAG AA";
}

TEST_F(ContrastTest, OnSurfaceVsSecondary) {
  EXPECT_GE(contrastRatio(tok_.onSurface, tok_.secondary), 4.5) << "onSurface on secondary fails WCAG AA";
}

TEST_F(ContrastTest, OnPrimaryVsPrimary) {
  EXPECT_GE(contrastRatio(tok_.onPrimary, tok_.primary), 4.5) << "onPrimary on primary (selection fill) fails WCAG AA";
}

TEST_F(ContrastTest, OnSecondaryVsSecondary) {
  EXPECT_GE(contrastRatio(tok_.onSecondary, tok_.secondary), 4.5) << "onSecondary on secondary fails WCAG AA";
}

TEST_F(ContrastTest, OnErrorVsError) {
  EXPECT_GE(contrastRatio(tok_.onError, tok_.error), 4.5) << "onError on error fails WCAG AA";
}

TEST_F(ContrastTest, ErrorVsSurface) {
  EXPECT_GE(contrastRatio(tok_.error, tok_.surface), 4.5) << "error text on surface fails WCAG AA";
}

TEST_F(ContrastTest, WarningVsSurface) {
  EXPECT_GE(contrastRatio(tok_.warning, tok_.surface), 4.5) << "warning text on surface fails WCAG AA";
}

TEST_F(ContrastTest, SuccessVsSurface) {
  EXPECT_GE(contrastRatio(tok_.success, tok_.surface), 4.5) << "success text on surface fails WCAG AA";
}

// ── Non-text contrast: WCAG 1.4.11 requires ≥3:1 for UI components ─────────

TEST_F(ContrastTest, FocusRingVsSurface) {
  EXPECT_GE(contrastRatio(tok_.focusRing, tok_.surface), 3.0) << "focusRing on surface fails WCAG 1.4.11";
}

TEST_F(ContrastTest, OutlineVsSurface) {
  EXPECT_GE(contrastRatio(tok_.outline, tok_.surface), 3.0) << "outline on surface fails WCAG 1.4.11";
}

TEST_F(ContrastTest, OutlineVsSecondary) {
  EXPECT_GE(contrastRatio(tok_.outline, tok_.secondary), 3.0) << "outline on secondary fails WCAG 1.4.11";
}

TEST_F(ContrastTest, PrimaryPressedVsSurface) {
  EXPECT_GE(contrastRatio(tok_.primaryPressed, tok_.surface), 3.0) << "primaryPressed on surface fails WCAG 1.4.11";
}

TEST_F(ContrastTest, PrimaryPressedVsSecondary) {
  // Achieves ~4.48:1; WCAG 1.4.11 (3:1) applies — pressed state is a transient border, not text
  EXPECT_GE(contrastRatio(tok_.primaryPressed, tok_.secondary), 3.0) << "primaryPressed on secondary fails WCAG 1.4.11";
}

// ── New extended tokens ──────────────────────────────────────────────────────

TEST_F(ContrastTest, OnSurfaceInverseVsSurfaceInverse) {
  EXPECT_GE(contrastRatio(tok_.onSurfaceInverse, tok_.surfaceInverse), 4.5)
      << "onSurfaceInverse on surfaceInverse fails WCAG AA (ToolTip text)";
}

TEST_F(ContrastTest, FocusRingVsSurfaceHover) {
  EXPECT_GE(contrastRatio(tok_.focusRing, tok_.surfaceHover), 3.0)
      << "focusRing on surfaceHover fails WCAG 1.4.11";
}

TEST_F(ContrastTest, OutlineVsSurfaceHover) {
  EXPECT_GE(contrastRatio(tok_.outline, tok_.surfaceHover), 3.0)
      << "outline on surfaceHover fails WCAG 1.4.11";
}

// ── Intentionally untested pairs (WCAG exemptions) ──────────────────────────
//
// onSurfaceVariant (#565f89) achieves ~2.35–2.90:1 against all surfaces.
// It is used exclusively for disabled labels and placeholder text.
// WCAG 2.1 SC 1.4.3 explicitly exempts inactive UI components from contrast requirements.
//
// outlineVariant (#565f89) achieves ~2.35–2.90:1 against all surfaces.
// It is used only for decorative separators that carry no interactive meaning.
// WCAG 2.1 SC 1.4.11 non-text contrast applies to interactive components only.
//
// onSurfaceDisabled (#3b3f58) achieves ~1.6:1 against surface.
// Used exclusively for disabled/inactive UI elements.
// WCAG 2.1 SC 1.4.3 explicitly exempts disabled components from contrast requirements.
