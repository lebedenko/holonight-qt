# Theme Loader & Resolver Layer Specification

**Project:** holonight-qt (Qt6 Dark Theme for Tiling Window Managers)  
**Version:** 1.0  
**Date:** 2026-06-29  
**Author:** Andrii L  

## Overview

This specification defines the Theme Loader & Resolver Layer, a new subsystem that abstracts theme configuration loading and color token resolution. It introduces two new classes (`ThemeLoader` and `ThemeResolver`) in a new `src/theme/` directory, restructures existing code under `src/`, extends `ThemeConfig` with new semantic fields, and ensures `HoloniightPalette` uses the new resolution path with change suppression.

---

## 1. Directory Restructure

### REQ-C-001: Reorganise source code into `src/` subdirectory

**Specification:** The build system shall relocate platform theme and style implementation code from project root into a structured `src/` directory tree.

**Details:**
- `platformtheme/` → `src/platformtheme/`
- `style/` → `src/style/`
- New directory: `src/theme/` for `ThemeLoader` and `ThemeResolver` classes
- Unchanged directories: `config/`, `palette/`, `qml/`, `tools/`, `tests/`, `demo/` (remain at project root)

**Acceptance Criteria:**
- `src/platformtheme/`, `src/style/`, and `src/theme/` directories exist
- `src/CMakeLists.txt` adds all three subdirectories via `add_subdirectory()`
- Root `CMakeLists.txt` calls `add_subdirectory(src)` and removed old calls to `add_subdirectory(platformtheme)` and `add_subdirectory(style)`
- `clang-tidy` glob in root `CMakeLists.txt` updated to scan `src/**/*.{cpp,h}` instead of `platformtheme/` and `style/` at root
- All build targets (`holonight_platform_theme`, `holonight_style`) reference files in new paths and build successfully
- All existing tests pass without modification

### REQ-C-002: Preserve installed plugin paths

**Specification:** The build system shall install compiled plugins to the same locations as before the restructure.

**Details:**
- `libholonight.so` (QStyle plugin) installed to `$PREFIX/lib/qt6/plugins/styles/`
- `libqholonight.so` (QPlatformTheme plugin) installed to `$PREFIX/lib/qt6/plugins/platformthemes/`

**Acceptance Criteria:**
- After `cmake --install`, both `.so` files exist in correct Qt plugin paths
- Plugin environment variables `QT_STYLE_OVERRIDE=holonight` and `QT_QPA_PLATFORMTHEME=holonight` load the plugins successfully
- Smoke test (e.g., running the demo app) confirms plugins are loaded and UI renders without errors

---

## 2. ThemeConfig Extensions

### REQ-F-001: Add `scheme` field to `ThemeConfig`

**Specification:** The `ThemeConfig` struct shall include a new `scheme` field to store the user's accent color preference.

**Details:**
- New field: `QString scheme` (or enum-backed variant)
- Valid values: `"cyan"`, `"blue"`, `"violet"`, `"yellow"`, empty string for none
- Default value: empty string (no override)
- Stored in memory but not yet acted upon by the resolver (reserved for future extension)

**Acceptance Criteria:**
- `ThemeConfig` has a readable `scheme` field
- Default-constructed `ThemeConfig()` returns empty/null `scheme`
- Field can be assigned and retrieved without compile errors
- Existing `ThemeConfig` tests still pass

### REQ-F-002: Add `accent` field to `ThemeConfig`

**Specification:** The `ThemeConfig` struct shall include a new `accent` field to store the user's accent color preference.

**Details:**
- New field: `QString accent` (or enum-backed variant)
- Valid values: `"cyan"`, `"blue"`, `"violet"`, `"yellow"`, empty string for none
- Default value: empty string (no override)
- Parsed from `[effects]` section of `theme.conf` under key `accent`

**Acceptance Criteria:**
- `ThemeConfig` has a readable `accent` field
- Default-constructed `ThemeConfig()` returns empty/null `accent`
- Field can be assigned and retrieved without compile errors
- Existing `ThemeConfig` tests still pass

### REQ-F-003: Add `transparency` field to `ThemeConfig`

**Specification:** The `ThemeConfig` struct shall include a new `transparency` field to store transparency/blur settings.

