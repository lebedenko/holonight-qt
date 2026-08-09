// SPDX-License-Identifier: GPL-3.0-or-later
#include "qt5probeappearance.h"

#include "holonight/theme_catalog.h"

#include <QByteArray>

#include <algorithm>
namespace Holonight {
int ResolvedAppearance::microSize() const { return (std::max)(6, ui_font_size - 1); }
int ResolvedAppearance::captionSize() const { return (std::max)(6, ui_font_size - 1); }
int ResolvedAppearance::bodySize() const { return ui_font_size; }
int ResolvedAppearance::subheadingSize() const { return (std::min)(48, ui_font_size + 1); }
int ResolvedAppearance::appTitleSize() const { return (std::min)(48, ui_font_size + 4); }
int ResolvedAppearance::headingSize() const { return (std::min)(48, ui_font_size + 6); }
ResolvedAppearance qt5ProbeAppearance() {
  QString scheme = normalizeSchemeId(QString::fromUtf8(qgetenv("HOLONIGHT_QT5_SCHEME")));
  if (scheme.isEmpty()) scheme = defaultSchemeId();
  const QString accent = normalizeAccentId(QString::fromUtf8(qgetenv("HOLONIGHT_QT5_ACCENT")));
  const ThemeSchemeKind kind = schemeKindForSchemeId(scheme);
  return {.scheme = scheme,
          .accent = accent,
          .theme_scheme = kind,
          .color_mode = colorModeForScheme(kind),
          .ui_font = QStringLiteral("Inter"),
          .ui_font_size = 12,
          .monospace_font = QStringLiteral("JetBrains Mono"),
          .monospace_font_size = 12,
          .title_font = QStringLiteral("Inter"),
          .title_font_size = 12,
          .display_font = QStringLiteral("Inter"),
          .display_font_size = 24,
          .icon_theme = QStringLiteral("holonight"),
          .fallback_icon_theme = QStringLiteral("hicolor")};
}
}  // namespace Holonight
