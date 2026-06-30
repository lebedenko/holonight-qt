# HoloNight Theme Tokens

The palette layer is the canonical source for resolved theme values. `Holonight::tokensForScheme(ThemeSchemeKind)` returns one of the built-in concrete schemes; all Qt palettes, widgets, QML controls, platform theme colors, and generated KDE color-scheme values should consume roles from `Holonight::ColorTokens`.

## Variant Status

| Variant | Status | Dark | Light | Notes |
| --- | --- | --- | --- | --- |
| HoloNight Dark / Light | Implemented | Yes | Yes | Default HoloNight schemes selected by legacy mode fallback. |
| HoloNight Mocha / Latte | Implemented | Yes | Yes | Catppuccin-based schemes selected by explicit scheme ID. |
| TokyoNight Storm / Day | Implemented | Yes | Yes | Bundled Tokyo Night inspired compatibility schemes. |
| HoloNight Ember / Sol | Implemented | Yes | Yes | Gruvbox-based high contrast schemes. |
| HoloNight Cyber D / L | Implemented | Yes | Yes | Cyberpunk-inspired neon schemes from palette.cpp. |
| Aurora | Planned | Planned | No | Nord inspired. Not parsed or exported yet. |
| Eclipse | Planned | Planned | No | Lower-chroma dark variant. Not parsed or exported yet. |
| Neon | Planned | Planned | No | Higher-accent dark variant. Not parsed or exported yet. |

Current scheme IDs are `holonight-dark`, `holonight-light`, `holonight-mocha`, `holonight-latte`,
`tokyonight-storm`, `tokyonight-day`, `holonight-ember`, `holonight-sol`, `holonight-cyber-d`, and `holonight-cyber-l`.
All current scheme IDs resolve to distinct built-in token sets.

## Canonical Roles

Surface roles:

| Token | Meaning |
| --- | --- |
| `background` | Main window/background plane. Maps to `QPalette::Window`. |
| `surface` | Base views, editor fields, and recessed control interiors. Maps to `QPalette::Base`. |
| `surfaceElevated` | Cards, alternate rows, and elevated containers. Maps to `QPalette::AlternateBase`. |
| `surfaceRaised` | Buttons, panels, menus, and popovers. Maps to `QPalette::Button`. |
| `surfaceHover` | Filled hover-state surface. |
| `surfaceInverse` | Tooltip and complementary inverse surface. Maps to `QPalette::ToolTipBase`. |

Text roles:

| Token | Meaning |
| --- | --- |
| `textPrimary` | Normal text/icons. |
| `textSecondary` | Secondary readable text. |
| `textMuted` | Placeholder and inactive text. |
| `textDisabled` | Disabled text; WCAG inactive-component exemptions apply. |
| `textInverse` | Text on `surfaceInverse`. |

Border roles:

| Token | Meaning |
| --- | --- |
| `borderSubtle` | Low-emphasis hairlines and decorative dividers. |
| `borderPassive` | Passive frames, separators, and inactive control borders. |
| `borderStrong` | Strong neutral border. |
| `borderHover` | Semi-transparent hover-state border overlay. |
| `borderActive` | Current, selected, or active border. |
| `borderFocus` | Keyboard focus only. |
| `borderUrgent` | Error, destructive, or urgent border. |

Accent/status roles:

| Token | Meaning |
| --- | --- |
| `primary` | Selection/link fill. Storm uses calmer blue `#7aa2f7`. |
| `accentCyan` | General cyan accent; `#00e0ff` is reserved for keyboard focus. |
| `accentBlue`, `accentViolet`, `accentYellow` | Secondary accent families. |
| `error`, `warning`, `success` | Semantic status colors. |

Overlay, radius, metric, and ANSI roles:

| Token group | Meaning |
| --- | --- |
| `glowCyanSoft`, `glowBlueSoft`, `glowVioletSoft` | Soft effect overlays. |
| `scrim`, `glassTint`, `hoverOverlay`, `pressedOverlay`, `disabledOverlay` | State and composition overlays with intentional alpha. |
| `radiusTight`, `radiusControl`, `radiusPopup`, `radiusPill` | Shared corner-radius tokens for style/QML controls. |
| `borderWidth`, `focusBorderWidth`, `separatorWidth`, `controlHeight`, `controlPadding` | Shared control metrics. |
| `ansiBlack` through `ansiBrightWhite` | Semantic ANSI terminal colors. `ansi0` through `ansi15` remain aliases. |

## Deprecated Aliases

The following fields remain populated for C++ and QML compatibility but should not be used by new code:

| Alias | Canonical replacement |
| --- | --- |
| `surfaceVariant` | `surface` |
| `surfaceContainer` | `surfaceElevated` |
| `secondary` | `surfaceRaised` |
| `onSurface` | `textPrimary` |
| `onSurfaceVariant` | `textMuted` |
| `onSurfaceDisabled` | `textDisabled` |
| `onSurfaceInverse` | `textInverse` |
| `onSecondary` | `textPrimary` |
| `outline` | `borderActive` or another semantic border role |
| `outlineVariant` | `borderPassive` |
| `hover` | `hoverOverlay` |
| `pressed` | `pressedOverlay` |
| `textSubtle` | `textSecondary` |

## QPalette Mapping

Active and inactive color groups are intentionally identical for tiling-window-manager use. Disabled fill roles keep the same surfaces; disabled text/accent roles use canonical disabled/dimmed values.

| QPalette role | Token |
| --- | --- |
| `Window` | `background` |
| `Base` | `surface` |
| `AlternateBase` | `surfaceElevated` |
| `Button` | `surfaceRaised` |
| `ToolTipBase` | `surfaceInverse` |
| `WindowText`, `Text`, `ButtonText`, `BrightText` | `textPrimary` |
| `PlaceholderText` | `textMuted` |
| `ToolTipText` | `textInverse` |
| `Highlight`, `Link` | `primary` |
| `HighlightedText` | `onPrimary` |
| `LinkVisited` | `error` |
| `Mid`, `Dark`, `Shadow` | `borderPassive` |

## QML Liveness

Palette colors, overlays, radius tokens, metrics, and ANSI values are live-capable QML properties with a `paletteChanged` notifier. `HoloniightPalette.reload()` reapplies the resolved token set and emits the notifier. It does not watch files.

Startup-only platform integration values remain on `HolonightTheme` as constants: icon theme names, font family names, base font size, scale factor, and derived font sizes.

## Configuration Precedence

Current implemented precedence is:

1. Built-in defaults from `ThemeConfig::defaults()`.
2. Config file selected from `HOLONIGHT_CONFIG_FILE` when set; otherwise user config from `~/.config/holonight/theme.conf`, or legacy `~/.config/holonight/theme.json` if INI is absent.
3. Environment value overrides such as `HOLONIGHT_APPEARANCE_MODE`, `HOLONIGHT_ICON_THEME`, `HOLONIGHT_FONT`, and `HOLONIGHT_SCALE_FACTOR`.

Appearance scheme is configured with `appearance/scheme`. It is the canonical selector.
Supported values are `holonight-dark`, `holonight-light`, `holonight-mocha`, `holonight-latte`,
`tokyonight-storm`, `tokyonight-day`, `holonight-ember`, `holonight-sol`, `holonight-cyber-d`, and `holonight-cyber-l`.
Values are trimmed and matched case-insensitively.

Legacy `appearance/mode` remains fallback metadata for older configs. If `scheme` is missing or invalid, `mode=light` resolves to `holonight-light`; `mode=dark`, `mode=system`, missing, or invalid values resolve to `holonight-dark`.
`system` no longer queries Qt's system color-scheme hint.

`appearance/accent` supports `cyan`, `blue`, `violet`, and `yellow`. Missing or invalid accents resolve to `cyan`.

JSON shape:

```json
{
  "appearance": {
    "scheme": "holonight-dark",
    "accent": "cyan",
    "mode": "dark"
  }
}
```

INI shape:

```ini
[appearance]
scheme=holonight-dark
accent=cyan
mode=dark
```

Environment override:

```bash
HOLONIGHT_APPEARANCE_MODE=light
```

Generated KDE schemes are `data/holonight-dark.colors` (`HoloNight Dark`),
`data/holonight-light.colors` (`HoloNight Light`), `data/holonight-mocha.colors` (`HoloNight Mocha`),
`data/holonight-latte.colors` (`HoloNight Latte`), `data/tokyonight-storm.colors` (`TokyoNight Storm`),
`data/tokyonight-day.colors` (`TokyoNight Day`), `data/holonight-ember.colors` (`HoloNight Ember`),
`data/holonight-sol.colors` (`HoloNight Sol`), `data/holonight-cyber-d.colors` (`HoloNight Cyber D`),
and `data/holonight-cyber-l.colors` (`HoloNight Cyber L`).

Future shared config will use `~/.config/holonight/config.toml`, but TOML parsing is not implemented in this pass and no new dependency is introduced.

Planned TOML shape:

```toml
[theme]
variant = "Storm"
mode = "dark"

[icons]
theme = "HoloNight"
fallback = "Papirus"

[fonts]
ui = "Inter"
fixed = "JetBrains Mono"
base_size = 10

[metrics]
scale_factor = 1.0
```
