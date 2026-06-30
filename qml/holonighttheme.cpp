// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "holonighttheme.h"

#include "holonight/theme_catalog.h"

#include <QVariantMap>

namespace {

[[nodiscard]] QString modeName(Holonight::ColorMode mode) {
  return mode == Holonight::ColorMode::Light ? QStringLiteral("light") : QStringLiteral("dark");
}

[[nodiscard]] QVariantMap variantMap(const Holonight::ThemeVariantCatalogEntry& variant) {
  return {
      {QStringLiteral("id"), variant.id},
      {QStringLiteral("name"), variant.name},
      {QStringLiteral("familyId"), variant.family_id},
      {QStringLiteral("mode"), modeName(variant.mode)},
      {QStringLiteral("defaultAccentColor"), variant.default_accent_color},
  };
}

}  // namespace

HolonightTheme::HolonightTheme(QObject* parent) : QObject{parent}, config_{Holonight::ThemeConfig::load()} {}

QVariantList HolonightTheme::themeFamilies() const {
  QVariantList result;
  for (const Holonight::ThemeFamilyCatalogEntry& family : Holonight::themeFamilies()) {
    QVariantList variants;
    for (const QString& variant_id : family.variant_ids) {
      const Holonight::ThemeVariantCatalogEntry* variant = Holonight::themeVariantForSchemeId(variant_id);
      if (variant != nullptr) {
        variants.append(variantMap(*variant));
      }
    }
    result.append(QVariantMap{
        {QStringLiteral("id"), family.id},
        {QStringLiteral("name"), family.name},
        {QStringLiteral("variants"), variants},
    });
  }
  return result;
}

QVariantList HolonightTheme::themeVariants() const {
  QVariantList result;
  for (const Holonight::ThemeVariantCatalogEntry& variant : Holonight::themeVariants()) {
    result.append(variantMap(variant));
  }
  return result;
}

QVariantList HolonightTheme::accentOptionsForScheme(const QString& scheme_id) const {
  QVariantList result;
  for (const Holonight::AccentCatalogEntry& accent : Holonight::accentOptionsForScheme(scheme_id)) {
    result.append(QVariantMap{
        {QStringLiteral("id"), accent.id},
        {QStringLiteral("name"), accent.name},
        {QStringLiteral("color"), accent.color},
        {QStringLiteral("isDefault"), accent.is_default},
    });
  }
  return result;
}
