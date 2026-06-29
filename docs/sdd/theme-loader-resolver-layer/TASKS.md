# SDD Tasks — theme-loader-resolver-layer

## Overview
Implement a theme resolution pipeline: ThemeLoader (config I/O) → ThemeResolver (mode/accent logic) → ColorTokens. Restructure plugin sources under `src/`, extend ThemeConfig with appearance options, and wire HoloniightPalette to use the new resolver.

## Task List

- [x] T-001: Restructure directories — create src/ subdirectory
  - REQs: REQ-1
  - Check: `src/CMakeLists.txt` exists and is included by root CMakeLists.txt

- [x] T-002: Migrate platformtheme plugin to src/platformtheme/
  - REQs: REQ-1
  - Check: `src/platformtheme/` contains all `.h` and `.cpp` files; original `platformtheme/` no longer exists

- [x] T-003: Migrate style plugin to src/style/
  - REQs: REQ-1
  - Check: `src/style/` contains all `.h` and `.cpp` files; original `style/` no longer exists

- [x] T-004: Update root CMakeLists.txt to reference new structure
  - REQs: REQ-1, REQ-7
  - Check: `cmake -B build` configures without "directory not found" errors; `cmake --build build` succeeds

- [x] T-005: Add ColorTokens operator== to palette.h
  - REQs: REQ-3
  - Check: `palette/holonight/palette.h` declares `operator==(const ColorTokens&) const noexcept = default;`

- [x] T-006: Add scheme, accent, transparency fields to ThemeConfig
  - REQs: REQ-2
  - Check: `config/holonight/config.h` declares `QString scheme;`, `QString accent;`, `qreal transparency;`

- [x] T-007: Update ThemeConfig::defaults() for new fields
  - REQs: REQ-2
  - Check: `config/config.cpp` defaults() returns config with scheme="light", accent="blue", transparency=1.0

- [x] T-008: Parse [appearance] and [effects] INI sections in ThemeConfig::load()
  - REQs: REQ-2
  - Check: `config/config.cpp` load() reads mode/scheme/accent from [appearance] and transparency from [effects]; existing tests pass unmodified

- [x] T-009: Create src/theme/ directory and CMakeLists.txt
  - REQs: REQ-4
  - Check: `src/theme/CMakeLists.txt` defines `holonight_theme` STATIC library; target links `holonight_palette`, `holonight_config`, `Qt6::Gui`

- [x] T-010: Implement ThemeLoader::load() method
  - REQs: REQ-4
  - Check: `src/theme/themeloader.h` declares `[[nodiscard]] ThemeConfig load() const;`; `.cpp` calls `ThemeConfig::load()`

- [x] T-011: Implement ThemeResolver::resolve() for mode detection
  - REQs: REQ-4
  - Check: `src/theme/themeresolver.h` declares `[[nodiscard]] ColorTokens resolve() const;`; method queries `QGuiApplication::styleHints()->colorScheme()` with null fallback to dark

- [x] T-012: Implement ThemeResolver accent override mapping
  - REQs: REQ-4
  - Check: Accent override logic produces correct hex values for cyan/blue/violet/yellow per specification table

- [x] T-013: Verify accent override table correctness for all four accents
  - REQs: REQ-4
  - Check: Unit tests confirm primary*, borderFocus, borderActive, focusRing, glow*Soft match table; non-accent slots identical to base tokens

- [x] T-014: Update HoloniightPalette constructor to use ThemeLoader
  - REQs: REQ-5
  - Check: `qml/holoniightpalette.cpp` constructor calls `ThemeLoader::load()` and `ThemeResolver::resolve()`

- [x] T-015: Implement reload() suppression in HoloniightPalette
  - REQs: REQ-5
  - Check: `HoloniightPalette::reload()` compares `newTok == tok_` and returns without emitting if equal

- [x] T-016: Update qml/CMakeLists.txt to link holonight_theme
  - REQs: REQ-5
  - Check: `qml/CMakeLists.txt` `target_link_libraries` includes `holonight_theme`

- [x] T-017: Create tests/test_theme_resolver.cpp with base structure
  - REQs: REQ-6
  - Check: `tests/test_theme_resolver.cpp` exists with TEST() macros and includes GTest, Qt, ColorTokens, ThemeResolver headers

- [x] T-018: Add mode resolution test cases
  - REQs: REQ-6
  - Check: Tests pass for: missing theme.conf→dark, invalid mode→dark, mode=dark, mode=light, mode=system headless→dark

- [x] T-019: Add accent override test cases (all four colors)
  - REQs: REQ-6
  - Check: Tests verify accent=cyan/blue/violet/yellow produce correct primary/border/focusRing slots; missing/invalid accent equals base mode

- [x] T-020: Add HoloniightPalette reload() integration tests
  - REQs: REQ-6
  - Check: Tests verify paletteChanged emitted on token change; signal suppressed when newTok == tok_

- [x] T-021: Add sizeof(ColorTokens) guard in test_theme_resolver.cpp
  - REQs: REQ-6
  - Check: File contains `static_assert(sizeof(ColorTokens) > 0);` to prevent silent operator== drift

- [x] T-022: Update tests/CMakeLists.txt with test registration and isolation
  - REQs: REQ-6
  - Check: `test_theme_resolver` registered with CTest; ENVIRONMENT property sets `QT_QPA_PLATFORMTHEME=;QT_STYLE_OVERRIDE=` for color tests

- [x] T-023: Update root CMakeLists.txt clang-tidy glob pattern
  - REQs: REQ-7
  - Check: clang-tidy target in root CMakeLists.txt uses `src/**/*.cpp` or GLOB_RECURSE instead of plugin-specific paths

- [ ] T-024: Full verification — build, tidy, test
  - REQs: REQ-1 through REQ-7
  - Check: `cmake --build build -j$(nproc) && cmake --build build --target tidy && QT_QPA_PLATFORM=offscreen ctest --test-dir build --output-on-failure` all succeed with zero warnings

## Dependency Map

```
T-001 ──┬─→ T-002 ──→ T-003 ──→ T-004
        │
        └─→ T-005 (independent)
        └─→ T-006 ──→ T-007 ──→ T-008 (independent)
        └─→ T-009 ──┬─→ T-010 ──┬─→ T-012 ──→ T-013
                    │           └─→ T-011 ──┘
                    └─→ (T-010, T-011 parallel)

T-013 ──→ T-014 ──→ T-015 ──→ T-016
T-010, T-011, T-012 ──→ T-017 ──→ T-018 ──→ T-019 ──→ T-020 ──→ T-021 ──→ T-022

T-004, T-022, T-008 ──→ T-023 ──→ T-024
```

## Verification Criteria

All tasks complete when:
1. Directory structure is `holonight-qt/{config,palette,qml,tools,demo,tests}/` and `holonight-qt/src/{platformtheme,style,theme}/`
2. `cmake --build build -j$(nproc)` compiles without errors
3. `cmake --build build --target tidy` reports zero warnings (Qt6's `-mno-direct-extern-access` stripped)
4. `QT_QPA_PLATFORM=offscreen ctest --test-dir build --output-on-failure` passes all 24 tests
5. Accent override table verified: all 4 colors × 6 token slots = 24 assertions in tests
6. No circular dependencies: `src/theme/` → `config/` → `palette/` (no reverse edges)
