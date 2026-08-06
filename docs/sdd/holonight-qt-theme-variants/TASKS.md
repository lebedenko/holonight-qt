# HoloNight Qt Theme Variants Tasks

## Config

- [x] Add centralized scheme validation for `holonight-dark`, `holonight-light`, `tokyonight-storm`, and `tokyonight-day`.
- [x] Resolve `ThemeConfig::scheme` before falling back to legacy `ThemeConfig::appearance_mode`.
- [x] Treat invalid or missing accent as `cyan`.
- [x] Keep parsing legacy `mode` for backward compatibility.
- [x] Add config tests for valid scheme IDs.
- [x] Add config tests for invalid/missing scheme fallback from legacy mode.
- [x] Add config tests proving `mode=system` resolves to the dark default when scheme is missing.

## Resolver

- [x] Add scheme-first resolution in `ThemeResolver`.
- [x] Map `holonight-dark` and `tokyonight-storm` to current dark tokens.
- [x] Map `holonight-light` and `tokyonight-day` to current light tokens.
- [x] Preserve existing accent-controlled token slots.
- [x] Add resolver tests for all four scheme IDs.
- [x] Add regression tests that TokyoNight dark/light values remain available under the new names.
- [x] Add tests proving radius, metrics, and typography/font defaults remain identical across schemes.

## Platform Theme

- [x] Make `HoloniightTheme::colorScheme()` derive from resolved scheme rather than raw mode.
- [x] Add platform theme tests for dark scheme IDs returning `Qt::ColorScheme::Dark`.
- [x] Add platform theme tests for light scheme IDs returning `Qt::ColorScheme::Light`.
- [x] Add a regression test for stale/mismatched `mode` not overriding a valid `scheme`.

## Palette/QML

- [x] Ensure `HoloniightPalette::reload()` continues to resolve through `ThemeResolver`.
- [x] Add palette token tests for documented HoloNight dark/light values.
- [x] Confirm accent changes update primary/focus/glow token slots.
- [x] Confirm no QML-side mode branching is required.

## Docs

- [x] Update `holonight-qt` README config examples to prefer `scheme` and `accent`.
- [x] Mark `mode` as derived legacy metadata in theme token docs.
- [x] Document the four scheme IDs and their current token aliases.
