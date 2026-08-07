// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "holonight/palette.h"
#include "holonight/theme_catalog.h"

#include <gtest/gtest.h>

TEST(BrandForegroundTokens, EveryBuiltInSchemeMatchesDocumentedValue) {
  const std::pair<Holonight::ThemeSchemeKind, QColor> expected[] = {
      {Holonight::ThemeSchemeKind::HoloNightDark, QColor{QStringLiteral("#9A8CFF")}},
      {Holonight::ThemeSchemeKind::HoloNightLight, QColor{QStringLiteral("#685BBD")}},
      {Holonight::ThemeSchemeKind::HoloNightMocha, QColor{QStringLiteral("#CBA6F7")}},
      {Holonight::ThemeSchemeKind::HoloNightLatte, QColor{QStringLiteral("#642AB0")}},
      {Holonight::ThemeSchemeKind::HoloNightStorm, QColor{QStringLiteral("#BB9AF7")}},
      {Holonight::ThemeSchemeKind::HoloNightDay, QColor{QStringLiteral("#7059BF")}},
      {Holonight::ThemeSchemeKind::HoloNightEmber, QColor{QStringLiteral("#E1AEBC")}},
      {Holonight::ThemeSchemeKind::HoloNightSol, QColor{QStringLiteral("#7F3865")}},
      {Holonight::ThemeSchemeKind::HoloNightCyberD, QColor{QStringLiteral("#B26CFF")}},
      {Holonight::ThemeSchemeKind::HoloNightCyberL, QColor{QStringLiteral("#893EDC")}},
      {Holonight::ThemeSchemeKind::HoloNightDracula, QColor{QStringLiteral("#E2C2FF")}},
      {Holonight::ThemeSchemeKind::HoloNightAlucard, QColor{QStringLiteral("#5137B3")}},
  };

  for (const auto& [scheme, color] : expected) {
    EXPECT_EQ(Holonight::tokensForScheme(scheme).brandForeground, color);
  }
}

TEST(SelectionTokens, EveryBuiltInSchemeProvidesSemanticSelectionRoles) {
  const Holonight::ThemeSchemeKind schemes[] = {
      Holonight::ThemeSchemeKind::HoloNightDark,    Holonight::ThemeSchemeKind::HoloNightLight,
      Holonight::ThemeSchemeKind::HoloNightMocha,   Holonight::ThemeSchemeKind::HoloNightLatte,
      Holonight::ThemeSchemeKind::HoloNightStorm,   Holonight::ThemeSchemeKind::HoloNightDay,
      Holonight::ThemeSchemeKind::HoloNightEmber,   Holonight::ThemeSchemeKind::HoloNightSol,
      Holonight::ThemeSchemeKind::HoloNightCyberD,  Holonight::ThemeSchemeKind::HoloNightCyberL,
      Holonight::ThemeSchemeKind::HoloNightDracula, Holonight::ThemeSchemeKind::HoloNightAlucard,
  };
  for (const auto scheme : schemes) {
    const Holonight::ColorTokens tok = Holonight::tokensForScheme(scheme);
    EXPECT_TRUE(tok.surfaceSelected.isValid());
    EXPECT_TRUE(tok.surfaceSelectedHover.isValid());
    EXPECT_TRUE(tok.selectionIndicator.isValid());
    EXPECT_EQ(tok.surfaceSelected.rgb(), tok.primary.rgb());
    EXPECT_EQ(tok.surfaceSelected.alpha(), 36);
    EXPECT_EQ(tok.surfaceSelectedHover.rgb(), tok.primary.rgb());
    EXPECT_EQ(tok.surfaceSelectedHover.alpha(), 51);
    EXPECT_EQ(tok.selectionIndicator, tok.primary);
  }
}

TEST(TextAccentTokens, EveryBuiltInSchemeTracksPrimaryAccent) {
  const Holonight::ThemeSchemeKind schemes[] = {
      Holonight::ThemeSchemeKind::HoloNightDark,    Holonight::ThemeSchemeKind::HoloNightLight,
      Holonight::ThemeSchemeKind::HoloNightMocha,   Holonight::ThemeSchemeKind::HoloNightLatte,
      Holonight::ThemeSchemeKind::HoloNightStorm,   Holonight::ThemeSchemeKind::HoloNightDay,
      Holonight::ThemeSchemeKind::HoloNightEmber,   Holonight::ThemeSchemeKind::HoloNightSol,
      Holonight::ThemeSchemeKind::HoloNightCyberD,  Holonight::ThemeSchemeKind::HoloNightCyberL,
      Holonight::ThemeSchemeKind::HoloNightDracula, Holonight::ThemeSchemeKind::HoloNightAlucard,
  };
  for (const auto scheme : schemes) {
    const Holonight::ColorTokens tok = Holonight::tokensForScheme(scheme);
    EXPECT_TRUE(tok.textAccent.isValid());
    EXPECT_EQ(tok.textAccent, tok.primary);
  }
}

