# HoloNight

HoloNight Qt Theme is the Qt/Qt Quick Controls theme component of the broader HoloNight desktop visual system.

This repository contains only the Qt theme implementation. The shell, icon theme, and GTK theme are planned as separate repositories.

## Screenshots

| Input | Navigation | Containers |
|:---:|:---:|:---:|
| ![Input controls](assets/images/input.png) | ![Navigation](assets/images/navigation.png) | ![Containers](assets/images/containers.png) |

| Text | Feedback |
|:---:|:---:|
| ![Text](assets/images/text.png) | ![Feedback](assets/images/feedback.png) |

## Layers

| Layer | Output | Activation |
|---|---|---|
| `src/style/` | `libholonight.so` — QStyle plugin | Selected by the platform theme |
| `src/platformtheme/` | `libqholonight.so` — QPlatformTheme plugin | `QT_QPA_PLATFORMTHEME=holonight` |
| `qml/` | QQC2 style components, design-system primitives, and reusable composites | `import Holonight`, `import Holonight.Core`, `import Holonight.Controls` |
| `palette/` | `libholonight_palette.a` — shared color tokens | static dependency |
| `config/` | Qt appearance projection, reader, and theme catalog | consumed by platform theme, style, and QML |

System-wide activation uses the platform theme for Qt Widgets and Qt Quick Controls' own runtime
style selector for QML controls.

KDE color schemes are also installed to `share/color-schemes`: `HoloNight Dark`, `HoloNight Light`,
`HoloNight Mocha`, `HoloNight Latte`, `HoloNight Storm`, `HoloNight Day`, `HoloNight Ember`, `HoloNight Sol`,
`HoloNight Cyber D`, `HoloNight Cyber L`, `HoloNight Dracula`, and `HoloNight Alucard`.

## Palette

HoloNight ships a scheme catalog selected with `tokensForScheme(ThemeSchemeKind)`: **HoloNight Dark**,
**HoloNight Light**, **HoloNight Mocha**, **HoloNight Latte**, **HoloNight Storm**, **HoloNight Day**,
**HoloNight Ember**, **HoloNight Sol**, **HoloNight Cyber D**, **HoloNight Cyber L**, **HoloNight Dracula**,
and **HoloNight Alucard**.
`appearance/scheme` is the canonical config selector; legacy `appearance/mode` is only used as a fallback when no
valid scheme is present. All resolved colors and metrics originate in `palette/holonight/palette.h`, and
downstream layers consume that token model rather than hard-coded color values.

The preferred public token roles are canonical names such as `background`, `surface`, `surfaceElevated`, `surfaceRaised`, `textPrimary`, `textMuted`, `borderPassive`, `borderActive`, and `borderFocus`. Older names such as `surfaceVariant`, `surfaceContainer`, `onSurface`, `outline`, and `textSubtle` remain available as deprecated compatibility aliases.

Key HoloNight Dark defaults:

| Role | Value | Usage |
|---|---|---|
| `background` | `#0C1118` | Main window/background plane |
| `surface` | `#131A24` | Views, text fields, and recessed base surfaces |
| `surfaceElevated` | `#18212D` | Cards, alternate rows, side/status panels |
| `surfaceRaised` | `#202B39` | Buttons, menus, popovers, raised controls |
| `textPrimary` | `#E7EDF5` | Normal text and icons |
| `textSecondary` | `#C5D0DE` | Secondary readable text |
| `textMuted` | `#8D99AD` | Placeholder and inactive text |
| `textDisabled` | `#5B6678` | Disabled UI text |
| `primary` | `#5EA2FF` | Selection/link fill |
| `borderFocus` | `#56D7FF` | Keyboard focus only |
| `borderActive` | `#5EA2FF` | Current, selected, or active border |
| `borderPassive` | `#36465A` | Passive frames and separators |

The token model also includes semantic status colors, overlay/effect tokens, radius and metric tokens, workspace indicators, and ANSI terminal colors. See [`docs/theme-tokens.md`](docs/theme-tokens.md) for the full schema, deprecated alias mapping, Qt palette mapping, variant status, and future `config.toml` shape.

WCAG AA contrast (4.5:1) is enforced by the test suite for text and selection pairs. Non-text UI components such as focus rings and active borders are tested at the WCAG 1.4.11 threshold of 3:1. Known intentional deviations from the design-system color table are documented in [`docs/holonight-design-deviations.md`](docs/holonight-design-deviations.md).

## Requirements

- Qt 6.11+
- CMake 3.25+
- Ninja
- GTest (for tests)

## Build from source

```bash
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
sudo cmake --install build --prefix /usr
```

Install to a local prefix (no sudo, useful during development):

```bash
cmake --install build --prefix ~/.local
```

