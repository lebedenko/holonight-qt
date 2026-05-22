// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "holonight/palette.h"

namespace Holonight {

ColorTokens darkTokens() {
  ColorTokens tok;

  // HoloNight Colors:Complementary — deepest background (design system: Background #10131f)
  tok.surface = QColor{0x10, 0x13, 0x1f};  // #10131f
  // HoloNight Colors:View — intermediate surface tier (design system: Surface #161925)
  tok.surfaceVariant = QColor{0x16, 0x19, 0x25};  // #161925
  // HoloNight Colors:View:BackgroundAlternate — card/container (design system: SurfaceElevated #1a1b26)
  tok.surfaceContainer = QColor{0x1a, 0x1b, 0x26};  // #1a1b26
  // HoloNight ForegroundNormal — contrast ≥9:1 on all surfaces (WCAG AAA)
  tok.onSurface = QColor{0xc0, 0xca, 0xf5};  // #c0caf5
  // HoloNight ForegroundInactive — disabled/placeholder text only; WCAG 1.4.3 exempt
  tok.onSurfaceVariant = QColor{0x56, 0x5f, 0x89};  // #565f89

  // HoloNight Colors:Selection:BackgroundNormal — selection fill (QPalette::Highlight)
  tok.primary = QColor{0x00, 0xa0, 0xdc};  // #00a0dc  contrast 5.75:1 with onPrimary (WCAG AA)
  // Derived: HSL ±15% lightness from focusRing (#00e0ff, H=187°, S=100%, L=50%)
  tok.primaryHover = QColor{0x4d, 0xe9, 0xff};    // #4de9ff  L=65%
  tok.primaryPressed = QColor{0x00, 0x9d, 0xb2};  // #009db2  L=35%
  tok.onPrimary = QColor{0x1a, 0x1b, 0x26};       // #1a1b26  dark text on selection fill

  // HoloNight Colors:Button:BackgroundNormal — raised surface for buttons/panels
  tok.secondary = QColor{0x24, 0x28, 0x3b};    // #24283b
  tok.onSecondary = QColor{0xc0, 0xca, 0xf5};  // #c0caf5  contrast 9.02:1 (WCAG AAA)

  // Muted cyan border — DEPRECATED: use semantic border tokens
  tok.outline = QColor{0x00, 0xa0, 0xc8};  // #00a0c8
  // HoloNight ForegroundInactive — DEPRECATED: use semantic border tokens
  tok.outlineVariant = QColor{0x56, 0x5f, 0x89};  // #565f89

  // Semantic borders — matches outlineVariant; value may diverge in a future revision
  tok.borderPassive = QColor{0x56, 0x5f, 0x89};  // #565f89  passive frame border
  // Semi-transparent overlay; alpha 0x4d = 77 ≈ 30% — intentional (see deviation log)
  tok.borderHover = QColor{0x7d, 0xcf, 0xff, 0x4d};  // #7dcfff ~30%  hover-state border
  // Same as focusRing by design — see palette.h comment
  tok.borderFocus = QColor{0x00, 0xe0, 0xff};   // #00e0ff  keyboard focus ring
  tok.borderActive = QColor{0x7a, 0xa2, 0xf7};  // #7aa2f7  selected/active border
  tok.borderUrgent = QColor{0xf7, 0x76, 0x8e};  // #f7768e  critical/error border

  // Semi-transparent cyan overlays — alpha < 255 is intentional
  tok.hover = QColor{0x00, 0xe0, 0xff, 0x1a};    // rgba(0,224,255, 10%)
  tok.pressed = QColor{0x00, 0xe0, 0xff, 0x26};  // rgba(0,224,255, 15%)
  // HoloNight DecorationFocus — pure cyan; full brightness for keyboard focus
  tok.focusRing = QColor{0x00, 0xe0, 0xff};  // #00e0ff

  // HoloNight ForegroundNegative — contrast 6.79:1 on surface (WCAG AA)
  tok.error = QColor{0xf7, 0x76, 0x8e};  // #f7768e
  // HoloNight ForegroundNeutral — contrast 8.84:1 on surface (WCAG AA)
  tok.warning = QColor{0xff, 0x9e, 0x64};  // #ff9e64
  // HoloNight ForegroundPositive — contrast 9.83:1 on surface (WCAG AA)
  tok.success = QColor{0x9e, 0xce, 0x6a};  // #9ece6a
  // Dark text on error-colored fills — contrast 6.79:1 against error (WCAG AA)
  tok.onError = QColor{0x12, 0x16, 0x26};  // #121626

  // HoloNight Colors:View with +5L elevation — fills hover cells and ToolButton hover bg
  tok.surfaceHover = QColor{0x1e, 0x22, 0x33};  // #1e2233
  // 50% blend of onSurfaceVariant (#565f89) and surface (#121626) — WCAG 1.4.3 exempt
  tok.onSurfaceDisabled = QColor{0x3b, 0x3f, 0x58};  // #3b3f58
  // Near-black inverse surface — ToolTip bg (pops out from every other surface)
  tok.surfaceInverse = QColor{0x0d, 0x11, 0x17};  // #0d1117
  // Same as onSurface — AAA contrast (11.6:1) on surfaceInverse
  tok.onSurfaceInverse = QColor{0xc0, 0xca, 0xf5};  // #c0caf5
  // Pure black; actual alpha applied by painter, not stored in token
  tok.shadow = QColor{0x00, 0x00, 0x00};  // #000000

  // HoloNight design-system accents and secondary text
  tok.accentCyan = QColor{0x7d, 0xcf, 0xff};    // #7dcfff  primary neon accent
  tok.accentBlue = QColor{0x7a, 0xa2, 0xf7};    // #7aa2f7  secondary accent
  tok.accentViolet = QColor{0xbb, 0x9a, 0xf7};  // #bb9af7  tertiary accent/glow
  tok.textSubtle = QColor{0xa9, 0xb1, 0xd6};    // #a9b1d6  secondary text

  // Workspace indicator fills
  tok.workspaceOccupied = QColor{0x1f, 0x23, 0x35};  // #1f2335  occupied/urgent pill fill
  tok.workspaceActive = QColor{0x20, 0x26, 0x3a};    // #20263a  active pill fill

  return tok;
}

QPalette buildPalette(const ColorTokens& tok) {
  // Derive disabled colors by blending 50% toward surface
  auto dim = [&](const QColor& col) -> QColor {
    return QColor::fromRgbF(static_cast<float>((col.redF() * 0.5) + (tok.surface.redF() * 0.5)),
                            static_cast<float>((col.greenF() * 0.5) + (tok.surface.greenF() * 0.5)),
                            static_cast<float>((col.blueF() * 0.5) + (tok.surface.blueF() * 0.5)));
  };

  QPalette pal;

  // Active group
  pal.setColor(QPalette::Active, QPalette::Window, tok.surface);
  pal.setColor(QPalette::Active, QPalette::WindowText, tok.onSurface);
  pal.setColor(QPalette::Active, QPalette::Base, tok.surfaceVariant);
  pal.setColor(QPalette::Active, QPalette::AlternateBase, tok.surfaceVariant);
  pal.setColor(QPalette::Active, QPalette::Text, tok.onSurface);
  pal.setColor(QPalette::Active, QPalette::Button, tok.surfaceVariant);
  pal.setColor(QPalette::Active, QPalette::ButtonText, tok.onSurface);
  pal.setColor(QPalette::Active, QPalette::Highlight, tok.primary);
  pal.setColor(QPalette::Active, QPalette::HighlightedText, tok.onPrimary);
  pal.setColor(QPalette::Active, QPalette::Link, tok.primary);
  pal.setColor(QPalette::Active, QPalette::LinkVisited, tok.error);
  pal.setColor(QPalette::Active, QPalette::Mid, tok.borderPassive);
  pal.setColor(QPalette::Active, QPalette::Dark, tok.borderPassive);
  pal.setColor(QPalette::Active, QPalette::Midlight, tok.secondary);
  pal.setColor(QPalette::Active, QPalette::Shadow, tok.borderPassive);
  pal.setColor(QPalette::Active, QPalette::BrightText, tok.onSurface);
  pal.setColor(QPalette::Active, QPalette::ToolTipBase, tok.surfaceContainer);
  pal.setColor(QPalette::Active, QPalette::ToolTipText, tok.onSurface);
  pal.setColor(QPalette::Active, QPalette::PlaceholderText, tok.onSurfaceVariant);

  // Inactive group — identical to Active for tiling WM use
  for (int role = 0; role < static_cast<int>(QPalette::NColorRoles); ++role) {
    const auto colorRole = static_cast<QPalette::ColorRole>(role);
    pal.setColor(QPalette::Inactive, colorRole, pal.color(QPalette::Active, colorRole));
  }

  // Disabled group — text/accent roles dimmed, fill roles unchanged
  pal.setColor(QPalette::Disabled, QPalette::Window, tok.surface);
  pal.setColor(QPalette::Disabled, QPalette::Base, tok.surfaceVariant);
  pal.setColor(QPalette::Disabled, QPalette::AlternateBase, tok.surfaceVariant);
  pal.setColor(QPalette::Disabled, QPalette::Button, tok.surfaceVariant);
  pal.setColor(QPalette::Disabled, QPalette::ToolTipBase, tok.surfaceContainer);
  pal.setColor(QPalette::Disabled, QPalette::Mid, tok.borderPassive);
  pal.setColor(QPalette::Disabled, QPalette::Dark, tok.borderPassive);
  pal.setColor(QPalette::Disabled, QPalette::Midlight, tok.secondary);
  pal.setColor(QPalette::Disabled, QPalette::Shadow, tok.borderPassive);
  pal.setColor(QPalette::Disabled, QPalette::WindowText, dim(tok.onSurface));
  pal.setColor(QPalette::Disabled, QPalette::Text, dim(tok.onSurface));
  pal.setColor(QPalette::Disabled, QPalette::ButtonText, dim(tok.onSurface));
  pal.setColor(QPalette::Disabled, QPalette::BrightText, dim(tok.onSurface));
  pal.setColor(QPalette::Disabled, QPalette::ToolTipText, dim(tok.onSurface));
  pal.setColor(QPalette::Disabled, QPalette::PlaceholderText, dim(tok.onSurfaceVariant));
  pal.setColor(QPalette::Disabled, QPalette::Highlight, dim(tok.primary));
  pal.setColor(QPalette::Disabled, QPalette::HighlightedText, dim(tok.onPrimary));
  pal.setColor(QPalette::Disabled, QPalette::Link, dim(tok.primary));
  pal.setColor(QPalette::Disabled, QPalette::LinkVisited, dim(tok.error));

  return pal;
}

}  // namespace Holonight
