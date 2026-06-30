// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#pragma once

#include <QColor>
#include <QPalette>

namespace Holonight {

enum class ColorMode : uint8_t {
  Dark,
  Light,
};

enum class ThemeSchemeKind {
  HoloNightDark,
  HoloNightLight,
  HoloNightMocha,
  HoloNightLatte,
  TokyoNightStorm,
  TokyoNightDay,
  HoloNightEmber,
  HoloNightSol,
  HoloNightCyberD,
  HoloNightCyberL,
};

struct ColorTokens {
  // Canonical surface roles
  QColor background;       // #10131f  Main window/background plane
  QColor surface;          // #161925  Base views and input fields
  QColor surfaceElevated;  // #1a1b26  Cards and elevated containers
  QColor surfaceRaised;    // #24283b  Buttons, panels, menus, popovers
  QColor surfaceHover;     // #1e2233  Hover-state filled surface
  QColor surfaceInverse;   // #0d1117  Tooltip/complementary inverse surface

  // Canonical text roles
  QColor textPrimary;    // #c0caf5  Primary text and icons
  QColor textSecondary;  // #a9b1d6  Secondary text
  QColor textMuted;      // #565f89  Placeholder/inactive text
  QColor textDisabled;   // #3b3f58  Disabled text (WCAG inactive exemption)
  QColor textInverse;    // #c0caf5  Text on inverse surface

  // Primary accent
  QColor primary;         // #7aa2f7  Selection highlight fill and link color
  QColor primaryHover;    // #4de9ff  Primary at hover state
  QColor primaryPressed;  // #5d7fc7  Primary at pressed state
  QColor onPrimary;       // #1a1b26  Text/icon on primary background

  // Canonical borders — luminance ordering: background < surface < surfaceElevated < surfaceRaised
  QColor borderSubtle;   // #2f354d  Hairlines and low-emphasis dividers
  QColor borderPassive;  // #565f89  Passive frame border — inactive controls, list views, separators
  QColor borderStrong;   // #7c86b8  Strong neutral border
  QColor borderHover;    // #7dcfff ~30% alpha  Semi-transparent hover-state border overlay
  QColor borderFocus;    // #00e0ff  Keyboard focus ring border (== focusRing by design)
  QColor borderActive;   // #7aa2f7  Selected / active state border
  QColor borderUrgent;   // #f7768e  Critical / destructive / error state border

  // Canonical overlays/effects
  QColor glowCyanSoft;     // #00e0ff33  Soft cyan glow
  QColor glowBlueSoft;     // #7aa2f733  Soft blue glow
  QColor glowVioletSoft;   // #bb9af733  Soft violet glow
  QColor scrim;            // #00000099  Modal/scrim overlay
  QColor glassTint;        // #161925cc  Translucent glass tint
  QColor hoverOverlay;     // #00e0ff1a  10% cyan overlay for hover
  QColor pressedOverlay;   // #00e0ff26  15% cyan overlay for press
  QColor disabledOverlay;  // #10131f80  Disabled state overlay
  QColor focusRing;        // #00e0ff  Keyboard focus ring
  QColor shadow;           // #000000  Drop-shadow base; alpha applied at paint time

  // Semantic status
  QColor error;    // #f7768e  Destructive / error
  QColor warning;  // #ff9e64  Warning / caution
  QColor success;  // #9ece6a  Confirmation / success
  QColor onError;  // #121626  Text/icon on error background

  // Design-system accent tokens
  QColor accentCyan;    // #7dcfff  Primary neon accent
  QColor accentBlue;    // #7aa2f7  Secondary accent
  QColor accentViolet;  // #bb9af7  Tertiary accent / special workspace outline
  QColor accentYellow;  // #e0af68  Warm accent / attention highlight

  // Workspace indicator fills (topbar workspace pills)
  QColor workspaceOccupied;  // #1f2335  Occupied / urgent workspace pill fill
  QColor workspaceActive;    // #20263a  Active workspace pill fill (slightly elevated)

  // Radius and metric tokens
  int radiusTight = 2;
  int radiusControl = 6;
  int radiusPopup = 6;
  int radiusPill = 999;
  int borderWidth = 1;
  int focusBorderWidth = 2;
  int separatorWidth = 1;
  int controlHeight = 32;
  int controlPadding = 6;

  // ANSI terminal palette
  QColor ansiBlack;
  QColor ansiRed;
  QColor ansiGreen;
  QColor ansiYellow;
  QColor ansiBlue;
  QColor ansiMagenta;
  QColor ansiCyan;
  QColor ansiWhite;
  QColor ansiBrightBlack;
  QColor ansiBrightRed;
  QColor ansiBrightGreen;
  QColor ansiBrightYellow;
  QColor ansiBrightBlue;
  QColor ansiBrightMagenta;
  QColor ansiBrightCyan;
  QColor ansiBrightWhite;

  // Deprecated compatibility aliases; new code should use canonical roles above.
  QColor surfaceVariant;     // DEPRECATED: use surface
  QColor surfaceContainer;   // DEPRECATED: use surfaceElevated
  QColor onSurface;          // DEPRECATED: use textPrimary
  QColor onSurfaceVariant;   // DEPRECATED: use textMuted
  QColor onSurfaceDisabled;  // DEPRECATED: use textDisabled
  QColor onSurfaceInverse;   // DEPRECATED: use textInverse
  QColor secondary;          // DEPRECATED: use surfaceRaised
  QColor onSecondary;        // DEPRECATED: use textPrimary
  QColor outline;            // DEPRECATED: use semantic border tokens
  QColor outlineVariant;     // DEPRECATED: use semantic border tokens
  QColor hover;              // DEPRECATED: use hoverOverlay
  QColor pressed;            // DEPRECATED: use pressedOverlay
  QColor textSubtle;         // DEPRECATED: use textSecondary
  QColor ansi0, ansi1, ansi2, ansi3, ansi4, ansi5, ansi6, ansi7;
  QColor ansi8, ansi9, ansi10, ansi11, ansi12, ansi13, ansi14, ansi15;

  [[nodiscard]] bool operator==(const ColorTokens&) const noexcept = default;
};

[[nodiscard]] ColorTokens tokensForScheme(ThemeSchemeKind scheme);
[[nodiscard]] QPalette buildPalette(const ColorTokens& tokens);

}  // namespace Holonight
