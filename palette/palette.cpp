// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "holonight/palette.h"

namespace Holonight {

ColorTokens darkTokens() {
  ColorTokens tok;

  tok.background = QColor{0x10, 0x13, 0x1f};       // #10131f
  tok.surface = QColor{0x16, 0x19, 0x25};          // #161925
  tok.surfaceElevated = QColor{0x1a, 0x1b, 0x26};  // #1a1b26
  tok.surfaceRaised = QColor{0x24, 0x28, 0x3b};    // #24283b
  tok.surfaceHover = QColor{0x1e, 0x22, 0x33};     // #1e2233
  tok.surfaceInverse = QColor{0x0d, 0x11, 0x17};   // #0d1117

  tok.textPrimary = QColor{0xc0, 0xca, 0xf5};   // #c0caf5
  tok.textSecondary = QColor{0xa9, 0xb1, 0xd6}; // #a9b1d6
  tok.textMuted = QColor{0x56, 0x5f, 0x89};     // #565f89
  tok.textDisabled = QColor{0x3b, 0x3f, 0x58};  // #3b3f58
  tok.textInverse = QColor{0xc0, 0xca, 0xf5};   // #c0caf5

  tok.primary = QColor{0x7a, 0xa2, 0xf7};  // #7aa2f7
  tok.primaryHover = QColor{0x4d, 0xe9, 0xff};    // #4de9ff  L=65%
  tok.primaryPressed = QColor{0x5d, 0x7f, 0xc7};  // #5d7fc7
  tok.onPrimary = QColor{0x1a, 0x1b, 0x26};       // #1a1b26  dark text on selection fill

  tok.borderSubtle = QColor{0x2f, 0x35, 0x4d};   // #2f354d
  tok.borderPassive = QColor{0x56, 0x5f, 0x89};  // #565f89  passive frame border
  tok.borderStrong = QColor{0x7c, 0x86, 0xb8};   // #7c86b8
  tok.borderHover = QColor{0x7d, 0xcf, 0xff, 0x4d};  // #7dcfff ~30%  hover-state border
  tok.borderFocus = QColor{0x00, 0xe0, 0xff};   // #00e0ff  keyboard focus ring
  tok.borderActive = QColor{0x7a, 0xa2, 0xf7};  // #7aa2f7  selected/active border
  tok.borderUrgent = QColor{0xf7, 0x76, 0x8e};  // #f7768e  critical/error border

  tok.glowCyanSoft = QColor{0x00, 0xe0, 0xff, 0x33};
  tok.glowBlueSoft = QColor{0x7a, 0xa2, 0xf7, 0x33};
  tok.glowVioletSoft = QColor{0xbb, 0x9a, 0xf7, 0x33};
  tok.scrim = QColor{0x00, 0x00, 0x00, 0x99};
  tok.glassTint = QColor{0x16, 0x19, 0x25, 0xcc};
  tok.hoverOverlay = QColor{0x00, 0xe0, 0xff, 0x1a};
  tok.pressedOverlay = QColor{0x00, 0xe0, 0xff, 0x26};
  tok.disabledOverlay = QColor{0x10, 0x13, 0x1f, 0x80};
  tok.focusRing = QColor{0x00, 0xe0, 0xff};  // #00e0ff
  tok.shadow = QColor{0x00, 0x00, 0x00};      // #000000

  tok.error = QColor{0xf7, 0x76, 0x8e};  // #f7768e
  tok.warning = QColor{0xff, 0x9e, 0x64};  // #ff9e64
  tok.success = QColor{0x9e, 0xce, 0x6a};  // #9ece6a
  tok.onError = QColor{0x12, 0x16, 0x26};  // #121626

  tok.accentCyan = QColor{0x7d, 0xcf, 0xff};    // #7dcfff  primary neon accent
  tok.accentBlue = QColor{0x7a, 0xa2, 0xf7};    // #7aa2f7  secondary accent
  tok.accentViolet = QColor{0xbb, 0x9a, 0xf7};  // #bb9af7  tertiary accent/glow
  tok.accentYellow = QColor{0xe0, 0xaf, 0x68};  // #e0af68  warm accent/attention

  tok.workspaceOccupied = QColor{0x1f, 0x23, 0x35};  // #1f2335  occupied/urgent pill fill
  tok.workspaceActive = QColor{0x20, 0x26, 0x3a};    // #20263a  active pill fill

  tok.ansiBlack = tok.background;
  tok.ansiRed = tok.error;
  tok.ansiGreen = tok.success;
  tok.ansiYellow = tok.warning;
  tok.ansiBlue = tok.primary;
  tok.ansiMagenta = tok.accentViolet;
  tok.ansiCyan = tok.accentCyan;
  tok.ansiWhite = tok.textPrimary;
  tok.ansiBrightBlack = tok.textMuted;
  tok.ansiBrightRed = QColor{0xff, 0x8c, 0xa3};
  tok.ansiBrightGreen = QColor{0xb9, 0xf2, 0x7c};
  tok.ansiBrightYellow = QColor{0xff, 0xc7, 0x77};
  tok.ansiBrightBlue = QColor{0x9a, 0xb8, 0xff};
  tok.ansiBrightMagenta = QColor{0xd7, 0xb0, 0xff};
  tok.ansiBrightCyan = QColor{0x9d, 0xe8, 0xff};
  tok.ansiBrightWhite = QColor{0xd5, 0xdc, 0xff};

  tok.surfaceVariant = tok.surface;
  tok.surfaceContainer = tok.surfaceElevated;
  tok.onSurface = tok.textPrimary;
  tok.onSurfaceVariant = tok.textMuted;
  tok.onSurfaceDisabled = tok.textDisabled;
  tok.onSurfaceInverse = tok.textInverse;
  tok.secondary = tok.surfaceRaised;
  tok.onSecondary = tok.textPrimary;
  tok.outline = tok.borderActive;
  tok.outlineVariant = tok.borderPassive;
  tok.hover = tok.hoverOverlay;
  tok.pressed = tok.pressedOverlay;
  tok.textSubtle = tok.textSecondary;
  tok.ansi0 = tok.ansiBlack;
  tok.ansi1 = tok.ansiRed;
  tok.ansi2 = tok.ansiGreen;
  tok.ansi3 = tok.ansiYellow;
  tok.ansi4 = tok.ansiBlue;
  tok.ansi5 = tok.ansiMagenta;
  tok.ansi6 = tok.ansiCyan;
  tok.ansi7 = tok.ansiWhite;
  tok.ansi8 = tok.ansiBrightBlack;
  tok.ansi9 = tok.ansiBrightRed;
  tok.ansi10 = tok.ansiBrightGreen;
  tok.ansi11 = tok.ansiBrightYellow;
  tok.ansi12 = tok.ansiBrightBlue;
  tok.ansi13 = tok.ansiBrightMagenta;
  tok.ansi14 = tok.ansiBrightCyan;
  tok.ansi15 = tok.ansiBrightWhite;

  return tok;
}

QPalette buildPalette(const ColorTokens& tok) {
  // Derive disabled colors by blending 50% toward surface
  auto dim = [&](const QColor& col) -> QColor {
    return QColor::fromRgbF(static_cast<float>((col.redF() * 0.5) + (tok.background.redF() * 0.5)),
                            static_cast<float>((col.greenF() * 0.5) + (tok.background.greenF() * 0.5)),
                            static_cast<float>((col.blueF() * 0.5) + (tok.background.blueF() * 0.5)));
  };

  QPalette pal;

  // Active group
  pal.setColor(QPalette::Active, QPalette::Window, tok.background);
  pal.setColor(QPalette::Active, QPalette::WindowText, tok.textPrimary);
  pal.setColor(QPalette::Active, QPalette::Base, tok.surface);
  pal.setColor(QPalette::Active, QPalette::AlternateBase, tok.surfaceElevated);
  pal.setColor(QPalette::Active, QPalette::Text, tok.textPrimary);
  pal.setColor(QPalette::Active, QPalette::Button, tok.surfaceRaised);
  pal.setColor(QPalette::Active, QPalette::ButtonText, tok.textPrimary);
  pal.setColor(QPalette::Active, QPalette::Highlight, tok.primary);
  pal.setColor(QPalette::Active, QPalette::HighlightedText, tok.onPrimary);
  pal.setColor(QPalette::Active, QPalette::Link, tok.primary);
  pal.setColor(QPalette::Active, QPalette::LinkVisited, tok.error);
  pal.setColor(QPalette::Active, QPalette::Mid, tok.borderPassive);
  pal.setColor(QPalette::Active, QPalette::Dark, tok.borderPassive);
  pal.setColor(QPalette::Active, QPalette::Midlight, tok.surfaceRaised);
  pal.setColor(QPalette::Active, QPalette::Shadow, tok.borderPassive);
  pal.setColor(QPalette::Active, QPalette::BrightText, tok.textPrimary);
  pal.setColor(QPalette::Active, QPalette::ToolTipBase, tok.surfaceInverse);
  pal.setColor(QPalette::Active, QPalette::ToolTipText, tok.textInverse);
  pal.setColor(QPalette::Active, QPalette::PlaceholderText, tok.textMuted);

  // Inactive group — identical to Active for tiling WM use
  for (int role = 0; role < static_cast<int>(QPalette::NColorRoles); ++role) {
    const auto colorRole = static_cast<QPalette::ColorRole>(role);
    pal.setColor(QPalette::Inactive, colorRole, pal.color(QPalette::Active, colorRole));
  }

  // Disabled group — text/accent roles dimmed, fill roles unchanged
  pal.setColor(QPalette::Disabled, QPalette::Window, tok.background);
  pal.setColor(QPalette::Disabled, QPalette::Base, tok.surface);
  pal.setColor(QPalette::Disabled, QPalette::AlternateBase, tok.surfaceElevated);
  pal.setColor(QPalette::Disabled, QPalette::Button, tok.surfaceRaised);
  pal.setColor(QPalette::Disabled, QPalette::ToolTipBase, tok.surfaceInverse);
  pal.setColor(QPalette::Disabled, QPalette::Mid, tok.borderPassive);
  pal.setColor(QPalette::Disabled, QPalette::Dark, tok.borderPassive);
  pal.setColor(QPalette::Disabled, QPalette::Midlight, tok.surfaceRaised);
  pal.setColor(QPalette::Disabled, QPalette::Shadow, tok.borderPassive);
  pal.setColor(QPalette::Disabled, QPalette::WindowText, tok.textDisabled);
  pal.setColor(QPalette::Disabled, QPalette::Text, tok.textDisabled);
  pal.setColor(QPalette::Disabled, QPalette::ButtonText, tok.textDisabled);
  pal.setColor(QPalette::Disabled, QPalette::BrightText, tok.textDisabled);
  pal.setColor(QPalette::Disabled, QPalette::ToolTipText, tok.textDisabled);
  pal.setColor(QPalette::Disabled, QPalette::PlaceholderText, tok.textDisabled);
  pal.setColor(QPalette::Disabled, QPalette::Highlight, dim(tok.primary));
  pal.setColor(QPalette::Disabled, QPalette::HighlightedText, dim(tok.onPrimary));
  pal.setColor(QPalette::Disabled, QPalette::Link, dim(tok.primary));
  pal.setColor(QPalette::Disabled, QPalette::LinkVisited, dim(tok.error));

  return pal;
}

}  // namespace Holonight
