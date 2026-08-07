// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "holonighttheme.h"

#include "appearancecontext.h"
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

HolonightTheme::HolonightTheme(Holonight::AppearanceReader* reader, QObject* parent)
    : QObject{parent}, reader_{reader} {
  connect(reader_, &Holonight::AppearanceReader::themeChanged, this, &HolonightTheme::themeChanged);
}

HolonightTheme* HolonightTheme::create(QQmlEngine* engine, QJSEngine* script_engine) {
  Q_UNUSED(script_engine)
  return new HolonightTheme{Holonight::appearanceReaderForEngine(engine), engine};
}

void HolonightTheme::reload() { reader_->reload(); }

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

QColor HolonightTheme::accentColorForScheme(const QString& scheme_id, const QString& accent_id) const {
  return Holonight::accentColorForScheme(scheme_id, accent_id);
}