TEST(HoloNightStormTokens, AllColorsValid) {
  const Holonight::ColorTokens tok = Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightStorm);
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
TEST(HoloNightStormTokens, OpaqueColorsAreOpaque) {
  const Holonight::ColorTokens tok = Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightStorm);
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

TEST(HoloNightStormTokens, NoTransparentColors) {
  const Holonight::ColorTokens tok = Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightStorm);
  EXPECT_NE(tok.surface, QColor(Qt::transparent));
  EXPECT_NE(tok.primary, QColor(Qt::transparent));
  EXPECT_NE(tok.textPrimary, QColor(Qt::transparent));
  EXPECT_NE(tok.error, QColor(Qt::transparent));
}

TEST(HoloNightStormTokens, HoverAndPressedAreSemiTransparent) {
  const Holonight::ColorTokens tok = Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightStorm);
  EXPECT_EQ(tok.hoverOverlay.alpha(), 0x1a);
  EXPECT_EQ(tok.pressedOverlay.alpha(), 0x26);
  EXPECT_EQ(tok.disabledOverlay.alpha(), 0x80);
  EXPECT_LT(tok.borderHover.alpha(), 255);
  EXPECT_TRUE(tok.borderHover.isValid());
}

TEST(HoloNightStormTokens, BorderTokenValues) {
  const Holonight::ColorTokens tok = Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightStorm);
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

TEST(HoloNightStormTokens, DesignSystemColorValues) {
  const Holonight::ColorTokens tok = Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightStorm);
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

TEST(HoloNightStormTokens, SurfaceValues) {
  const Holonight::ColorTokens tok = Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightStorm);
  EXPECT_EQ(tok.background.rgb(), QColor(0x10, 0x13, 0x1f).rgb());
  EXPECT_EQ(tok.surface.rgb(), QColor(0x16, 0x19, 0x25).rgb());
  EXPECT_EQ(tok.surfaceElevated.rgb(), QColor(0x1a, 0x1b, 0x26).rgb());
  EXPECT_EQ(tok.surfaceRaised.rgb(), QColor(0x24, 0x28, 0x3b).rgb());
}

TEST(HoloNightStormTokens, MetricAndAnsiTokens) {
  const Holonight::ColorTokens tok = Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightStorm);
  EXPECT_EQ(tok.ansiBlue, tok.primary);
  EXPECT_EQ(tok.ansiCyan, tok.accentCyan);
}

TEST(HoloNightStormTokens, SurfaceLuminanceOrdering) {
  const Holonight::ColorTokens tok = Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightStorm);
  auto lum = [](const QColor& c) -> double {
    auto lin = [](double v) { return v <= 0.04045 ? v / 12.92 : std::pow((v + 0.055) / 1.055, 2.4); };
    return 0.2126 * lin(c.redF()) + 0.7152 * lin(c.greenF()) + 0.0722 * lin(c.blueF());
  };
  EXPECT_LT(lum(tok.background), lum(tok.surface)) << "background must be darker than surface";
  EXPECT_LT(lum(tok.surface), lum(tok.surfaceElevated)) << "surface must be darker than surfaceElevated";
  EXPECT_LT(lum(tok.surfaceElevated), lum(tok.surfaceRaised)) << "surfaceElevated must be darker than surfaceRaised";
}

TEST(HoloNightDayTokens, AllColorsValid) {
  const Holonight::ColorTokens tok = Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightDay);
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
  EXPECT_TRUE(tok.borderSubtle.isValid());
  EXPECT_TRUE(tok.borderPassive.isValid());
  EXPECT_TRUE(tok.borderStrong.isValid());
  EXPECT_TRUE(tok.borderHover.isValid());
  EXPECT_TRUE(tok.borderFocus.isValid());
  EXPECT_TRUE(tok.borderActive.isValid());
  EXPECT_TRUE(tok.borderUrgent.isValid());
  EXPECT_TRUE(tok.hoverOverlay.isValid());
  EXPECT_TRUE(tok.pressedOverlay.isValid());
  EXPECT_TRUE(tok.disabledOverlay.isValid());
  EXPECT_TRUE(tok.error.isValid());
  EXPECT_TRUE(tok.warning.isValid());
  EXPECT_TRUE(tok.success.isValid());
  EXPECT_TRUE(tok.onError.isValid());
  EXPECT_TRUE(tok.accentCyan.isValid());
  EXPECT_TRUE(tok.accentBlue.isValid());
  EXPECT_TRUE(tok.accentViolet.isValid());
  EXPECT_TRUE(tok.accentYellow.isValid());
  EXPECT_TRUE(tok.workspaceOccupied.isValid());
  EXPECT_TRUE(tok.workspaceActive.isValid());
  EXPECT_TRUE(tok.ansiBlack.isValid());
  EXPECT_TRUE(tok.ansiBrightWhite.isValid());
}

