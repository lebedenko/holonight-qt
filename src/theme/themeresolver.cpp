// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "themeresolver.h"

namespace Holonight {

ColorTokens ThemeResolver::resolveBase(ThemeSchemeKind scheme) { return tokensForScheme(scheme); }

void ThemeResolver::applyAccent(ColorTokens& tok, const QString& accent) {
  if (accent == QStringLiteral("cyan")) {
    tok.primary = QColor{QStringLiteral("#7dcfff")};
    tok.primaryHover = QColor{QStringLiteral("#a3d8ff")};
    tok.primaryPressed = QColor{QStringLiteral("#5cb8f5")};
    tok.borderFocus = QColor{QStringLiteral("#7dcfff")};
    tok.borderActive = QColor{QStringLiteral("#7dcfff")};
    tok.focusRing = QColor{QStringLiteral("#7dcfff55")};
    tok.glowCyanSoft = QColor{QStringLiteral("#7dcfff22")};
    tok.glowBlueSoft = QColor{QStringLiteral("#7dcfff18")};
    tok.glowVioletSoft = QColor{QStringLiteral("#7dcfff12")};
  } else if (accent == QStringLiteral("blue")) {
    tok.primary = QColor{QStringLiteral("#7aa2f7")};
    tok.primaryHover = QColor{QStringLiteral("#9ab5ff")};
    tok.primaryPressed = QColor{QStringLiteral("#5f88e8")};
    tok.borderFocus = QColor{QStringLiteral("#7aa2f7")};
    tok.borderActive = QColor{QStringLiteral("#7aa2f7")};
    tok.focusRing = QColor{QStringLiteral("#7aa2f755")};
    tok.glowCyanSoft = QColor{QStringLiteral("#7aa2f722")};
    tok.glowBlueSoft = QColor{QStringLiteral("#7aa2f718")};
    tok.glowVioletSoft = QColor{QStringLiteral("#7aa2f712")};
  } else if (accent == QStringLiteral("violet")) {
    tok.primary = QColor{QStringLiteral("#bb9af7")};
    tok.primaryHover = QColor{QStringLiteral("#cdb4ff")};
    tok.primaryPressed = QColor{QStringLiteral("#a07fdf")};
    tok.borderFocus = QColor{QStringLiteral("#bb9af7")};
    tok.borderActive = QColor{QStringLiteral("#bb9af7")};
    tok.focusRing = QColor{QStringLiteral("#bb9af755")};
    tok.glowCyanSoft = QColor{QStringLiteral("#bb9af722")};
    tok.glowBlueSoft = QColor{QStringLiteral("#bb9af718")};
    tok.glowVioletSoft = QColor{QStringLiteral("#bb9af712")};
  } else if (accent == QStringLiteral("yellow")) {
    tok.primary = QColor{QStringLiteral("#e0af68")};
    tok.primaryHover = QColor{QStringLiteral("#f0c98a")};
    tok.primaryPressed = QColor{QStringLiteral("#c99050")};
    tok.borderFocus = QColor{QStringLiteral("#e0af68")};
    tok.borderActive = QColor{QStringLiteral("#e0af68")};
    tok.focusRing = QColor{QStringLiteral("#e0af6855")};
    tok.glowCyanSoft = QColor{QStringLiteral("#e0af6822")};
    tok.glowBlueSoft = QColor{QStringLiteral("#e0af6818")};
    tok.glowVioletSoft = QColor{QStringLiteral("#e0af6812")};
  }
}

ColorTokens ThemeResolver::resolve(const ThemeConfig& config) {
  ColorTokens tok = resolveBase(config.resolvedThemeScheme());
  applyAccent(tok, config.resolvedAccent());
  return tok;
}

}  // namespace Holonight
