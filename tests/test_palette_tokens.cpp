// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "holonight/palette.h"

#include <gtest/gtest.h>

TEST(DarkTokens, AllColorsValid) {
  const Holonight::ColorTokens tok = Holonight::darkTokens();
  EXPECT_TRUE(tok.background.isValid());
  EXPECT_TRUE(tok.surface.isValid());
  EXPECT_TRUE(tok.surfaceElevated.isValid());
  EXPECT_TRUE(tok.surfaceRaised.isValid());
  EXPECT_TRUE(tok.surfaceHover.isValid());
  EXPECT_TRUE(tok.surfaceInverse.isValid());
  EXPECT_TRUE(tok.textPrimary.isValid());
  EXPECT_TRUE(tok.textSecondary.isValid());
  EXPECT_TRUE(tok.textMuted.isValid());
  EXPECT_TRUE(tok.textDisabled.isValid());
  EXPECT_TRUE(tok.textInverse.isValid());
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
  EXPECT_TRUE(tok.borderSubtle.isValid());
  EXPECT_TRUE(tok.borderPassive.isValid());
  EXPECT_TRUE(tok.borderStrong.isValid());
  EXPECT_TRUE(tok.borderHover.isValid());
  EXPECT_TRUE(tok.borderFocus.isValid());
  EXPECT_TRUE(tok.borderActive.isValid());
  EXPECT_TRUE(tok.borderUrgent.isValid());
  EXPECT_TRUE(tok.accentCyan.isValid());
  EXPECT_TRUE(tok.accentBlue.isValid());
  EXPECT_TRUE(tok.accentViolet.isValid());
  EXPECT_TRUE(tok.accentYellow.isValid());
  EXPECT_TRUE(tok.workspaceOccupied.isValid());
  EXPECT_TRUE(tok.workspaceActive.isValid());
  EXPECT_TRUE(tok.ansiBlack.isValid());
  EXPECT_TRUE(tok.ansiBrightWhite.isValid());
}

// hover, pressed, and borderHover are intentionally semi-transparent; shadow alpha is set at paint time
TEST(DarkTokens, OpaqueColorsAreOpaque) {
  const Holonight::ColorTokens tok = Holonight::darkTokens();
  EXPECT_EQ(tok.background.alpha(), 255);
  EXPECT_EQ(tok.surface.alpha(), 255);
  EXPECT_EQ(tok.surfaceElevated.alpha(), 255);
  EXPECT_EQ(tok.surfaceRaised.alpha(), 255);
  EXPECT_EQ(tok.surfaceHover.alpha(), 255);
  EXPECT_EQ(tok.surfaceInverse.alpha(), 255);
  EXPECT_EQ(tok.textPrimary.alpha(), 255);
  EXPECT_EQ(tok.textSecondary.alpha(), 255);
  EXPECT_EQ(tok.textMuted.alpha(), 255);
  EXPECT_EQ(tok.textDisabled.alpha(), 255);
  EXPECT_EQ(tok.textInverse.alpha(), 255);
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
  EXPECT_EQ(tok.borderSubtle.alpha(), 255);
  EXPECT_EQ(tok.borderPassive.alpha(), 255);
  EXPECT_EQ(tok.borderStrong.alpha(), 255);
  EXPECT_EQ(tok.borderFocus.alpha(), 255);
  EXPECT_EQ(tok.borderActive.alpha(), 255);
  EXPECT_EQ(tok.borderUrgent.alpha(), 255);
  EXPECT_EQ(tok.accentCyan.alpha(), 255);
  EXPECT_EQ(tok.accentBlue.alpha(), 255);
  EXPECT_EQ(tok.accentViolet.alpha(), 255);
  EXPECT_EQ(tok.accentYellow.alpha(), 255);
  EXPECT_EQ(tok.workspaceOccupied.alpha(), 255);
  EXPECT_EQ(tok.workspaceActive.alpha(), 255);
}

TEST(DarkTokens, NoTransparentColors) {
  const Holonight::ColorTokens tok = Holonight::darkTokens();
  EXPECT_NE(tok.surface, QColor(Qt::transparent));
  EXPECT_NE(tok.primary, QColor(Qt::transparent));
  EXPECT_NE(tok.textPrimary, QColor(Qt::transparent));
  EXPECT_NE(tok.error, QColor(Qt::transparent));
}

TEST(DarkTokens, HoverAndPressedAreSemiTransparent) {
  const Holonight::ColorTokens tok = Holonight::darkTokens();
  EXPECT_EQ(tok.hoverOverlay.alpha(), 0x1a);
  EXPECT_EQ(tok.pressedOverlay.alpha(), 0x26);
  EXPECT_EQ(tok.disabledOverlay.alpha(), 0x80);
  EXPECT_LT(tok.hover.alpha(), 255);
  EXPECT_LT(tok.pressed.alpha(), 255);
  EXPECT_TRUE(tok.hover.isValid());
  EXPECT_TRUE(tok.pressed.isValid());
  EXPECT_LT(tok.borderHover.alpha(), 255);
  EXPECT_TRUE(tok.borderHover.isValid());
}

