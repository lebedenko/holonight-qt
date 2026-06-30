// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "holonight/palette.h"

#include <QString>

namespace Holonight {

namespace {

void populateCompatibilityAliases(ColorTokens* tok, const QColor& outline, const QColor& outlineVariant) {
  tok->surfaceVariant = tok->surface;
  tok->surfaceContainer = tok->surfaceElevated;
  tok->onSurface = tok->textPrimary;
  tok->onSurfaceVariant = tok->textMuted;
  tok->onSurfaceDisabled = tok->textDisabled;
  tok->onSurfaceInverse = tok->textInverse;
  tok->secondary = tok->surfaceRaised;
  tok->onSecondary = tok->textPrimary;
  tok->outline = outline;
  tok->outlineVariant = outlineVariant;
  tok->hover = tok->hoverOverlay;
  tok->pressed = tok->pressedOverlay;
  tok->textSubtle = tok->textSecondary;
  tok->ansi0 = tok->ansiBlack;
  tok->ansi1 = tok->ansiRed;
  tok->ansi2 = tok->ansiGreen;
  tok->ansi3 = tok->ansiYellow;
  tok->ansi4 = tok->ansiBlue;
  tok->ansi5 = tok->ansiMagenta;
  tok->ansi6 = tok->ansiCyan;
  tok->ansi7 = tok->ansiWhite;
  tok->ansi8 = tok->ansiBrightBlack;
  tok->ansi9 = tok->ansiBrightRed;
  tok->ansi10 = tok->ansiBrightGreen;
  tok->ansi11 = tok->ansiBrightYellow;
  tok->ansi12 = tok->ansiBrightBlue;
  tok->ansi13 = tok->ansiBrightMagenta;
  tok->ansi14 = tok->ansiBrightCyan;
  tok->ansi15 = tok->ansiBrightWhite;
}

QColor colorWithAlpha(const QString& value, int alpha) {
  QColor color{value};
  color.setAlpha(alpha);
  return color;
}

ColorTokens holoNightDarkTokens() {
  ColorTokens tok;

  tok.background = QColor{QStringLiteral("#0C1118")};
  tok.surface = QColor{QStringLiteral("#131A24")};
  tok.surfaceElevated = QColor{QStringLiteral("#18212D")};
  tok.surfaceRaised = QColor{QStringLiteral("#202B39")};
  tok.surfaceHover = QColor{QStringLiteral("#253243")};
  tok.surfaceInverse = QColor{QStringLiteral("#F3F6FA")};

  tok.textPrimary = QColor{QStringLiteral("#E7EDF5")};
  tok.textSecondary = QColor{QStringLiteral("#C5D0DE")};
  tok.textMuted = QColor{QStringLiteral("#8D99AD")};
  tok.textDisabled = QColor{QStringLiteral("#5B6678")};
  tok.textInverse = QColor{QStringLiteral("#0F141C")};

  tok.primary = QColor{QStringLiteral("#5EA2FF")};
  tok.primaryHover = QColor{QStringLiteral("#77B2FF")};
  tok.primaryPressed = QColor{QStringLiteral("#438CEB")};
  tok.onPrimary = QColor{QStringLiteral("#081018")};

  tok.borderSubtle = QColor{QStringLiteral("#263241")};
  tok.borderPassive = QColor{QStringLiteral("#36465A")};
  tok.borderStrong = QColor{QStringLiteral("#53677F")};
  tok.borderHover = QColor{QStringLiteral("#56D7FF55")};
  tok.borderFocus = QColor{QStringLiteral("#56D7FF")};
  tok.borderActive = QColor{QStringLiteral("#5EA2FF")};
  tok.borderUrgent = QColor{QStringLiteral("#FF718C")};

  tok.glowCyanSoft = QColor{QStringLiteral("#56D7FF33")};
  tok.glowBlueSoft = QColor{QStringLiteral("#5EA2FF30")};
  tok.glowVioletSoft = QColor{QStringLiteral("#9A8CFF30")};
  tok.scrim = QColor{QStringLiteral("#00000088")};
  tok.glassTint = QColor{QStringLiteral("#18212DD8")};
  tok.hoverOverlay = QColor{QStringLiteral("#56D7FF12")};
  tok.pressedOverlay = QColor{QStringLiteral("#56D7FF22")};
  tok.disabledOverlay = QColor{QStringLiteral("#0C111880")};
  tok.focusRing = QColor{QStringLiteral("#56D7FF")};
  tok.shadow = QColor{0x00, 0x00, 0x00};

  tok.error = QColor{QStringLiteral("#FF718C")};
  tok.warning = QColor{QStringLiteral("#F2C46B")};
  tok.success = QColor{QStringLiteral("#79D97F")};
  tok.onError = QColor{QStringLiteral("#081018")};

  tok.accentCyan = QColor{QStringLiteral("#56D7FF")};
  tok.accentBlue = QColor{QStringLiteral("#5EA2FF")};
  tok.accentViolet = QColor{QStringLiteral("#9A8CFF")};
  tok.accentYellow = QColor{QStringLiteral("#F2C46B")};

  tok.workspaceOccupied = QColor{QStringLiteral("#18212D")};
  tok.workspaceActive = QColor{QStringLiteral("#202B39")};

  tok.ansiBlack = QColor{QStringLiteral("#1A222D")};
  tok.ansiRed = QColor{QStringLiteral("#F06A84")};
  tok.ansiGreen = QColor{QStringLiteral("#74D88B")};
  tok.ansiYellow = QColor{QStringLiteral("#EFC36B")};
  tok.ansiBlue = QColor{QStringLiteral("#5EA2FF")};
  tok.ansiMagenta = QColor{QStringLiteral("#A88CFF")};
  tok.ansiCyan = QColor{QStringLiteral("#56D7FF")};
  tok.ansiWhite = QColor{QStringLiteral("#DCE6F2")};
  tok.ansiBrightBlack = QColor{QStringLiteral("#4B5B70")};
  tok.ansiBrightRed = QColor{QStringLiteral("#FF8CA0")};
  tok.ansiBrightGreen = QColor{QStringLiteral("#99F2A9")};
  tok.ansiBrightYellow = QColor{QStringLiteral("#FFD98A")};
  tok.ansiBrightBlue = QColor{QStringLiteral("#82BAFF")};
  tok.ansiBrightMagenta = QColor{QStringLiteral("#C3AEFF")};
  tok.ansiBrightCyan = QColor{QStringLiteral("#8EE8FF")};
  tok.ansiBrightWhite = QColor{QStringLiteral("#FFFFFF")};

  populateCompatibilityAliases(&tok, tok.borderActive, tok.borderPassive);
  return tok;
}

ColorTokens holoNightLightTokens() {
  ColorTokens tok;

  tok.background = QColor{QStringLiteral("#F3F7FB")};
  tok.surface = QColor{QStringLiteral("#EDF2F8")};
  tok.surfaceElevated = QColor{QStringLiteral("#FFFFFF")};
  tok.surfaceRaised = QColor{QStringLiteral("#E5ECF4")};
  tok.surfaceHover = QColor{QStringLiteral("#DCE6F1")};
  tok.surfaceInverse = QColor{QStringLiteral("#121922")};

  tok.textPrimary = QColor{QStringLiteral("#1B2533")};
  tok.textSecondary = QColor{QStringLiteral("#435164")};
  tok.textMuted = QColor{QStringLiteral("#6F7D91")};
  tok.textDisabled = QColor{QStringLiteral("#A7B1BF")};
  tok.textInverse = QColor{QStringLiteral("#EEF3F8")};

  tok.primary = QColor{QStringLiteral("#3E7BDB")};
  tok.primaryHover = QColor{QStringLiteral("#4C8BEF")};
  tok.primaryPressed = QColor{QStringLiteral("#3266BE")};
  tok.onPrimary = QColor{QStringLiteral("#FFFFFF")};

  tok.borderSubtle = QColor{QStringLiteral("#D6DEE8")};
  tok.borderPassive = QColor{QStringLiteral("#B7C3D2")};
  tok.borderStrong = QColor{QStringLiteral("#8898AB")};
  tok.borderHover = QColor{QStringLiteral("#00A8D833")};
  tok.borderFocus = QColor{QStringLiteral("#00A8D8")};
  tok.borderActive = QColor{QStringLiteral("#3E7BDB")};
  tok.borderUrgent = QColor{QStringLiteral("#D84A68")};

  tok.glowCyanSoft = QColor{QStringLiteral("#00A8D833")};
  tok.glowBlueSoft = QColor{QStringLiteral("#3E7BDB30")};
  tok.glowVioletSoft = QColor{QStringLiteral("#7566D430")};
  tok.scrim = QColor{QStringLiteral("#1B253399")};
  tok.glassTint = QColor{QStringLiteral("#FFFFFFCC")};
  tok.hoverOverlay = QColor{QStringLiteral("#00A8D812")};
  tok.pressedOverlay = QColor{QStringLiteral("#00A8D822")};
  tok.disabledOverlay = QColor{QStringLiteral("#F3F7FB80")};
  tok.focusRing = QColor{QStringLiteral("#00A8D8")};
  tok.shadow = QColor{0x00, 0x00, 0x00};

  tok.error = QColor{QStringLiteral("#D84A68")};
  tok.warning = QColor{QStringLiteral("#C38A1C")};
  tok.success = QColor{QStringLiteral("#3E9449")};
  tok.onError = QColor{QStringLiteral("#FFFFFF")};

  tok.accentCyan = QColor{QStringLiteral("#00A8D8")};
  tok.accentBlue = QColor{QStringLiteral("#3E7BDB")};
  tok.accentViolet = QColor{QStringLiteral("#7566D4")};
  tok.accentYellow = QColor{QStringLiteral("#C38A1C")};

  tok.workspaceOccupied = QColor{QStringLiteral("#E3EAF3")};
  tok.workspaceActive = QColor{QStringLiteral("#D8E4F2")};

  tok.ansiBlack = QColor{QStringLiteral("#3B4554")};
  tok.ansiRed = QColor{QStringLiteral("#D94C67")};
  tok.ansiGreen = QColor{QStringLiteral("#3E964A")};
  tok.ansiYellow = QColor{QStringLiteral("#B97C00")};
  tok.ansiBlue = QColor{QStringLiteral("#3E7BDB")};
  tok.ansiMagenta = QColor{QStringLiteral("#7566D4")};
  tok.ansiCyan = QColor{QStringLiteral("#009CC8")};
  tok.ansiWhite = QColor{QStringLiteral("#F5F7FA")};
  tok.ansiBrightBlack = QColor{QStringLiteral("#7A8698")};
  tok.ansiBrightRed = QColor{QStringLiteral("#E86C84")};
  tok.ansiBrightGreen = QColor{QStringLiteral("#56B867")};
  tok.ansiBrightYellow = QColor{QStringLiteral("#D89A25")};
  tok.ansiBrightBlue = QColor{QStringLiteral("#5E97ED")};
  tok.ansiBrightMagenta = QColor{QStringLiteral("#9587EA")};
  tok.ansiBrightCyan = QColor{QStringLiteral("#1DB8E5")};
  tok.ansiBrightWhite = QColor{QStringLiteral("#FFFFFF")};

  populateCompatibilityAliases(&tok, tok.borderPassive, tok.borderSubtle);
  return tok;
}

ColorTokens catppuccinMochaTokens() {
  ColorTokens tok;

  tok.background = QColor{QStringLiteral("#11111B")};
  tok.surface = QColor{QStringLiteral("#181825")};
  tok.surfaceElevated = QColor{QStringLiteral("#1E1E2E")};
  tok.surfaceRaised = QColor{QStringLiteral("#313244")};
  tok.surfaceHover = QColor{QStringLiteral("#45475A")};
  tok.surfaceInverse = QColor{QStringLiteral("#EFF1F5")};

  tok.textPrimary = QColor{QStringLiteral("#CDD6F4")};
  tok.textSecondary = QColor{QStringLiteral("#BAC2DE")};
  tok.textMuted = QColor{QStringLiteral("#A6ADC8")};
  tok.textDisabled = QColor{QStringLiteral("#6C7086")};
  tok.textInverse = QColor{QStringLiteral("#4C4F69")};

  tok.primary = QColor{QStringLiteral("#89B4FA")};
  tok.primaryHover = QColor{QStringLiteral("#89DCEB")};
  tok.primaryPressed = QColor{QStringLiteral("#74C7EC")};
  tok.onPrimary = QColor{QStringLiteral("#11111B")};

  tok.borderSubtle = QColor{QStringLiteral("#313244")};
  tok.borderPassive = QColor{QStringLiteral("#6C7086")};
  tok.borderStrong = QColor{QStringLiteral("#7F849C")};
  tok.borderHover = colorWithAlpha(QStringLiteral("#89DCEB"), 0x55);
  tok.borderFocus = QColor{QStringLiteral("#B4BEFE")};
  tok.borderActive = QColor{QStringLiteral("#89B4FA")};
  tok.borderUrgent = QColor{QStringLiteral("#F38BA8")};

  tok.glowCyanSoft = colorWithAlpha(QStringLiteral("#89DCEB"), 0x33);
  tok.glowBlueSoft = colorWithAlpha(QStringLiteral("#89B4FA"), 0x30);
  tok.glowVioletSoft = colorWithAlpha(QStringLiteral("#CBA6F7"), 0x30);
  tok.scrim = colorWithAlpha(QStringLiteral("#000000"), 0x88);
  tok.glassTint = colorWithAlpha(QStringLiteral("#181825"), 0xd8);
  tok.hoverOverlay = colorWithAlpha(QStringLiteral("#89DCEB"), 0x12);
  tok.pressedOverlay = colorWithAlpha(QStringLiteral("#89DCEB"), 0x22);
  tok.disabledOverlay = colorWithAlpha(QStringLiteral("#11111B"), 0x80);
  tok.focusRing = QColor{QStringLiteral("#B4BEFE")};
  tok.shadow = QColor{0x00, 0x00, 0x00};

  tok.error = QColor{QStringLiteral("#F38BA8")};
  tok.warning = QColor{QStringLiteral("#F9E2AF")};
  tok.success = QColor{QStringLiteral("#A6E3A1")};
  tok.onError = QColor{QStringLiteral("#11111B")};

  tok.accentCyan = QColor{QStringLiteral("#89DCEB")};
  tok.accentBlue = QColor{QStringLiteral("#89B4FA")};
  tok.accentViolet = QColor{QStringLiteral("#CBA6F7")};
  tok.accentYellow = QColor{QStringLiteral("#F9E2AF")};

  tok.workspaceOccupied = QColor{QStringLiteral("#1E1E2E")};
  tok.workspaceActive = QColor{QStringLiteral("#313244")};

  tok.ansiBlack = tok.background;
  tok.ansiRed = tok.error;
  tok.ansiGreen = tok.success;
  tok.ansiYellow = tok.warning;
  tok.ansiBlue = tok.primary;
  tok.ansiMagenta = tok.accentViolet;
  tok.ansiCyan = tok.accentCyan;
  tok.ansiWhite = tok.textPrimary;
  tok.ansiBrightBlack = tok.textDisabled;
  tok.ansiBrightRed = QColor{QStringLiteral("#F5A6BD")};
  tok.ansiBrightGreen = QColor{QStringLiteral("#B9F2B3")};
  tok.ansiBrightYellow = QColor{QStringLiteral("#FAE8C1")};
  tok.ansiBrightBlue = QColor{QStringLiteral("#A5C9FF")};
  tok.ansiBrightMagenta = QColor{QStringLiteral("#DAB9FF")};
  tok.ansiBrightCyan = QColor{QStringLiteral("#A6E9F2")};
  tok.ansiBrightWhite = QColor{QStringLiteral("#F5E0DC")};

  populateCompatibilityAliases(&tok, tok.borderActive, tok.borderPassive);
  return tok;
}

ColorTokens catppuccinLatteTokens() {
  ColorTokens tok;

  tok.background = QColor{QStringLiteral("#EFF1F5")};
  tok.surface = QColor{QStringLiteral("#E6E9EF")};
  tok.surfaceElevated = QColor{QStringLiteral("#CCD0DA")};
  tok.surfaceRaised = QColor{QStringLiteral("#BCC0CC")};
  tok.surfaceHover = QColor{QStringLiteral("#ACB0BE")};
  tok.surfaceInverse = QColor{QStringLiteral("#11111B")};

  tok.textPrimary = QColor{QStringLiteral("#4C4F69")};
  tok.textSecondary = QColor{QStringLiteral("#5C5F77")};
  tok.textMuted = QColor{QStringLiteral("#6C6F85")};
  tok.textDisabled = QColor{QStringLiteral("#9CA0B0")};
  tok.textInverse = QColor{QStringLiteral("#CDD6F4")};

  tok.primary = QColor{QStringLiteral("#1E66F5")};
  tok.primaryHover = QColor{QStringLiteral("#04A5E5")};
  tok.primaryPressed = QColor{QStringLiteral("#209FB5")};
  tok.onPrimary = QColor{QStringLiteral("#FFFFFF")};

  tok.borderSubtle = QColor{QStringLiteral("#CCD0DA")};
  tok.borderPassive = QColor{QStringLiteral("#9CA0B0")};
  tok.borderStrong = QColor{QStringLiteral("#8C8FA1")};
  tok.borderHover = colorWithAlpha(QStringLiteral("#04A5E5"), 0x33);
  tok.borderFocus = QColor{QStringLiteral("#7287FD")};
  tok.borderActive = QColor{QStringLiteral("#1E66F5")};
  tok.borderUrgent = QColor{QStringLiteral("#D20F39")};

  tok.glowCyanSoft = colorWithAlpha(QStringLiteral("#04A5E5"), 0x33);
  tok.glowBlueSoft = colorWithAlpha(QStringLiteral("#1E66F5"), 0x30);
  tok.glowVioletSoft = colorWithAlpha(QStringLiteral("#8839EF"), 0x30);
  tok.scrim = colorWithAlpha(QStringLiteral("#4C4F69"), 0x99);
  tok.glassTint = colorWithAlpha(QStringLiteral("#EFF1F5"), 0xcc);
  tok.hoverOverlay = colorWithAlpha(QStringLiteral("#04A5E5"), 0x12);
  tok.pressedOverlay = colorWithAlpha(QStringLiteral("#04A5E5"), 0x22);
  tok.disabledOverlay = colorWithAlpha(QStringLiteral("#EFF1F5"), 0x80);
  tok.focusRing = QColor{QStringLiteral("#7287FD")};
  tok.shadow = QColor{0x00, 0x00, 0x00};

  tok.error = QColor{QStringLiteral("#D20F39")};
  tok.warning = QColor{QStringLiteral("#DF8E1D")};
  tok.success = QColor{QStringLiteral("#40A02B")};
  tok.onError = QColor{QStringLiteral("#FFFFFF")};

  tok.accentCyan = QColor{QStringLiteral("#04A5E5")};
  tok.accentBlue = QColor{QStringLiteral("#1E66F5")};
  tok.accentViolet = QColor{QStringLiteral("#8839EF")};
  tok.accentYellow = QColor{QStringLiteral("#DF8E1D")};

  tok.workspaceOccupied = QColor{QStringLiteral("#CCD0DA")};
  tok.workspaceActive = QColor{QStringLiteral("#BCC0CC")};

  tok.ansiBlack = QColor{QStringLiteral("#DCE0E8")};
  tok.ansiRed = tok.error;
  tok.ansiGreen = tok.success;
  tok.ansiYellow = tok.warning;
  tok.ansiBlue = tok.primary;
  tok.ansiMagenta = tok.accentViolet;
  tok.ansiCyan = tok.accentCyan;
  tok.ansiWhite = tok.textPrimary;
  tok.ansiBrightBlack = tok.textDisabled;
  tok.ansiBrightRed = QColor{QStringLiteral("#E64553")};
  tok.ansiBrightGreen = QColor{QStringLiteral("#179299")};
  tok.ansiBrightYellow = QColor{QStringLiteral("#FE640B")};
  tok.ansiBrightBlue = QColor{QStringLiteral("#209FB5")};
  tok.ansiBrightMagenta = QColor{QStringLiteral("#EA76CB")};
  tok.ansiBrightCyan = QColor{QStringLiteral("#04A5E5")};
  tok.ansiBrightWhite = QColor{QStringLiteral("#4C4F69")};

  populateCompatibilityAliases(&tok, tok.borderPassive, tok.borderSubtle);
  return tok;
}

ColorTokens tokyoNightStormTokens() {
  ColorTokens tok;

  tok.background = QColor{0x10, 0x13, 0x1f};       // #10131f
  tok.surface = QColor{0x16, 0x19, 0x25};          // #161925
  tok.surfaceElevated = QColor{0x1a, 0x1b, 0x26};  // #1a1b26
  tok.surfaceRaised = QColor{0x24, 0x28, 0x3b};    // #24283b
  tok.surfaceHover = QColor{0x1e, 0x22, 0x33};     // #1e2233
  tok.surfaceInverse = QColor{0x0d, 0x11, 0x17};   // #0d1117

  tok.textPrimary = QColor{0xc0, 0xca, 0xf5};    // #c0caf5
  tok.textSecondary = QColor{0xa9, 0xb1, 0xd6};  // #a9b1d6
  tok.textMuted = QColor{0x56, 0x5f, 0x89};      // #565f89
  tok.textDisabled = QColor{0x3b, 0x3f, 0x58};   // #3b3f58
  tok.textInverse = QColor{0xc0, 0xca, 0xf5};    // #c0caf5

  tok.primary = QColor{0x7a, 0xa2, 0xf7};         // #7aa2f7
  tok.primaryHover = QColor{0x4d, 0xe9, 0xff};    // #4de9ff  L=65%
  tok.primaryPressed = QColor{0x5d, 0x7f, 0xc7};  // #5d7fc7
  tok.onPrimary = QColor{0x1a, 0x1b, 0x26};       // #1a1b26  dark text on selection fill

  tok.borderSubtle = QColor{0x2f, 0x35, 0x4d};       // #2f354d
  tok.borderPassive = QColor{0x56, 0x5f, 0x89};      // #565f89  passive frame border
  tok.borderStrong = QColor{0x7c, 0x86, 0xb8};       // #7c86b8
  tok.borderHover = QColor{0x7d, 0xcf, 0xff, 0x4d};  // #7dcfff ~30%  hover-state border
  tok.borderFocus = QColor{0x00, 0xe0, 0xff};        // #00e0ff  keyboard focus ring
  tok.borderActive = QColor{0x7a, 0xa2, 0xf7};       // #7aa2f7  selected/active border
  tok.borderUrgent = QColor{0xf7, 0x76, 0x8e};       // #f7768e  critical/error border

  tok.glowCyanSoft = QColor{0x00, 0xe0, 0xff, 0x33};
  tok.glowBlueSoft = QColor{0x7a, 0xa2, 0xf7, 0x33};
  tok.glowVioletSoft = QColor{0xbb, 0x9a, 0xf7, 0x33};
  tok.scrim = QColor{0x00, 0x00, 0x00, 0x99};
  tok.glassTint = QColor{0x16, 0x19, 0x25, 0xcc};
  tok.hoverOverlay = QColor{0x00, 0xe0, 0xff, 0x1a};
  tok.pressedOverlay = QColor{0x00, 0xe0, 0xff, 0x26};
  tok.disabledOverlay = QColor{0x10, 0x13, 0x1f, 0x80};
  tok.focusRing = QColor{0x00, 0xe0, 0xff};  // #00e0ff
  tok.shadow = QColor{0x00, 0x00, 0x00};     // #000000

  tok.error = QColor{0xf7, 0x76, 0x8e};    // #f7768e
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

  populateCompatibilityAliases(&tok, tok.borderActive, tok.borderPassive);

  return tok;
}

ColorTokens tokyoNightDayTokens() {
  ColorTokens tok;

  tok.background = QColor{0xf4, 0xf7, 0xfb};
  tok.surface = QColor{0xed, 0xf2, 0xf8};
  tok.surfaceElevated = QColor{0xff, 0xff, 0xff};
  tok.surfaceRaised = QColor{0xe6, 0xed, 0xf6};
  tok.surfaceHover = QColor{0xdb, 0xe7, 0xf4};
  tok.surfaceInverse = QColor{0x15, 0x18, 0x27};

  tok.textPrimary = QColor{0x1f, 0x23, 0x35};
  tok.textSecondary = QColor{0x3b, 0x42, 0x61};
  tok.textMuted = QColor{0x6b, 0x73, 0x94};
  tok.textDisabled = QColor{0xa7, 0xae, 0xc4};
  tok.textInverse = QColor{0xc0, 0xca, 0xf5};

  tok.primary = QColor{0x2f, 0x6f, 0xe4};
  tok.primaryHover = QColor{0x00, 0x8f, 0xc7};
  tok.primaryPressed = QColor{0x27, 0x57, 0xb8};
  tok.onPrimary = QColor{0xff, 0xff, 0xff};

  tok.borderSubtle = QColor{0xd6, 0xde, 0xec};
  tok.borderPassive = QColor{0xae, 0xb9, 0xcf};
  tok.borderStrong = QColor{0x7f, 0x8d, 0xad};
  tok.borderHover = QColor{0x00, 0x8f, 0xc7, 0x66};
  tok.borderFocus = QColor{0x00, 0xa6, 0xd6};
  tok.borderActive = QColor{0x2f, 0x6f, 0xe4};
  tok.borderUrgent = QColor{0xd9, 0x3f, 0x5c};

  tok.glowCyanSoft = QColor{0x00, 0xa6, 0xd6, 0x33};
  tok.glowBlueSoft = QColor{0x2f, 0x6f, 0xe4, 0x33};
  tok.glowVioletSoft = QColor{0x7b, 0x61, 0xd1, 0x33};
  tok.scrim = QColor{0x1f, 0x23, 0x35, 0x99};
  tok.glassTint = QColor{0xff, 0xff, 0xff, 0xcc};
  tok.hoverOverlay = QColor{0x00, 0xa6, 0xd6, 0x1a};
  tok.pressedOverlay = QColor{0x00, 0xa6, 0xd6, 0x26};
  tok.disabledOverlay = QColor{0xf4, 0xf7, 0xfb, 0x80};
  tok.focusRing = QColor{0x00, 0xa6, 0xd6};
  tok.shadow = QColor{0x00, 0x00, 0x00};

  tok.error = QColor{0xd9, 0x3f, 0x5c};
  tok.warning = QColor{0xc4, 0x7a, 0x1c};
  tok.success = QColor{0x4d, 0x8f, 0x31};
  tok.onError = QColor{0xff, 0xff, 0xff};

  tok.accentCyan = QColor{0x00, 0x8f, 0xc7};
  tok.accentBlue = QColor{0x2f, 0x6f, 0xe4};
  tok.accentViolet = QColor{0x7b, 0x61, 0xd1};
  tok.accentYellow = QColor{0xb8, 0x79, 0x00};

  tok.workspaceOccupied = QColor{0xe3, 0xea, 0xf5};
  tok.workspaceActive = QColor{0xd7, 0xe4, 0xf6};

  tok.ansiBlack = QColor{0xd6, 0xde, 0xec};
  tok.ansiRed = QColor{0xd9, 0x3f, 0x5c};
  tok.ansiGreen = QColor{0x4d, 0x8f, 0x31};
  tok.ansiYellow = QColor{0xb8, 0x79, 0x00};
  tok.ansiBlue = QColor{0x2f, 0x6f, 0xe4};
  tok.ansiMagenta = QColor{0x7b, 0x61, 0xd1};
  tok.ansiCyan = QColor{0x00, 0x8f, 0xc7};
  tok.ansiWhite = QColor{0x1f, 0x23, 0x35};
  tok.ansiBrightBlack = QColor{0x8c, 0x96, 0xad};
  tok.ansiBrightRed = QColor{0xb9, 0x1f, 0x3d};
  tok.ansiBrightGreen = QColor{0x36, 0x75, 0x1f};
  tok.ansiBrightYellow = QColor{0x94, 0x62, 0x00};
  tok.ansiBrightBlue = QColor{0x1e, 0x55, 0xc8};
  tok.ansiBrightMagenta = QColor{0x66, 0x48, 0xc3};
  tok.ansiBrightCyan = QColor{0x00, 0x76, 0xa6};
  tok.ansiBrightWhite = QColor{0x10, 0x13, 0x1f};

  populateCompatibilityAliases(&tok, tok.borderPassive, tok.borderSubtle);
  return tok;
}

ColorTokens gruvboxEmberTokens() {
  ColorTokens tok;

  tok.background = QColor{QStringLiteral("#1d2021")};
  tok.surface = QColor{QStringLiteral("#282828")};
  tok.surfaceElevated = QColor{QStringLiteral("#3c3836")};
  tok.surfaceRaised = QColor{QStringLiteral("#504945")};
  tok.surfaceHover = QColor{QStringLiteral("#665c54")};
  tok.surfaceInverse = QColor{QStringLiteral("#f9f5d7")};

  tok.textPrimary = QColor{QStringLiteral("#fbf1c7")};
  tok.textSecondary = QColor{QStringLiteral("#ebdbb2")};
  tok.textMuted = QColor{QStringLiteral("#bdae93")};
  tok.textDisabled = QColor{QStringLiteral("#928374")};
  tok.textInverse = QColor{QStringLiteral("#282828")};

  tok.primary = QColor{QStringLiteral("#8ec07c")};
  tok.primaryHover = QColor{QStringLiteral("#a8d3c5")};
  tok.primaryPressed = QColor{QStringLiteral("#83a598")};
  tok.onPrimary = QColor{QStringLiteral("#1d2021")};

  tok.borderSubtle = QColor{QStringLiteral("#3c3836")};
  tok.borderPassive = QColor{QStringLiteral("#504945")};
  tok.borderStrong = QColor{QStringLiteral("#665c54")};
  tok.borderHover = colorWithAlpha(QStringLiteral("#8ec07c"), 0x55);
  tok.borderFocus = QColor{QStringLiteral("#fabd2f")};
  tok.borderActive = QColor{QStringLiteral("#8ec07c")};
  tok.borderUrgent = QColor{QStringLiteral("#ff6550")};

  tok.glowCyanSoft = colorWithAlpha(QStringLiteral("#8ec07c"), 0x33);
  tok.glowBlueSoft = colorWithAlpha(QStringLiteral("#83a598"), 0x30);
  tok.glowVioletSoft = colorWithAlpha(QStringLiteral("#d3869b"), 0x30);
  tok.scrim = colorWithAlpha(QStringLiteral("#000000"), 0x88);
  tok.glassTint = colorWithAlpha(QStringLiteral("#282828"), 0xd8);
  tok.hoverOverlay = colorWithAlpha(QStringLiteral("#8ec07c"), 0x12);
  tok.pressedOverlay = colorWithAlpha(QStringLiteral("#8ec07c"), 0x22);
  tok.disabledOverlay = colorWithAlpha(QStringLiteral("#1d2021"), 0x80);
  tok.focusRing = QColor{QStringLiteral("#fabd2f")};
  tok.shadow = QColor{0, 0, 0};

  tok.error = QColor{QStringLiteral("#ff6550")};
  tok.warning = QColor{QStringLiteral("#fe8019")};
  tok.success = QColor{QStringLiteral("#b8bb26")};
  tok.onError = QColor{QStringLiteral("#1d2021")};

  tok.accentCyan = QColor{QStringLiteral("#8ec07c")};
  tok.accentBlue = QColor{QStringLiteral("#83a598")};
  tok.accentViolet = QColor{QStringLiteral("#d3869b")};
  tok.accentYellow = QColor{QStringLiteral("#fabd2f")};

  tok.workspaceOccupied = QColor{QStringLiteral("#3c3836")};
  tok.workspaceActive = QColor{QStringLiteral("#504945")};

  tok.ansiBlack = QColor{QStringLiteral("#282828")};
  tok.ansiRed = QColor{QStringLiteral("#cc241d")};
  tok.ansiGreen = QColor{QStringLiteral("#98971a")};
  tok.ansiYellow = QColor{QStringLiteral("#d79921")};
  tok.ansiBlue = QColor{QStringLiteral("#458588")};
  tok.ansiMagenta = QColor{QStringLiteral("#b16286")};
  tok.ansiCyan = QColor{QStringLiteral("#689d6a")};
  tok.ansiWhite = QColor{QStringLiteral("#a89984")};
  tok.ansiBrightBlack = QColor{QStringLiteral("#928374")};
  tok.ansiBrightRed = QColor{QStringLiteral("#fb4934")};
  tok.ansiBrightGreen = QColor{QStringLiteral("#b8bb26")};
  tok.ansiBrightYellow = QColor{QStringLiteral("#fabd2f")};
  tok.ansiBrightBlue = QColor{QStringLiteral("#83a598")};
  tok.ansiBrightMagenta = QColor{QStringLiteral("#d3869b")};
  tok.ansiBrightCyan = QColor{QStringLiteral("#8ec07c")};
  tok.ansiBrightWhite = QColor{QStringLiteral("#fbf1c7")};

  populateCompatibilityAliases(&tok, tok.borderActive, tok.borderPassive);
  return tok;
}

ColorTokens gruvboxSolTokens() {
  ColorTokens tok;

  tok.background = QColor{QStringLiteral("#f9f5d7")};
  tok.surface = QColor{QStringLiteral("#fbf1c7")};
  tok.surfaceElevated = QColor{QStringLiteral("#ebdbb2")};
  tok.surfaceRaised = QColor{QStringLiteral("#d5c4a1")};
  tok.surfaceHover = QColor{QStringLiteral("#bdae93")};
  tok.surfaceInverse = QColor{QStringLiteral("#1d2021")};

  tok.textPrimary = QColor{QStringLiteral("#282828")};
  tok.textSecondary = QColor{QStringLiteral("#3c3836")};
  tok.textMuted = QColor{QStringLiteral("#665c54")};
  tok.textDisabled = QColor{QStringLiteral("#928374")};
  tok.textInverse = QColor{QStringLiteral("#fbf1c7")};

  tok.primary = QColor{QStringLiteral("#076678")};
  tok.primaryHover = QColor{QStringLiteral("#458588")};
  tok.primaryPressed = QColor{QStringLiteral("#054955")};
  tok.onPrimary = QColor{QStringLiteral("#ffffff")};

  tok.borderSubtle = QColor{QStringLiteral("#ebdbb2")};
  tok.borderPassive = QColor{QStringLiteral("#d5c4a1")};
  tok.borderStrong = QColor{QStringLiteral("#bdae93")};
  tok.borderHover = colorWithAlpha(QStringLiteral("#458588"), 0x33);
  tok.borderFocus = QColor{QStringLiteral("#9e3202")};
  tok.borderActive = QColor{QStringLiteral("#076678")};
  tok.borderUrgent = QColor{QStringLiteral("#9d0006")};

  tok.glowCyanSoft = colorWithAlpha(QStringLiteral("#427b58"), 0x33);
  tok.glowBlueSoft = colorWithAlpha(QStringLiteral("#076678"), 0x30);
  tok.glowVioletSoft = colorWithAlpha(QStringLiteral("#8f3f71"), 0x30);
  tok.scrim = colorWithAlpha(QStringLiteral("#282828"), 0x99);
  tok.glassTint = colorWithAlpha(QStringLiteral("#ffffff"), 0xcc);
  tok.hoverOverlay = colorWithAlpha(QStringLiteral("#458588"), 0x12);
  tok.pressedOverlay = colorWithAlpha(QStringLiteral("#458588"), 0x22);
  tok.disabledOverlay = colorWithAlpha(QStringLiteral("#f9f5d7"), 0x80);
  tok.focusRing = QColor{QStringLiteral("#9e3202")};
  tok.shadow = QColor{0, 0, 0};

  tok.error = QColor{QStringLiteral("#9d0006")};
  tok.warning = QColor{QStringLiteral("#9e3202")};
  tok.success = QColor{QStringLiteral("#736e0f")};
  tok.onError = QColor{QStringLiteral("#ffffff")};

  tok.accentCyan = QColor{QStringLiteral("#427b58")};
  tok.accentBlue = QColor{QStringLiteral("#076678")};
  tok.accentViolet = QColor{QStringLiteral("#8f3f71")};
  tok.accentYellow = QColor{QStringLiteral("#b57614")};

  tok.workspaceOccupied = QColor{QStringLiteral("#ebdbb2")};
  tok.workspaceActive = QColor{QStringLiteral("#d5c4a1")};

  tok.ansiBlack = QColor{QStringLiteral("#fbf1c7")};
  tok.ansiRed = QColor{QStringLiteral("#9d0006")};
  tok.ansiGreen = QColor{QStringLiteral("#79740e")};
  tok.ansiYellow = QColor{QStringLiteral("#b57614")};
  tok.ansiBlue = QColor{QStringLiteral("#076678")};
  tok.ansiMagenta = QColor{QStringLiteral("#8f3f71")};
  tok.ansiCyan = QColor{QStringLiteral("#427b58")};
  tok.ansiWhite = QColor{QStringLiteral("#3c3836")};
  tok.ansiBrightBlack = QColor{QStringLiteral("#928374")};
  tok.ansiBrightRed = QColor{QStringLiteral("#cc241d")};
  tok.ansiBrightGreen = QColor{QStringLiteral("#98971a")};
  tok.ansiBrightYellow = QColor{QStringLiteral("#d79921")};
  tok.ansiBrightBlue = QColor{QStringLiteral("#458588")};
  tok.ansiBrightMagenta = QColor{QStringLiteral("#b16286")};
  tok.ansiBrightCyan = QColor{QStringLiteral("#689d6a")};
  tok.ansiBrightWhite = QColor{QStringLiteral("#282828")};

  populateCompatibilityAliases(&tok, tok.borderPassive, tok.borderSubtle);
  return tok;
}

ColorTokens holoNightCyberDTokens() {
  ColorTokens tok;

  tok.background = QColor{QStringLiteral("#080B12")};
  tok.surface = QColor{QStringLiteral("#0B0F17")};
  tok.surfaceElevated = QColor{QStringLiteral("#11161F")};
  tok.surfaceRaised = QColor{QStringLiteral("#1A2031")};
  tok.surfaceHover = QColor{QStringLiteral("#151B2B")};
  tok.surfaceInverse = QColor{QStringLiteral("#05070D")};

  tok.textPrimary = QColor{QStringLiteral("#E6ECFF")};
  tok.textSecondary = QColor{QStringLiteral("#A0A8C8")};
  tok.textMuted = QColor{QStringLiteral("#6B7596")};
  tok.textDisabled = QColor{QStringLiteral("#424A63")};
  tok.textInverse = QColor{QStringLiteral("#E6ECFF")};

  tok.primary = QColor{QStringLiteral("#39D5FF")};
  tok.primaryHover = QColor{QStringLiteral("#6BE4FF")};
  tok.primaryPressed = QColor{QStringLiteral("#1FAEDB")};
  tok.onPrimary = QColor{QStringLiteral("#05070D")};

  tok.borderSubtle = QColor{QStringLiteral("#18202F")};
  tok.borderPassive = QColor{QStringLiteral("#1F2A44")};
  tok.borderStrong = QColor{QStringLiteral("#36406A")};
  tok.borderHover = QColor{0x39, 0xD5, 0xFF, 0x4D};
  tok.borderFocus = QColor{QStringLiteral("#00E1FF")};
  tok.borderActive = QColor{QStringLiteral("#39D5FF")};
  tok.borderUrgent = QColor{QStringLiteral("#F92838")};

  tok.glowCyanSoft = QColor{0x39, 0xD5, 0xFF, 0x40};
  tok.glowBlueSoft = QColor{0x6B, 0x4F, 0xE8, 0x40};
  tok.glowVioletSoft = QColor{0xB2, 0x6C, 0xFF, 0x40};
  tok.scrim = QColor{0x00, 0x00, 0x00, 0x99};
  tok.glassTint = QColor{0x0B, 0x0F, 0x17, 0xCC};
  tok.hoverOverlay = QColor{0x39, 0xD5, 0xFF, 0x1A};
  tok.pressedOverlay = QColor{0x39, 0xD5, 0xFF, 0x26};
  tok.disabledOverlay = QColor{0x08, 0x0B, 0x12, 0x80};
  tok.focusRing = QColor{QStringLiteral("#00E1FF")};
  tok.shadow = QColor{QStringLiteral("#000000")};

  tok.error = QColor{QStringLiteral("#F92838")};
  tok.warning = QColor{QStringLiteral("#FEA257")};
  tok.success = QColor{QStringLiteral("#10E169")};
  tok.onError = QColor{QStringLiteral("#FFFFFF")};

  tok.accentCyan = QColor{QStringLiteral("#39D5FF")};
  tok.accentBlue = QColor{QStringLiteral("#6B4FE8")};
  tok.accentViolet = QColor{QStringLiteral("#B26CFF")};
  tok.accentYellow = QColor{QStringLiteral("#F4C56B")};

  tok.workspaceOccupied = QColor{QStringLiteral("#161C2E")};
  tok.workspaceActive = QColor{QStringLiteral("#1C2540")};

  tok.radiusTight = 4;
  tok.radiusControl = 8;
  tok.radiusPopup = 12;
  tok.radiusPill = 999;
  tok.borderWidth = 1;
  tok.focusBorderWidth = 2;
  tok.separatorWidth = 1;
  tok.controlHeight = 32;
  tok.controlPadding = 8;

  tok.ansiBlack = QColor{QStringLiteral("#0B0F17")};
  tok.ansiRed = QColor{QStringLiteral("#F92838")};
  tok.ansiGreen = QColor{QStringLiteral("#10E169")};
  tok.ansiYellow = QColor{QStringLiteral("#FEA257")};
  tok.ansiBlue = QColor{QStringLiteral("#6B4FE8")};
  tok.ansiMagenta = QColor{QStringLiteral("#B26CFF")};
  tok.ansiCyan = QColor{QStringLiteral("#39D5FF")};
  tok.ansiWhite = QColor{QStringLiteral("#A0A8C8")};
  tok.ansiBrightBlack = QColor{QStringLiteral("#2A3350")};
  tok.ansiBrightRed = QColor{QStringLiteral("#FF5566")};
  tok.ansiBrightGreen = QColor{QStringLiteral("#5BF59B")};
  tok.ansiBrightYellow = QColor{QStringLiteral("#FFC07A")};
  tok.ansiBrightBlue = QColor{QStringLiteral("#8470FF")};
  tok.ansiBrightMagenta = QColor{QStringLiteral("#CB9BFF")};
  tok.ansiBrightCyan = QColor{QStringLiteral("#7DE8FF")};
  tok.ansiBrightWhite = QColor{QStringLiteral("#E6ECFF")};

  populateCompatibilityAliases(&tok, tok.borderActive, tok.borderPassive);
  return tok;
}

ColorTokens holoNightCyberLTokens() {
  ColorTokens tok;

  tok.background = QColor{QStringLiteral("#EEF1F8")};
  tok.surface = QColor{QStringLiteral("#FFFFFF")};
  tok.surfaceElevated = QColor{QStringLiteral("#F4F6FC")};
  tok.surfaceRaised = QColor{QStringLiteral("#E6EAF4")};
  tok.surfaceHover = QColor{QStringLiteral("#DCE2F0")};
  tok.surfaceInverse = QColor{QStringLiteral("#0B0F17")};

  tok.textPrimary = QColor{QStringLiteral("#1A2138")};
  tok.textSecondary = QColor{QStringLiteral("#4A5578")};
  tok.textMuted = QColor{QStringLiteral("#7A85A8")};
  tok.textDisabled = QColor{QStringLiteral("#AEB6CE")};
  tok.textInverse = QColor{QStringLiteral("#E6ECFF")};

  tok.primary = QColor{QStringLiteral("#0E9BD6")};
  tok.primaryHover = QColor{QStringLiteral("#12B4F2")};
  tok.primaryPressed = QColor{QStringLiteral("#0A7AAA")};
  tok.onPrimary = QColor{QStringLiteral("#FFFFFF")};

  tok.borderSubtle = QColor{QStringLiteral("#DDE3F0")};
  tok.borderPassive = QColor{QStringLiteral("#C2CADE")};
  tok.borderStrong = QColor{QStringLiteral("#9AA3C0")};
  tok.borderHover = QColor{0x0E, 0x9B, 0xD6, 0x4D};
  tok.borderFocus = QColor{QStringLiteral("#008FC4")};
  tok.borderActive = QColor{QStringLiteral("#0E9BD6")};
  tok.borderUrgent = QColor{QStringLiteral("#E11D48")};

  tok.glowCyanSoft = QColor{0x0E, 0x9B, 0xD6, 0x33};
  tok.glowBlueSoft = QColor{0x6B, 0x4F, 0xE8, 0x33};
  tok.glowVioletSoft = QColor{0xB2, 0x6C, 0xFF, 0x33};
  tok.scrim = QColor{0x00, 0x00, 0x00, 0x59};
  tok.glassTint = QColor{0xFF, 0xFF, 0xFF, 0xCC};
  tok.hoverOverlay = QColor{0x0E, 0x9B, 0xD6, 0x14};
  tok.pressedOverlay = QColor{0x0E, 0x9B, 0xD6, 0x1F};
  tok.disabledOverlay = QColor{0xEE, 0xF1, 0xF8, 0x99};
  tok.focusRing = QColor{QStringLiteral("#008FC4")};
  tok.shadow = QColor{QStringLiteral("#000000")};

  tok.error = QColor{QStringLiteral("#E11D48")};
  tok.warning = QColor{QStringLiteral("#C2710C")};
  tok.success = QColor{QStringLiteral("#0E9C56")};
  tok.onError = QColor{QStringLiteral("#FFFFFF")};

  tok.accentCyan = QColor{QStringLiteral("#0E9BD6")};
  tok.accentBlue = QColor{QStringLiteral("#5538D6")};
  tok.accentViolet = QColor{QStringLiteral("#8B3FE0")};
  tok.accentYellow = QColor{QStringLiteral("#B8862A")};

  tok.workspaceOccupied = QColor{QStringLiteral("#E2E6F2")};
  tok.workspaceActive = QColor{QStringLiteral("#D6DCEE")};

  tok.radiusTight = 4;
  tok.radiusControl = 8;
  tok.radiusPopup = 12;
  tok.radiusPill = 999;
  tok.borderWidth = 1;
  tok.focusBorderWidth = 2;
  tok.separatorWidth = 1;
  tok.controlHeight = 32;
  tok.controlPadding = 8;

  tok.ansiBlack = QColor{QStringLiteral("#C2CADE")};
  tok.ansiRed = QColor{QStringLiteral("#E11D48")};
  tok.ansiGreen = QColor{QStringLiteral("#0E9C56")};
  tok.ansiYellow = QColor{QStringLiteral("#B8742A")};
  tok.ansiBlue = QColor{QStringLiteral("#5538D6")};
  tok.ansiMagenta = QColor{QStringLiteral("#8B3FE0")};
  tok.ansiCyan = QColor{QStringLiteral("#0E8FC4")};
  tok.ansiWhite = QColor{QStringLiteral("#4A5578")};
  tok.ansiBrightBlack = QColor{QStringLiteral("#9AA3C0")};
  tok.ansiBrightRed = QColor{QStringLiteral("#F43F5E")};
  tok.ansiBrightGreen = QColor{QStringLiteral("#16B364")};
  tok.ansiBrightYellow = QColor{QStringLiteral("#D98A2B")};
  tok.ansiBrightBlue = QColor{QStringLiteral("#6B4FE8")};
  tok.ansiBrightMagenta = QColor{QStringLiteral("#A855F7")};
  tok.ansiBrightCyan = QColor{QStringLiteral("#22B8E6")};
  tok.ansiBrightWhite = QColor{QStringLiteral("#1A2138")};

  populateCompatibilityAliases(&tok, tok.borderPassive, tok.borderSubtle);
  return tok;
}

}  // namespace

ColorTokens tokensForScheme(ThemeSchemeKind scheme) {
  switch (scheme) {
    case ThemeSchemeKind::HoloNightDark:
      return holoNightDarkTokens();
    case ThemeSchemeKind::HoloNightLight:
      return holoNightLightTokens();
    case ThemeSchemeKind::HoloNightMocha:
      return catppuccinMochaTokens();
    case ThemeSchemeKind::HoloNightLatte:
      return catppuccinLatteTokens();
    case ThemeSchemeKind::TokyoNightStorm:
      return tokyoNightStormTokens();
    case ThemeSchemeKind::TokyoNightDay:
      return tokyoNightDayTokens();
    case ThemeSchemeKind::HoloNightEmber:
      return gruvboxEmberTokens();
    case ThemeSchemeKind::HoloNightSol:
      return gruvboxSolTokens();
    case ThemeSchemeKind::HoloNightCyberD:
      return holoNightCyberDTokens();
    case ThemeSchemeKind::HoloNightCyberL:
      return holoNightCyberLTokens();
  }
  return holoNightDarkTokens();
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
