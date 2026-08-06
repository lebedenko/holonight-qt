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
      {.id = QStringLiteral("holonight-storm"),
       .name = QStringLiteral("HoloNight Storm"),
       .family_id = QStringLiteral("tokyonight"),
       .scheme = Holonight::ThemeSchemeKind::HoloNightStorm,
       .mode = Holonight::ColorMode::Dark,
       .default_accent_color = defaultAccentColor(Holonight::ThemeSchemeKind::HoloNightStorm),
       .kde_color_scheme_name = QStringLiteral("HoloNight Storm")},
      {.id = QStringLiteral("holonight-day"),
       .name = QStringLiteral("HoloNight Day"),
       .family_id = QStringLiteral("tokyonight"),
       .scheme = Holonight::ThemeSchemeKind::HoloNightDay,
       .mode = Holonight::ColorMode::Light,
       .default_accent_color = defaultAccentColor(Holonight::ThemeSchemeKind::HoloNightDay),
       .kde_color_scheme_name = QStringLiteral("HoloNight Day")},
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
      {.id = QStringLiteral("holonight-dracula"),
       .name = QStringLiteral("HoloNight Dracula"),
       .family_id = QStringLiteral("dracula"),
       .scheme = Holonight::ThemeSchemeKind::HoloNightDracula,
       .mode = Holonight::ColorMode::Dark,
       .default_accent_color = defaultAccentColor(Holonight::ThemeSchemeKind::HoloNightDracula),
       .kde_color_scheme_name = QStringLiteral("HoloNight Dracula")},
      {.id = QStringLiteral("holonight-alucard"),
       .name = QStringLiteral("HoloNight Alucard"),
       .family_id = QStringLiteral("dracula"),
       .scheme = Holonight::ThemeSchemeKind::HoloNightAlucard,
       .mode = Holonight::ColorMode::Light,
       .default_accent_color = defaultAccentColor(Holonight::ThemeSchemeKind::HoloNightAlucard),
       .kde_color_scheme_name = QStringLiteral("HoloNight Alucard")},
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
       .variant_ids = {QStringLiteral("holonight-storm"), QStringLiteral("holonight-day")}},
      {.id = QStringLiteral("gruvbox"),
       .name = QStringLiteral("Gruvbox"),
       .variant_ids = {QStringLiteral("holonight-ember"), QStringLiteral("holonight-sol")}},
      {.id = QStringLiteral("cyber"),
       .name = QStringLiteral("Cyber"),
       .variant_ids = {QStringLiteral("holonight-cyber-d"), QStringLiteral("holonight-cyber-l")}},
      {.id = QStringLiteral("dracula"),
       .name = QStringLiteral("Dracula"),
       .variant_ids = {QStringLiteral("holonight-dracula"), QStringLiteral("holonight-alucard")}},
  };
  return entries;
}

const QVector<ThemeVariantCatalogEntry>& themeVariants() { return variants(); }

QVector<AccentCatalogEntry> accentOptionsForScheme(const QString& scheme_id) {
  const ThemeSchemeKind scheme = schemeKindForSchemeId(scheme_id);
  const ColorTokens tokens = tokensForScheme(scheme);
  return {
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

  if (compact == QStringLiteral("holonight") || compact == QStringLiteral("tokyonightstorm")) {
    return QStringLiteral("holonight-storm");
  }
  if (compact == QStringLiteral("holonightday") || compact == QStringLiteral("tokyonightday")) {
    return QStringLiteral("holonight-day");
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
  if (normalized == defaultAccentId()) {
    const ThemeSchemeKind scheme = schemeKindForSchemeId(scheme_id);
    return tokensForScheme(scheme).primary;
  }
  for (const AccentCatalogEntry& accent : accentOptionsForScheme(scheme_id)) {
    if (accent.id == normalized) {
      return accent.color;
    }
  }
  Q_UNREACHABLE_RETURN({});
}

}  // namespace Holonight
