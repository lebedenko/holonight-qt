// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "holonight/palette.h"

#include <gtest/gtest.h>

TEST(DarkTokens, AllColorsValid) {
  const Holonight::ColorTokens tok = Holonight::darkTokens();
  EXPECT_TRUE(tok.surface.isValid());
  EXPECT_TRUE(tok.surfaceVariant.isValid());
  EXPECT_TRUE(tok.surfaceContainer.isValid());
  EXPECT_TRUE(tok.surfaceHover.isValid());
  EXPECT_TRUE(tok.surfaceInverse.isValid());
  EXPECT_TRUE(tok.onSurface.isValid());
  EXPECT_TRUE(tok.onSurfaceVariant.isValid());
  EXPECT_TRUE(tok.onSurfaceDisabled.isValid());
  EXPECT_TRUE(tok.onSurfaceInverse.isValid());
  EXPECT_TRUE(tok.primary.isValid());
  EXPECT_TRUE(tok.primaryHover.isValid());
  EXPECT_TRUE(tok.primaryPressed.isValid());
  EXPECT_TRUE(tok.onPrimary.isValid());
  EXPECT_TRUE(tok.secondary.isValid());
  EXPECT_TRUE(tok.onSecondary.isValid());
  EXPECT_TRUE(tok.outline.isValid());
  EXPECT_TRUE(tok.outlineVariant.isValid());
  EXPECT_TRUE(tok.hover.isValid());
  EXPECT_TRUE(tok.pressed.isValid());
  EXPECT_TRUE(tok.focusRing.isValid());
  EXPECT_TRUE(tok.error.isValid());
  EXPECT_TRUE(tok.warning.isValid());
  EXPECT_TRUE(tok.success.isValid());
  EXPECT_TRUE(tok.onError.isValid());
  EXPECT_TRUE(tok.shadow.isValid());
  EXPECT_TRUE(tok.borderPassive.isValid());
  EXPECT_TRUE(tok.borderHover.isValid());
  EXPECT_TRUE(tok.borderFocus.isValid());
  EXPECT_TRUE(tok.borderActive.isValid());
  EXPECT_TRUE(tok.borderUrgent.isValid());
  EXPECT_TRUE(tok.accentCyan.isValid());
  EXPECT_TRUE(tok.accentBlue.isValid());
  EXPECT_TRUE(tok.accentViolet.isValid());
  EXPECT_TRUE(tok.textSubtle.isValid());
  EXPECT_TRUE(tok.workspaceOccupied.isValid());
  EXPECT_TRUE(tok.workspaceActive.isValid());
}

// hover, pressed, and borderHover are intentionally semi-transparent; shadow alpha is set at paint time
TEST(DarkTokens, OpaqueColorsAreOpaque) {
  const Holonight::ColorTokens tok = Holonight::darkTokens();
  EXPECT_EQ(tok.surface.alpha(), 255);
  EXPECT_EQ(tok.surfaceVariant.alpha(), 255);
  EXPECT_EQ(tok.surfaceContainer.alpha(), 255);
  EXPECT_EQ(tok.surfaceHover.alpha(), 255);
  EXPECT_EQ(tok.surfaceInverse.alpha(), 255);
  EXPECT_EQ(tok.onSurface.alpha(), 255);
  EXPECT_EQ(tok.onSurfaceVariant.alpha(), 255);
  EXPECT_EQ(tok.onSurfaceDisabled.alpha(), 255);
  EXPECT_EQ(tok.onSurfaceInverse.alpha(), 255);
  EXPECT_EQ(tok.primary.alpha(), 255);
  EXPECT_EQ(tok.primaryHover.alpha(), 255);
  EXPECT_EQ(tok.primaryPressed.alpha(), 255);
  EXPECT_EQ(tok.onPrimary.alpha(), 255);
  EXPECT_EQ(tok.secondary.alpha(), 255);
  EXPECT_EQ(tok.onSecondary.alpha(), 255);
  EXPECT_EQ(tok.outline.alpha(), 255);
  EXPECT_EQ(tok.outlineVariant.alpha(), 255);
  EXPECT_EQ(tok.focusRing.alpha(), 255);
  EXPECT_EQ(tok.error.alpha(), 255);
  EXPECT_EQ(tok.warning.alpha(), 255);
  EXPECT_EQ(tok.success.alpha(), 255);
  EXPECT_EQ(tok.onError.alpha(), 255);
  EXPECT_EQ(tok.shadow.alpha(), 255);
  // Semantic border tokens — borderHover excluded (intentionally semi-transparent)
  EXPECT_EQ(tok.borderPassive.alpha(), 255);
  EXPECT_EQ(tok.borderFocus.alpha(), 255);
  EXPECT_EQ(tok.borderActive.alpha(), 255);
  EXPECT_EQ(tok.borderUrgent.alpha(), 255);
  EXPECT_EQ(tok.accentCyan.alpha(), 255);
  EXPECT_EQ(tok.accentBlue.alpha(), 255);
  EXPECT_EQ(tok.accentViolet.alpha(), 255);
  EXPECT_EQ(tok.textSubtle.alpha(), 255);
  EXPECT_EQ(tok.workspaceOccupied.alpha(), 255);
  EXPECT_EQ(tok.workspaceActive.alpha(), 255);
}

