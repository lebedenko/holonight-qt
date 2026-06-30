# HoloNight Qt Theme Variants Design

## Current Shape

`holonight-qt` already has the pieces needed for this change:

- `ThemeConfig` stores `appearance_mode`, `scheme`, and `accent`.
- `ThemeConfig::load()` reads config from `theme.conf` and legacy files.
- `ThemeResolver::resolve()` maps config to `ColorTokens`.
- `HoloniightTheme` exposes `QPlatformTheme::colorScheme()`.
- `HoloniightPalette` reloads resolved tokens for QML consumers.

The change is mostly a resolution policy change, not a new architecture.

## Proposed Model

Add a small scheme catalog near the theme resolver/config layer:

```cpp
enum class ThemeSchemeKind {
  HoloNightDark,
  HoloNightLight,
  TokyoNightStorm,
  TokyoNightDay,
};
```

The exact API can be simpler if it fits the existing code better, but the behavior should be centralized so config loading, token resolution, and platform theme color-scheme all agree.

Useful helpers:

- `QString normalizeScheme(QStringView value)`
- `ThemeSchemeKind resolveScheme(const ThemeConfig& config)`
- `ColorMode colorModeForScheme(ThemeSchemeKind scheme)`
- `QString derivedModeForScheme(ThemeSchemeKind scheme)`

## Config Loading

`ThemeConfig::load()` should continue to parse:

- INI `appearance/mode`
- INI `appearance/scheme`
- INI `appearance/accent`
- legacy JSON appearance values
- supported environment overrides

The key behavior change is that callers must resolve `scheme` first. `appearance_mode` remains present on `ThemeConfig` for compatibility and legacy fallback, but it is no longer the primary decision for palette or platform color-scheme.

If environment overrides currently only target mode, keep them as legacy input. A future `HOLONIGHT_SCHEME` override can be added separately if useful.

## ThemeResolver

`ThemeResolver::resolve(config)` should:

1. Resolve the scheme from `config.scheme`, falling back from `config.appearance_mode`.
2. Select the base tokens from the resolved scheme.
3. Apply normalized accent overrides.
4. Return the complete token set.

Initial mapping:

- `HoloNightDark` -> current dark tokens.
- `TokyoNightStorm` -> current dark tokens.
- `HoloNightLight` -> current light tokens.
- `TokyoNightDay` -> current light tokens.

This deliberately avoids duplicating token values in this pass.

## Platform Theme

`HoloniightTheme` should store the resolved scheme or resolved color mode, not raw `appearance_mode`.

`colorScheme()` should map from resolved scheme:

```cpp
return colorModeForScheme(resolved_scheme_) == ColorMode::Light
    ? Qt::ColorScheme::Light
    : Qt::ColorScheme::Dark;
```

This makes Qt application system appearance match the selected theme variant even when legacy `mode` is stale or missing.

## Palette Reload

`HoloniightPalette::reload()` should continue to call through the loader/resolver path. No QML-side scheme logic is needed.

Because the shell already emits `paletteReloadRequested()` when `theme.conf` changes, QML consumers should pick up scheme/accent changes through the existing reload bridge.

## Compatibility

Legacy configs remain accepted:

```ini
[appearance]
mode=light
```

This resolves to `holonight-light`.

Configs written by the shell contain all fields:

```ini
[appearance]
scheme=tokyonight-day
accent=violet
mode=light
```

This resolves from `scheme`; `mode` is ignored except as metadata.
