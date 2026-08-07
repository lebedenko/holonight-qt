// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#pragma once

#include "holonight/palette.h"

#include <QString>
#include <QVector>

#include <optional>

namespace HoloNight::Config {
struct Appearance;
struct Diagnostic;
}  // namespace HoloNight::Config

namespace Holonight {

enum class ResolvedShapeStyle { Inherit, Hybrid, Rounded, Chamfered };

enum class AppearanceDiagnosticCode { Configuration, UnsupportedScheme, UnsupportedAccent };
enum class AppearanceDiagnosticSeverity { Info, Error };

struct AppearanceDiagnostic {
  AppearanceDiagnosticCode code = AppearanceDiagnosticCode::Configuration;
  AppearanceDiagnosticSeverity severity = AppearanceDiagnosticSeverity::Error;
  QString message;
  QString path;

  [[nodiscard]] bool operator==(const AppearanceDiagnostic&) const = default;
};

struct ResolvedAppearance {
  QString scheme;
  QString accent;
  ThemeSchemeKind theme_scheme = ThemeSchemeKind::HoloNightDark;
  ColorMode color_mode = ColorMode::Dark;

  QString ui_font;
  int ui_font_size = 12;
  QString monospace_font;
  int monospace_font_size = 12;
  QString title_font;
  int title_font_size = 10;
  QString display_font;
  int display_font_size = 24;

  QString icon_theme;
  QString fallback_icon_theme;
  QString cursor_theme;
  qreal layout_scale = 1.0;

  ResolvedShapeStyle shape_style = ResolvedShapeStyle::Inherit;
  qreal shape_scale = 1.0;
  std::optional<qreal> base_radius;
  std::optional<qreal> base_chamfer;

  [[nodiscard]] int microSize() const;
  [[nodiscard]] int captionSize() const;
  [[nodiscard]] int bodySize() const;
  [[nodiscard]] int subheadingSize() const;
  [[nodiscard]] int appTitleSize() const;
  [[nodiscard]] int headingSize() const;

  [[nodiscard]] bool operator==(const ResolvedAppearance&) const = default;
};

struct AppearanceResolution {
  std::optional<ResolvedAppearance> value;
  QVector<AppearanceDiagnostic> diagnostics;

  [[nodiscard]] explicit operator bool() const noexcept { return value.has_value(); }
};

[[nodiscard]] AppearanceResolution resolveAppearance(const HoloNight::Config::Appearance& appearance);
[[nodiscard]] AppearanceDiagnostic qtDiagnostic(const HoloNight::Config::Diagnostic& diagnostic);

}  // namespace Holonight