**Details:**
- New field: opaque value type (e.g., `bool`, `double`, or struct)
- Parsed from `[effects]` section of `theme.conf` under key `transparency`
- Default value: reasonable default (e.g., `false` or `1.0`)
- Stored in memory but not yet acted upon (reserved for future extension)

**Acceptance Criteria:**
- `ThemeConfig` has a readable `transparency` field
- Default-constructed `ThemeConfig()` returns sensible default value
- Field can be assigned and retrieved without compile errors
- Existing `ThemeConfig` tests still pass

### REQ-C-003: Maintain backward compatibility of `ThemeConfig` API

**Specification:** All existing `ThemeConfig` public members and methods shall remain unchanged and functional.

**Details:**
- `ThemeConfig::load()` must continue to work
- `ThemeConfig::defaults()` must continue to work
- `ThemeConfig::configFilePath()` must continue to work
- `ThemeConfig::resolvedColorMode()` must continue to work
- New fields are additions; no member removals or signature changes

**Acceptance Criteria:**
- All existing tests in `tests/test_config.cpp` pass without modification
- `ThemeConfig::load()` invocation on existing config files produces a valid `ThemeConfig` object
- `ThemeConfig::defaults()` returns a `ThemeConfig` with sensible defaults for all fields (old and new)

---

## 3. ThemeLoader

### REQ-F-004: Implement `ThemeLoader` class in `src/theme/`

**Specification:** The `ThemeLoader` class shall read and parse `~/.config/holonight/theme.conf` and return a `ThemeConfig` value object.

**Details:**
- Header: `src/theme/holonight/theme_loader.h`
- Implementation: `src/theme/theme_loader.cpp`
- Public method: `static ThemeConfig load()`
- Reads from `ThemeConfig::configFilePath()` (uses existing config location resolution)

**Acceptance Criteria:**
- `ThemeLoader` class is declared in `src/theme/holonight/theme_loader.h`
- `ThemeLoader::load()` is callable and returns a `ThemeConfig` object
- Compiles without errors or warnings

### REQ-F-005: ThemeLoader shall parse `[appearance]` section

**Specification:** The `ThemeLoader` class shall extract and normalize the `mode` key from the `[appearance]` section of `theme.conf`.

**Details:**
- INI section: `[appearance]`
- Key: `mode`
- Valid values in file: `"dark"`, `"light"`, `"system"` (case-insensitive)
- Normalization: store in `ThemeConfig::mode` as one of the three canonical values
- Fallback: if key missing or value invalid, default to `"dark"`

**Acceptance Criteria:**
- When `theme.conf` contains `[appearance]\nmode=dark`, returned `ThemeConfig::mode` is `"dark"`
- When `theme.conf` contains `[appearance]\nmode=light`, returned `ThemeConfig::mode` is `"light"`
- When `theme.conf` contains `[appearance]\nmode=system`, returned `ThemeConfig::mode` is `"system"`
- When `[appearance]` section or `mode` key is missing, returned `ThemeConfig::mode` is `"dark"`
- When `mode=INVALID`, returned `ThemeConfig::mode` is `"dark"`

### REQ-F-006: ThemeLoader shall parse `[effects]` section for `accent`

**Specification:** The `ThemeLoader` class shall extract the `accent` key from the `[effects]` section of `theme.conf`.

**Details:**
- INI section: `[effects]`
- Key: `accent`
- Valid values: `"cyan"`, `"blue"`, `"violet"`, `"yellow"`, or empty for none
- Case-insensitive
- Fallback: if key missing, default to empty string (no accent override)

**Acceptance Criteria:**
- When `theme.conf` contains `[effects]\naccent=cyan`, returned `ThemeConfig::accent` is `"cyan"`
- When `theme.conf` contains `[effects]\naccent=blue`, returned `ThemeConfig::accent` is `"blue"`
- When `[effects]` section is missing, returned `ThemeConfig::accent` is empty
- When `accent` key is missing, returned `ThemeConfig::accent` is empty
- When `accent=INVALID`, returned `ThemeConfig::accent` is empty or the invalid value is normalized to empty on resolver's acceptance of the value

### REQ-F-007: ThemeLoader shall parse `[effects]` section for `transparency`

**Specification:** The `ThemeLoader` class shall extract the `transparency` key from the `[effects]` section of `theme.conf`.

