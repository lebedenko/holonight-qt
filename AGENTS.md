# Repository Guidelines

Use Conventional Commits for every new commit: `type(scope): imperative summary`, or `type: imperative summary` when a scope adds no clarity.

## Project Structure & Module Organization

HoloNight is a Qt 6 theme split into small CMake targets. `palette/` owns shared color tokens and palette construction; all theme colors should originate in `palette/holonight/palette.h`. `src/style/` builds the QStyle plugin, `src/platformtheme/` builds the QPlatformTheme plugin, and `qml/` exports the `Holonight`, `Holonight.Core`, and `Holonight.Controls` modules. Add standard Qt Quick Controls style implementations to `Holonight`, design-system primitives to `Holonight.Core`, and reusable application composites to `Holonight.Controls`. `tests/` contains GTest/CTest coverage, including palette, style, platform theme, and QML smoke tests. `demo/` provides a visual style test app, `examples/controls-gallery/` exercises shared controls, `data/` contains KDE color schemes, `assets/images/` stores README screenshots, and `docs/` holds usage guides, design notes, and specs.

## Build, Test, and Development Commands

Use Task when available:

```bash
task build      # Configure Debug with tests and build
task test       # Run headless CTest with QT_QPA_PLATFORM=offscreen
task lint       # Run run-clang-tidy against build/
task verify     # Build, lint, and test
task demo       # Build and run the demo app from the build tree
```

Manual equivalents:

```bash
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON
cmake --build build -j$(nproc)
QT_QPA_PLATFORM=offscreen ctest --test-dir build --output-on-failure
```

Use `task install:local` for `~/.local` installs and `task install:system` only when a system install is required.

## Coding Style & Naming Conventions

The project uses C++23 with `.clang-format` based on Google style: 2-space indentation, 120-column limit, sorted/regrouped includes. Run formatting before submitting broad C++ edits. `.clang-tidy` enables diagnostic, analyzer, modernize, performance, readability, and cppcoreguidelines checks. Classes use `CamelCase`, functions use `camelBack`, members use `lower_case_` with a trailing underscore for private members. Preserve intentional names with three i's, such as `HoloniightStyle`, `HoloniightTheme`, and `HoloniightPalette`.

## Testing Guidelines

Tests are GTest binaries registered with CTest. Add new C++ tests under `tests/test_*.cpp` and register them in `tests/CMakeLists.txt`. Keep Qt UI tests headless by setting `QT_QPA_PLATFORM=offscreen` and clearing theme/style overrides unless a test explicitly needs them. Palette changes must keep WCAG contrast tests passing or update the documented design deviations.

## Adding Theme Variants

New built-in themes must be added through the full scheme pipeline, not only by adding colors. Extend `ThemeSchemeKind`, `tokensForScheme()`, `colorModeForScheme()`, config scheme parsing, KDE color-scheme name fallback, and `tools/generate_holonight_colors.cpp`. Add generated `data/<scheme-id>.colors` files for KDE integration and update `docs/theme-tokens.md` with the new scheme IDs and display names. Keep public scheme IDs lowercase kebab-case, for example `holonight-mocha`, and display names title-cased, for example `HoloNight Mocha`.

When mapping external palettes such as Catppuccin, use official palette colors for base roles and document any derived HoloNight-only semantic roles such as hover, pressed, focus, overlay, glow, workspace, and ANSI bright colors. Add focused tests for exact documented token values, alias consistency, color-mode classification, resolver/config parsing, KDE fallback names, generated color files, and WCAG contrast.

## Commit & Pull Request Guidelines

Recent history uses conventional commits, for example `feat: semantic border & surface tokens`, `docs: add demo screenshots to README`, and `build(demo): add install target`. Use the same style: `type(scope): summary` when a scope helps. Pull requests should describe the behavior change, list validation commands run, link related issues or docs, and include screenshots when visual output changes.

## QML import and executable defaults

Application QML in `demo/` and `examples/controls-gallery/` imports `QtQuick.Controls as Controls` and qualifies
standard control instances, attached properties and enums. Do not import the `Holonight` style or Basic directly
in applications. Each executable embeds `:/qtquickcontrols2.conf` with `[Controls]` and `Style=Holonight`; preserve
explicit environment, command-line and configuration overrides and avoid imperative startup style selection.

Shared composites use their existing `QtQuick.Controls as C` namespace. Core and the popup geometry helper must
remain independent of runtime Controls and styles. Standard style implementations use Templates; explicit
compatibility-test fixtures may import competing styles. Use composite error/sizing APIs or guard optional
style-specific properties so explicit Fusion loads without invalid-property errors.

When changing examples, enable `BUILD_DEMO` and `BUILD_CONTROLS_GALLERY` and run their `startup_` tests plus
`holonight_package_install_test`. Run the QML policy checks for the provider and both example directories.