TEST(HoloNightDayTokens, DocumentedColorValues) {
  const Holonight::ColorTokens tok = Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightDay);
  EXPECT_EQ(tok.background.rgb(), QColor(0xe9, 0xee, 0xf5).rgb());
  EXPECT_EQ(tok.surface.rgb(), QColor(0xf2, 0xf5, 0xfa).rgb());
  EXPECT_EQ(tok.surfaceElevated.rgb(), QColor(0xf8, 0xfa, 0xfd).rgb());
  EXPECT_EQ(tok.surfaceRaised.rgb(), QColor(0xff, 0xff, 0xff).rgb());
  EXPECT_EQ(tok.surfaceHover.rgb(), QColor(0xe3, 0xeb, 0xf5).rgb());
  EXPECT_EQ(tok.textPrimary.rgb(), QColor(0x1f, 0x23, 0x35).rgb());
  EXPECT_EQ(tok.primary.rgb(), QColor(0x2f, 0x6f, 0xe4).rgb());
  EXPECT_EQ(tok.primaryHover.rgb(), QColor(0x00, 0x8f, 0xc7).rgb());
  EXPECT_EQ(tok.borderFocus.rgb(), QColor(0x00, 0xa6, 0xd6).rgb());
  EXPECT_EQ(tok.error.rgb(), QColor(0xd9, 0x3f, 0x5c).rgb());
  EXPECT_EQ(tok.warning.rgb(), QColor(0xc4, 0x7a, 0x1c).rgb());
  EXPECT_EQ(tok.success.rgb(), QColor(0x4d, 0x8f, 0x31).rgb());
  EXPECT_EQ(tok.ansiBrightWhite.rgb(), QColor(0x10, 0x13, 0x1f).rgb());
}

TEST(HoloNightDayTokens, OverlayAlphaValuesMatchDarkTokenExpectations) {
  const Holonight::ColorTokens tok = Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightDay);
  EXPECT_EQ(tok.borderHover.alpha(), 0x66);
  EXPECT_EQ(tok.glowCyanSoft.alpha(), 0x33);
  EXPECT_EQ(tok.glowBlueSoft.alpha(), 0x33);
  EXPECT_EQ(tok.glowVioletSoft.alpha(), 0x33);
  EXPECT_EQ(tok.scrim.alpha(), 0x99);
  EXPECT_EQ(tok.glassTint.alpha(), 0xcc);
  EXPECT_EQ(tok.hoverOverlay.alpha(), 0x1a);
  EXPECT_EQ(tok.pressedOverlay.alpha(), 0x26);
  EXPECT_EQ(tok.disabledOverlay.alpha(), 0x80);
}

TEST(SchemeCatalog, HoloNightDarkMatchesDocumentedValues) {
  const Holonight::ColorTokens tok = Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightDark);
  EXPECT_EQ(tok.background, QColor(QStringLiteral("#0C1118")));
  EXPECT_EQ(tok.surface, QColor(QStringLiteral("#131A24")));
  EXPECT_EQ(tok.surfaceElevated, QColor(QStringLiteral("#18212D")));
  EXPECT_EQ(tok.surfaceRaised, QColor(QStringLiteral("#202B39")));
  EXPECT_EQ(tok.surfaceHover, QColor(QStringLiteral("#253243")));
  EXPECT_EQ(tok.surfaceInverse, QColor(QStringLiteral("#F3F6FA")));
  EXPECT_EQ(tok.textPrimary, QColor(QStringLiteral("#E7EDF5")));
  EXPECT_EQ(tok.textSecondary, QColor(QStringLiteral("#C5D0DE")));
  EXPECT_EQ(tok.textMuted, QColor(QStringLiteral("#8D99AD")));
  EXPECT_EQ(tok.textDisabled, QColor(QStringLiteral("#5B6678")));
  EXPECT_EQ(tok.textInverse, QColor(QStringLiteral("#0F141C")));
  EXPECT_EQ(tok.primary, QColor(QStringLiteral("#5EA2FF")));
  EXPECT_EQ(tok.primaryHover, QColor(QStringLiteral("#77B2FF")));
  EXPECT_EQ(tok.primaryPressed, QColor(QStringLiteral("#438CEB")));
  EXPECT_EQ(tok.onPrimary, QColor(QStringLiteral("#081018")));
  EXPECT_EQ(tok.borderSubtle, QColor(QStringLiteral("#263241")));
  EXPECT_EQ(tok.borderPassive, QColor(QStringLiteral("#36465A")));
  EXPECT_EQ(tok.borderStrong, QColor(QStringLiteral("#53677F")));
  EXPECT_EQ(tok.borderHover, QColor(QStringLiteral("#56D7FF55")));
  EXPECT_EQ(tok.borderFocus, QColor(QStringLiteral("#56D7FF")));
  EXPECT_EQ(tok.borderActive, QColor(QStringLiteral("#5EA2FF")));
  EXPECT_EQ(tok.borderUrgent, QColor(QStringLiteral("#FF718C")));
  EXPECT_EQ(tok.success, QColor(QStringLiteral("#79D97F")));
  EXPECT_EQ(tok.warning, QColor(QStringLiteral("#F2C46B")));
  EXPECT_EQ(tok.error, QColor(QStringLiteral("#FF718C")));
  EXPECT_EQ(tok.ansiBlack, QColor(QStringLiteral("#1A222D")));
  EXPECT_EQ(tok.ansiBrightWhite, QColor(QStringLiteral("#FFFFFF")));
}

