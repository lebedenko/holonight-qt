// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "holonighttheme.h"

#include "holonight/theme_catalog.h"

#include <QDir>
#include <QFileInfo>
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

HolonightTheme::HolonightTheme(QObject* parent) : QObject{parent}, config_{Holonight::ThemeConfig::load()} {
  connect(&watcher_, &QFileSystemWatcher::fileChanged, this, [this] { reload(); });
  connect(&watcher_, &QFileSystemWatcher::directoryChanged, this, [this] { reload(); });
  rearmWatcher();
}

void HolonightTheme::reload() {
  const Holonight::ThemeConfig next = Holonight::ThemeConfig::load();
  rearmWatcher();
  if (next.icon_theme == config_.icon_theme && next.fallback_icon_theme == config_.fallback_icon_theme &&
      next.ui_font == config_.ui_font && next.fixed_font == config_.fixed_font &&
      next.header_font == config_.header_font && next.display_font == config_.display_font &&
      next.base_font_size == config_.base_font_size && next.fixed_font_size == config_.fixed_font_size &&
      next.scale_factor == config_.scale_factor) {
    return;
  }
  config_ = next;
  emit themeChanged();
}

void HolonightTheme::rearmWatcher() {
  const QString path = Holonight::ThemeConfig::configFilePath();
  QString directory = QFileInfo{path}.absolutePath();
  while (!QFileInfo{directory}.exists()) {
    const QString parent = QFileInfo{directory}.absolutePath();
    if (parent == directory) {
      break;
    }
    directory = parent;
  }

  const QStringList watched = watcher_.files() + watcher_.directories();
  if (!watched.isEmpty()) {
    watcher_.removePaths(watched);
  }
  if (QFileInfo{directory}.isDir()) {
    watcher_.addPath(directory);
  }
  if (QFileInfo{path}.isFile()) {
    watcher_.addPath(path);
  }
}

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
