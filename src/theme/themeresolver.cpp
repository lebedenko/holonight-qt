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
      *out = {
          .primary = QColor{QStringLiteral("#89DCEB")},
          .hover = QColor{QStringLiteral("#94E2D5")},
          .pressed = QColor{QStringLiteral("#74C7EC")},
      };
      return true;
    }
    if (accent == QStringLiteral("blue")) {
      *out = {
          .primary = QColor{QStringLiteral("#89B4FA")},
          .hover = QColor{QStringLiteral("#89DCEB")},
          .pressed = QColor{QStringLiteral("#74C7EC")},
      };
      return true;
    }
    if (accent == QStringLiteral("violet")) {
      *out = {
          .primary = QColor{QStringLiteral("#CBA6F7")},
          .hover = QColor{QStringLiteral("#F5C2E7")},
          .pressed = QColor{QStringLiteral("#B4BEFE")},
      };
      return true;
    }
    if (accent == QStringLiteral("yellow")) {
      *out = {
          .primary = QColor{QStringLiteral("#F9E2AF")},
          .hover = QColor{QStringLiteral("#FAB387")},
          .pressed = QColor{QStringLiteral("#EBA0AC")},
      };
      return true;
    }
  }

  if (scheme == ThemeSchemeKind::HoloNightLatte) {
    if (accent == QStringLiteral("cyan")) {
      *out = {
          .primary = QColor{QStringLiteral("#04A5E5")},
          .hover = QColor{QStringLiteral("#179299")},
          .pressed = QColor{QStringLiteral("#209FB5")},
      };
      return true;
    }
    if (accent == QStringLiteral("blue")) {
      *out = {
          .primary = QColor{QStringLiteral("#1E66F5")},
          .hover = QColor{QStringLiteral("#04A5E5")},
          .pressed = QColor{QStringLiteral("#209FB5")},
      };
      return true;
    }
    if (accent == QStringLiteral("violet")) {
      *out = {
          .primary = QColor{QStringLiteral("#8839EF")},
          .hover = QColor{QStringLiteral("#EA76CB")},
          .pressed = QColor{QStringLiteral("#7287FD")},
      };
      return true;
    }
    if (accent == QStringLiteral("yellow")) {
      *out = {
          .primary = QColor{QStringLiteral("#DF8E1D")},
          .hover = QColor{QStringLiteral("#FE640B")},
          .pressed = QColor{QStringLiteral("#E64553")},
      };
      return true;
    }
  }

  return false;
}

bool gruvboxAccent(const QString& accent, ThemeSchemeKind scheme, AccentOverride* out) {
  if (scheme == ThemeSchemeKind::HoloNightEmber) {
    if (accent == QStringLiteral("cyan")) {
      *out = {
          .primary = QColor{QStringLiteral("#8ec07c")},
          .hover = QColor{QStringLiteral("#a8d3c5")},
          .pressed = QColor{QStringLiteral("#83a598")},
      };
      return true;
    }
    if (accent == QStringLiteral("blue")) {
      *out = {
          .primary = QColor{QStringLiteral("#83a598")},
          .hover = QColor{QStringLiteral("#a8d3c5")},
          .pressed = QColor{QStringLiteral("#95c3b1")},
      };
      return true;
    }
    if (accent == QStringLiteral("violet")) {
      *out = {
          .primary = QColor{QStringLiteral("#d3869b")},
          .hover = QColor{QStringLiteral("#e5b3c3")},
          .pressed = QColor{QStringLiteral("#c1728a")},
      };
      return true;
    }
    if (accent == QStringLiteral("yellow")) {
      *out = {
          .primary = QColor{QStringLiteral("#fabd2f")},
          .hover = QColor{QStringLiteral("#fcd268")},
          .pressed = QColor{QStringLiteral("#e5aa20")},
      };
      return true;
    }
  }

  if (scheme == ThemeSchemeKind::HoloNightSol) {
    if (accent == QStringLiteral("cyan")) {
      *out = {
          .primary = QColor{QStringLiteral("#427b58")},
          .hover = QColor{QStringLiteral("#689d6a")},
          .pressed = QColor{QStringLiteral("#2d5c3f")},
      };
      return true;
    }
    if (accent == QStringLiteral("blue")) {
      *out = {
          .primary = QColor{QStringLiteral("#076678")},
          .hover = QColor{QStringLiteral("#458588")},
          .pressed = QColor{QStringLiteral("#054955")},
      };
      return true;
    }
    if (accent == QStringLiteral("violet")) {
      *out = {
          .primary = QColor{QStringLiteral("#8f3f71")},
          .hover = QColor{QStringLiteral("#b16286")},
          .pressed = QColor{QStringLiteral("#6b2d52")},
      };
      return true;
    }
    if (accent == QStringLiteral("yellow")) {
      *out = {
          .primary = QColor{QStringLiteral("#b57614")},
          .hover = QColor{QStringLiteral("#d79921")},
          .pressed = QColor{QStringLiteral("#8c580c")},
      };
      return true;
    }
  }

  return false;
}

