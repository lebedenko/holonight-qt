// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "holonight/appearance.h"
#include "holonight/theme_catalog.h"
#include "semanticappearance.h"
#include "themeresolver.h"

#include <array>
#include <gtest/gtest.h>
namespace {

Holonight::ResolvedAppearance resolvedDefaults() {
  return {
      .scheme = QStringLiteral("holonight-dark"),
      .accent = QStringLiteral("blue"),
      .theme_scheme = Holonight::ThemeSchemeKind::HoloNightDark,
      .color_mode = Holonight::ColorMode::Dark,
      .ui_font = QStringLiteral("Inter"),
      .ui_font_size = 12,
      .monospace_font = QStringLiteral("JetBrains Mono"),
      .monospace_font_size = 12,
      .icon_theme = QStringLiteral("HoloNight"),
      .fallback_icon_theme = QStringLiteral("Papirus"),
      .cursor_theme = QStringLiteral("HoloNight"),
  };
}

QColor sourceOverOpaque(const QColor& source, const QColor& destination) {
  const int alpha = source.alpha();
  const auto channel = [alpha](int source_channel, int destination_channel) {
    return ((source_channel * alpha) + (destination_channel * (255 - alpha)) + 127) / 255;
  };
  return QColor::fromRgb(channel(source.red(), destination.red()), channel(source.green(), destination.green()),
                         channel(source.blue(), destination.blue()));
}

void expectValidColors(const Holonight::SemanticAppearance& appearance) {
  const std::array colors{
      appearance.accent,
      appearance.accent_foreground,
      appearance.window_surface,
      appearance.view_surface,
      appearance.elevated_surface,
      appearance.raised_surface,
      appearance.hover_surface,
      appearance.disabled_surface,
      appearance.strong_selection,
      appearance.strong_selection_foreground,
      appearance.subtle_selection,
      appearance.subtle_selection_hover,
      appearance.subtle_selection_foreground,
      appearance.primary_text,
      appearance.secondary_text,
      appearance.disabled_text,
      appearance.inverse_text,
      appearance.passive_border,
      appearance.active_border,
      appearance.focus_border,
      appearance.destructive_border,
      appearance.success,
      appearance.warning,
      appearance.error,
      appearance.error_foreground,
  };
  for (const QColor& color : colors) {
    EXPECT_TRUE(color.isValid());
  }
}

}  // namespace

TEST(SemanticAppearance, MapsCanonicalRolesAndMetadata) {
  const Holonight::ResolvedAppearance input = resolvedDefaults();
  const Holonight::ColorTokens tokens = Holonight::ThemeResolver::resolve(input);
  const Holonight::SemanticAppearance result = Holonight::resolveSemanticAppearance(input);

  EXPECT_EQ(result.contract_version, 1U);
  EXPECT_EQ(result.scheme_id, input.scheme);
  EXPECT_EQ(result.accent_id, input.accent);
  EXPECT_EQ(result.color_mode, input.color_mode);
  EXPECT_EQ(result.accent, tokens.primary);
  EXPECT_EQ(result.accent_foreground, tokens.onPrimary);
  EXPECT_EQ(result.window_surface, tokens.background);
  EXPECT_EQ(result.view_surface, tokens.surface);
  EXPECT_EQ(result.elevated_surface, tokens.surfaceElevated);
  EXPECT_EQ(result.raised_surface, tokens.surfaceRaised);
  EXPECT_EQ(result.hover_surface, tokens.surfaceHover);
  EXPECT_EQ(result.strong_selection, tokens.primary);
  EXPECT_EQ(result.strong_selection_foreground, tokens.onPrimary);
  EXPECT_EQ(result.subtle_selection, tokens.surfaceSelected);
  EXPECT_EQ(result.subtle_selection_hover, tokens.surfaceSelectedHover);
  EXPECT_EQ(result.subtle_selection_foreground, tokens.textPrimary);
  EXPECT_EQ(result.primary_text, tokens.textPrimary);
  EXPECT_EQ(result.secondary_text, tokens.textSecondary);
  EXPECT_EQ(result.disabled_text, tokens.textDisabled);
  EXPECT_EQ(result.inverse_text, tokens.textInverse);
  EXPECT_EQ(result.passive_border, tokens.borderPassive);
  EXPECT_EQ(result.active_border, tokens.borderActive);
  EXPECT_EQ(result.focus_border, tokens.borderFocus);
  EXPECT_EQ(result.destructive_border, tokens.borderUrgent);
  EXPECT_EQ(result.success, tokens.success);
  EXPECT_EQ(result.warning, tokens.warning);
  EXPECT_EQ(result.error, tokens.error);
  EXPECT_EQ(result.error_foreground, tokens.onError);
}