TEST(SchemeCatalog, HoloNightLightMatchesDocumentedValues) {
  const Holonight::ColorTokens tok = Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightLight);
  EXPECT_EQ(tok.background, QColor(QStringLiteral("#E7EEF5")));
  EXPECT_EQ(tok.surface, QColor(QStringLiteral("#F1F5FA")));
  EXPECT_EQ(tok.surfaceElevated, QColor(QStringLiteral("#F7FAFD")));
  EXPECT_EQ(tok.surfaceRaised, QColor(QStringLiteral("#FFFFFF")));
  EXPECT_EQ(tok.surfaceHover, QColor(QStringLiteral("#E2EBF5")));
  EXPECT_EQ(tok.surfaceInverse, QColor(QStringLiteral("#121922")));
  EXPECT_EQ(tok.textPrimary, QColor(QStringLiteral("#1B2533")));
  EXPECT_EQ(tok.textSecondary, QColor(QStringLiteral("#435164")));
  EXPECT_EQ(tok.textMuted, QColor(QStringLiteral("#6F7D91")));
  EXPECT_EQ(tok.textDisabled, QColor(QStringLiteral("#A7B1BF")));
  EXPECT_EQ(tok.textInverse, QColor(QStringLiteral("#EEF3F8")));
  EXPECT_EQ(tok.primary, QColor(QStringLiteral("#3E7BDB")));
  EXPECT_EQ(tok.primaryHover, QColor(QStringLiteral("#4C8BEF")));
  EXPECT_EQ(tok.primaryPressed, QColor(QStringLiteral("#3266BE")));
  EXPECT_EQ(tok.onPrimary, QColor(QStringLiteral("#FFFFFF")));
  EXPECT_EQ(tok.borderSubtle, QColor(QStringLiteral("#D6DEE8")));
  EXPECT_EQ(tok.borderPassive, QColor(QStringLiteral("#B7C3D2")));
  EXPECT_EQ(tok.borderStrong, QColor(QStringLiteral("#8898AB")));
  EXPECT_EQ(tok.borderHover, QColor(QStringLiteral("#00A8D833")));
  EXPECT_EQ(tok.borderFocus, QColor(QStringLiteral("#00A8D8")));
  EXPECT_EQ(tok.borderActive, QColor(QStringLiteral("#3E7BDB")));
  EXPECT_EQ(tok.borderUrgent, QColor(QStringLiteral("#D84A68")));
  EXPECT_EQ(tok.success, QColor(QStringLiteral("#3E9449")));
  EXPECT_EQ(tok.warning, QColor(QStringLiteral("#C38A1C")));
  EXPECT_EQ(tok.error, QColor(QStringLiteral("#D84A68")));
  EXPECT_EQ(tok.ansiBlack, QColor(QStringLiteral("#3B4554")));
  EXPECT_EQ(tok.ansiBrightWhite, QColor(QStringLiteral("#FFFFFF")));
}

TEST(SchemeCatalog, HoloNightStormAndDayPreserveExistingValues) {
  const Holonight::ColorTokens storm = Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightStorm);
  const Holonight::ColorTokens day = Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightDay);

  EXPECT_EQ(storm.background.rgb(), QColor(0x10, 0x13, 0x1f).rgb());
  EXPECT_EQ(storm.primary.rgb(), QColor(0x7a, 0xa2, 0xf7).rgb());
  EXPECT_EQ(storm.borderFocus.rgb(), QColor(0x00, 0xe0, 0xff).rgb());
  EXPECT_EQ(storm.ansiBrightWhite.rgb(), QColor(0xd5, 0xdc, 0xff).rgb());
  EXPECT_EQ(day.background.rgb(), QColor(0xe9, 0xee, 0xf5).rgb());
  EXPECT_EQ(day.primary.rgb(), QColor(0x2f, 0x6f, 0xe4).rgb());
  EXPECT_EQ(day.borderFocus.rgb(), QColor(0x00, 0xa6, 0xd6).rgb());
  EXPECT_EQ(day.ansiBrightWhite.rgb(), QColor(0x10, 0x13, 0x1f).rgb());
}