With [Task](https://taskfile.dev):

```bash
task install:system   # Release build → /usr (sudo)
task install:local    # Debug build → ~/.local
```

## Usage

```bash
QT_QPA_PLATFORMTHEME=holonight \
QT_QUICK_CONTROLS_STYLE=Holonight \
your-qt-app
```

To make this permanent, export both variables in your compositor environment (for example,
`~/.config/hypr/hyprland.conf`, `~/.config/sway/config`, or `/etc/environment`). The different
casing is intentional: `holonight` is the platform plugin key and `Holonight` is the
case-sensitive QML style URI.

Do not export `QT_STYLE_OVERRIDE=holonight` globally. It bypasses platform-theme style selection
and is unsupported for hybrid Widgets/Qt Quick applications. A Widgets-only application may use
it as a command-local override:

```bash
QT_STYLE_OVERRIDE=holonight widgets-only-app
```

### Compatibility

| Consumer | Expected result | Boundary |
| --- | --- | --- |
| Qt Widgets | HoloNight through the platform theme | The application must honor platform-theme selection |
| Runtime-style Qt Quick Controls | HoloNight through `QT_QUICK_CONTROLS_STYLE` | The application must not select another style before engine startup |
| Hybrid Widgets and Qt Quick | Both surfaces use HoloNight with the two-variable contract | Global `QT_STYLE_OVERRIDE` is unsupported |
| Compile-time third-party Qt Quick style | The third-party style remains authoritative | HoloNight does not force replacement |
| Custom-painted controls | Application rendering remains authoritative | Palette adoption is application-specific |
| Static application | Works only when the required HoloNight plugins and modules are linked and discoverable | Environment variables cannot add missing code |
| Non-Qt toolkit | Unchanged | Outside HoloNight's scope |

## Configuration

HoloNight loads user-facing theme configuration from:

```text
~/.config/holonight/appearance.toml
```

`HOLONIGHT_APPEARANCE_FILE` can point at another file for testing. It is the only supported configuration override.
The canonical token schema and Qt palette mapping are documented in [`docs/theme-tokens.md`](docs/theme-tokens.md).

Use the versioned canonical TOML schema:

```toml
version = 1

[theme]
scheme = "holonight-dark"
accent = "blue"

[typography]
ui_family = "Inter"
ui_size = 12
monospace_family = "JetBrains Mono"
monospace_size = 12
title_family = "Audiowide"
title_size = 10
display_family = "Rajdhani"
display_size = 24
```

Supported appearance schemes are `holonight-dark`, `holonight-light`, `holonight-mocha`, `holonight-latte`,
`holonight-storm`, `holonight-day`, `holonight-ember`, `holonight-sol`, `holonight-cyber-d`, `holonight-cyber-l`,
`holonight-dracula`, and `holonight-alucard`.
The selected scheme determines dark/light mode. Supported accents are `default`, `cyan`, `blue`, `violet`, and
`yellow`. Invalid canonical documents are rejected as a unit; live consumers retain their last known good state.

The supported environment override is:

```bash
HOLONIGHT_APPEARANCE_FILE=/path/to/appearance.toml
```

The same loaded values are exposed to QML through the `HolonightTheme` singleton:

```qml
import Holonight.Core

Item {
    property string iconTheme: HolonightTheme.iconTheme
    property string uiFont: HolonightTheme.uiFont
    property int bodySize: HolonightTheme.bodySize
    property int captionSize: HolonightTheme.captionSize
}
```

Reusable application composites are exported from `Holonight.Controls`. See the
[shared controls usage guide](docs/shared-controls-usage-guide.md) for selection, composition, sizing, keyboard,
icon actions and menus, elided-title tooltips, and separator patterns.

## Development

```bash
# Configure, build, and test
task build
task test

# Full pre-commit checklist (build + clang-tidy + tests)
task verify

# Visual demo app
task demo
task demo -- --theme=holonight-day
```

Manual equivalents:

```bash
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON
cmake --build build -j$(nproc)
QT_QPA_PLATFORM=offscreen ctest --test-dir build --output-on-failure
```

## Architecture

- **All colors** originate in `palette/holonight/palette.h` (`tokensForScheme(ThemeSchemeKind)` → `buildPalette()`). Change colors there, nowhere else.
- **Appearance** is validated by `HoloNight::Config`, projected once per consumer, and published atomically.
- **QML module URIs** are `Holonight` for styled Qt Quick Controls, `Holonight.Core` for design-system primitives,
  and `Holonight.Controls` for reusable composites.
- **Platform theme** reads configured icon theme, fallback icon theme, UI font, fixed font, and base font size. Defaults are HoloNight/Papirus icons, Inter UI font, JetBrains Mono fixed font, and 10pt body size.
- **Tests** compile plugin sources directly into the test binary — no installed plugins required, `QT_QPA_PLATFORM=offscreen` is sufficient.
- **Naming**: class names carry three i's (`HoloniightStyle`, `HoloniightTheme`) — intentional.

## License

GPL-3.0-or-later — see [LICENSE](LICENSE).

Contains colors derived from TokyoNight (MIT licensed).

## Inspiration

HoloNight draws visual inspiration from the TokyoNight Storm palette
by folke.

## Credits

- TokyoNight palette by folke
  <https://github.com/folke/tokyonight.nvim>
