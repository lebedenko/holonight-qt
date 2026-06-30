// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "holonight/config.h"
#include "holonight/palette.h"

#include <gtest/gtest.h>

class PaletteTest : public ::testing::TestWithParam<Holonight::ThemeSchemeKind> {
 protected:
  void SetUp() override {
    tok_ = Holonight::tokensForScheme(GetParam());
    palette_ = QPalette{Holonight::buildPalette(tok_)};
  }

  Holonight::ColorTokens tok_;
  QPalette palette_;
  QPalette defaultPalette_;
};

TEST_P(PaletteTest, ActiveGroupPopulated) {
  const QList<QPalette::ColorRole> roles = {
      QPalette::Window,     QPalette::WindowText,  QPalette::Base,        QPalette::Text,
      QPalette::Button,     QPalette::ButtonText,  QPalette::Highlight,   QPalette::HighlightedText,
      QPalette::Link,       QPalette::Mid,         QPalette::Dark,        QPalette::Shadow,
      QPalette::BrightText, QPalette::ToolTipBase, QPalette::ToolTipText,
  };

  for (const auto role : roles) {
    if (Holonight::colorModeForScheme(GetParam()) == Holonight::ColorMode::Light) {
      EXPECT_TRUE(palette_.color(QPalette::Active, role).isValid())
          << "Role " << static_cast<int>(role) << " not valid in Active group";
      continue;
    }
    EXPECT_NE(palette_.color(QPalette::Active, role), defaultPalette_.color(QPalette::Active, role))
        << "Role " << static_cast<int>(role) << " not set in Active group";
  }
}

TEST_P(PaletteTest, InactiveGroupMatchesActive) {
  const QList<QPalette::ColorRole> roles = {
      QPalette::Window, QPalette::WindowText, QPalette::Base, QPalette::Text,
      QPalette::Button, QPalette::Highlight,  QPalette::Mid,  QPalette::Shadow,
  };

  for (const auto role : roles) {
    EXPECT_EQ(palette_.color(QPalette::Inactive, role), palette_.color(QPalette::Active, role))
        << "Inactive role " << static_cast<int>(role) << " differs from Active unexpectedly";
  }
}

TEST_P(PaletteTest, DisabledGroupDimmed) {
  const QList<QPalette::ColorRole> dimmedRoles = {
      QPalette::WindowText,
      QPalette::Text,
      QPalette::ButtonText,
      QPalette::BrightText,
  };

  for (const auto role : dimmedRoles) {
    const QColor activeColor = palette_.color(QPalette::Active, role);
    const QColor disabledColor = palette_.color(QPalette::Disabled, role);
    EXPECT_NE(activeColor, disabledColor)
        << "Disabled role " << static_cast<int>(role) << " is identical to Active — dimming not applied";
  }
}

TEST_P(PaletteTest, DisabledFillRolesUnchanged) {
  const QList<QPalette::ColorRole> fillRoles = {
      QPalette::Window,
      QPalette::Base,
      QPalette::Button,
  };

  for (const auto role : fillRoles) {
    EXPECT_EQ(palette_.color(QPalette::Disabled, role), palette_.color(QPalette::Active, role))
        << "Disabled fill role " << static_cast<int>(role) << " should match Active";
  }
}

TEST_P(PaletteTest, HighlightIsHolonightPrimary) {
  const Holonight::ColorTokens& tok = tok_;
  EXPECT_EQ(palette_.color(QPalette::Active, QPalette::Highlight), tok.primary);
  EXPECT_EQ(palette_.color(QPalette::Active, QPalette::HighlightedText), tok.onPrimary);
}

TEST_P(PaletteTest, BorderRolesAreBorderPassive) {
  const Holonight::ColorTokens& tok = tok_;
  EXPECT_EQ(palette_.color(QPalette::Active, QPalette::Mid), tok.borderPassive)
      << "Mid should be borderPassive (#565f89), not cyan outline";
  EXPECT_EQ(palette_.color(QPalette::Active, QPalette::Dark), tok.borderPassive)
      << "Dark should be borderPassive (#565f89)";
  EXPECT_EQ(palette_.color(QPalette::Active, QPalette::Shadow), tok.borderPassive)
      << "Shadow should be borderPassive (#565f89)";
  EXPECT_EQ(palette_.color(QPalette::Inactive, QPalette::Mid), tok.borderPassive);
  EXPECT_EQ(palette_.color(QPalette::Inactive, QPalette::Dark), tok.borderPassive);
  EXPECT_EQ(palette_.color(QPalette::Inactive, QPalette::Shadow), tok.borderPassive);
  EXPECT_EQ(palette_.color(QPalette::Disabled, QPalette::Mid), tok.borderPassive);
  EXPECT_EQ(palette_.color(QPalette::Disabled, QPalette::Dark), tok.borderPassive);
  EXPECT_EQ(palette_.color(QPalette::Disabled, QPalette::Shadow), tok.borderPassive);
}

TEST_P(PaletteTest, CanonicalSurfaceRolesAreMapped) {
  const Holonight::ColorTokens& tok = tok_;
  EXPECT_EQ(palette_.color(QPalette::Active, QPalette::Window), tok.background);
  EXPECT_EQ(palette_.color(QPalette::Active, QPalette::Base), tok.surface);
  EXPECT_EQ(palette_.color(QPalette::Active, QPalette::AlternateBase), tok.surfaceElevated);
  EXPECT_EQ(palette_.color(QPalette::Active, QPalette::Button), tok.surfaceRaised);
  EXPECT_EQ(palette_.color(QPalette::Active, QPalette::ToolTipBase), tok.surfaceInverse);
  EXPECT_EQ(palette_.color(QPalette::Inactive, QPalette::Base), tok.surface);
  EXPECT_EQ(palette_.color(QPalette::Disabled, QPalette::Base), tok.surface);
}

TEST_P(PaletteTest, CanonicalTextRolesAreMapped) {
  const Holonight::ColorTokens& tok = tok_;
  EXPECT_EQ(palette_.color(QPalette::Active, QPalette::WindowText), tok.textPrimary);
  EXPECT_EQ(palette_.color(QPalette::Active, QPalette::Text), tok.textPrimary);
  EXPECT_EQ(palette_.color(QPalette::Active, QPalette::ButtonText), tok.textPrimary);
  EXPECT_EQ(palette_.color(QPalette::Active, QPalette::PlaceholderText), tok.textMuted);
  EXPECT_EQ(palette_.color(QPalette::Active, QPalette::ToolTipText), tok.textInverse);
  EXPECT_EQ(palette_.color(QPalette::Disabled, QPalette::Text), tok.textDisabled);
  EXPECT_EQ(palette_.color(QPalette::Disabled, QPalette::PlaceholderText), tok.textDisabled);
}

TEST_P(PaletteTest, LinkAndVisitedLinkUseCanonicalTokens) {
  const Holonight::ColorTokens& tok = tok_;
  EXPECT_EQ(palette_.color(QPalette::Active, QPalette::Link), tok.primary);
  EXPECT_EQ(palette_.color(QPalette::Active, QPalette::LinkVisited), tok.error);
}

INSTANTIATE_TEST_SUITE_P(AllSchemes, PaletteTest,
                         ::testing::Values(Holonight::ThemeSchemeKind::HoloNightDark,
                                           Holonight::ThemeSchemeKind::HoloNightLight,
                                           Holonight::ThemeSchemeKind::TokyoNightStorm,
                                           Holonight::ThemeSchemeKind::TokyoNightDay));
