// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "holonight/theme_catalog.h"

namespace {

[[nodiscard]] QString normalizedString(const QString& value) { return value.trimmed().toLower(); }

[[nodiscard]] QString compactThemeName(const QString& value) {
  QString normalized = normalizedString(value);
  normalized.remove(QLatin1Char(' '));
  normalized.remove(QLatin1Char('-'));
  normalized.remove(QLatin1Char('_'));
  return normalized;
}

[[nodiscard]] QColor defaultAccentColor(Holonight::ThemeSchemeKind scheme) {
  return Holonight::tokensForScheme(scheme).primary;
}

[[nodiscard]] const QVector<Holonight::ThemeVariantCatalogEntry>& variants() {
  static const QVector<Holonight::ThemeVariantCatalogEntry> entries{
      {.id = QStringLiteral("holonight-dark"),
       .name = QStringLiteral("HoloNight Dark"),
       .family_id = QStringLiteral("holonight"),
       .scheme = Holonight::ThemeSchemeKind::HoloNightDark,
       .mode = Holonight::ColorMode::Dark,
       .default_accent_color = defaultAccentColor(Holonight::ThemeSchemeKind::HoloNightDark),
       .kde_color_scheme_name = QStringLiteral("HoloNight Dark")},
      {.id = QStringLiteral("holonight-light"),
       .name = QStringLiteral("HoloNight Light"),
       .family_id = QStringLiteral("holonight"),
       .scheme = Holonight::ThemeSchemeKind::HoloNightLight,
       .mode = Holonight::ColorMode::Light,
       .default_accent_color = defaultAccentColor(Holonight::ThemeSchemeKind::HoloNightLight),
       .kde_color_scheme_name = QStringLiteral("HoloNight Light")},
      {.id = QStringLiteral("holonight-mocha"),
       .name = QStringLiteral("HoloNight Mocha"),
       .family_id = QStringLiteral("catppuccin"),
       .scheme = Holonight::ThemeSchemeKind::HoloNightMocha,
       .mode = Holonight::ColorMode::Dark,
       .default_accent_color = defaultAccentColor(Holonight::ThemeSchemeKind::HoloNightMocha),
       .kde_color_scheme_name = QStringLiteral("HoloNight Mocha")},
      {.id = QStringLiteral("holonight-latte"),
       .name = QStringLiteral("HoloNight Latte"),
       .family_id = QStringLiteral("catppuccin"),
       .scheme = Holonight::ThemeSchemeKind::HoloNightLatte,
       .mode = Holonight::ColorMode::Light,
       .default_accent_color = defaultAccentColor(Holonight::ThemeSchemeKind::HoloNightLatte),
       .kde_color_scheme_name = QStringLiteral("HoloNight Latte")},
      {.id = QStringLiteral("tokyonight-storm"),
       .name = QStringLiteral("TokyoNight Storm"),
       .family_id = QStringLiteral("tokyonight"),
       .scheme = Holonight::ThemeSchemeKind::TokyoNightStorm,
       .mode = Holonight::ColorMode::Dark,
       .default_accent_color = defaultAccentColor(Holonight::ThemeSchemeKind::TokyoNightStorm),
       .kde_color_scheme_name = QStringLiteral("TokyoNight Storm")},
      {.id = QStringLiteral("tokyonight-day"),
       .name = QStringLiteral("TokyoNight Day"),
       .family_id = QStringLiteral("tokyonight"),
       .scheme = Holonight::ThemeSchemeKind::TokyoNightDay,
       .mode = Holonight::ColorMode::Light,
       .default_accent_color = defaultAccentColor(Holonight::ThemeSchemeKind::TokyoNightDay),
       .kde_color_scheme_name = QStringLiteral("TokyoNight Day")},
      {.id = QStringLiteral("holonight-ember"),
       .name = QStringLiteral("HoloNight Ember"),
       .family_id = QStringLiteral("gruvbox"),
       .scheme = Holonight::ThemeSchemeKind::HoloNightEmber,
       .mode = Holonight::ColorMode::Dark,
       .default_accent_color = defaultAccentColor(Holonight::ThemeSchemeKind::HoloNightEmber),
       .kde_color_scheme_name = QStringLiteral("HoloNight Ember")},
      {.id = QStringLiteral("holonight-sol"),
       .name = QStringLiteral("HoloNight Sol"),
       .family_id = QStringLiteral("gruvbox"),
       .scheme = Holonight::ThemeSchemeKind::HoloNightSol,
       .mode = Holonight::ColorMode::Light,
       .default_accent_color = defaultAccentColor(Holonight::ThemeSchemeKind::HoloNightSol),
       .kde_color_scheme_name = QStringLiteral("HoloNight Sol")},
      {.id = QStringLiteral("holonight-cyber-d"),
       .name = QStringLiteral("HoloNight Cyber D"),
       .family_id = QStringLiteral("cyber"),
       .scheme = Holonight::ThemeSchemeKind::HoloNightCyberD,
       .mode = Holonight::ColorMode::Dark,
       .default_accent_color = defaultAccentColor(Holonight::ThemeSchemeKind::HoloNightCyberD),
       .kde_color_scheme_name = QStringLiteral("HoloNight Cyber D")},
      {.id = QStringLiteral("holonight-cyber-l"),
       .name = QStringLiteral("HoloNight Cyber L"),
       .family_id = QStringLiteral("cyber"),
       .scheme = Holonight::ThemeSchemeKind::HoloNightCyberL,
       .mode = Holonight::ColorMode::Light,
       .default_accent_color = defaultAccentColor(Holonight::ThemeSchemeKind::HoloNightCyberL),
       .kde_color_scheme_name = QStringLiteral("HoloNight Cyber L")},
  };
  return entries;
}

}  // namespace