**Details:**
- INI section: `[effects]`
- Key: `transparency`
- Value type and semantics: implementation-defined (e.g., `bool`, `double` in range 0.0–1.0)
- Fallback: if key missing, use sensible default (e.g., `false` or `1.0`)

**Acceptance Criteria:**
- When `theme.conf` contains `[effects]\ntransparency=true`, returned `ThemeConfig::transparency` reflects the true value
- When `[effects]` section is missing, returned `ThemeConfig::transparency` is the default value
- No parse errors or crashes on malformed values

### REQ-F-008: Environment variable overrides config file

**Specification:** The `HOLONIGHT_APPEARANCE_MODE` environment variable shall override the `mode` value from `theme.conf`.

**Details:**
- If `HOLONIGHT_APPEARANCE_MODE` is set to `"dark"`, `"light"`, or `"system"`, use that value
- If not set, read from config file (per REQ-F-005)
- If set to invalid value, ignore and use config file value

**Acceptance Criteria:**
- When `HOLONIGHT_APPEARANCE_MODE=dark` is set, `ThemeLoader::load()` returns `ThemeConfig` with `mode="dark"` regardless of `theme.conf` value
- When `HOLONIGHT_APPEARANCE_MODE=light` is set, `ThemeLoader::load()` returns `ThemeConfig` with `mode="light"`
- When `HOLONIGHT_APPEARANCE_MODE=system` is set, `ThemeLoader::load()` returns `ThemeConfig` with `mode="system"`
- When `HOLONIGHT_APPEARANCE_MODE` is unset, `theme.conf` value is used
- When `HOLONIGHT_APPEARANCE_MODE=invalid`, config file value is used

### REQ-C-004: ThemeLoader integration with existing `ThemeConfig::load()`

**Specification:** The existing `ThemeConfig::load()` method shall either delegate to `ThemeLoader` internally or use a parallel path that produces identical results.

**Details:**
- Backward compatibility requirement: existing code calling `ThemeConfig::load()` must continue to work
- Either: `ThemeConfig::load()` internally calls `ThemeLoader::load()`, or
- Both: maintain separate but equivalent implementations
- Result must be identical (same `ThemeConfig` content)

**Acceptance Criteria:**
- Existing code calling `ThemeConfig::load()` produces a `ThemeConfig` object with `mode`, `scheme`, `accent`, `transparency` fields populated
- All existing tests in `tests/test_config.cpp` pass without modification
- Manual verification: calling `ThemeConfig::load()` and `ThemeLoader::load()` on the same config file produces identical results

---

## 4. ThemeResolver

### REQ-F-009: Implement `ThemeResolver` class in `src/theme/`

**Specification:** The `ThemeResolver` class shall accept a `ThemeConfig` value object and resolve it to an effective `ColorTokens` struct.

**Details:**
- Header: `src/theme/holonight/theme_resolver.h`
- Implementation: `src/theme/theme_resolver.cpp`
- Public method: `static ColorTokens resolve(const ThemeConfig &config)`
- Takes semantic user intent (`ThemeConfig`) and produces concrete color data (`ColorTokens`)

**Acceptance Criteria:**
- `ThemeResolver` class is declared in `src/theme/holonight/theme_resolver.h`
- `ThemeResolver::resolve(const ThemeConfig &)` is callable and returns a `ColorTokens` object
- Compiles without errors or warnings

### REQ-F-010: ThemeResolver shall resolve `system` mode

**Specification:** When `ThemeConfig::mode` is `"system"`, the `ThemeResolver` shall query Qt's system color scheme preference and resolve to dark or light accordingly.

**Details:**
- Use `QGuiApplication::styleHints()->colorScheme()` to detect system preference
- If system returns `Qt::ColorScheme::Dark`, use `darkTokens()`
- If system returns `Qt::ColorScheme::Light`, use `lightTokens()`
- Fallback: if no Qt application exists or preference is unknown, resolve to `"dark"`

**Acceptance Criteria:**
- When `ThemeConfig::mode="system"` and a `QGuiApplication` exists with dark preference, returned `ColorTokens` equals `darkTokens()`
- When `ThemeConfig::mode="system"` and a `QGuiApplication` exists with light preference, returned `ColorTokens` equals `lightTokens()`
- When `ThemeConfig::mode="system"` in a headless environment (no `QGuiApplication`), returned `ColorTokens` equals `darkTokens()`
- No crashes or assertion failures

