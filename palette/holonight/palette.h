// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#pragma once

#include <QColor>
#include <QPalette>

namespace Holonight {

struct ColorTokens {
  // Surface roles
  QColor surface;           // #10131f  Main background
  QColor surfaceVariant;    // #161925  Base panels and tray containers
  QColor surfaceContainer;  // #1a1b26  Raised cards and popovers
  QColor onSurface;         // #c0caf5  Primary text and icons
  QColor onSurfaceVariant;  // #565f89  Disabled / placeholder text

  // Primary accent
  QColor primary;         // #00a0dc  Selection highlight fill
  QColor primaryHover;    // #4de9ff  Primary at hover state
  QColor primaryPressed;  // #009db2  Primary at pressed state
  QColor onPrimary;       // #1a1b26  Text/icon on primary background

  // Secondary / neutral
  QColor secondary;    // #24283b  Panel / menu / popup surface
  QColor onSecondary;  // #c0caf5  Text/icon on secondary background

  // Outline / border
  // DEPRECATED: use borderPassive, borderHover, borderFocus, borderActive, or borderUrgent instead
  QColor outline;  // #00a0c8  Default border, separator
  // DEPRECATED: use borderPassive, borderHover, borderFocus, borderActive, or borderUrgent instead
  QColor outlineVariant;  // #565f89  Subtle divider, inactive border

  // Semantic borders — surface luminance ordering: surface < surfaceVariant < surfaceContainer < secondary
  QColor borderPassive;  // #565f89  Passive frame border — inactive controls, list views, separators
  QColor borderHover;    // #7dcfff ~30% alpha  Semi-transparent hover-state border overlay
  QColor borderFocus;    // #00e0ff  Keyboard focus ring border (== focusRing by design)
  QColor borderActive;   // #7aa2f7  Selected / active state border
  QColor borderUrgent;   // #f7768e  Critical / destructive / error state border

  // State overlays
  QColor hover;      // #00e0ff1a  10% cyan overlay for hover
  QColor pressed;    // #00e0ff26  15% cyan overlay for press
  QColor focusRing;  // #00e0ff  Keyboard focus ring

  // Semantic status
  QColor error;    // #f7768e  Destructive / error
  QColor warning;  // #ff9e64  Warning / caution
  QColor success;  // #9ece6a  Confirmation / success
  QColor onError;  // #121626  Text/icon on error background

  // Extended surface / state tokens
  QColor surfaceHover;       // #1e2233  Hover-state filled surface (view +5L)
  QColor onSurfaceDisabled;  // #3b3f58  Disabled text/border (WCAG 1.4.3 exempt)
  QColor surfaceInverse;     // #0d1117  ToolTip background — pops out from all surfaces
  QColor onSurfaceInverse;   // #c0caf5  Text on inverse surface (11.6:1 on #0d1117)
  QColor shadow;             // #000000  Drop-shadow base; alpha applied at paint time

  // Design-system accent tokens
  QColor accentCyan;    // #7dcfff  Primary neon accent
  QColor accentBlue;    // #7aa2f7  Secondary accent
  QColor accentViolet;  // #bb9af7  Tertiary accent / special workspace outline
  QColor accentYellow;  // #e0af68  Warm accent / attention highlight
  QColor textSubtle;    // #a9b1d6  Secondary text

  // Workspace indicator fills (topbar workspace pills)
  QColor workspaceOccupied;  // #1f2335  Occupied / urgent workspace pill fill
  QColor workspaceActive;    // #20263a  Active workspace pill fill (slightly elevated)
};

[[nodiscard]] ColorTokens darkTokens();
[[nodiscard]] QPalette buildPalette(const ColorTokens& tokens);

}  // namespace Holonight
