// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "../style/holonightstyle.h"
#include "holonight/palette.h"

#include <QListView>
#include <QStatusBar>

#include <gtest/gtest.h>

TEST(StyleSmoke, InstantiatesWithoutCrash) {
  HoloniightStyle style;
  SUCCEED();
}

TEST(StyleSmoke, StandardPaletteNonDefault) {
  HoloniightStyle style;
  const QPalette pal = style.standardPalette();
  const QPalette defaultPal;
  EXPECT_NE(pal.color(QPalette::Active, QPalette::Window), defaultPal.color(QPalette::Active, QPalette::Window));
}

TEST(StyleSmoke, ScrollBarExtentIsEight) {
  HoloniightStyle style;
  EXPECT_EQ(style.pixelMetric(QStyle::PM_ScrollBarExtent), 8);
}

TEST(StyleSmoke, ButtonMarginIsSix) {
  HoloniightStyle style;
  EXPECT_EQ(style.pixelMetric(QStyle::PM_ButtonMargin), 6);
}

TEST(StyleSmoke, PolishPlacesViewUsesVariantSurface) {
  HoloniightStyle style;
  QListView view;
  view.setObjectName(QStringLiteral("placesPanel"));
  style.polish(&view);
  const auto tok = Holonight::darkTokens();
  EXPECT_EQ(view.palette().color(QPalette::Base), tok.surfaceVariant);
  EXPECT_EQ(view.viewport()->palette().color(QPalette::Base), tok.surfaceVariant);
}

TEST(StyleSmoke, PolishStatusBarUsesContainerSurface) {
  HoloniightStyle style;
  QStatusBar statusBar;
  style.polish(&statusBar);
  const auto tok = Holonight::darkTokens();
  EXPECT_EQ(statusBar.palette().color(QPalette::Window), tok.surfaceContainer);
}