bool cyberAccent(const QString& accent, ThemeSchemeKind scheme, AccentOverride* out) {
  if (scheme == ThemeSchemeKind::HoloNightCyberD) {
    if (accent == QStringLiteral("cyan")) {
      *out = {
          .primary = QColor{QStringLiteral("#39D5FF")},
          .hover = QColor{QStringLiteral("#6BE4FF")},
          .pressed = QColor{QStringLiteral("#1FAEDB")},
      };
      return true;
    }
    if (accent == QStringLiteral("blue")) {
      *out = {
          .primary = QColor{QStringLiteral("#6B4FE8")},
          .hover = QColor{QStringLiteral("#8470FF")},
          .pressed = QColor{QStringLiteral("#533CBF")},
      };
      return true;
    }
    if (accent == QStringLiteral("violet")) {
      *out = {
          .primary = QColor{QStringLiteral("#B26CFF")},
          .hover = QColor{QStringLiteral("#CB9BFF")},
          .pressed = QColor{QStringLiteral("#944DFF")},
      };
      return true;
    }
    if (accent == QStringLiteral("yellow")) {
      *out = {
          .primary = QColor{QStringLiteral("#F4C56B")},
          .hover = QColor{QStringLiteral("#FFD98A")},
          .pressed = QColor{QStringLiteral("#D6A34A")},
      };
      return true;
    }
  }
  if (scheme == ThemeSchemeKind::HoloNightCyberL) {
    if (accent == QStringLiteral("cyan")) {
      *out = {
          .primary = QColor{QStringLiteral("#0E9BD6")},
          .hover = QColor{QStringLiteral("#12B4F2")},
          .pressed = QColor{QStringLiteral("#0A7AAA")},
      };
      return true;
    }
    if (accent == QStringLiteral("blue")) {
      *out = {
          .primary = QColor{QStringLiteral("#5538D6")},
          .hover = QColor{QStringLiteral("#6B4FE8")},
          .pressed = QColor{QStringLiteral("#3F25B5")},
      };
      return true;
    }
    if (accent == QStringLiteral("violet")) {
      *out = {
          .primary = QColor{QStringLiteral("#8B3FE0")},
          .hover = QColor{QStringLiteral("#A855F7")},
          .pressed = QColor{QStringLiteral("#6F2CB3")},
      };
      return true;
    }
    if (accent == QStringLiteral("yellow")) {
      *out = {
          .primary = QColor{QStringLiteral("#B8862A")},
          .hover = QColor{QStringLiteral("#D98A2B")},
          .pressed = QColor{QStringLiteral("#91671D")},
      };
      return true;
    }
  }
  return false;
}

