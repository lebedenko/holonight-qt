// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#pragma once

#include "holonight/palette.h"

#include <QColor>
#include <QString>
#include <QVector>

namespace Holonight {

struct ThemeVariantCatalogEntry {
  QString id;
  QString name;
  QString family_id;
  ThemeSchemeKind scheme;
  ColorMode mode;
  QColor default_accent_color;
  QString kde_color_scheme_name;
};

struct ThemeFamilyCatalogEntry {
  QString id;
  QString name;
  QVector<QString> variant_ids;
};

struct AccentCatalogEntry {
  QString id;
  QString name;
  QColor color;
  bool is_default = false;
};

[[nodiscard]] const QVector<ThemeFamilyCatalogEntry>& themeFamilies();
[[nodiscard]] const QVector<ThemeVariantCatalogEntry>& themeVariants();
[[nodiscard]] QVector<AccentCatalogEntry> accentOptionsForScheme(const QString& scheme_id);

[[nodiscard]] QString defaultSchemeId();
[[nodiscard]] QString defaultAccentId();
[[nodiscard]] QString normalizeSchemeId(const QString& scheme_id);
[[nodiscard]] QString normalizeAccentId(const QString& accent_id);
[[nodiscard]] QString schemeIdForKind(ThemeSchemeKind scheme);
[[nodiscard]] QString schemeIdForKdeColorSchemeName(const QString& kde_color_scheme_name);
[[nodiscard]] QString modeNameForScheme(const QString& scheme_id);
[[nodiscard]] const ThemeVariantCatalogEntry* themeVariantForSchemeId(const QString& scheme_id);
[[nodiscard]] ThemeSchemeKind schemeKindForSchemeId(const QString& scheme_id);
[[nodiscard]] QColor accentColorForScheme(const QString& scheme_id, const QString& accent_id);

}  // namespace Holonight
