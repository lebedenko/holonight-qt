# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project

**holonight** is a dark Qt6 theme for tiling window managers (Hyprland, sway, niri). It ships three layers:
- `style/` — QStyle plugin (`libholonight.so`), activated via `QT_STYLE_OVERRIDE=holonight`
- `platformtheme/` — QPlatformTheme plugin (`libqholonight.so`), activated via `QT_QPA_PLATFORMTHEME=holonight` (recommended — loads all layers automatically)
- `qml/` — QQC2 custom components + Fusion fallback
- `palette/` — static lib with `ColorTokens` struct; all other modules depend on it

## Build

```bash
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON
cmake --build build -j$(nproc)
```

Build with the visual demo app:
```bash
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug -DBUILD_DEMO=ON
```

Install to a local prefix (avoid sudo during development):
```bash
cmake --install build --prefix ~/.local
```

Run tests (requires `QT_QPA_PLATFORM=offscreen` for headless):
```bash
QT_QPA_PLATFORM=offscreen ctest --test-dir build --output-on-failure
```

## Code Style

**Enforced by `.clang-format` and `.clang-tidy` — always run both before committing.**

- C++23, no extensions (`CMAKE_CXX_EXTENSIONS OFF`)
- Column limit: **120** (not 80/100)
- Indent: **2 spaces**
- **Include order**: local headers `"..."` first, Qt headers `<Q*>` second, system headers last
- **Naming**:
  - Classes: `CamelCase`
  - Free functions and methods: `camelBack`
  - Private members: `lower_case_` (trailing underscore — not `m_member`)
- No trailing return types (`modernize-use-trailing-return-type` is disabled)
- `[[nodiscard]]` on functions whose return value must be used

## Architecture Notes