bool draculaAccent(const QString& accent, ThemeSchemeKind scheme, AccentOverride* out) {
  if (scheme == ThemeSchemeKind::HoloNightDracula) {
    if (accent == QStringLiteral("cyan")) {
      *out = {
          .primary = QColor{QStringLiteral("#8BE9FD")},
          .hover = QColor{QStringLiteral("#A4FFFF")},
          .pressed = QColor{QStringLiteral("#6FD3E7")},
      };
      return true;
    }
    if (accent == QStringLiteral("blue")) {
      *out = {
          .primary = QColor{QStringLiteral("#6272A4")},
          .hover = QColor{QStringLiteral("#8292C4")},
          .pressed = QColor{QStringLiteral("#4C567A")},
      };
      return true;
    }
    if (accent == QStringLiteral("violet")) {
      *out = {
          .primary = QColor{QStringLiteral("#BD93F9")},
          .hover = QColor{QStringLiteral("#CFAAFF")},
          .pressed = QColor{QStringLiteral("#A470ED")},
      };
      return true;
    }
    if (accent == QStringLiteral("yellow")) {
      *out = {
          .primary = QColor{QStringLiteral("#F1FA8C")},
          .hover = QColor{QStringLiteral("#FFFFA5")},
          .pressed = QColor{QStringLiteral("#D5DE70")},
      };
      return true;
    }
  }

  if (scheme == ThemeSchemeKind::HoloNightAlucard) {
    if (accent == QStringLiteral("cyan")) {
      *out = {
          .primary = QColor{QStringLiteral("#036A96")},
          .hover = QColor{QStringLiteral("#167FAB")},
          .pressed = QColor{QStringLiteral("#025477")},
      };
      return true;
    }
    if (accent == QStringLiteral("blue")) {
      *out = {
          .primary = QColor{QStringLiteral("#3454B4")},
          .hover = QColor{QStringLiteral("#4A68C8")},
          .pressed = QColor{QStringLiteral("#284292")},
      };
      return true;
    }
    if (accent == QStringLiteral("violet")) {
      *out = {
          .primary = QColor{QStringLiteral("#644AC9")},
          .hover = QColor{QStringLiteral("#765ED6")},
          .pressed = QColor{QStringLiteral("#5137B3")},
      };
      return true;
    }
    if (accent == QStringLiteral("yellow")) {
      *out = {
          .primary = QColor{QStringLiteral("#846E15")},
          .hover = QColor{QStringLiteral("#9B8326")},
          .pressed = QColor{QStringLiteral("#69570F")},
      };
      return true;
    }
  }

  return false;
}

bool nordAccent(const QString& accent, ThemeSchemeKind scheme, AccentOverride* out) {
  if (scheme == ThemeSchemeKind::HoloNightFrost) {
    if (accent == QStringLiteral("cyan")) {
      *out = {
          .primary = QColor{QStringLiteral("#8FBCBB")},
          .hover = QColor{QStringLiteral("#88C0D0")},
          .pressed = QColor{QStringLiteral("#7AA8A7")},
      };
      return true;
    }
    if (accent == QStringLiteral("blue")) {
      *out = {
          .primary = QColor{QStringLiteral("#81A1C1")},
          .hover = QColor{QStringLiteral("#88C0D0")},
          .pressed = QColor{QStringLiteral("#6D8EAF")},
      };
      return true;
    }
    if (accent == QStringLiteral("violet")) {
      *out = {
          .primary = QColor{QStringLiteral("#B48EAD")},
          .hover = QColor{QStringLiteral("#C5A2BF")},
          .pressed = QColor{QStringLiteral("#9E7998")},
      };
      return true;
    }
    if (accent == QStringLiteral("yellow")) {
      *out = {
          .primary = QColor{QStringLiteral("#EBCB8B")},
          .hover = QColor{QStringLiteral("#F2D9A6")},
          .pressed = QColor{QStringLiteral("#D4B371")},
      };
      return true;
    }
  }
  if (scheme == ThemeSchemeKind::HoloNightSnow) {
    if (accent == QStringLiteral("cyan")) {
      *out = {
          .primary = QColor{QStringLiteral("#315E7A")},
          .hover = QColor{QStringLiteral("#3C718F")},
          .pressed = QColor{QStringLiteral("#274B62")},
      };
      return true;
    }
    if (accent == QStringLiteral("blue")) {
      *out = {
          .primary = QColor{QStringLiteral("#3F5F85")},
          .hover = QColor{QStringLiteral("#4C719B")},
          .pressed = QColor{QStringLiteral("#354F70")},
      };
      return true;
    }
    if (accent == QStringLiteral("violet")) {
      *out = {
          .primary = QColor{QStringLiteral("#5B4675")},
          .hover = QColor{QStringLiteral("#71598D")},
          .pressed = QColor{QStringLiteral("#48375D")},
      };
      return true;
    }
    if (accent == QStringLiteral("yellow")) {
      *out = {
          .primary = QColor{QStringLiteral("#78620F")},
          .hover = QColor{QStringLiteral("#90781C")},
          .pressed = QColor{QStringLiteral("#604E0C")},
      };
      return true;
    }
  }
  return false;
}

