// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "themeresolver.h"

#include "holonight/theme_catalog.h"

namespace Holonight {

ColorTokens ThemeResolver::resolveBase(ThemeSchemeKind scheme) { return tokensForScheme(scheme); }

namespace {

struct AccentOverride {
  QColor primary;
  QColor hover;
  QColor pressed;
};

bool catppuccinAccent(const QString& accent, ThemeSchemeKind scheme, AccentOverride* out) {
  if (scheme == ThemeSchemeKind::HoloNightMocha) {
    if (accent == QStringLiteral("cyan")) {
      *out = {QColor{QStringLiteral("#89DCEB")}, QColor{QStringLiteral("#94E2D5")}, QColor{QStringLiteral("#74C7EC")}};
      return true;
    }
    if (accent == QStringLiteral("blue")) {
      *out = {QColor{QStringLiteral("#89B4FA")}, QColor{QStringLiteral("#89DCEB")}, QColor{QStringLiteral("#74C7EC")}};
      return true;
    }
    if (accent == QStringLiteral("violet")) {
      *out = {QColor{QStringLiteral("#CBA6F7")}, QColor{QStringLiteral("#F5C2E7")}, QColor{QStringLiteral("#B4BEFE")}};
      return true;
    }
    if (accent == QStringLiteral("yellow")) {
      *out = {QColor{QStringLiteral("#F9E2AF")}, QColor{QStringLiteral("#FAB387")}, QColor{QStringLiteral("#EBA0AC")}};
      return true;
    }
  }

  if (scheme == ThemeSchemeKind::HoloNightLatte) {
    if (accent == QStringLiteral("cyan")) {
      *out = {QColor{QStringLiteral("#04A5E5")}, QColor{QStringLiteral("#179299")}, QColor{QStringLiteral("#209FB5")}};
      return true;
    }
    if (accent == QStringLiteral("blue")) {
      *out = {QColor{QStringLiteral("#1E66F5")}, QColor{QStringLiteral("#04A5E5")}, QColor{QStringLiteral("#209FB5")}};
      return true;
    }
    if (accent == QStringLiteral("violet")) {
      *out = {QColor{QStringLiteral("#8839EF")}, QColor{QStringLiteral("#EA76CB")}, QColor{QStringLiteral("#7287FD")}};
      return true;
    }
    if (accent == QStringLiteral("yellow")) {
      *out = {QColor{QStringLiteral("#DF8E1D")}, QColor{QStringLiteral("#FE640B")}, QColor{QStringLiteral("#E64553")}};
      return true;
    }
  }

  return false;
}

bool gruvboxAccent(const QString& accent, ThemeSchemeKind scheme, AccentOverride* out) {
  if (scheme == ThemeSchemeKind::HoloNightEmber) {
    if (accent == QStringLiteral("cyan")) {
      *out = {QColor{QStringLiteral("#8ec07c")}, QColor{QStringLiteral("#a8d3c5")}, QColor{QStringLiteral("#83a598")}};
      return true;
    }
    if (accent == QStringLiteral("blue")) {
      *out = {QColor{QStringLiteral("#83a598")}, QColor{QStringLiteral("#a8d3c5")}, QColor{QStringLiteral("#95c3b1")}};
      return true;
    }
    if (accent == QStringLiteral("violet")) {
      *out = {QColor{QStringLiteral("#d3869b")}, QColor{QStringLiteral("#e5b3c3")}, QColor{QStringLiteral("#c1728a")}};
      return true;
    }
    if (accent == QStringLiteral("yellow")) {
      *out = {QColor{QStringLiteral("#fabd2f")}, QColor{QStringLiteral("#fcd268")}, QColor{QStringLiteral("#e5aa20")}};
      return true;
    }
  }

  if (scheme == ThemeSchemeKind::HoloNightSol) {
    if (accent == QStringLiteral("cyan")) {
      *out = {QColor{QStringLiteral("#427b58")}, QColor{QStringLiteral("#689d6a")}, QColor{QStringLiteral("#2d5c3f")}};
      return true;
    }
    if (accent == QStringLiteral("blue")) {
      *out = {QColor{QStringLiteral("#076678")}, QColor{QStringLiteral("#458588")}, QColor{QStringLiteral("#054955")}};
      return true;
    }
    if (accent == QStringLiteral("violet")) {
      *out = {QColor{QStringLiteral("#8f3f71")}, QColor{QStringLiteral("#b16286")}, QColor{QStringLiteral("#6b2d52")}};
      return true;
    }
    if (accent == QStringLiteral("yellow")) {
      *out = {QColor{QStringLiteral("#b57614")}, QColor{QStringLiteral("#d79921")}, QColor{QStringLiteral("#8c580c")}};
      return true;
    }
  }

  return false;
}

bool cyberAccent(const QString& accent, ThemeSchemeKind scheme, AccentOverride* out) {
  if (scheme == ThemeSchemeKind::HoloNightCyberD) {
    if (accent == QStringLiteral("cyan")) {
      *out = {QColor{QStringLiteral("#39D5FF")}, QColor{QStringLiteral("#6BE4FF")}, QColor{QStringLiteral("#1FAEDB")}};
      return true;
    }
    if (accent == QStringLiteral("blue")) {
      *out = {QColor{QStringLiteral("#6B4FE8")}, QColor{QStringLiteral("#8470FF")}, QColor{QStringLiteral("#533CBF")}};
      return true;
    }
    if (accent == QStringLiteral("violet")) {
      *out = {QColor{QStringLiteral("#B26CFF")}, QColor{QStringLiteral("#CB9BFF")}, QColor{QStringLiteral("#944DFF")}};
      return true;
    }
    if (accent == QStringLiteral("yellow")) {
      *out = {QColor{QStringLiteral("#F4C56B")}, QColor{QStringLiteral("#FFD98A")}, QColor{QStringLiteral("#D6A34A")}};
      return true;
    }
  }
  if (scheme == ThemeSchemeKind::HoloNightCyberL) {
    if (accent == QStringLiteral("cyan")) {
      *out = {QColor{QStringLiteral("#0E9BD6")}, QColor{QStringLiteral("#12B4F2")}, QColor{QStringLiteral("#0A7AAA")}};
      return true;
    }
    if (accent == QStringLiteral("blue")) {
      *out = {QColor{QStringLiteral("#5538D6")}, QColor{QStringLiteral("#6B4FE8")}, QColor{QStringLiteral("#3F25B5")}};
      return true;
    }
    if (accent == QStringLiteral("violet")) {
      *out = {QColor{QStringLiteral("#8B3FE0")}, QColor{QStringLiteral("#A855F7")}, QColor{QStringLiteral("#6F2CB3")}};
      return true;
    }
    if (accent == QStringLiteral("yellow")) {
      *out = {QColor{QStringLiteral("#B8862A")}, QColor{QStringLiteral("#D98A2B")}, QColor{QStringLiteral("#91671D")}};
      return true;
    }
  }
  return false;
}

bool draculaAccent(const QString& accent, ThemeSchemeKind scheme, AccentOverride* out) {
  if (scheme == ThemeSchemeKind::HoloNightDracula) {
    if (accent == QStringLiteral("cyan")) {
      *out = {QColor{QStringLiteral("#8BE9FD")}, QColor{QStringLiteral("#A4FFFF")}, QColor{QStringLiteral("#6FD3E7")}};
      return true;
    }
    if (accent == QStringLiteral("blue")) {
      *out = {QColor{QStringLiteral("#6272A4")}, QColor{QStringLiteral("#8292C4")}, QColor{QStringLiteral("#4C567A")}};
      return true;
    }
    if (accent == QStringLiteral("violet")) {
      *out = {QColor{QStringLiteral("#BD93F9")}, QColor{QStringLiteral("#CFAAFF")}, QColor{QStringLiteral("#A470ED")}};
      return true;
    }
    if (accent == QStringLiteral("yellow")) {
      *out = {QColor{QStringLiteral("#F1FA8C")}, QColor{QStringLiteral("#FFFFA5")}, QColor{QStringLiteral("#D5DE70")}};
      return true;
    }
  }

  if (scheme == ThemeSchemeKind::HoloNightAlucard) {
    if (accent == QStringLiteral("cyan")) {
      *out = {QColor{QStringLiteral("#036A96")}, QColor{QStringLiteral("#167FAB")}, QColor{QStringLiteral("#025477")}};
      return true;
    }
    if (accent == QStringLiteral("blue")) {
      *out = {QColor{QStringLiteral("#3454B4")}, QColor{QStringLiteral("#4A68C8")}, QColor{QStringLiteral("#284292")}};
      return true;
    }
    if (accent == QStringLiteral("violet")) {
      *out = {QColor{QStringLiteral("#644AC9")}, QColor{QStringLiteral("#765ED6")}, QColor{QStringLiteral("#5137B3")}};
      return true;
    }
    if (accent == QStringLiteral("yellow")) {
      *out = {QColor{QStringLiteral("#846E15")}, QColor{QStringLiteral("#9B8326")}, QColor{QStringLiteral("#69570F")}};
      return true;
    }
  }

  return false;
}

bool tokenAccent(const QString& accent, const ColorTokens& tok, AccentOverride* out) {
  QColor primary;
  if (accent == QStringLiteral("cyan")) {
    primary = tok.accentCyan;
  } else if (accent == QStringLiteral("blue")) {
    primary = tok.accentBlue;
  } else if (accent == QStringLiteral("violet")) {
    primary = tok.accentViolet;
  } else if (accent == QStringLiteral("yellow")) {
    primary = tok.accentYellow;
  } else {
    return false;
  }
  *out = {primary, primary.lighter(115), primary.darker(115)};
  return true;
}

void applyAccentOverride(ColorTokens& tok, const AccentOverride& colors) {
  tok.primary = colors.primary;
  tok.primaryHover = colors.hover;
  tok.primaryPressed = colors.pressed;
  tok.borderFocus = colors.primary;
  tok.borderActive = colors.primary;
  tok.focusRing = colors.primary;
  tok.focusRing.setAlpha(0x55);
  tok.glowCyanSoft = colors.primary;
  tok.glowCyanSoft.setAlpha(0x22);
  tok.glowBlueSoft = colors.primary;
  tok.glowBlueSoft.setAlpha(0x18);
  tok.glowVioletSoft = colors.primary;
  tok.glowVioletSoft.setAlpha(0x12);
}

}  // namespace

void ThemeResolver::applyAccent(ColorTokens& tok, const QString& accent, ThemeSchemeKind scheme) {
  if (accent == defaultAccentId()) {
    return;
  }

  AccentOverride overrideColors;
  if (catppuccinAccent(accent, scheme, &overrideColors)) {
    applyAccentOverride(tok, overrideColors);
    return;
  }
  if (gruvboxAccent(accent, scheme, &overrideColors)) {
    applyAccentOverride(tok, overrideColors);
    return;
  }
  if (cyberAccent(accent, scheme, &overrideColors)) {
    applyAccentOverride(tok, overrideColors);
    return;
  }
  if (draculaAccent(accent, scheme, &overrideColors)) {
    applyAccentOverride(tok, overrideColors);
    return;
  }

  if (tokenAccent(accent, tok, &overrideColors)) {
    applyAccentOverride(tok, overrideColors);
  }
}

ColorTokens ThemeResolver::resolve(const ThemeConfig& config) {
  const ThemeSchemeKind scheme = config.resolvedThemeScheme();
  ColorTokens tok = resolveBase(scheme);
  applyAccent(tok, config.resolvedAccent(), scheme);
  tok.textAccent = tok.primary;
  return tok;
}

}  // namespace Holonight