### REQ-F-011: ThemeResolver shall select base tokens for `dark` mode

**Specification:** When `ThemeConfig::mode` is `"dark"`, the `ThemeResolver` shall return tokens from `darkTokens()`.

**Acceptance Criteria:**
- When `ThemeConfig::mode="dark"`, `ThemeResolver::resolve()` returns `ColorTokens` equal to `darkTokens()`
- Returned object's `primary`, `text`, `surface` slots match `darkTokens()` values

### REQ-F-012: ThemeResolver shall select base tokens for `light` mode

**Specification:** When `ThemeConfig::mode` is `"light"`, the `ThemeResolver` shall return tokens from `lightTokens()`.

**Acceptance Criteria:**
- When `ThemeConfig::mode="light"`, `ThemeResolver::resolve()` returns `ColorTokens` equal to `lightTokens()`
- Returned object's `primary`, `text`, `surface` slots match `lightTokens()` values

### REQ-F-013: ThemeResolver shall apply accent override for `cyan`

**Specification:** When `ThemeConfig::accent` is `"cyan"`, the `ThemeResolver` shall replace accent-related `ColorTokens` slots with cyan-derived values.

**Details:**
- Accent-related slots: `primary`, `primaryHover`, `primaryPressed`, `accentCyan`, `accentBlue`, `accentViolet`, `accentYellow`, `borderFocus`, `borderActive`, `focusRing`, `glowCyanSoft`
- Non-accent slots remain unchanged: `surface`, `surfaceVariant`, `text`, `textSecondary`, `borderPassive`, `borderSubtle`, `borderStrong`, `borderHover`, `borderUrgent`, `statusSuccess`, `statusWarning`, `statusError`, `statusInfo`, `hover`, `pressed`
- Accent values derived from a consistent cyan palette (existing or to be defined during implementation)

**Acceptance Criteria:**
- When `ThemeConfig::accent="cyan"`, returned `ColorTokens::primary` differs from `darkTokens().primary` (or `lightTokens().primary` for light mode)
- `ColorTokens::text` equals the base mode's `text` value (unchanged)
- `ColorTokens::surface` equals the base mode's `surface` value (unchanged)
- `ColorTokens::borderPassive` equals the base mode's `borderPassive` value (unchanged)

### REQ-F-014: ThemeResolver shall apply accent override for `blue`

**Specification:** When `ThemeConfig::accent` is `"blue"`, the `ThemeResolver` shall replace accent-related `ColorTokens` slots with blue-derived values.

**Details:**
- Same accent slots as cyan (REQ-F-013)
- Non-accent slots unchanged
- Blue values derived from a consistent blue palette

**Acceptance Criteria:**
- When `ThemeConfig::accent="blue"`, returned `ColorTokens::primary` differs from base mode's primary
- `ColorTokens::text` equals the base mode's `text` value (unchanged)
- `ColorTokens::surface` equals the base mode's `surface` value (unchanged)
- Returned tokens differ from cyan-accent tokens when both are applied to the same base mode

### REQ-F-015: ThemeResolver shall apply accent override for `violet`

**Specification:** When `ThemeConfig::accent` is `"violet"`, the `ThemeResolver` shall replace accent-related `ColorTokens` slots with violet-derived values.

**Details:**
- Same accent slots as cyan
- Non-accent slots unchanged

**Acceptance Criteria:**
- When `ThemeConfig::accent="violet"`, returned `ColorTokens::primary` differs from base mode's primary
- Non-accent slots match base mode
- Returned tokens differ from cyan and blue accent tokens

### REQ-F-016: ThemeResolver shall apply accent override for `yellow`

**Specification:** When `ThemeConfig::accent` is `"yellow"`, the `ThemeResolver` shall replace accent-related `ColorTokens` slots with yellow-derived values.

**Details:**
- Same accent slots as cyan
- Non-accent slots unchanged

**Acceptance Criteria:**
- When `ThemeConfig::accent="yellow"`, returned `ColorTokens::primary` differs from base mode's primary
- Non-accent slots match base mode
- Returned tokens differ from cyan, blue, and violet accent tokens

### REQ-F-017: ThemeResolver shall ignore invalid or missing accent

**Specification:** When `ThemeConfig::accent` is empty, missing, or unrecognized, the `ThemeResolver` shall not apply any accent override; returned tokens shall equal the base mode tokens.