TEST(DarkTokens, BorderTokenValues) {
  const Holonight::ColorTokens tok = Holonight::darkTokens();
  EXPECT_EQ(tok.borderSubtle.rgb(), QColor(0x2f, 0x35, 0x4d).rgb());
  EXPECT_EQ(tok.borderPassive.rgb(), QColor(0x56, 0x5f, 0x89).rgb());
  EXPECT_EQ(tok.borderStrong.rgb(), QColor(0x7c, 0x86, 0xb8).rgb());
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
  EXPECT_EQ(tok.background.rgb(), QColor(0x10, 0x13, 0x1f).rgb());
  EXPECT_EQ(tok.surface.rgb(), QColor(0x16, 0x19, 0x25).rgb());
  EXPECT_EQ(tok.surfaceElevated.rgb(), QColor(0x1a, 0x1b, 0x26).rgb());
  EXPECT_EQ(tok.surfaceRaised.rgb(), QColor(0x24, 0x28, 0x3b).rgb());
  EXPECT_EQ(tok.primary.rgb(), QColor(0x7a, 0xa2, 0xf7).rgb());
  EXPECT_EQ(tok.borderPassive.rgb(), QColor(0x56, 0x5f, 0x89).rgb());
  EXPECT_EQ(tok.textPrimary.rgb(), QColor(0xc0, 0xca, 0xf5).rgb());
  EXPECT_EQ(tok.textSecondary.rgb(), QColor(0xa9, 0xb1, 0xd6).rgb());
  EXPECT_EQ(tok.accentCyan.rgb(), QColor(0x7d, 0xcf, 0xff).rgb());
  EXPECT_EQ(tok.accentBlue.rgb(), QColor(0x7a, 0xa2, 0xf7).rgb());
  EXPECT_EQ(tok.accentViolet.rgb(), QColor(0xbb, 0x9a, 0xf7).rgb());
  EXPECT_EQ(tok.accentYellow.rgb(), QColor(0xe0, 0xaf, 0x68).rgb());
  EXPECT_EQ(tok.success.rgb(), QColor(0x9e, 0xce, 0x6a).rgb());
  EXPECT_EQ(tok.warning.rgb(), QColor(0xff, 0x9e, 0x64).rgb());
  EXPECT_EQ(tok.error.rgb(), QColor(0xf7, 0x76, 0x8e).rgb());
}

TEST(DarkTokens, SurfaceValues) {
  const Holonight::ColorTokens tok = Holonight::darkTokens();
  EXPECT_EQ(tok.background.rgb(), QColor(0x10, 0x13, 0x1f).rgb());
  EXPECT_EQ(tok.surface.rgb(), QColor(0x16, 0x19, 0x25).rgb());
  EXPECT_EQ(tok.surfaceElevated.rgb(), QColor(0x1a, 0x1b, 0x26).rgb());
  EXPECT_EQ(tok.surfaceRaised.rgb(), QColor(0x24, 0x28, 0x3b).rgb());
}

TEST(DarkTokens, DeprecatedAliasesMatchCanonicalRoles) {
  const Holonight::ColorTokens tok = Holonight::darkTokens();
  EXPECT_EQ(tok.surfaceVariant, tok.surface);
  EXPECT_EQ(tok.surfaceContainer, tok.surfaceElevated);
  EXPECT_EQ(tok.onSurface, tok.textPrimary);
  EXPECT_EQ(tok.onSurfaceVariant, tok.textMuted);
  EXPECT_EQ(tok.onSurfaceDisabled, tok.textDisabled);
  EXPECT_EQ(tok.onSurfaceInverse, tok.textInverse);
  EXPECT_EQ(tok.secondary, tok.surfaceRaised);
  EXPECT_EQ(tok.onSecondary, tok.textPrimary);
  EXPECT_EQ(tok.outline, tok.borderActive);
  EXPECT_EQ(tok.outlineVariant, tok.borderPassive);
  EXPECT_EQ(tok.hover, tok.hoverOverlay);
  EXPECT_EQ(tok.pressed, tok.pressedOverlay);
  EXPECT_EQ(tok.textSubtle, tok.textSecondary);
}

TEST(DarkTokens, RadiusMetricAndAnsiTokens) {
  const Holonight::ColorTokens tok = Holonight::darkTokens();
  EXPECT_EQ(tok.radiusTight, 2);
  EXPECT_EQ(tok.radiusControl, 6);
  EXPECT_EQ(tok.radiusPopup, 6);
  EXPECT_EQ(tok.borderWidth, 1);
  EXPECT_EQ(tok.focusBorderWidth, 2);
  EXPECT_EQ(tok.controlHeight, 32);
  EXPECT_EQ(tok.ansiBlue, tok.primary);
  EXPECT_EQ(tok.ansiCyan, tok.accentCyan);
  EXPECT_EQ(tok.ansi0, tok.ansiBlack);
  EXPECT_EQ(tok.ansi15, tok.ansiBrightWhite);
}

TEST(DarkTokens, SurfaceLuminanceOrdering) {
  const Holonight::ColorTokens tok = Holonight::darkTokens();
  auto lum = [](const QColor& c) -> double {
    auto lin = [](double v) { return v <= 0.04045 ? v / 12.92 : std::pow((v + 0.055) / 1.055, 2.4); };
    return 0.2126 * lin(c.redF()) + 0.7152 * lin(c.greenF()) + 0.0722 * lin(c.blueF());
  };
  EXPECT_LT(lum(tok.background), lum(tok.surface)) << "background must be darker than surface";
  EXPECT_LT(lum(tok.surface), lum(tok.surfaceElevated)) << "surface must be darker than surfaceElevated";
  EXPECT_LT(lum(tok.surfaceElevated), lum(tok.surfaceRaised)) << "surfaceElevated must be darker than surfaceRaised";
}