bool everforestAccent(const QString& accent, ThemeSchemeKind scheme, AccentOverride* out) {
  if (scheme == ThemeSchemeKind::HoloNightCanopy) {
    if (accent == QStringLiteral("cyan")) {
      *out = {
          .primary = QColor{QStringLiteral("#83C092")},
          .hover = QColor{QStringLiteral("#9DCEAA")},
          .pressed = QColor{QStringLiteral("#6DA77C")},
      };
      return true;
    }
    if (accent == QStringLiteral("blue")) {
      *out = {
          .primary = QColor{QStringLiteral("#7FBBB3")},
          .hover = QColor{QStringLiteral("#9CCBC5")},
          .pressed = QColor{QStringLiteral("#68A19A")},
      };
      return true;
    }
    if (accent == QStringLiteral("violet")) {
      *out = {
          .primary = QColor{QStringLiteral("#D699B6")},
          .hover = QColor{QStringLiteral("#E0AFC7")},
          .pressed = QColor{QStringLiteral("#BE819F")},
      };
      return true;
    }
    if (accent == QStringLiteral("yellow")) {
      *out = {
          .primary = QColor{QStringLiteral("#DBBC7F")},
          .hover = QColor{QStringLiteral("#E4CC9A")},
          .pressed = QColor{QStringLiteral("#C3A366")},
      };
      return true;
    }
  }
  if (scheme == ThemeSchemeKind::HoloNightGlade) {
    if (accent == QStringLiteral("cyan")) {
      *out = {
          .primary = QColor{QStringLiteral("#2F6F61")},
          .hover = QColor{QStringLiteral("#398474")},
          .pressed = QColor{QStringLiteral("#26594E")},
      };
      return true;
    }
    if (accent == QStringLiteral("blue")) {
      *out = {
          .primary = QColor{QStringLiteral("#315E7A")},
          .hover = QColor{QStringLiteral("#3A7191")},
          .pressed = QColor{QStringLiteral("#274B62")},
      };
      return true;
    }
    if (accent == QStringLiteral("violet")) {
      *out = {
          .primary = QColor{QStringLiteral("#6A4570")},
          .hover = QColor{QStringLiteral("#805687")},
          .pressed = QColor{QStringLiteral("#55375A")},
      };
      return true;
    }
    if (accent == QStringLiteral("yellow")) {
      *out = {
          .primary = QColor{QStringLiteral("#8A4D18")},
          .hover = QColor{QStringLiteral("#A55F24")},
          .pressed = QColor{QStringLiteral("#6E3D13")},
      };
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
  *out = {.primary = primary, .hover = primary.lighter(115), .pressed = primary.darker(115)};
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
  if (nordAccent(accent, scheme, &overrideColors)) {
    applyAccentOverride(tok, overrideColors);
    return;
  }
  if (everforestAccent(accent, scheme, &overrideColors)) {
    applyAccentOverride(tok, overrideColors);
    return;
  }

  if (tokenAccent(accent, tok, &overrideColors)) {
    applyAccentOverride(tok, overrideColors);
  }
}

ColorTokens ThemeResolver::resolve(const ResolvedAppearance& appearance) {
  ColorTokens tok = resolveBase(appearance.theme_scheme);
  applyAccent(tok, appearance.accent, appearance.theme_scheme);
  tok.textAccent = tok.primary;
  return tok;
}

}  // namespace Holonight