**Acceptance Criteria:**
- When `ThemeConfig::accent=""` (empty), returned tokens equal base mode tokens (e.g., `darkTokens()` for dark mode)
- When `ThemeConfig::accent="invalid"`, returned tokens equal base mode tokens
- Returned tokens have no accent-specific modifications

### REQ-F-018: ThemeResolver shall store `scheme` and `transparency` without action

**Specification:** The `ThemeResolver` shall parse and store `ThemeConfig::scheme` and `ThemeConfig::transparency` fields but take no action on them during this release.

**Details:**
- Fields are validated and stored in `ThemeConfig` for future extension
- No color selection or rendering behavior depends on these fields yet
- Reserved for future work

**Acceptance Criteria:**
- `ThemeConfig::scheme` can be set and retrieved without error
- `ThemeConfig::transparency` can be set and retrieved without error
- Changing these fields does not affect returned `ColorTokens` in `ThemeResolver::resolve()`

### REQ-C-005: ThemeResolver replaces `tokensForMode()` as canonical path

**Specification:** `ThemeResolver::resolve()` shall become the canonical entry point for resolving a `ThemeConfig` to `ColorTokens` inside `HoloniightPalette` and related code.

**Details:**
- `HoloniightPalette::reload()` uses `ThemeResolver::resolve()` instead of `tokensForMode()`
- `tokensForMode()` may continue to exist for backward compatibility but is not the primary user-facing path
- All new code in `HoloniightPalette` and plugins uses the resolver

**Acceptance Criteria:**
- `HoloniightPalette::reload()` calls `ThemeResolver::resolve()`
- Returned `ColorTokens` are applied to build the palette
- Existing tests pass and UI behavior is unchanged

---

## 5. ColorTokens Equality

### REQ-F-019: Add `operator==` to `ColorTokens` struct

**Specification:** The `ColorTokens` struct shall define `operator==` to enable value comparison.

**Details:**
- Header: `palette/holonight/palette.h`
- Returns `true` if and only if all color slots are equal
- Enables change suppression in `HoloniightPalette::reload()`

**Acceptance Criteria:**
- `ColorTokens` objects can be compared using `==`
- `darkTokens() == darkTokens()` returns `true`
- `darkTokens() == lightTokens()` returns `false`
- Comparison compiles without errors

### REQ-C-006: Equality comparison must be exhaustive

**Specification:** The `operator==` implementation shall compare all color slots to ensure complete value equality.

**Details:**
- Every field in `ColorTokens` must be included in the comparison
- No shortcuts or partial comparisons

**Acceptance Criteria:**
- Code review confirms all `ColorTokens` fields are compared
- Unit test creates two tokens with identical colors and confirms equality
- Unit test creates two tokens differing in one field and confirms inequality

---

## 6. HoloniightPalette Reload

### REQ-F-020: `HoloniightPalette::reload()` shall use ThemeLoader and ThemeResolver

**Specification:** The `HoloniightPalette::reload()` method shall use `ThemeLoader` to load config and `ThemeResolver` to resolve `ColorTokens`.

**Details:**
- Call sequence: `ThemeLoader::load()` → `ThemeResolver::resolve()` → update palette
- Replace existing ad-hoc resolution logic with this standardized path
- Maintain existing public API

**Acceptance Criteria:**
- `HoloniightPalette::reload()` calls `ThemeLoader::load()` and `ThemeResolver::resolve()`
- Palette is updated with resolved tokens
- Existing tests pass

### REQ-F-021: `HoloniightPalette::reload()` shall suppress `paletteChanged` on no-op reload

**Specification:** The `HoloniightPalette::reload()` method shall not emit `paletteChanged` signal when the newly resolved `ColorTokens` equal the currently stored tokens.

**Details:**
- Use `operator==` to detect no-op reloads
- Emit `paletteChanged` only when tokens actually change
- Reduces unnecessary UI updates and redraws

**Acceptance Criteria:**
- Call `HoloniightPalette::reload()` twice with identical config → `paletteChanged` signal emitted only once (on first reload)
- Modify config to change accent, call `reload()` → `paletteChanged` signal emitted
- Monitor signal emission via test or debug spy and verify suppression works

### REQ-F-022: `HoloniightPalette` constructor uses ThemeResolver path

**Specification:** The `HoloniightPalette` constructor shall initialize the palette using `ThemeLoader` and `ThemeResolver`.

