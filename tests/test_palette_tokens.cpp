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
}

// hover and pressed are intentionally semi-transparent overlays; shadow alpha is set at paint time
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
}
