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
| `style/` | `libholonight.so` — QStyle plugin | `QT_STYLE_OVERRIDE=holonight` |
| `platformtheme/` | `libqholonight.so` — QPlatformTheme plugin | `QT_QPA_PLATFORMTHEME=holonight` |
| `qml/` | QQC2 components (Button, CheckBox, RadioButton, ComboBox, Slider, Switch, ProgressBar, TabBar, TabButton, Menu, MenuItem, ToolTip, ScrollBar, TextField) | `import Holonight` |
| `palette/` | `libholonight_palette.a` — shared color tokens | static dependency |

Using `QT_QPA_PLATFORMTHEME=holonight` is the recommended activation method — it loads the style and QML layers automatically.

A KDE color scheme (`data/holonight.colors`) is also installed to `share/color-schemes`.

## Palette

Colors are based on the [HoloNight](https://github.com/topics/kde-color-scheme) KDE color scheme — a futuristic cyan/violet dark palette. All tokens are defined in `palette/holonight/palette.h` and originate from `darkTokens()`.

**Surface elevation** (ordered darkest to lightest):

| Role | Value | Usage |
|---|---|---|
| `surface` | `#10131f` | Main background |
| `surfaceVariant` | `#161925` | View / content background |
| `surfaceContainer` | `#1a1b26` | Cards, grouped containers |
| `secondary` | `#24283b` | Panel / menu / popup surface |
| `surfaceHover` | `#1e2233` | Hover-state filled surface overlay |
| `surfaceInverse` | `#0d1117` | ToolTip / pop-out surface |

**Content and text:**

| Role | Value | Usage |
|---|---|---|
| `onSurface` | `#c0caf5` | Primary text and icons |
| `onSurfaceVariant` | `#565f89` | Disabled / placeholder text (WCAG 1.4.3 exempt) |
| `onSurfaceDisabled` | `#3b3f58` | Inactive UI elements (WCAG 1.4.3 exempt) |
| `onSurfaceInverse` | `#c0caf5` | Text on inverse surface |
| `primary` | `#00a0dc` | Selection highlight fill |
| `error` | `#f7768e` | Destructive / error |
| `warning` | `#ff9e64` | Caution |
| `success` | `#9ece6a` | Confirmation |
| `shadow` | `#000000` | Drop-shadow base (alpha applied at paint time) |

**Semantic border tokens:**

| Role | Value | Usage |
|---|---|---|
| `borderPassive` | `#565f89` | Passive / inactive control frames and separators (WCAG 1.4.11 exempt — not interactive) |
| `borderHover` | `#7dcfff` ~30% α | Transient hover-state border overlay (semi-transparent by design) |
| `borderFocus` | `#00e0ff` | Keyboard focus ring border |
| `borderActive` | `#7aa2f7` | Selected / active state border |
| `borderUrgent` | `#f7768e` | Error / destructive state border |

**Workspace indicator tokens:**

| Role | Value | Usage |
|---|---|---|
| `workspaceOccupied` | `#1f2335` | Occupied / urgent workspace pill fill |
| `workspaceActive` | `#20263a` | Active workspace pill fill |
| `accentCyan` | `#7dcfff` | Active workspace border / neon-cyan accent |
| `textSubtle` | `#a9b1d6` | Occupied workspace label text |

WCAG AA contrast (4.5:1) is enforced by the test suite for all text and selection pairs. Non-text UI components (borders, focus rings) are tested at the WCAG 1.4.11 threshold of 3:1. Known intentional deviations are documented in [`docs/holonight-design-deviations.md`](docs/holonight-design-deviations.md).

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
# Platform theme (recommended — loads style + QML automatically)
QT_QPA_PLATFORMTHEME=holonight your-qt-app

# Style only
QT_STYLE_OVERRIDE=holonight your-qt-app
```

To make this permanent, export `QT_QPA_PLATFORMTHEME=holonight` in your compositor environment (e.g. `~/.config/hypr/hyprland.conf`, `~/.config/sway/config`, or `/etc/environment`).

## Development

```bash
# Configure, build, and test
task build
task test

# Full pre-commit checklist (build + clang-tidy + tests)
task verify

# Visual demo app
task demo
```

Manual equivalents:

```bash
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON
cmake --build build -j$(nproc)
QT_QPA_PLATFORM=offscreen ctest --test-dir build --output-on-failure
```

## Architecture

- **All colors** originate in `palette/holonight/palette.h` (`darkTokens()` → `buildPalette()`). Change colors there, nowhere else.
- **QML module URI** is `Holonight` (capital N). Use `import Holonight` in QML files to access all components and the `HoloniightPalette` singleton. A lowercase alias (`import holonight`) is also installed for compatibility.
- **Platform theme** sets Inter 12pt (system/title), JetBrainsMono Nerd Font 12pt (fixed), Inter 10pt (small), Inter 8pt (mini), and Papirus-Dark as the icon theme (no bundled assets).
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