**Details:**
- Constructor calls `ThemeLoader::load()` to read config
- Constructor calls `ThemeResolver::resolve()` to get initial `ColorTokens`
- Maintains initialization order and consistency with `reload()`

**Acceptance Criteria:**
- `HoloniightPalette` can be constructed and produces a valid palette
- Initial palette colors match the resolved config
- No crashes or uninitialized data

### REQ-C-007: Preserve existing `HoloniightPalette` QML singleton API

**Specification:** All existing public properties, methods, and signals of `HoloniightPalette` shall remain unchanged.

**Details:**
- `reload()` invokable remains
- `paletteChanged` signal remains
- All color access properties remain
- No breaking changes to QML interface

**Acceptance Criteria:**
- Existing QML code invoking `HoloniightPalette.reload()` works without modification
- Existing QML code connecting to `paletteChanged` signal works without modification
- Existing tests pass

---

## 7. Tests

### REQ-F-023: Test suite for ThemeLoader and ThemeResolver

**Specification:** A new test file `tests/test_theme_resolver.cpp` shall contain comprehensive unit tests for `ThemeLoader` and `ThemeResolver` behaviour.

**Details:**
- File: `tests/test_theme_resolver.cpp`
- Framework: Qt Test (existing test infrastructure)
- Coverage: all requirements in sections 3 and 4 (ThemeLoader and ThemeResolver)

**Acceptance Criteria:**
- Test file exists and compiles
- `cmake --build build` includes `test_theme_resolver` target
- All tests pass: `QT_QPA_PLATFORM=offscreen ctest --test-dir build -R test_theme_resolver`

### REQ-F-024: Test missing or empty `theme.conf`

**Specification:** The test suite shall verify that `ThemeLoader` correctly handles missing or empty config file.

**Details:**
- Test case: `theme.conf` does not exist
- Test case: `theme.conf` exists but is empty
- Expected: both return `ThemeConfig` with `mode="dark"` (default)

**Acceptance Criteria:**
- Test creates a temporary directory without `theme.conf`
- `ThemeLoader::load()` returns `ThemeConfig::mode == "dark"`
- No crash or exception
- Returned object has sensible defaults for all fields

### REQ-F-025: Test invalid `appearance/mode` value

**Specification:** The test suite shall verify that invalid `mode` values fall back to dark.

**Details:**
- Test case: `[appearance]\nmode=INVALID`
- Expected: returned `ThemeConfig::mode == "dark"`

**Acceptance Criteria:**
- Test creates config with `mode=INVALID`
- `ThemeLoader::load()` returns `ThemeConfig::mode == "dark"`
- No crash or exception

### REQ-F-026: Test `mode=dark` resolution

**Specification:** The test suite shall verify that `mode=dark` is correctly parsed and resolves to dark tokens.

**Details:**
- Test case: config with `[appearance]\nmode=dark`
- Expected: `ThemeResolver::resolve()` returns tokens equal to `darkTokens()`

**Acceptance Criteria:**
- Test creates config with `mode=dark`
- `ThemeResolver::resolve()` returns `ColorTokens` equal to `darkTokens()`
- Tokens can be compared for equality

### REQ-F-027: Test `mode=light` resolution

**Specification:** The test suite shall verify that `mode=light` is correctly parsed and resolves to light tokens.

**Details:**
- Test case: config with `[appearance]\nmode=light`
- Expected: `ThemeResolver::resolve()` returns tokens equal to `lightTokens()`

**Acceptance Criteria:**
- Test creates config with `mode=light`
- `ThemeResolver::resolve()` returns `ColorTokens` equal to `lightTokens()`

### REQ-F-028: Test `mode=system` resolution in headless environment

**Specification:** The test suite shall verify that `mode=system` is correctly resolved to dark tokens in a headless (no Qt app) environment.

**Details:**
- Test case: config with `[appearance]\nmode=system`, no `QGuiApplication`
- Expected: `ThemeResolver::resolve()` returns tokens equal to `darkTokens()` (fallback)

**Acceptance Criteria:**
- Test runs in headless environment (`QT_QPA_PLATFORM=offscreen`)
- Test creates config with `mode=system`
- `ThemeResolver::resolve()` returns dark tokens (fallback)

### REQ-F-029: Test accent override for cyan