TEST(SemanticAppearance, CompositesDisabledOverlayOntoViewSurface) {
  const Holonight::ResolvedAppearance input = resolvedDefaults();
  const Holonight::ColorTokens tokens = Holonight::ThemeResolver::resolve(input);
  const Holonight::SemanticAppearance result = Holonight::resolveSemanticAppearance(input);

  EXPECT_EQ(result.disabled_surface, sourceOverOpaque(tokens.disabledOverlay, tokens.surface));
  EXPECT_EQ(result.disabled_surface.alpha(), 255);
}

TEST(SemanticAppearance, PropagatesTypographyAndThemeIdentifiers) {
  Holonight::ResolvedAppearance input = resolvedDefaults();
  input.ui_font = QStringLiteral("Semantic UI");
  input.ui_font_size = 15;
  input.monospace_font = QStringLiteral("Semantic Mono");
  input.monospace_font_size = 13;
  input.icon_theme = QStringLiteral("semantic-icons");
  input.fallback_icon_theme = QStringLiteral("fallback-icons");
  input.cursor_theme = QStringLiteral("semantic-cursor");

  const Holonight::SemanticAppearance result = Holonight::resolveSemanticAppearance(input);
  EXPECT_EQ(result.ui_font_family, input.ui_font);
  EXPECT_EQ(result.ui_font_point_size, input.ui_font_size);
  EXPECT_EQ(result.monospace_font_family, input.monospace_font);
  EXPECT_EQ(result.monospace_font_point_size, input.monospace_font_size);
  EXPECT_EQ(result.icon_theme, input.icon_theme);
  EXPECT_EQ(result.fallback_icon_theme, input.fallback_icon_theme);
  EXPECT_EQ(result.cursor_theme, input.cursor_theme);
}

TEST(SemanticAppearance, ResolutionIsDeterministic) {
  const Holonight::ResolvedAppearance input = resolvedDefaults();
  EXPECT_EQ(Holonight::resolveSemanticAppearance(input), Holonight::resolveSemanticAppearance(input));
}

TEST(SemanticAppearance, ResolvesEveryBuiltInSchemeAndAccent) {
  const std::array accents{QStringLiteral("default"), QStringLiteral("cyan"), QStringLiteral("blue"),
                           QStringLiteral("violet"), QStringLiteral("yellow")};
  for (const Holonight::ThemeVariantCatalogEntry& variant : Holonight::themeVariants()) {
    for (const QString& accent : accents) {
      Holonight::ResolvedAppearance input = resolvedDefaults();
      input.scheme = variant.id;
      input.theme_scheme = variant.scheme;
      input.color_mode = variant.mode;
      input.accent = accent;
      const Holonight::ColorTokens tokens = Holonight::ThemeResolver::resolve(input);
      const Holonight::SemanticAppearance result = Holonight::resolveSemanticAppearance(input);

      SCOPED_TRACE((variant.id + QLatin1Char('/') + accent).toStdString());
      expectValidColors(result);
      EXPECT_EQ(result.strong_selection, tokens.primary);
      EXPECT_EQ(result.strong_selection_foreground, tokens.onPrimary);
      EXPECT_EQ(result.subtle_selection, tokens.surfaceSelected);
      EXPECT_EQ(result.subtle_selection_hover, tokens.surfaceSelectedHover);
      EXPECT_EQ(result.subtle_selection_foreground, tokens.textPrimary);
      EXPECT_NE(result.strong_selection, result.subtle_selection);
      EXPECT_NE(result.strong_selection_foreground, result.subtle_selection_foreground);
    }
  }
}