TEST(DarkTokens, NoTransparentColors) {
  const Holonight::ColorTokens tok = Holonight::darkTokens();
  EXPECT_NE(tok.surface, QColor(Qt::transparent));
  EXPECT_NE(tok.primary, QColor(Qt::transparent));
  EXPECT_NE(tok.onSurface, QColor(Qt::transparent));
  EXPECT_NE(tok.error, QColor(Qt::transparent));
}

TEST(DarkTokens, HoverAndPressedAreSemiTransparent) {
  const Holonight::ColorTokens tok = Holonight::darkTokens();
  EXPECT_LT(tok.hover.alpha(), 255);
  EXPECT_LT(tok.pressed.alpha(), 255);
  EXPECT_TRUE(tok.hover.isValid());
  EXPECT_TRUE(tok.pressed.isValid());
  EXPECT_LT(tok.borderHover.alpha(), 255);
  EXPECT_TRUE(tok.borderHover.isValid());
}

TEST(DarkTokens, BorderTokenValues) {
  const Holonight::ColorTokens tok = Holonight::darkTokens();
  EXPECT_EQ(tok.borderPassive.rgb(), QColor(0x56, 0x5f, 0x89).rgb());
  EXPECT_EQ(tok.borderHover.red(), 0x7d);
  EXPECT_EQ(tok.borderHover.green(), 0xcf);
  EXPECT_EQ(tok.borderHover.blue(), 0xff);
  EXPECT_NEAR(tok.borderHover.alpha(), 77, 2);
  EXPECT_EQ(tok.borderFocus.rgb(), tok.focusRing.rgb());
  EXPECT_EQ(tok.borderFocus.rgb(), QColor(0x00, 0xe0, 0xff).rgb());
  EXPECT_EQ(tok.borderActive.rgb(), QColor(0x7a, 0xa2, 0xf7).rgb());
  EXPECT_EQ(tok.borderUrgent.rgb(), QColor(0xf7, 0x76, 0x8e).rgb());
}

TEST(DarkTokens, DesignSystemColorValues) {
  const Holonight::ColorTokens tok = Holonight::darkTokens();
  EXPECT_EQ(tok.surface.rgb(), QColor(0x10, 0x13, 0x1f).rgb());
  EXPECT_EQ(tok.surfaceVariant.rgb(), QColor(0x16, 0x19, 0x25).rgb());
  EXPECT_EQ(tok.surfaceContainer.rgb(), QColor(0x1a, 0x1b, 0x26).rgb());
  EXPECT_EQ(tok.secondary.rgb(), QColor(0x24, 0x28, 0x3b).rgb());
  EXPECT_EQ(tok.borderPassive.rgb(), QColor(0x56, 0x5f, 0x89).rgb());
  EXPECT_EQ(tok.onSurface.rgb(), QColor(0xc0, 0xca, 0xf5).rgb());
  EXPECT_EQ(tok.textSubtle.rgb(), QColor(0xa9, 0xb1, 0xd6).rgb());
  EXPECT_EQ(tok.accentCyan.rgb(), QColor(0x7d, 0xcf, 0xff).rgb());
  EXPECT_EQ(tok.accentBlue.rgb(), QColor(0x7a, 0xa2, 0xf7).rgb());
  EXPECT_EQ(tok.accentViolet.rgb(), QColor(0xbb, 0x9a, 0xf7).rgb());
  EXPECT_EQ(tok.success.rgb(), QColor(0x9e, 0xce, 0x6a).rgb());
  EXPECT_EQ(tok.warning.rgb(), QColor(0xff, 0x9e, 0x64).rgb());
  EXPECT_EQ(tok.error.rgb(), QColor(0xf7, 0x76, 0x8e).rgb());
}

TEST(DarkTokens, SurfaceValues) {
  const Holonight::ColorTokens tok = Holonight::darkTokens();
  EXPECT_EQ(tok.surface.rgb(), QColor(0x10, 0x13, 0x1f).rgb());
  EXPECT_EQ(tok.surfaceVariant.rgb(), QColor(0x16, 0x19, 0x25).rgb());
  EXPECT_EQ(tok.surfaceContainer.rgb(), QColor(0x1a, 0x1b, 0x26).rgb());
  EXPECT_EQ(tok.secondary.rgb(), QColor(0x24, 0x28, 0x3b).rgb());
}

TEST(DarkTokens, SurfaceLuminanceOrdering) {
  const Holonight::ColorTokens tok = Holonight::darkTokens();
  auto lum = [](const QColor& c) -> double {
    auto lin = [](double v) { return v <= 0.04045 ? v / 12.92 : std::pow((v + 0.055) / 1.055, 2.4); };
    return 0.2126 * lin(c.redF()) + 0.7152 * lin(c.greenF()) + 0.0722 * lin(c.blueF());
  };
  EXPECT_LT(lum(tok.surface), lum(tok.surfaceVariant)) << "surface must be darker than surfaceVariant";
  EXPECT_LT(lum(tok.surfaceVariant), lum(tok.surfaceContainer))
      << "surfaceVariant must be darker than surfaceContainer";
  EXPECT_LT(lum(tok.surfaceContainer), lum(tok.secondary)) << "surfaceContainer must be darker than secondary";
}