**Specification:** The test suite shall verify that `accent=cyan` produces cyan-themed tokens with unchanged non-accent slots.

**Details:**
- Test case: config with `[effects]\naccent=cyan` and `mode=dark`
- Expected: `primary`, `primaryHover`, etc. differ from dark defaults
- Expected: `text`, `surface`, `borderPassive`, etc. unchanged

**Acceptance Criteria:**
- Test creates config with `accent=cyan`
- `ThemeResolver::resolve()` returns tokens with cyan primary colors
- Returned `ColorTokens::text` equals `darkTokens().text`
- Returned `ColorTokens::surface` equals `darkTokens().surface`

### REQ-F-030: Test accent override for blue

**Specification:** The test suite shall verify that `accent=blue` produces blue-themed tokens.

**Acceptance Criteria:**
- Test creates config with `accent=blue`
- `ThemeResolver::resolve()` returns tokens with blue primary colors
- Non-accent slots match base mode
- Tokens differ from cyan-accent tokens

### REQ-F-031: Test accent override for violet

**Specification:** The test suite shall verify that `accent=violet` produces violet-themed tokens.

**Acceptance Criteria:**
- Test creates config with `accent=violet`
- `ThemeResolver::resolve()` returns tokens with violet primary colors
- Non-accent slots match base mode

### REQ-F-032: Test accent override for yellow

**Specification:** The test suite shall verify that `accent=yellow` produces yellow-themed tokens.

**Acceptance Criteria:**
- Test creates config with `accent=yellow`
- `ThemeResolver::resolve()` returns tokens with yellow primary colors
- Non-accent slots match base mode

### REQ-F-033: Test missing or invalid accent

**Specification:** The test suite shall verify that missing or invalid accent values produce tokens without accent override.

**Details:**
- Test case: config with no `[effects]` section
- Test case: config with `[effects]` but no `accent` key
- Test case: config with `[effects]\naccent=INVALID`
- Expected: all return tokens equal to base mode (no accent modification)

**Acceptance Criteria:**
- Test with missing accent: returned tokens equal base mode
- Test with invalid accent: returned tokens equal base mode
- No crash or exception

### REQ-F-034: Test `paletteChanged` emission on token change

**Specification:** The test suite shall verify that `HoloniightPalette::reload()` emits `paletteChanged` when resolved tokens differ from current tokens.

**Details:**
- Test case: reload with different config (e.g., change from dark to light)
- Expected: `paletteChanged` signal emitted

**Acceptance Criteria:**
- Test connects to `paletteChanged` signal
- Test calls `reload()` with config that changes tokens
- Signal is emitted exactly once
- Signal can be verified via `QSignalSpy` or equivalent

### REQ-F-035: Test `paletteChanged` suppression on no-op reload

**Specification:** The test suite shall verify that `HoloniightPalette::reload()` does NOT emit `paletteChanged` when resolved tokens are unchanged.

**Details:**
- Test case: reload with identical config (or reload twice without changing config)
- Expected: `paletteChanged` signal NOT emitted on second reload

**Acceptance Criteria:**
- Test connects to `paletteChanged` signal via `QSignalSpy`
- Test calls `reload()` with config that produces identical tokens
- Signal is not emitted on second/subsequent reload
- Signal was emitted on first load (sanity check)

### REQ-C-008: Existing config tests must not break

**Specification:** All existing tests in `tests/test_config.cpp` shall pass without modification.

**Details:**
- Backward compatibility requirement
- No changes to test code or expectations

**Acceptance Criteria:**
- `cmake --build build` includes existing test targets
- `QT_QPA_PLATFORM=offscreen ctest --test-dir build -R test_config` passes all tests
- No test modifications required

---

## Non-Functional Requirements

### REQ-NF-001: Code quality and style compliance

**Specification:** All code in `src/theme/` shall adhere to the project's style and quality standards.

**Details:**
- C++23 syntax and semantics
- Column limit: 120 characters
- Indent: 2 spaces
- Include order: local `"..."`, Qt `<Q*>`, system headers
- Naming: `CamelCase` for classes, `camelBack` for functions/methods, `lower_case_` for private members
- `[[nodiscard]]` on functions with return values that must be used
- No trailing return types

**Acceptance Criteria:**
- `cmake --build build --target tidy` reports zero warnings on `src/theme/**` files
- `clang-format --dry-run -i` reports no formatting issues
- Code review confirms style compliance