TEST(SchemeCatalog, HoloNightMochaMatchesCatppuccinValues) {
  const Holonight::ColorTokens tok = Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightMocha);
  EXPECT_EQ(tok.background, QColor(QStringLiteral("#11111B")));
  EXPECT_EQ(tok.surface, QColor(QStringLiteral("#181825")));
  EXPECT_EQ(tok.surfaceElevated, QColor(QStringLiteral("#1E1E2E")));
  EXPECT_EQ(tok.surfaceRaised, QColor(QStringLiteral("#313244")));
  EXPECT_EQ(tok.surfaceHover, QColor(QStringLiteral("#45475A")));
  EXPECT_EQ(tok.textPrimary, QColor(QStringLiteral("#CDD6F4")));
  EXPECT_EQ(tok.textSecondary, QColor(QStringLiteral("#BAC2DE")));
  EXPECT_EQ(tok.textMuted, QColor(QStringLiteral("#A6ADC8")));
  EXPECT_EQ(tok.textDisabled, QColor(QStringLiteral("#6C7086")));
  EXPECT_EQ(tok.primary, QColor(QStringLiteral("#89B4FA")));
  EXPECT_EQ(tok.primaryHover, QColor(QStringLiteral("#89DCEB")));
  EXPECT_EQ(tok.primaryPressed, QColor(QStringLiteral("#74C7EC")));
  EXPECT_EQ(tok.borderFocus, QColor(QStringLiteral("#B4BEFE")));
  EXPECT_EQ(tok.error, QColor(QStringLiteral("#F38BA8")));
  EXPECT_EQ(tok.warning, QColor(QStringLiteral("#F9E2AF")));
  EXPECT_EQ(tok.success, QColor(QStringLiteral("#A6E3A1")));
  EXPECT_EQ(tok.accentCyan, QColor(QStringLiteral("#89DCEB")));
  EXPECT_EQ(tok.accentBlue, QColor(QStringLiteral("#89B4FA")));
  EXPECT_EQ(tok.accentViolet, QColor(QStringLiteral("#CBA6F7")));
  EXPECT_EQ(tok.accentYellow, QColor(QStringLiteral("#F9E2AF")));
}

TEST(SchemeCatalog, HoloNightLatteMatchesCatppuccinValues) {
  const Holonight::ColorTokens tok = Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightLatte);
  EXPECT_EQ(tok.background, QColor(QStringLiteral("#EFF1F5")));
  EXPECT_EQ(tok.surface, QColor(QStringLiteral("#E6E9EF")));
  EXPECT_EQ(tok.surfaceElevated, QColor(QStringLiteral("#CCD0DA")));
  EXPECT_EQ(tok.surfaceRaised, QColor(QStringLiteral("#BCC0CC")));
  EXPECT_EQ(tok.surfaceHover, QColor(QStringLiteral("#ACB0BE")));
  EXPECT_EQ(tok.textPrimary, QColor(QStringLiteral("#4C4F69")));
  EXPECT_EQ(tok.textSecondary, QColor(QStringLiteral("#5C5F77")));
  EXPECT_EQ(tok.textMuted, QColor(QStringLiteral("#6C6F85")));
  EXPECT_EQ(tok.textDisabled, QColor(QStringLiteral("#9CA0B0")));
  EXPECT_EQ(tok.primary, QColor(QStringLiteral("#1E66F5")));
  EXPECT_EQ(tok.primaryHover, QColor(QStringLiteral("#04A5E5")));
  EXPECT_EQ(tok.primaryPressed, QColor(QStringLiteral("#209FB5")));
  EXPECT_EQ(tok.borderFocus, QColor(QStringLiteral("#7287FD")));
  EXPECT_EQ(tok.error, QColor(QStringLiteral("#D20F39")));
  EXPECT_EQ(tok.warning, QColor(QStringLiteral("#DF8E1D")));
  EXPECT_EQ(tok.success, QColor(QStringLiteral("#40A02B")));
  EXPECT_EQ(tok.accentCyan, QColor(QStringLiteral("#04A5E5")));
  EXPECT_EQ(tok.accentBlue, QColor(QStringLiteral("#1E66F5")));
  EXPECT_EQ(tok.accentViolet, QColor(QStringLiteral("#8839EF")));
  EXPECT_EQ(tok.accentYellow, QColor(QStringLiteral("#DF8E1D")));
}

