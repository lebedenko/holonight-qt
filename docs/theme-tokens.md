# HoloNight Theme Tokens

The palette layer is the canonical source for resolved theme values. `Holonight::tokensForScheme(ThemeSchemeKind)` returns one of the built-in concrete schemes; all Qt palettes, widgets, QML controls, platform theme colors, and generated KDE color-scheme values should consume roles from `Holonight::ColorTokens`.

## Variant Status

| Variant | Status | Dark | Light | Notes |
| --- | --- | --- | --- | --- |
| HoloNight Dark / Light | Implemented | Yes | Yes | Default HoloNight schemes selected by legacy mode fallback. |
| HoloNight Mocha / Latte | Implemented | Yes | Yes | Catppuccin-based schemes selected by explicit scheme ID. |
| HoloNight Storm / Day | Implemented | Yes | Yes | Bundled Tokyo Night inspired compatibility schemes. |
| HoloNight Ember / Sol | Implemented | Yes | Yes | Gruvbox-based high contrast schemes. |
| HoloNight Cyber D / L | Implemented | Yes | Yes | Cyberpunk-inspired neon schemes from palette.cpp. |
| HoloNight Dracula / Alucard | Implemented | Yes | Yes | Official Dracula OSS base palettes by Zeno Rocha / draculatheme.com (MIT License), with documented HoloNight semantic derivations. |
| Aurora | Planned | Planned | No | Nord inspired. Not parsed or exported yet. |
| Eclipse | Planned | Planned | No | Lower-chroma dark variant. Not parsed or exported yet. |
| Neon | Planned | Planned | No | Higher-accent dark variant. Not parsed or exported yet. |

Current scheme IDs are `holonight-dark`, `holonight-light`, `holonight-mocha`, `holonight-latte`,
`holonight-storm`, `holonight-day`, `holonight-ember`, `holonight-sol`, `holonight-cyber-d`, `holonight-cyber-l`,
`holonight-dracula`, and `holonight-alucard`.
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
| `surfaceSelected` | Quiet accent-tinted selected surface for list and choice controls. |
| `surfaceSelectedHover` | Hover state layered into an already selected surface. |
| `surfaceInverse` | Tooltip and complementary inverse surface. Maps to `QPalette::ToolTipBase`. |

Text roles:

| Token | Meaning |
| --- | --- |
| `textPrimary` | Normal text/icons. |
| `textSecondary` | Secondary readable text. |
| `textMuted` | Placeholder and inactive text. |
| `textDisabled` | Disabled text; WCAG inactive-component exemptions apply. |
| `textInverse` | Text on `surfaceInverse`. |
| `textAccent` | Accent-colored text and icons on canonical surfaces. Tracks the selected primary accent. |

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
| `selectionIndicator` | High-emphasis selected edge, outline, or check indicator. |

Accent/status roles:

| Token | Meaning |
| --- | --- |
| `primary` | Selection/link fill. Storm uses calmer blue `#7aa2f7`. |
| `accentCyan` | General cyan accent; `#00e0ff` is reserved for keyboard focus. |
| `accentBlue`, `accentViolet`, `accentYellow` | Secondary accent families. |
| `brandForeground` | Contrast-safe HoloNight brand text, independent of the selected primary accent. |
| `error`, `warning`, `success` | Semantic status colors. |

Overlay, radius, metric, and ANSI roles:

| Token group | Meaning |
| --- | --- |
| `glowCyanSoft`, `glowBlueSoft`, `glowVioletSoft` | Soft effect overlays. |
| `scrim`, `glassTint`, `hoverOverlay`, `pressedOverlay`, `disabledOverlay` | State and composition overlays with intentional alpha. |
| `borderWidth`, `focusBorderWidth`, `separatorWidth`, `controlHeight`, `controlPadding` | Shared control metrics. |
| `HnMetrics.appTitleIconSize`, `appTitleIconSpacing`, `appTitleTextSpacing` | Shared application-title icon and spacing metrics. |
| `HnMetrics.headerHeight` | Canonical 56-pixel application header-region height. |
| `ansiBlack` through `ansiBrightWhite` | Semantic ANSI terminal colors. |

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

Palette colors, overlays, radius tokens, metrics, and ANSI values are live-capable QML properties with a
`paletteChanged` notifier. `HoloniightPalette` watches the resolved theme configuration path and its containing
directory, reloads when the configuration changes or appears, and emits the notifier when the resolved tokens
change. Applications may also call `HoloniightPalette.reload()` explicitly.

Startup-only platform integration values remain on `HolonightTheme` as constants: icon theme names, font family names, base font size, scale factor, and derived font sizes.

## Configuration Precedence

Current implemented precedence is:

1. Canonical defaults owned by `HoloNight::Config`.
2. The versioned TOML document selected by `HOLONIGHT_APPEARANCE_FILE`, or
   `~/.config/holonight/appearance.toml` by default.

Appearance scheme is configured with `theme.scheme` and determines dark/light mode.
Supported values are `holonight-dark`, `holonight-light`, `holonight-mocha`, `holonight-latte`,
`holonight-storm`, `holonight-day`, `holonight-ember`, `holonight-sol`, `holonight-cyber-d`, `holonight-cyber-l`,
`holonight-dracula`, and `holonight-alucard`.
`theme.accent` supports `default`, `cyan`, `blue`, `violet`, and `yellow`. The provider validates and normalizes the
complete document before Qt publishes it.

Path override:

```bash
HOLONIGHT_APPEARANCE_FILE=/path/to/appearance.toml
```

Generated KDE schemes are `data/holonight-dark.colors` (`HoloNight Dark`),
`data/holonight-light.colors` (`HoloNight Light`), `data/holonight-mocha.colors` (`HoloNight Mocha`),
`data/holonight-latte.colors` (`HoloNight Latte`), `data/holonight-storm.colors` (`HoloNight Storm`),
`data/holonight-day.colors` (`HoloNight Day`), `data/holonight-ember.colors` (`HoloNight Ember`),
`data/holonight-sol.colors` (`HoloNight Sol`), `data/holonight-cyber-d.colors` (`HoloNight Cyber D`),
`data/holonight-cyber-l.colors` (`HoloNight Cyber L`), `data/holonight-dracula.colors` (`HoloNight Dracula`),
and `data/holonight-alucard.colors` (`HoloNight Alucard`).

### Dracula palette mapping

Dracula and Alucard use the official OSS palette values for background, foreground, comment/muted text, current-line
or selection surfaces, cyan, green, orange, pink, purple, red, and yellow. HoloNight roles without a direct Dracula
equivalent are derived: layered surfaces, hover and pressed colors, borders, focus/glow alpha variants, overlays,
workspace states, blue accents, and ANSI bright colors. Dracula blue derives from the official Comment color, while
Alucard blue preserves the same muted blue relationship. These derivations preserve the official palette's
hue relationships while meeting the contrast and interaction requirements of the shared HoloNight token model.

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
