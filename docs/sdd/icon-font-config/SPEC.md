# SPEC: Icon Theme and Font Configuration

**Feature:** Hardcoded icon theme and font configuration in `HoloniightTheme` QPlatformTheme plugin.

**Date:** 2026-05-20  
**Status:** Ready for implementation

---

## Overview

The `HoloniightTheme` class (in `platformtheme/`) shall provide hardcoded icon theme names and font configurations to ensure consistent typography and icon rendering across all holonight-themed Qt6 applications. All values are compile-time constants with no runtime configuration.

---

## Icon Theme

### REQ-F-001: Set primary icon theme name
**Statement:** The `HoloniightTheme` constructor shall call `QIcon::setThemeName("Papirus-Dark")` to configure the system icon theme.

**Acceptance criteria:**
- Constructor invokes `QIcon::setThemeName()` with the string `"Papirus-Dark"`
- Icon theme name is confirmed via `QIcon::themeName()` to return `"Papirus-Dark"`
- Applications using the platform theme access Papirus-Dark icons by default

### REQ-F-002: Set fallback icon theme name
**Statement:** The `HoloniightTheme` constructor shall call `QIcon::setFallbackThemeName("breeze-dark")` to provide a fallback icon source.

**Acceptance criteria:**
- Constructor invokes `QIcon::setFallbackThemeName()` with the string `"breeze-dark"`
- Fallback theme name is confirmed via `QIcon::fallbackThemeName()` to return `"breeze-dark"`
- Missing icons in Papirus-Dark resolve from breeze-dark

---

## Fonts

### REQ-F-003: Override font() method to provide system font
**Statement:** The `HoloniightTheme::font(FontType type)` method shall return an `Inter` regular 12pt font when `type` is `SystemFont`.

**Acceptance criteria:**
- Method returns a non-null `QFont` for `FontType::SystemFont`
- Font family is `"Inter"`
- Font weight is `QFont::Normal`
- Font point size is exactly `12`
- Test confirms font() returns this configuration

### REQ-F-004: Override font() method to provide title bar font
**Statement:** The `HoloniightTheme::font(FontType type)` method shall return an `Inter` regular 12pt font when `type` is `TitleBarFont`.

**Acceptance criteria:**
- Method returns a non-null `QFont` for `FontType::TitleBarFont`
- Font family is `"Inter"`
- Font weight is `QFont::Normal`
- Font point size is exactly `12`
- Test confirms font() returns this configuration

### REQ-F-005: Override font() method to provide fixed-width font
**Statement:** The `HoloniightTheme::font(FontType type)` method shall return a `JetBrainsMono Nerd Font` regular 12pt font when `type` is `FixedFont`.

**Acceptance criteria:**
- Method returns a non-null `QFont` for `FontType::FixedFont`
- Font family is `"JetBrainsMono Nerd Font"`
- Font weight is `QFont::Normal`
- Font point size is exactly `12`
- Test confirms font() returns this configuration

### REQ-F-006: Override font() method to provide small font
**Statement:** The `HoloniightTheme::font(FontType type)` method shall return an `Inter` regular 10pt font when `type` is `SmallFont`.

**Acceptance criteria:**
- Method returns a non-null `QFont` for `FontType::SmallFont`
- Font family is `"Inter"`
- Font weight is `QFont::Normal`
- Font point size is exactly `10`
- Test confirms font() returns this configuration

### REQ-F-007: Override font() method to provide mini font
**Statement:** The `HoloniightTheme::font(FontType type)` method shall return an `Inter` regular 8pt font when `type` is `MiniFont`.

**Acceptance criteria:**
- Method returns a non-null `QFont` for `FontType::MiniFont`
- Font family is `"Inter"`
- Font weight is `QFont::Normal`
- Font point size is exactly `8`
- Test confirms font() returns this configuration

### REQ-F-008: Delegate unrecognized font roles to base class
**Statement:** The `HoloniightTheme::font(FontType type)` method shall return `nullptr` (or call base class implementation) for any font role not listed in REQ-F-003 through REQ-F-007.

**Acceptance criteria:**
- Method returns `nullptr` or defers to parent `QPlatformTheme::font()`
- Unrecognized font roles (e.g., `MenuBarFont`, `PushButtonFont`) do not have hardcoded overrides
- Test confirms fallback behavior for at least three unspecified font roles

---

## Testing

### REQ-F-009: Unit test all font roles
**Statement:** The test suite shall include a unit test that verifies each font role (SystemFont, TitleBarFont, FixedFont, SmallFont, MiniFont) returns the correct font properties.

