// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#pragma once

#include <QColor>
#include <QPalette>

namespace Holonight {

struct ColorTokens {
  // Surface roles
  QColor surface;           // #1c1c23  Main background
  QColor surfaceVariant;    // #26262f  Elevated surface (panels, buttons)
  QColor surfaceContainer;  // #2e2e38  Card / grouped container background
  QColor onSurface;         // #dcdce8  Primary text and icons
  QColor onSurfaceVariant;  // #9090a8  Secondary / subdued text

  // Primary accent
  QColor primary;         // #5e81f4  Accent, active highlight, link
  QColor primaryHover;    // #7090f6  Primary at hover state
  QColor primaryPressed;  // #4a68d4  Primary at pressed state
  QColor onPrimary;       // #ffffff  Text/icon on primary background

  // Secondary / neutral
  QColor secondary;    // #44445a  Secondary action background
  QColor onSecondary;  // #dcdce8  Text/icon on secondary background

  // Outline / border
  // DEPRECATED: use borderPassive, borderHover, borderFocus, borderActive, or borderUrgent instead
  QColor outline;         // #00a0c8  Default border, separator
  // DEPRECATED: use borderPassive, borderHover, borderFocus, borderActive, or borderUrgent instead
  QColor outlineVariant;  // #565f89  Subtle divider, inactive border

  // Semantic borders — surface luminance ordering: surface < surfaceVariant < surfaceContainer < secondary
  QColor borderPassive;  // #565f89  Passive frame border — inactive controls, list views, separators
  QColor borderHover;    // #7dcfff ~30% alpha  Semi-transparent hover-state border overlay
  QColor borderFocus;    // #00e0ff  Keyboard focus ring border (== focusRing by design)
  QColor borderActive;   // #7aa2f7  Selected / active state border
  QColor borderUrgent;   // #f7768e  Critical / destructive / error state border

  // State overlays
  QColor hover;      // #ffffff1a  12% white overlay for hover
  QColor pressed;    // #ffffff26  15% white overlay for press
  QColor focusRing;  // #5e81f4  Same as primary; 2px border on focus

  // Semantic status
  QColor error;    // #f7768e  Destructive / error
  QColor warning;  // #ff9e64  Warning / caution
  QColor success;  // #9ece6a  Confirmation / success
  QColor onError;  // #121626  Text/icon on error background

  // Extended surface / state tokens
  QColor surfaceHover;        // #1e2233  Hover-state filled surface (view +5L)
  QColor onSurfaceDisabled;   // #3b3f58  Disabled text/border (WCAG 1.4.3 exempt)
  QColor surfaceInverse;      // #0d1117  ToolTip background — pops out from all surfaces
  QColor onSurfaceInverse;    // #c0caf5  Text on inverse surface (11.6:1 on #0d1117)
  QColor shadow;              // #000000  Drop-shadow base; alpha applied at paint time
};

[[nodiscard]] ColorTokens darkTokens();
[[nodiscard]] QPalette buildPalette(const ColorTokens& tokens);

}  // namespace Holonight