### REQ-NF-002: Build performance

**Specification:** The new `src/theme/` library shall not significantly increase build time.

**Details:**
- Library is static and header-only compatible where possible
- No circular dependencies
- Minimal Qt dependencies (only core types like `QString`, `QColor`)

**Acceptance Criteria:**
- Clean build time increase is less than 10% (measured against current main)
- Incremental rebuild after file change compiles in under 2 seconds

### REQ-NF-003: Backward compatibility

**Specification:** All public APIs in `palette/`, `config/`, and `platformtheme/` shall remain backward compatible.

**Details:**
- No removals or signature changes
- New classes are additions, not replacements
- Existing entry points (`ThemeConfig::load()`, `HoloniightPalette`) unchanged in public interface

**Acceptance Criteria:**
- Existing code consuming `ThemeConfig`, `ColorTokens`, `HoloniightPalette` compiles and runs without modification
- All existing tests pass

---

## Constraints

### REQ-C-009: Plugin metadata structure

**Specification:** Plugin JSON files (`style/holonightstyle.json`, `platformtheme/qholonight.json`) shall maintain the existing metadata structure.

**Details:**
- Must contain only `{ "Keys": ["holonight"] }`
- No `IID`, `MetaData`, or `className` keys
- Moved to `src/style/` and `src/platformtheme/` respectively

**Acceptance Criteria:**
- Plugin files exist in new locations with correct content
- Plugins load successfully after install
- `QPluginLoader::metaData()` shows correct structure

### REQ-C-010: Testing environment isolation

**Specification:** Tests that compare palette colors shall not be affected by system-wide installation of holonight plugins.

**Details:**
- Add `QT_QPA_PLATFORMTHEME=;QT_STYLE_OVERRIDE=` to test environment for color-comparison tests
- Prevents system-wide plugins from overriding test expectations

**Acceptance Criteria:**
- Test targets (`test_palette_contrast`, `test_platformtheme_fonts`, `test_platformtheme_smoke`) include environment isolation in CMakeLists.txt
- Tests pass in both isolated and installed environments

### REQ-C-011: CMake build system updates

**Specification:** All CMakeLists.txt files shall be updated to reflect the directory restructure and new `src/theme/` library.

**Details:**
- Root `CMakeLists.txt`: add `add_subdirectory(src)` and update glob
- New `src/CMakeLists.txt`: add three subdirectories
- New `src/theme/CMakeLists.txt`: define `holonight_theme` static library
- Update `target_link_libraries` in dependent targets

**Acceptance Criteria:**
- `cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON` succeeds
- `cmake --build build -j$(nproc)` succeeds with no errors
- All targets link correctly

---

## Acceptance & Verification

### Global Acceptance Criteria

All individual requirement ACs must pass:
- Unit tests in `tests/test_theme_resolver.cpp` pass
- Existing tests in `tests/test_config.cpp` and other test files pass
- `clang-format` and `clang-tidy` pass with zero warnings on new code
- Build succeeds: `cmake -B build -G Ninja && cmake --build build -j$(nproc)`
- Smoke test: demo app or installed plugins load and render without errors
- Plugins install to correct paths and are loadable via environment variables

### Pre-Commit Checklist

Before committing changes implementing this spec:

1. `cmake --build build -j$(nproc)` — zero errors
2. `cmake --build build --target tidy` — zero warnings on `src/theme/**` and modified files
3. `QT_QPA_PLATFORM=offscreen ctest --test-dir build --output-on-failure` — all tests pass
4. Manual verification: demo app loads and palette can be changed via config file
5. Verify installed plugins load: `QT_STYLE_OVERRIDE=holonight <qt-app>` and `QT_QPA_PLATFORMTHEME=holonight <qt-app>`

---

## References

- **CLAUDE.md:** Project architecture, C++ style, and Qt6 API pitfalls
- **Existing ThemeConfig:** `config/holonight/config.h`, `config/config.cpp`
- **ColorTokens:** `palette/holonight/palette.h`, `palette/palette.cpp`
- **HoloniightPalette:** `qml/src/holonightpalette.cpp`, `qml/src/holonightpalette.h`
- **Qt6 Documentation:** `QGuiApplication::styleHints()`, `QStyle`, `QPlatformTheme`
