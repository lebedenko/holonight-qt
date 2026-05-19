# holonight

A dark Qt6 theme designed for tiling window managers (Hyprland, sway, niri).

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

| Role | Value | Usage |
|---|---|---|
| `surface` | `#121626` | Main background |
| `surfaceVariant` | `#1a1b26` | View / content background |
| `surfaceContainer` | `#1f2335` | Cards, grouped containers |
| `surfaceHover` | `#1e2233` | Hover-state filled surface |
| `surfaceInverse` | `#0d1117` | ToolTip / pop-out surface |
| `onSurface` | `#c0caf5` | Primary text and icons |
| `onSurfaceVariant` | `#565f89` | Disabled / placeholder text (WCAG exempt) |
| `onSurfaceDisabled` | `#3b3f58` | Inactive UI elements (WCAG 1.4.3 exempt) |
| `onSurfaceInverse` | `#c0caf5` | Text on inverse surface |
| `primary` | `#00a0dc` | Selection highlight fill |
| `focusRing` | `#00e0ff` | Keyboard focus indicator (pure cyan) |
| `outline` | `#00a0c8` | Borders, separators |
| `error` | `#f7768e` | Destructive / error |
| `warning` | `#ff9e64` | Caution |
| `success` | `#9ece6a` | Confirmation |
| `shadow` | `#000000` | Drop-shadow base (alpha applied at paint time) |

WCAG AA contrast (4.5:1) is enforced by the test suite for all text and selection pairs. Non-text UI components (borders, focus rings) are tested at the WCAG 1.4.11 threshold of 3:1.

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
- **QML module URI** is `Holonight` (capital N). Use `import Holonight` in QML files to access all components and the `HoloniightPalette` singleton.
- **Platform theme** sets Inter 10pt as the system font hint and Papirus as the icon theme hint (both by name only — no bundled assets).
- **Tests** compile plugin sources directly into the test binary — no installed plugins required, `QT_QPA_PLATFORM=offscreen` is sufficient.
- **Naming**: class names carry three i's (`HoloniightStyle`, `HoloniightTheme`) — intentional.

## License

GPL-3.0-or-later — see [LICENSE](LICENSE).