- **Qt6 API pitfalls** (things that don't exist or changed from Qt5/docs):
  - `QStyle::State_Up` was removed — use `QStyleOptionHeader::sortIndicator == SortDown` for `PE_IndicatorHeaderArrow` direction.
  - `QPlatformTheme` uses `Qt::ColorScheme colorScheme() const` returning `Qt::ColorScheme::Dark` — NOT `colorSchemeHint()` or `QPlatformTheme::Appearance` (neither exists in Qt6).
- **QStyle test fixture** — Never call `QStyleOption::initFrom(nullptr)`; it dereferences the widget pointer and crashes. Set `opt.rect`, `opt.state`, `opt.palette` manually.
- **QML MODULE target** — `qt_add_qml_module` produces a `MODULE_LIBRARY` that cannot appear in `target_link_libraries`. QML smoke tests must use `engine.addImportPath(HOLONIGHT_QML_IMPORT_PATH)` where `HOLONIGHT_QML_IMPORT_PATH` is a CMake compile-time define pointing to `${CMAKE_BINARY_DIR}/qml`.
- **`add_dependencies` for QML plugin** — when `holonight_qml` is removed from `target_link_libraries` (MODULE restriction), add `add_dependencies(target holonight_qml)` to preserve build ordering.
- **QQC2 style name** — use `QQuickStyle::setStyle("Holonight")` (capital N, matching the `qt_add_qml_module` URI). Lowercase `"holonight"` hits a legacy system-installed module at `~/.local/lib/qt6/qml/holonight/` with stale QML. The build-tree module at `build/qml/Holonight/` is found first via `addImportPath` when the style name matches.
- **Qt diagnostic output** — on this system Qt routes `qWarning`/QML errors to systemd journal, not stderr. To see QML load errors: `journalctl -n 20 --no-pager | grep <appname>`.
- **ctest isolation** — If the platform theme is installed system-wide, the `QPalette{}` default constructor returns the holonight palette and breaks palette/style tests. Add `QT_QPA_PLATFORMTHEME=;QT_STYLE_OVERRIDE=` to the ctest `ENVIRONMENT` property for any test target that compares palette colors.
- **clang-tidy `-mno-direct-extern-access`** — Qt6 injects this flag into compile commands; clang-tidy logs it as an unknown argument. It is NOT a code issue — ignore it.
- **`../wshell` as tooling reference** — sibling Qt6 project; check it for prior art on CMake/clang-tidy patterns before solving from scratch.
- **Font role test coverage** — Font point sizes are asserted in both `test_platformtheme_fonts.cpp` and `test_platformtheme_smoke.cpp`. When changing font sizes, update both files and `holonighttheme.cpp`.
- **QML lowercase alias** — The `holonight` (lowercase) alias module is generated from `qml/holonight-alias.qmldir`. When adding a new QML component, add the filename to `HOLONIGHT_QML_FILES` in `qml/CMakeLists.txt` AND to `qml/holonight-alias.qmldir`.
- **QML `PlaceholderText`** is a private Qt type from `QtQuick.Controls.impl` — `qmlcachegen` cannot resolve it at build time. Use a plain `Text` element instead (set `color`, `elide`, `visible`, `renderType` manually); behavior is identical.
- **QML singleton registration** — Use `QML_SINGLETON + QML_ELEMENT` macros (Qt 6.2+) with AUTOMOC ON; URI must be `"Holonight"` (capital N) to match the `qt_add_qml_module` URI exactly.

- Plugin JSON files (`style/holonightstyle.json`, `platformtheme/qholonight.json`) must contain **only** `{ "Keys": ["holonight"] }`. Do NOT add `IID`/`MetaData`/`className` — `Q_PLUGIN_METADATA` embeds the file as `MetaData`; wrapping it causes double-nesting and `QFactoryLoader` silently ignores the plugin.
- To diagnose plugin loading: `QPluginLoader::metaData()` reveals the actual structure Qt sees; check that `Keys` is at `MetaData.Keys`, not `MetaData.MetaData.Keys`.
- For system-level testing (not unit tests), install with `sudo cmake --install build --prefix /usr` and use `Release` builds — debug plugins have `"debug": true` metadata which differs from system Qt plugins.

- All colors originate in `palette/holonight/palette.h` (`ColorTokens` struct → `tokensForScheme(ThemeSchemeKind)` → `buildPalette()`). Change colors there, nowhere else.
- **Adding theme variants** — add schemes through the full pipeline, not only the palette table: extend `ThemeSchemeKind`, `tokensForScheme()`, `colorModeForScheme()`, config scheme parsing, KDE color-scheme name fallback, and `tools/generate_holonight_colors.cpp`; generate `data/<scheme-id>.colors`; update `docs/theme-tokens.md`; add tests for token values, aliases, color mode, resolver/config parsing, KDE fallback names, generated color files, and WCAG contrast.
- **External palette mapping** — when importing palettes such as Catppuccin, use official palette colors for base roles and document derived HoloNight-only roles (`primaryHover`, `primaryPressed`, border tiers, focus, overlays, glows, workspace fills, and ANSI bright colors). Keep scheme IDs lowercase kebab-case (`holonight-mocha`) and display names title-cased (`HoloNight Mocha`).
- Class names use three i's: `HoloniightStyle`, `HoloniightTheme` — this is intentional.
- Tests compile plugin source directly (not via Qt plugin-loading) so `QApplication` + `QT_QPA_PLATFORM=offscreen` is sufficient; no actual plugin install needed for tests.
- WCAG AA contrast (4.5:1) is enforced by `tests/test_palette_contrast.cpp` — if you add color tokens or modify existing ones, update the contrast test.
- Semi-transparent overlay tokens (`hover`, `pressed`, `focusRing`) are intentionally alpha < 255.
- Disabled-state colors are derived by blending with surface, not hardcoded.
- **5-site surface elevation rule** — `QPalette::Base` maps to `tok.surfaceVariant`. All five sites must change together or `polish(QWidget*)` silently reverts `Base` on every polished widget: (1) `buildPalette()` Active Base, (2) `buildPalette()` Disabled Base, (3) `polish(QWidget*)` line ~34, (4) `PE_PanelLineEdit` fill brush, (5) `PE_PanelItemViewRow` default branch.
- **Direct-fill controls** — `PE_PanelLineEdit` and `CC_ComboBox` set their fill brush directly from a token (not from `QPalette::Base`). Changing the palette alone does not update their rendered background.
- **File-scoped constants** — put in an anonymous namespace (`namespace { constexpr qreal kFoo = ...; } // namespace`), not `static constexpr` at file scope and not in headers. Use the `k` prefix per naming convention.
- **`drawRoundedRect` invariant** — all calls in `style/holonightstyle.cpp` must use a `kRadius*` constant. Verify with: `grep -n "drawRoundedRect" style/holonightstyle.cpp | grep -v "kRadius"` (must return zero matches).

## Known Pre-existing clang-tidy Warnings

`cmake --build build --target tidy` reports these on unmodified code — they are **not regressions**, do not chase them:
- `readability-identifier-naming` on test fixture members (`palette_`, `style_`, `painter_`, `tok_`, `engine_`) — trailing-underscore naming conflicts with the tidy config for test files
- `readability-identifier-length` for `fm` in `drawItemViewItemImpl`
- `cppcoreguidelines-narrowing-conversions` (×2) in `drawItemViewItemImpl` for `Qt::Alignment | Qt::TextWordWrap`
- `readability-function-cognitive-complexity` on `subControlRect` (score 31, threshold 25)
- `readability-convert-member-functions-to-static` on `drawItemViewItemImpl`

## Commits

Follow **Conventional Commits**: `feat:`, `fix:`, `refactor:`, `test:`, `chore:`, `docs:`, `style:`, etc.

## Pre-Commit Checklist

1. `cmake --build build -j$(nproc)` — must compile cleanly
2. `cmake --build build --target tidy` — zero warnings (strips Qt6's `-mno-direct-extern-access` before running clang-tidy)
3. `QT_QPA_PLATFORM=offscreen ctest --test-dir build --output-on-failure` — all tests pass

Run `/verify` to do all three in sequence.