**Acceptance criteria:**
- Test file exists in `tests/` directory (e.g., `test_platformtheme_fonts.cpp`)
- Test instantiates `HoloniightTheme`
- Test calls `font(FontType)` for each of the five roles
- Test asserts correct family, weight, and point size for each
- Test passes when run with `QT_QPA_PLATFORM=offscreen ctest`

### REQ-F-010: Unit test icon theme names
**Statement:** The test suite shall include a unit test that verifies icon theme names are set correctly during `HoloniightTheme` construction.

**Acceptance criteria:**
- Test file exists in `tests/` directory (e.g., `test_platformtheme_icons.cpp`)
- Test instantiates `HoloniightTheme`
- Test confirms `QIcon::themeName()` is `"Papirus-Dark"`
- Test confirms `QIcon::fallbackThemeName()` is `"breeze-dark"`
- Test passes when run with `QT_QPA_PLATFORM=offscreen ctest`

### REQ-F-011: Manual visual verification
**Statement:** A developer shall manually verify that applications using the platform theme (via `QT_QPA_PLATFORMTHEME=holonight`) display Inter and JetBrainsMono Nerd Font typography and Papirus-Dark icons correctly.

**Acceptance criteria:**
- Developer runs a demo app (e.g., `cmake -B build -DBUILD_DEMO=ON && cmake --build build`)
- Demo app displays with holonight theme active
- All UI text uses Inter (or falls back to system default if unavailable)
- Fixed-width text (e.g., in code editor or terminal) uses JetBrainsMono Nerd Font
- Icons render from Papirus-Dark theme
- No broken icon placeholders appear

---

## Constraints

### REQ-C-001: C++23 compliance
**Statement:** Implementation shall use only C++23-compliant features with no compiler extensions.

**Acceptance criteria:**
- Code compiles with `CMAKE_CXX_EXTENSIONS OFF`
- No warnings from clang-tidy
- Compiles on the target platform (Linux, GCC/Clang)

### REQ-C-002: Column limit and indentation
**Statement:** All code shall respect a 120-character column limit and 2-space indentation.

**Acceptance criteria:**
- `.clang-format` validation passes on all modified files
- No lines exceed 120 characters
- All indentation is 2 spaces (no tabs)

### REQ-C-003: Include order
**Statement:** Source files shall follow the include order: local headers `"..."` first, Qt headers `<Q*>` second, system headers last.

**Acceptance criteria:**
- Includes in `.cpp` and `.h` files follow the prescribed order
- `.clang-format` does not flag include ordering issues
- Code review confirms compliance

### REQ-C-004: Restrict implementation scope
**Statement:** Changes shall be confined to the `platformtheme/` directory; no modifications to `style/`, `qml/`, or `palette/` directories.

**Acceptance criteria:**
- Git diff shows changes only in `platformtheme/`
- `HoloniightStyle` class is unmodified
- `holonight_qml` module is unmodified
- `palette/holonight/palette.h` is unmodified

### REQ-C-005: No new external dependencies
**Statement:** Implementation shall not introduce new dependencies beyond Qt6 Core and GUI modules already in use.

**Acceptance criteria:**
- CMakeLists.txt for `platformtheme/` target requires only existing `Qt6::Core` and `Qt6::Gui`
- No new `find_package()` or `FetchContent()` directives added
- Build succeeds without new library installations

### REQ-C-006: Class naming convention
**Statement:** The platform theme class shall be named `HoloniightTheme` with three consecutive 'i' characters, consistent with project convention.

**Acceptance criteria:**
- Class is declared and defined as `HoloniightTheme`
- No typos; exactly three 'i' characters in sequence
- Code review confirms intentional naming per project guidelines

### REQ-C-007: No runtime configuration
**Statement:** All icon theme names and font configurations shall be compile-time constants; no external config files, environment variable parsing, or user configuration shall modify these values.

**Acceptance criteria:**
- Constructor and `font()` method contain hardcoded string and font literals
- No config file I/O (e.g., `.ini`, JSON, YAML parsing)
- No environment variable checks for font or theme names
- Values are identical across all deployments (unless recompiled)

---

## Acceptance Summary

This feature is complete when:

1. ✅ `HoloniightTheme` constructor calls `QIcon::setThemeName()` and `QIcon::setFallbackThemeName()`
2. ✅ `font(FontType)` method returns correct fonts for all five roles
3. ✅ `font(FontType)` delegates other roles to base class
4. ✅ Unit tests verify all font roles and icon theme names
5. ✅ All code passes clang-format and clang-tidy
6. ✅ All tests pass with `QT_QPA_PLATFORM=offscreen ctest`
7. ✅ Developer confirms visual correctness in demo app
8. ✅ Implementation scope is limited to `platformtheme/`
