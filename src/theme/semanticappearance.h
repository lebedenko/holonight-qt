// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#pragma once

#include "holonight/appearance.h"

#include <QColor>
#include <QString>

#include <cstdint>

namespace Holonight {

inline constexpr std::uint32_t kSemanticAppearanceContractVersion = 1;

struct SemanticAppearance {
  std::uint32_t contract_version = kSemanticAppearanceContractVersion;
  QString scheme_id;
  QString accent_id;
  ColorMode color_mode = ColorMode::Dark;

  QColor accent;
  QColor accent_foreground;

  QColor window_surface;
  QColor view_surface;
  QColor elevated_surface;
  QColor raised_surface;
  QColor hover_surface;
  QColor disabled_surface;

  QColor strong_selection;
  QColor strong_selection_foreground;
  QColor subtle_selection;
  QColor subtle_selection_hover;
  QColor subtle_selection_foreground;

  QColor primary_text;
  QColor secondary_text;
  QColor disabled_text;
  QColor inverse_text;

  QColor passive_border;
  QColor active_border;
  QColor focus_border;
  QColor destructive_border;

  QColor success;
  QColor warning;
  QColor error;
  QColor error_foreground;

  QString ui_font_family;
  int ui_font_point_size = 12;
  QString monospace_font_family;
  int monospace_font_point_size = 12;

  QString icon_theme;
  QString fallback_icon_theme;
  QString cursor_theme;

  [[nodiscard]] bool operator==(const SemanticAppearance&) const = default;
};

[[nodiscard]] SemanticAppearance resolveSemanticAppearance(const ResolvedAppearance& appearance);

}  // namespace Holonight