TEST(SchemeCatalog, HoloNightEmberMatchesGruvboxValues) {
  const Holonight::ColorTokens tok = Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightEmber);
  EXPECT_EQ(tok.background, QColor(QStringLiteral("#1d2021")));
  EXPECT_EQ(tok.surface, QColor(QStringLiteral("#282828")));
  EXPECT_EQ(tok.surfaceElevated, QColor(QStringLiteral("#3c3836")));
  EXPECT_EQ(tok.surfaceRaised, QColor(QStringLiteral("#504945")));
  EXPECT_EQ(tok.surfaceHover, QColor(QStringLiteral("#665c54")));
  EXPECT_EQ(tok.textPrimary, QColor(QStringLiteral("#fbf1c7")));
  EXPECT_EQ(tok.textSecondary, QColor(QStringLiteral("#ebdbb2")));
  EXPECT_EQ(tok.textMuted, QColor(QStringLiteral("#bdae93")));
  EXPECT_EQ(tok.textDisabled, QColor(QStringLiteral("#928374")));
  EXPECT_EQ(tok.primary, QColor(QStringLiteral("#8ec07c")));
  EXPECT_EQ(tok.primaryHover, QColor(QStringLiteral("#a8d3c5")));
  EXPECT_EQ(tok.primaryPressed, QColor(QStringLiteral("#83a598")));
  EXPECT_EQ(tok.borderFocus, QColor(QStringLiteral("#fabd2f")));
  EXPECT_EQ(tok.error, QColor(QStringLiteral("#ff6550")));
  EXPECT_EQ(tok.warning, QColor(QStringLiteral("#fe8019")));
  EXPECT_EQ(tok.success, QColor(QStringLiteral("#b8bb26")));
  EXPECT_EQ(tok.accentCyan, QColor(QStringLiteral("#8ec07c")));
  EXPECT_EQ(tok.accentBlue, QColor(QStringLiteral("#83a598")));
  EXPECT_EQ(tok.accentViolet, QColor(QStringLiteral("#d3869b")));
  EXPECT_EQ(tok.accentYellow, QColor(QStringLiteral("#fabd2f")));
}

TEST(SchemeCatalog, HoloNightSolMatchesGruvboxValues) {
  const Holonight::ColorTokens tok = Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightSol);
  EXPECT_EQ(tok.background, QColor(QStringLiteral("#f9f5d7")));
  EXPECT_EQ(tok.surface, QColor(QStringLiteral("#fbf1c7")));
  EXPECT_EQ(tok.surfaceElevated, QColor(QStringLiteral("#ebdbb2")));
  EXPECT_EQ(tok.surfaceRaised, QColor(QStringLiteral("#d5c4a1")));
  EXPECT_EQ(tok.surfaceHover, QColor(QStringLiteral("#bdae93")));
  EXPECT_EQ(tok.textPrimary, QColor(QStringLiteral("#282828")));
  EXPECT_EQ(tok.textSecondary, QColor(QStringLiteral("#3c3836")));
  EXPECT_EQ(tok.textMuted, QColor(QStringLiteral("#665c54")));
  EXPECT_EQ(tok.textDisabled, QColor(QStringLiteral("#928374")));
  EXPECT_EQ(tok.primary, QColor(QStringLiteral("#076678")));
  EXPECT_EQ(tok.primaryHover, QColor(QStringLiteral("#458588")));
  EXPECT_EQ(tok.primaryPressed, QColor(QStringLiteral("#054955")));
  EXPECT_EQ(tok.borderFocus, QColor(QStringLiteral("#9e3202")));
  EXPECT_EQ(tok.error, QColor(QStringLiteral("#9d0006")));
  EXPECT_EQ(tok.warning, QColor(QStringLiteral("#9e3202")));
  EXPECT_EQ(tok.success, QColor(QStringLiteral("#736e0f")));
  EXPECT_EQ(tok.accentCyan, QColor(QStringLiteral("#427b58")));
  EXPECT_EQ(tok.accentBlue, QColor(QStringLiteral("#076678")));
  EXPECT_EQ(tok.accentViolet, QColor(QStringLiteral("#8f3f71")));
  EXPECT_EQ(tok.accentYellow, QColor(QStringLiteral("#b57614")));
}

