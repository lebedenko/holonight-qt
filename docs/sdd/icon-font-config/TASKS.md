# SDD Tasks — icon-font-config

- [x] T-001: Update `holonighttheme.h` — replace `QFont font_` with five named members
  - REQs: REQ-F-003, REQ-F-004, REQ-F-005, REQ-F-006, REQ-F-007
  - Check: Header declares five private `const QFont` members (`font_system_`, `font_title_`, `font_fixed_`, `font_small_`, `font_mini_`) and removes single `font_` member.

- [x] T-002: Update `holonighttheme.cpp` — add `#include <QIcon>` in Qt header block
  - REQs: REQ-C-003
  - Check: QIcon header is included after other Qt headers and before system headers with no clang-format violations.

- [x] T-003: Update `holonighttheme.cpp` constructor — initialize five QFont members and call QIcon setters
  - REQs: REQ-F-001, REQ-F-002, REQ-F-003, REQ-F-004, REQ-F-005, REQ-F-006, REQ-F-007
  - Check: Constructor initializer list constructs all five font members with correct families and point sizes, and constructor body calls `QIcon::setThemeName("Papirus-Dark")` and `QIcon::setFallbackThemeName("breeze-dark")`.

- [x] T-004: Update `holonighttheme.cpp` — rewrite `font()` method to switch on role and return member pointers
  - REQs: REQ-F-003, REQ-F-004, REQ-F-005, REQ-F-006, REQ-F-007, REQ-F-008
  - Check: Method returns pointer to matching member for SystemFont/TitleBarFont/FixedFont/SmallFont/MiniFont, and delegates unrecognized roles to `QPlatformTheme::font(type)`.

- [x] T-005: Update `holonighttheme.cpp` — remove SystemIconThemeName and SystemIconFallbackThemeName cases from `themeHint()`
  - REQs: REQ-F-001, REQ-F-002
  - Check: `themeHint()` method no longer contains cases for `SystemIconThemeName` or `SystemIconFallbackThemeName`; both fall through to default.

- [x] T-006: Update `test_platformtheme_smoke.cpp` — fix `FontIsInterTenPt` test to expect 14pt
  - REQs: REQ-F-003
  - Check: Test renamed to `FontIsInterFourteenPt` and expects `pointSize() == 14` instead of 10.

- [x] T-007: Update `test_platformtheme_smoke.cpp` — remove or replace `IconThemeIsPapirus` test
  - REQs: REQ-F-001
  - Check: `IconThemeIsPapirus` test is removed or replaced, avoiding test failure when themeHint cases are deleted.

- [x] T-008: Add `tests/test_platformtheme_fonts.cpp` — test all five font roles and fallback delegation
  - REQs: REQ-F-003, REQ-F-004, REQ-F-005, REQ-F-006, REQ-F-007, REQ-F-008, REQ-F-009
  - Check: File contains separate test cases for each of five font roles asserting family/weight/pointSize, and separate test cases for three unspecified roles (MenuBarFont, PushButtonFont, LabelFont) each asserting nullptr return.

- [x] T-009: Add `tests/test_platformtheme_icons.cpp` — test icon theme names are set during construction
  - REQs: REQ-F-001, REQ-F-002, REQ-F-010
  - Check: File contains test that instantiates HoloniightTheme and asserts `QIcon::themeName() == "Papirus-Dark"` and `QIcon::fallbackThemeName() == "breeze-dark"`.

- [x] T-010: Update `tests/CMakeLists.txt` — add new test source files to holonight_tests executable
  - REQs: REQ-F-009, REQ-F-010
  - Check: CMakeLists.txt includes `test_platformtheme_fonts.cpp` and `test_platformtheme_icons.cpp` in the holonight_tests source list.

- [x] T-011: Verify all code changes pass cmake build, clang-tidy, and ctest
  - REQs: REQ-C-001, REQ-C-002, REQ-F-009, REQ-F-010
  - Check: `/verify` command completes with zero build errors, zero clang-tidy warnings on modified files, and all ctest tests pass with `QT_QPA_PLATFORM=offscreen`.

- [x] T-012: Manual visual verification — demo app displays correct fonts and icons
  - REQs: REQ-F-011
  - Check: Demo app built with `cmake -B build -DBUILD_DEMO=ON && cmake --build build` displays Inter and JetBrainsMono Nerd Font typography and Papirus-Dark icons without broken icon placeholders.

- [x] T-013: Decrease all font sizes by 2pt (post-review adjustment)
  - REQs: REQ-F-003, REQ-F-004, REQ-F-005, REQ-F-006, REQ-F-007
  - Check: holonighttheme.cpp uses 12/12/12/10/8pt; tests and SPEC.md updated to match; all ctest tests pass.