namespace Holonight {

const QVector<ThemeFamilyCatalogEntry>& themeFamilies() {
  static const QVector<ThemeFamilyCatalogEntry> entries{
      {.id = QStringLiteral("holonight"),
       .name = QStringLiteral("HoloNight"),
       .variant_ids = {QStringLiteral("holonight-dark"), QStringLiteral("holonight-light")}},
      {.id = QStringLiteral("catppuccin"),
       .name = QStringLiteral("Catppuccin"),
       .variant_ids = {QStringLiteral("holonight-mocha"), QStringLiteral("holonight-latte")}},
      {.id = QStringLiteral("tokyonight"),
       .name = QStringLiteral("TokyoNight"),
       .variant_ids = {QStringLiteral("tokyonight-storm"), QStringLiteral("tokyonight-day")}},
      {.id = QStringLiteral("gruvbox"),
       .name = QStringLiteral("Gruvbox"),
       .variant_ids = {QStringLiteral("holonight-ember"), QStringLiteral("holonight-sol")}},
      {.id = QStringLiteral("cyber"),
       .name = QStringLiteral("Cyber"),
       .variant_ids = {QStringLiteral("holonight-cyber-d"), QStringLiteral("holonight-cyber-l")}},
  };
  return entries;
}

const QVector<ThemeVariantCatalogEntry>& themeVariants() { return variants(); }

QVector<AccentCatalogEntry> accentOptionsForScheme(const QString& scheme_id) {
  const ThemeSchemeKind scheme = schemeKindForSchemeId(scheme_id);
  const ColorTokens tokens = tokensForScheme(scheme);
  return {
      {.id = defaultAccentId(), .name = QStringLiteral("Default"), .color = tokens.primary, .is_default = true},
      {.id = QStringLiteral("cyan"), .name = QStringLiteral("Cyan"), .color = tokens.accentCyan},
      {.id = QStringLiteral("blue"), .name = QStringLiteral("Blue"), .color = tokens.accentBlue},
      {.id = QStringLiteral("violet"), .name = QStringLiteral("Violet"), .color = tokens.accentViolet},
      {.id = QStringLiteral("yellow"), .name = QStringLiteral("Yellow"), .color = tokens.accentYellow},
  };
}

QString defaultSchemeId() { return QStringLiteral("holonight-dark"); }

QString defaultAccentId() { return QStringLiteral("default"); }

QString normalizeSchemeId(const QString& scheme_id) {
  const QString normalized = normalizedString(scheme_id);
  for (const ThemeVariantCatalogEntry& variant : variants()) {
    if (variant.id == normalized) {
      return normalized;
    }
  }
  return {};
}

QString normalizeAccentId(const QString& accent_id) {
  const QString normalized = normalizedString(accent_id);
  if (normalized == defaultAccentId() || normalized == QLatin1String("cyan") || normalized == QLatin1String("blue") ||
      normalized == QLatin1String("violet") || normalized == QLatin1String("yellow")) {
    return normalized;
  }
  return defaultAccentId();
}

QString schemeIdForKind(ThemeSchemeKind scheme) {
  for (const ThemeVariantCatalogEntry& variant : variants()) {
    if (variant.scheme == scheme) {
      return variant.id;
    }
  }
  return defaultSchemeId();
}

QString schemeIdForKdeColorSchemeName(const QString& kde_color_scheme_name) {
  const QString compact = compactThemeName(kde_color_scheme_name);
  for (const ThemeVariantCatalogEntry& variant : variants()) {
    if (compactThemeName(variant.kde_color_scheme_name) == compact) {
      return variant.id;
    }
  }

  if (compact == QStringLiteral("holonight")) {
    return QStringLiteral("tokyonight-storm");
  }
  if (compact == QStringLiteral("holonightday")) {
    return QStringLiteral("tokyonight-day");
  }

  return {};
}

QString modeNameForScheme(const QString& scheme_id) {
  const ThemeVariantCatalogEntry* variant = themeVariantForSchemeId(scheme_id);
  return variant != nullptr && variant->mode == ColorMode::Light ? QStringLiteral("light") : QStringLiteral("dark");
}

const ThemeVariantCatalogEntry* themeVariantForSchemeId(const QString& scheme_id) {
  const QString normalized = normalizeSchemeId(scheme_id);
  for (const ThemeVariantCatalogEntry& variant : variants()) {
    if (variant.id == normalized) {
      return &variant;
    }
  }
  return nullptr;
}

ThemeSchemeKind schemeKindForSchemeId(const QString& scheme_id) {
  const ThemeVariantCatalogEntry* variant = themeVariantForSchemeId(scheme_id);
  return variant != nullptr ? variant->scheme : ThemeSchemeKind::HoloNightDark;
}

QColor accentColorForScheme(const QString& scheme_id, const QString& accent_id) {
  const QString normalized = normalizeAccentId(accent_id);
  for (const AccentCatalogEntry& accent : accentOptionsForScheme(scheme_id)) {
    if (accent.id == normalized) {
      return accent.color;
    }
  }
  return accentOptionsForScheme(scheme_id).first().color;
}

}  // namespace Holonight
