// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "semanticappearance.h"

#include "themeresolver.h"

namespace Holonight {
namespace {

[[nodiscard]] QColor sourceOverOpaque(const QColor& source, const QColor& destination) {
  const int source_alpha = source.alpha();
  const auto composite_channel = [source_alpha](int source_channel, int destination_channel) {
    return ((source_channel * source_alpha) + (destination_channel * (255 - source_alpha)) + 127) / 255;
  };
  return QColor::fromRgb(composite_channel(source.red(), destination.red()),
                         composite_channel(source.green(), destination.green()),
                         composite_channel(source.blue(), destination.blue()));
}

}  // namespace

SemanticAppearance resolveSemanticAppearance(const ResolvedAppearance& appearance) {
  const ColorTokens tokens = ThemeResolver::resolve(appearance);
  return {
      .contract_version = kSemanticAppearanceContractVersion,
      .scheme_id = appearance.scheme,
      .accent_id = appearance.accent,
      .color_mode = appearance.color_mode,
      .accent = tokens.primary,
      .accent_foreground = tokens.onPrimary,
      .window_surface = tokens.background,
      .view_surface = tokens.surface,
      .elevated_surface = tokens.surfaceElevated,
      .raised_surface = tokens.surfaceRaised,
      .hover_surface = tokens.surfaceHover,
      .disabled_surface = sourceOverOpaque(tokens.disabledOverlay, tokens.surface),
      .strong_selection = tokens.primary,
      .strong_selection_foreground = tokens.onPrimary,
      .subtle_selection = tokens.surfaceSelected,
      .subtle_selection_hover = tokens.surfaceSelectedHover,
      .subtle_selection_foreground = tokens.textPrimary,
      .primary_text = tokens.textPrimary,
      .secondary_text = tokens.textSecondary,
      .disabled_text = tokens.textDisabled,
      .inverse_text = tokens.textInverse,
      .passive_border = tokens.borderPassive,
      .active_border = tokens.borderActive,
      .focus_border = tokens.borderFocus,
      .destructive_border = tokens.borderUrgent,
      .success = tokens.success,
      .warning = tokens.warning,
      .error = tokens.error,
      .error_foreground = tokens.onError,
      .ui_font_family = appearance.ui_font,
      .ui_font_point_size = appearance.ui_font_size,
      .monospace_font_family = appearance.monospace_font,
      .monospace_font_point_size = appearance.monospace_font_size,
      .icon_theme = appearance.icon_theme,
      .fallback_icon_theme = appearance.fallback_icon_theme,
      .cursor_theme = appearance.cursor_theme,
  };
}

}  // namespace Holonight
