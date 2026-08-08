// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "holonight/appearance.h"

#include "holonight/theme_catalog.h"

#include <algorithm>
#include <holonight/config/config.h>

namespace Holonight {
namespace {

constexpr int kMinimumFontSize = 6;
constexpr int kMaximumFontSize = 48;

[[nodiscard]] QString fromUtf8(const std::string& value) {
  return QString::fromUtf8(value.data(), static_cast<qsizetype>(value.size()));
}

[[nodiscard]] ResolvedShapeStyle shapeStyle(HoloNight::Config::ShapeStyle style) {
  switch (style) {
    case HoloNight::Config::ShapeStyle::Inherit:
      return ResolvedShapeStyle::Inherit;
    case HoloNight::Config::ShapeStyle::Hybrid:
      return ResolvedShapeStyle::Hybrid;
    case HoloNight::Config::ShapeStyle::Rounded:
      return ResolvedShapeStyle::Rounded;
    case HoloNight::Config::ShapeStyle::Chamfered:
      return ResolvedShapeStyle::Chamfered;
  }
  return ResolvedShapeStyle::Inherit;
}

}  // namespace

int ResolvedAppearance::microSize() const { return (std::max)(kMinimumFontSize, ui_font_size - 1); }

int ResolvedAppearance::captionSize() const { return (std::max)(kMinimumFontSize, ui_font_size - 1); }

int ResolvedAppearance::bodySize() const { return ui_font_size; }

int ResolvedAppearance::subheadingSize() const { return (std::min)(kMaximumFontSize, ui_font_size + 1); }

int ResolvedAppearance::appTitleSize() const { return (std::min)(kMaximumFontSize, ui_font_size + 4); }

int ResolvedAppearance::headingSize() const { return (std::min)(kMaximumFontSize, ui_font_size + 6); }

AppearanceResolution resolveAppearance(const HoloNight::Config::Appearance& appearance) {
  const QString requested_scheme = fromUtf8(appearance.theme.scheme).trimmed().toLower();
  const ThemeVariantCatalogEntry* variant = themeVariantForSchemeId(requested_scheme);
  if (variant == nullptr) {
    return {.value = std::nullopt,
            .diagnostics = {{.code = AppearanceDiagnosticCode::UnsupportedScheme,
                             .message = QStringLiteral("Unsupported HoloNight scheme: %1").arg(requested_scheme)}}};
  }

  const QString requested_accent = fromUtf8(appearance.theme.accent).trimmed().toLower();
  const QString normalized_accent = normalizeAccentId(requested_accent);
  if (normalized_accent != requested_accent) {
    return {.value = std::nullopt,
            .diagnostics = {{.code = AppearanceDiagnosticCode::UnsupportedAccent,
                             .message = QStringLiteral("Unsupported HoloNight accent: %1").arg(requested_accent)}}};
  }

  ResolvedAppearance result{
      .scheme = variant->id,
      .accent = normalized_accent,
      .theme_scheme = variant->scheme,
      .color_mode = variant->mode,
      .ui_font = fromUtf8(appearance.typography.ui_family),
      .ui_font_size = static_cast<int>(appearance.typography.ui_size),
      .monospace_font = fromUtf8(appearance.typography.monospace_family),
      .monospace_font_size = static_cast<int>(appearance.typography.monospace_size),
      .title_font = fromUtf8(appearance.typography.title_family),
      .title_font_size = static_cast<int>(appearance.typography.title_size),
      .display_font = fromUtf8(appearance.typography.display_family),
      .display_font_size = static_cast<int>(appearance.typography.display_size),
      .icon_theme = fromUtf8(appearance.icons.theme),
      .fallback_icon_theme = fromUtf8(appearance.icons.fallback),
      .cursor_theme = fromUtf8(appearance.icons.cursor),
      .layout_scale = appearance.layout.scale,
      .shape_style = shapeStyle(appearance.shape.style),
      .shape_scale = appearance.shape.scale,
  };
  if (appearance.shape.base_radius.has_value()) {
    result.base_radius = *appearance.shape.base_radius;
  }
  if (appearance.shape.base_chamfer.has_value()) {
    result.base_chamfer = *appearance.shape.base_chamfer;
  }
  return {.value = std::move(result), .diagnostics = {}};
}

AppearanceDiagnostic qtDiagnostic(const HoloNight::Config::Diagnostic& diagnostic) {
  return {.code = AppearanceDiagnosticCode::Configuration,
          .severity = diagnostic.severity == HoloNight::Config::Severity::Info ? AppearanceDiagnosticSeverity::Info
                                                                               : AppearanceDiagnosticSeverity::Error,
          .message = fromUtf8(diagnostic.message),
          .path = diagnostic.path.has_value() ? QString::fromStdString(diagnostic.path->string()) : QString{}};
}

}  // namespace Holonight
