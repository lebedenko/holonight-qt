# HoloNight Theme Tokens

The palette layer is the canonical source for resolved theme values. `Holonight::darkTokens()` currently returns the complete Storm variant; all Qt palettes, widgets, QML controls, platform theme colors, and generated KDE color-scheme values should consume roles from `Holonight::ColorTokens`.

## Variant Status

| Variant | Status | Dark | Light | Notes |
| --- | --- | --- | --- | --- |
| Storm | Implemented | Yes | No | Default resolved theme. Tokyo Night inspired. |
| Aurora | Planned | Planned | No | Nord inspired. Not parsed or exported yet. |
| Mocha | Planned | Planned | No | Catppuccin inspired. Not parsed or exported yet. |
| Ember | Planned | Planned | No | Warm dark variant. Not parsed or exported yet. |
| Eclipse | Planned | Planned | No | Lower-chroma dark variant. Not parsed or exported yet. |
| Neon | Planned | Planned | No | Higher-accent dark variant. Not parsed or exported yet. |

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
2. Config file selected from `HOLONIGHT_CONFIG_FILE` when set; otherwise legacy user config from `~/.config/holonight/theme.json`, or `~/.config/holonight/theme.conf` if JSON is absent.
3. Environment value overrides such as `HOLONIGHT_ICON_THEME`, `HOLONIGHT_FONT`, and `HOLONIGHT_SCALE_FACTOR`.

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