TEST(SchemeCatalog, CatppuccinOverlayAlphasMatchHoloNightConventions) {
  const Holonight::ColorTokens mocha = Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightMocha);
  EXPECT_EQ(mocha.borderHover.alpha(), 0x55);
  EXPECT_EQ(mocha.glowCyanSoft.alpha(), 0x33);
  EXPECT_EQ(mocha.glowBlueSoft.alpha(), 0x30);
  EXPECT_EQ(mocha.glowVioletSoft.alpha(), 0x30);
  EXPECT_EQ(mocha.scrim.alpha(), 0x88);
  EXPECT_EQ(mocha.glassTint.alpha(), 0xd8);
  EXPECT_EQ(mocha.hoverOverlay.alpha(), 0x12);
  EXPECT_EQ(mocha.pressedOverlay.alpha(), 0x22);
  EXPECT_EQ(mocha.disabledOverlay.alpha(), 0x80);

  const Holonight::ColorTokens latte = Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightLatte);
  EXPECT_EQ(latte.borderHover.alpha(), 0x33);
  EXPECT_EQ(latte.glowCyanSoft.alpha(), 0x33);
  EXPECT_EQ(latte.glowBlueSoft.alpha(), 0x30);
  EXPECT_EQ(latte.glowVioletSoft.alpha(), 0x30);
  EXPECT_EQ(latte.scrim.alpha(), 0x99);
  EXPECT_EQ(latte.glassTint.alpha(), 0xcc);
  EXPECT_EQ(latte.hoverOverlay.alpha(), 0x12);
  EXPECT_EQ(latte.pressedOverlay.alpha(), 0x22);
  EXPECT_EQ(latte.disabledOverlay.alpha(), 0x80);
}

TEST(SchemeCatalog, SchemesDifferWithinSameMode) {
  EXPECT_NE(Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightDark),
            Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightStorm));
  EXPECT_NE(Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightDark),
            Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightMocha));
  EXPECT_NE(Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightStorm),
            Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightMocha));
  EXPECT_NE(Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightLight),
            Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightDay));
  EXPECT_NE(Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightLight),
            Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightLatte));
  EXPECT_NE(Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightDay),
            Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightLatte));
  EXPECT_NE(Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightDark),
            Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightCyberD));
  EXPECT_NE(Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightLight),
            Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightCyberL));
}

TEST(SchemeCatalog, CyberDMatchesFileValues) {
  const Holonight::ColorTokens tok = Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightCyberD);
  EXPECT_EQ(tok.background, QColor(QStringLiteral("#080B12")));
  EXPECT_EQ(tok.surface, QColor(QStringLiteral("#0B0F17")));
  EXPECT_EQ(tok.surfaceElevated, QColor(QStringLiteral("#11161F")));
  EXPECT_EQ(tok.surfaceRaised, QColor(QStringLiteral("#1A2031")));
  EXPECT_EQ(tok.surfaceHover, QColor(QStringLiteral("#151B2B")));
  EXPECT_EQ(tok.textPrimary, QColor(QStringLiteral("#E6ECFF")));
  EXPECT_EQ(tok.textSecondary, QColor(QStringLiteral("#A0A8C8")));
  EXPECT_EQ(tok.textMuted, QColor(QStringLiteral("#6B7596")));
  EXPECT_EQ(tok.textDisabled, QColor(QStringLiteral("#424A63")));
  EXPECT_EQ(tok.primary, QColor(QStringLiteral("#39D5FF")));
  EXPECT_EQ(tok.primaryHover, QColor(QStringLiteral("#6BE4FF")));
  EXPECT_EQ(tok.primaryPressed, QColor(QStringLiteral("#1FAEDB")));
  EXPECT_EQ(tok.borderFocus, QColor(QStringLiteral("#00E1FF")));
  EXPECT_EQ(tok.error, QColor(QStringLiteral("#F92838")));
  EXPECT_EQ(tok.warning, QColor(QStringLiteral("#FEA257")));
  EXPECT_EQ(tok.success, QColor(QStringLiteral("#10E169")));
  EXPECT_EQ(tok.accentCyan, QColor(QStringLiteral("#39D5FF")));
  EXPECT_EQ(tok.accentBlue, QColor(QStringLiteral("#6B4FE8")));
  EXPECT_EQ(tok.accentViolet, QColor(QStringLiteral("#B26CFF")));
  EXPECT_EQ(tok.accentYellow, QColor(QStringLiteral("#F4C56B")));
}

TEST(SchemeCatalog, CyberLMatchesFileValues) {
  const Holonight::ColorTokens tok = Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightCyberL);
  EXPECT_EQ(tok.background, QColor(QStringLiteral("#EAF0F7")));
  EXPECT_EQ(tok.surface, QColor(QStringLiteral("#F2F5FB")));
  EXPECT_EQ(tok.surfaceElevated, QColor(QStringLiteral("#F8FAFD")));
  EXPECT_EQ(tok.surfaceRaised, QColor(QStringLiteral("#FFFFFF")));
  EXPECT_EQ(tok.surfaceHover, QColor(QStringLiteral("#E3E8F5")));
  EXPECT_EQ(tok.textPrimary, QColor(QStringLiteral("#1A2138")));
  EXPECT_EQ(tok.textSecondary, QColor(QStringLiteral("#4A5578")));
  EXPECT_EQ(tok.textMuted, QColor(QStringLiteral("#7A85A8")));
  EXPECT_EQ(tok.textDisabled, QColor(QStringLiteral("#AEB6CE")));
  EXPECT_EQ(tok.primary, QColor(QStringLiteral("#0E9BD6")));
  EXPECT_EQ(tok.primaryHover, QColor(QStringLiteral("#12B4F2")));
  EXPECT_EQ(tok.primaryPressed, QColor(QStringLiteral("#0A7AAA")));
  EXPECT_EQ(tok.borderFocus, QColor(QStringLiteral("#008FC4")));
  EXPECT_EQ(tok.error, QColor(QStringLiteral("#E11D48")));
  EXPECT_EQ(tok.warning, QColor(QStringLiteral("#C2710C")));
  EXPECT_EQ(tok.success, QColor(QStringLiteral("#0E9C56")));
  EXPECT_EQ(tok.accentCyan, QColor(QStringLiteral("#0E9BD6")));
  EXPECT_EQ(tok.accentBlue, QColor(QStringLiteral("#5538D6")));
  EXPECT_EQ(tok.accentViolet, QColor(QStringLiteral("#8B3FE0")));
  EXPECT_EQ(tok.accentYellow, QColor(QStringLiteral("#B8862A")));
}

TEST(SchemeCatalog, DraculaMatchesFileValues) {
  const Holonight::ColorTokens tok = Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightDracula);
  EXPECT_EQ(tok.background, QColor(QStringLiteral("#21222C")));
  EXPECT_EQ(tok.surface, QColor(QStringLiteral("#282A36")));
  EXPECT_EQ(tok.surfaceElevated, QColor(QStringLiteral("#343746")));
  EXPECT_EQ(tok.surfaceRaised, QColor(QStringLiteral("#404354")));
  EXPECT_EQ(tok.textPrimary, QColor(QStringLiteral("#F8F8F2")));
  EXPECT_EQ(tok.textMuted, QColor(QStringLiteral("#6272A4")));
  EXPECT_EQ(tok.primary, QColor(QStringLiteral("#BD93F9")));
  EXPECT_EQ(tok.borderFocus, QColor(QStringLiteral("#8BE9FD")));
  EXPECT_EQ(tok.error, QColor(QStringLiteral("#FF5555")));
  EXPECT_EQ(tok.warning, QColor(QStringLiteral("#FFB86C")));
  EXPECT_EQ(tok.success, QColor(QStringLiteral("#50FA7B")));
  EXPECT_EQ(tok.accentCyan, QColor(QStringLiteral("#8BE9FD")));
  EXPECT_EQ(tok.accentBlue, QColor(QStringLiteral("#6272A4")));
  EXPECT_EQ(tok.accentViolet, QColor(QStringLiteral("#BD93F9")));
}

TEST(SchemeCatalog, AlucardMatchesFileValues) {
  const Holonight::ColorTokens tok = Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightAlucard);
  EXPECT_EQ(tok.background, QColor(QStringLiteral("#FFFFFF")));
  EXPECT_EQ(tok.surface, QColor(QStringLiteral("#FFFBEB")));
  EXPECT_EQ(tok.surfaceElevated, QColor(QStringLiteral("#F5F0DC")));
  EXPECT_EQ(tok.surfaceRaised, QColor(QStringLiteral("#EBE5CC")));
  EXPECT_EQ(tok.surfaceHover, QColor(QStringLiteral("#CFCFDE")));
  EXPECT_EQ(tok.textPrimary, QColor(QStringLiteral("#1F1F1F")));
  EXPECT_EQ(tok.textMuted, QColor(QStringLiteral("#6C664B")));
  EXPECT_EQ(tok.primary, QColor(QStringLiteral("#644AC9")));
  EXPECT_EQ(tok.borderFocus, QColor(QStringLiteral("#036A96")));
  EXPECT_EQ(tok.error, QColor(QStringLiteral("#CB3A2A")));
  EXPECT_EQ(tok.warning, QColor(QStringLiteral("#A34D14")));
  EXPECT_EQ(tok.success, QColor(QStringLiteral("#14710A")));
  EXPECT_EQ(tok.accentCyan, QColor(QStringLiteral("#036A96")));
  EXPECT_EQ(tok.accentBlue, QColor(QStringLiteral("#3454B4")));
  EXPECT_EQ(tok.accentViolet, QColor(QStringLiteral("#644AC9")));
  EXPECT_EQ(tok.accentYellow, QColor(QStringLiteral("#846E15")));
}
