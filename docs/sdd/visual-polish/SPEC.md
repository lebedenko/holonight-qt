# SPEC: Visual Polish — Corner Radius, Border De-Cyanification, Surface Elevation

**Feature:** Visual polish pass on the HoloNight Qt6 theme covering corner radius unification, passive border de-cyanification, and surface elevation refinement.

**Date:** 2026-05-20  
**Status:** Ready for implementation

---

## Overview

The HoloNight Qt6 theme shall undergo a visual polish pass addressing three design refinements:

1. **Corner Radius Unification** — Replace all hardcoded radius literals with a compile-time constant `kRadiusControl = 6.0`, ensuring all controls render with consistent rounded corners.
2. **Passive Border De-Cyanification** — Replace cyan `outline` color in passive/inactive control borders with muted blue-gray `borderPassive`, improving visual hierarchy and reducing cyan fatigue.
3. **Surface Elevation** — Lift list views, text fields, and item row backgrounds from `surface` (#10131f) to `surfaceVariant` (#161925), improving visual separation between passive and elevated surfaces.

These changes shall apply across the palette (`palette/palette.cpp`) and style (`style/holonightstyle.cpp`) layers, with all existing tests passing.

---

## 1. Corner Radius Unification

### REQ-F-001: Define corner radius constant
**Statement:** The file `style/holonightstyle.cpp` shall define a compile-time constant `constexpr qreal kRadiusControl = 6.0;` at the top of the file (after includes and before any function definitions).

**Acceptance criteria:**
- `kRadiusControl` is declared as `constexpr qreal` with value `6.0`
- The constant appears before all draw functions that use it
- Code compiles without warnings
- `kRadiusControl` is not declared in the header (only in the `.cpp` implementation)

---

### REQ-F-002: Replace radius literals in push button rendering
**Statement:** The `drawPushButtonBevelImpl()` function in `style/holonightstyle.cpp` shall replace all hardcoded radius literals (`3.0`, `4.0`, `2.0`) with `kRadiusControl` (or `kRadiusTight = 2.0` if used for focus ring highlights).

**Acceptance criteria:**
- `git diff style/holonightstyle.cpp | grep -E "^\-.*[0-9]\.[0-9].*painter->drawRoundedRect"` identifies all replaced draw calls
- All radius values in button drawing now reference `kRadiusControl`
- If a tighter radius (2.0) is used for focus ring, a separate `kRadiusTight` constant may be defined or a comment explains the hardcoded value
- Push buttons render with smooth 6.0-radius corners
- No regression in button appearance in visual tests

---

### REQ-F-003: Replace radius literals in panel button rendering
**Statement:** The `drawPanelButtonImpl()` function shall replace all hardcoded radius literals with `kRadiusControl`.

**Acceptance criteria:**
- All `painter->drawRoundedRect()` calls in `drawPanelButtonImpl()` use `kRadiusControl` or a documented exception
- Buttons on panels render with consistent corner radius
- Code compiles without warnings

---

### REQ-F-004: Replace radius literals in checkbox rendering
**Statement:** The `drawCheckBoxImpl()` function shall replace all hardcoded radius literals with `kRadiusControl`.

**Acceptance criteria:**
- Checkbox background and indicator corners use `kRadiusControl`
- `git diff` shows all numeric radius literals removed from checkbox drawing
- Checkboxes render with uniform corner treatment

---

### REQ-F-005: Replace radius literals in input field rendering
**Statement:** The `PE_PanelLineEdit` primitive element rendering shall replace all hardcoded radius literals with `kRadiusControl`.

**Acceptance criteria:**
- Line edit backgrounds, focus rings, and borders use `kRadiusControl`
- `PE_FrameLineEdit` (if separate) also uses `kRadiusControl`
- Text input fields display with consistent rounded corners

---

### REQ-F-006: Replace radius literals in group box frame rendering
**Statement:** The `PE_FrameGroupBox` primitive element rendering shall replace all hardcoded radius literals with `kRadiusControl`.

**Acceptance criteria:**
- Group box frames render corners using `kRadiusControl`
- Focus and non-focus states both use the same radius constant
- Group box appearance is consistent with other controls

---

### REQ-F-007: Replace radius literals in item view rendering
**Statement:** The `drawPanelItemViewImpl()` and `drawItemViewItemImpl()` functions shall replace all hardcoded radius literals with `kRadiusControl` (or `kRadiusTight` for focus ring highlights, with explanation).

**Acceptance criteria:**
- Item view backgrounds and selection indicators use `kRadiusControl`
- Focus ring highlighting may use a tighter radius with a documented reason (e.g., "2.0pt for visual emphasis")
- List items, tree items, and table cells render with consistent corner treatment

---

### REQ-F-008: Verify no remaining hardcoded radius literals
**Statement:** A post-implementation grep of `style/holonightstyle.cpp` shall find no remaining hardcoded floating-point radius values (other than `kRadiusControl` or `kRadiusTight` definitions) in draw functions.

**Acceptance criteria:**
- `grep -n "drawRoundedRect\|setRoundRadius\|border-radius" style/holonightstyle.cpp | grep -v kRadius` returns no matches (or only matches in comments explaining exceptions)
- All radius values are constants, not literals
- Visual inspection of diff confirms no radius numbers remain in code

---

## 2. Passive Border De-Cyanification

### REQ-F-010: Update QPalette::Mid to use borderPassive in palette builder
**Statement:** The `buildPalette()` function in `palette/palette.cpp` shall set `QPalette::Mid` (Active group) to `tok.borderPassive` instead of `tok.outline` for all three color groups (Active, Inactive, Disabled).

**Acceptance criteria:**
- `git diff palette/palette.cpp` shows `palette.setColor(QPalette::Active, QPalette::Mid, tok.borderPassive)`
- Inactive group also receives the same change (currently copies Active)
- Disabled group uses `tok.borderPassive`
- Color token value is `#565f89` (muted blue-gray)

---

### REQ-F-011: Update QPalette::Dark to use borderPassive in palette builder
**Statement:** The `buildPalette()` function shall set `QPalette::Dark` (Active, Inactive, and Disabled groups) to `tok.borderPassive`.

**Acceptance criteria:**
- `git diff palette/palette.cpp` shows `palette.setColor(QPalette::Active, QPalette::Dark, tok.borderPassive)` for all three groups
- Value is `#565f89`
- Previously unset Dark role now has an explicit assignment

---

### REQ-F-012: Update QPalette::Shadow to use borderPassive in palette builder
**Statement:** The `buildPalette()` function shall set `QPalette::Shadow` (all three groups) to `tok.borderPassive`.

**Acceptance criteria:**
- `git diff palette/palette.cpp` shows `palette.setColor(QPalette::Active, QPalette::Shadow, tok.borderPassive)` for Active, Inactive, and Disabled
- Shadow color is now `#565f89` instead of `tok.outlineVariant`
- All three color groups are updated identically

---

### REQ-F-013: Update standardPalette() to set Mid, Dark, Shadow to borderPassive
**Statement:** The `standardPalette()` method in `style/holonightstyle.cpp` (around line 41) shall explicitly assign `QPalette::Mid`, `QPalette::Dark`, and `QPalette::Shadow` to `tok.borderPassive` for the Active color group.

**Acceptance criteria:**
- Line 41 area now includes: `palette.setColor(QPalette::Active, QPalette::Mid, tok.borderPassive)`
- Similar lines set `Dark` and `Shadow` to `tok.borderPassive`
- `git diff` shows these three assignments (previously only `Mid` was set)
- Color is `#565f89`

---

### REQ-F-014: Verify outline token is not removed from palette
**Statement:** The `tok.outline` member shall remain in the `ColorTokens` struct and `darkTokens()` initialization after this refactor.

**Acceptance criteria:**
- `git diff palette/holonight/palette.h` does NOT include a line removing `outline` member
- `git diff palette/palette.cpp` does NOT include a line removing `tok.outline` initialization
- The token is marked deprecated with a comment if desired, but remains in the struct for backward compatibility
- `grep -n "tok.outline" palette/palette.cpp` returns zero matches (no longer used in role assignments)

---

### REQ-F-015: Verify color change in control rendering (visual)
**Statement:** A visual inspection of control rendering (buttons, frames, separators, input fields) shall show that borders are now muted blue-gray (#565f89) instead of cyan (#00a0c8).

**Acceptance criteria:**
- QML screenshot or demo app shows controls with `borderPassive` (#565f89) borders in inactive state
- No cyan borders visible on default/unfocused controls
- Focused and active states retain their distinct border colors (not changed by this requirement)
- Visual regression test (if available) passes

---

## 3. Surface Elevation

### REQ-F-020: Update QPalette::Base to use surfaceVariant in palette builder
**Statement:** The `buildPalette()` function in `palette/palette.cpp` shall set `QPalette::Base` to `tok.surfaceVariant` instead of `tok.surface` for the Active and Disabled color groups.

**Acceptance criteria:**
- `git diff palette/palette.cpp | grep -A2 "Base"` shows `palette.setColor(QPalette::Active, QPalette::Base, tok.surfaceVariant)`
- Disabled group is also updated identically
- Value is `#161925` (lighter than `surface` #10131f)
- Inactive group may inherit from Active (no separate Inactive::Base assignment needed)

---

### REQ-F-021: Update standardPalette() to set Base to surfaceVariant
**Statement:** The `standardPalette()` method in `style/holonightstyle.cpp` (around line 34) shall change `Base = tok.surface` to `Base = tok.surfaceVariant`.

**Acceptance criteria:**
- Line 34 now reads: `palette.setColor(QPalette::Active, QPalette::Base, tok.surfaceVariant)`
- Color value is `#161925`
- This elevates list views, text edit areas, and other viewport backgrounds above the window surface

---

### REQ-F-022: Update PE_PanelLineEdit fill brush to use surfaceVariant
**Statement:** The `PE_PanelLineEdit` primitive element rendering (approximately line 737) shall use `tok.surfaceVariant` for the background fill brush instead of `tok.surface`.

**Acceptance criteria:**
- `git diff style/holonightstyle.cpp | grep -B3 -A3 "PE_PanelLineEdit"` shows `painter->setBrush(tok.surfaceVariant)` instead of `painter->setBrush(tok.surface)`
- Line edit backgrounds render with `#161925` fill
- Text input fields appear elevated above the window background

---

### REQ-F-023: Update PE_PanelItemViewRow default background to use surfaceVariant
**Statement:** The `PE_PanelItemViewRow` or similar item view row rendering (approximately line 725) shall set the default (non-selected) row background to `tok.surfaceVariant` instead of `tok.surface`.

**Acceptance criteria:**
- `git diff style/holonightstyle.cpp | grep -B3 -A3 "PE_PanelItemViewRow"` shows the default row background uses `tok.surfaceVariant`
- Value is `#161925`
- List view and tree view rows match the viewport Base color (`surfaceVariant`) for visual cohesion
- Selected/highlighted rows retain their distinct colors (not changed by this requirement)

---

### REQ-F-024: Verify surface elevation in visual rendering
**Statement:** A visual inspection of list views, text fields, and item rows shall show that these elements now render with `surfaceVariant` (#161925) backgrounds, appearing elevated above the window background.

**Acceptance criteria:**
- Demo app or screenshot shows list items, text input fields with `#161925` background
- These elements are visually lighter than the window background (`#10131f`)
- No visual regression in contrast or readability
- Selected items and hover states remain distinct and readable

---

## 4. Contrast & WCAG Compliance

### REQ-NF-001: Verify borderPassive contrast on all surfaces
**Statement:** The `borderPassive` token (#565f89) shall maintain a contrast ratio ≥ 3:1 when used as a control border on all common surface backgrounds (background, surface, surfaceVariant, panel).

**Acceptance criteria:**
- Contrast ratio between `#565f89` and each surface color is computed using WCAG 2.0 formula
- All ratios are ≥ 3:1
- `tests/test_palette_contrast.cpp` includes a test case verifying this
- Test output reports actual ratios

---

### REQ-NF-002: Verify onSurface text contrast on surfaceVariant
**Statement:** Text rendered on the new `surfaceVariant` (#161925) background in conjunction with standard text color `onSurface` (#c0caf5) shall maintain a contrast ratio ≥ 4.5:1.

**Acceptance criteria:**
- Contrast ratio between `#c0caf5` and `#161925` is ≥ 4.5:1 (likely very high given the hue difference)
- Test case in `test_palette_contrast.cpp` verifies this
- No readability regression on elevated surfaces

---

### REQ-NF-003: No new contrast failures introduced
**Statement:** All existing contrast tests shall pass after the refactor; no new failures shall be introduced by color changes.

**Acceptance criteria:**
- `QT_QPA_PLATFORM=offscreen ctest --test-dir build -R contrast --output-on-failure` reports all passing
- If any test fails, the failure is investigated and documented (see REQ-F-050)

---

## 5. Build & Code Quality

### REQ-C-001: C++23 compliance
**Statement:** All modified code shall use only C++23-compliant features with no compiler extensions.

**Acceptance criteria:**
- Code compiles with `CMAKE_CXX_EXTENSIONS OFF`
- No new compiler warnings from GCC or Clang
- `cmake --build build -j$(nproc)` completes cleanly

---

### REQ-C-002: Code style enforcement
**Statement:** All modified code shall respect the 120-character column limit and 2-space indentation.

**Acceptance criteria:**
- `cmake --build build --target clang-format` reports no changes needed
- `cmake --build build --target tidy` (clang-tidy) reports zero new warnings
- `.clang-format` validation passes on all modified files
- No lines exceed 120 characters

---

### REQ-C-003: Include order compliance
**Statement:** Source files shall follow the include order: local headers `"..."` first, Qt headers `<Q*>` second, system headers last.

**Acceptance criteria:**
- Any new or modified includes in `palette/palette.cpp` and `style/holonightstyle.cpp` follow this order
- `.clang-format` does not flag ordering issues
- Code review confirms compliance

---

### REQ-C-004: No new external dependencies
**Statement:** The refactor shall not introduce new external dependencies; only C++23, Qt6 Core, and Qt6 Gui shall be used.

**Acceptance criteria:**
- CMakeLists.txt is unchanged (no new `find_package()` or `FetchContent()`)
- `build/compile_commands.json` shows no new linked libraries
- Build succeeds without additional package installations

---

### REQ-C-005: Naming conventions
**Statement:** All new constants and functions shall follow project naming conventions: `kRadiusControl` (const, `k` prefix), `camelBack()` for functions, `CamelCase` for classes.

**Acceptance criteria:**
- `kRadiusControl` uses the `k` prefix (not `gRadiusControl` or `RADIUS_CONTROL`)
- No new class definitions introduced
- Variable names follow `lower_case_` pattern for private members (if any)
- Code review confirms naming consistency with existing style

---

## 6. Testing

### REQ-F-030: Verify all tests pass with modified palette
**Statement:** All unit and integration tests in `tests/` shall pass after the palette and style modifications.

**Acceptance criteria:**
- `QT_QPA_PLATFORM=offscreen ctest --test-dir build --output-on-failure` reports 100% tests passing
- No failures introduced by color or radius changes
- If a test fails, root cause is investigated and either the code or test is corrected

---

### REQ-F-031: Verify button rendering in QStyle tests
**Statement:** Existing QStyle button rendering tests shall confirm that buttons render with `kRadiusControl` (6.0) radius and no visual regression.

**Acceptance criteria:**
- `test_style_*.cpp` (if any) includes button rendering tests
- Test passes with the new radius constant
- Visual output (if snapshot tested) matches expected appearance with 6.0-radius corners

---

### REQ-F-032: Verify palette color assignments in unit tests
**Statement:** Palette unit tests shall confirm that `QPalette::Mid`, `Dark`, and `Shadow` are now `borderPassive` and `Base` is now `surfaceVariant`.

**Acceptance criteria:**
- Test calls `buildPalette()` and verifies:
  - `palette.color(QPalette::Active, QPalette::Mid) == tok.borderPassive`
  - `palette.color(QPalette::Active, QPalette::Dark) == tok.borderPassive`
  - `palette.color(QPalette::Active, QPalette::Shadow) == tok.borderPassive`
  - `palette.color(QPalette::Active, QPalette::Base) == tok.surfaceVariant`
- Test passes in Release build

---

### REQ-F-033: Manual visual verification of controls
**Statement:** A developer shall manually verify that all control types (buttons, input fields, checkboxes, group boxes, list items) render correctly with the new radius and colors.

**Acceptance criteria:**
- Demo app is built with `cmake -B build -DBUILD_DEMO=ON && cmake --build build`
- Demo displays all control types (button, checkbox, input field, group box, list view, tree view, combo box)
- Visually inspect:
  - All controls have consistent 6.0pt corner radius
  - Borders on inactive controls are muted blue-gray (#565f89), not cyan
  - List views and text fields have elevated (#161925) backgrounds
  - Focused, hovered, and active states remain visually distinct and readable
- No visual regressions compared to the previous release

---

## 7. Scope & Constraints

### REQ-C-010: Modification scope
**Statement:** Changes shall be limited to the following files: `palette/palette.cpp`, `style/holonightstyle.cpp`. No modifications to headers, QML, or other layers.

**Acceptance criteria:**
- `git diff --name-only` returns exactly two modified files (or three if palette.h is needed for comment updates)
- No changes to `qml/`, `platformtheme/`, or test source files (test behavior may change, but source is not modified)
- `palette/holonight/palette.h` and `style/holonightstyle.h` are not modified (unless adding a comment)

---

### REQ-C-011: No palette token additions or removals
**Statement:** The `ColorTokens` struct shall not gain new members or lose existing members as a result of this refactor.

**Acceptance criteria:**
- `git diff palette/holonight/palette.h | grep -E "^\+\s*QColor|^\-\s*QColor"` returns zero matches
- All six color groups remain unchanged
- No new tokens introduced; existing tokens are repurposed
- `tok.outline` and `tok.outlineVariant` remain in the struct (though unused in palette assignments)

---

### REQ-C-012: No visual impact on active/focused/pressed states
**Statement:** Changes shall not alter the rendering of controls in active, focused, or pressed states; only passive/default and selected/elevated states are affected.

**Acceptance criteria:**
- Focused controls still use `focusRing` color for keyboard focus indicators
- Pressed/active buttons and items retain their distinct pressed state colors
- Hover state borders remain unchanged (if using `borderHover`, that was a prior refactor)
- Only default (unfocused, unselected) state borders and surfaces are modified

---

## 8. Acceptance Summary

This feature is complete when:

1. ✅ `kRadiusControl = 6.0` constant is defined and used in all draw functions
2. ✅ All hardcoded radius literals (3.0, 4.0, 2.0) in buttons, input fields, checkboxes, group boxes, and list items are replaced with `kRadiusControl`
3. ✅ `QPalette::Mid`, `Dark`, `Shadow` use `borderPassive` (#565f89) in both `buildPalette()` and `standardPalette()`
4. ✅ `QPalette::Base` uses `surfaceVariant` (#161925) in both `buildPalette()` and `standardPalette()`
5. ✅ `PE_PanelLineEdit` and `PE_PanelItemViewRow` fill backgrounds use `surfaceVariant`
6. ✅ `tok.outline` token remains in the struct (not removed)
7. ✅ All contrast tests pass (borderPassive ≥ 3:1, text on surfaceVariant ≥ 4.5:1)
8. ✅ No new clang-tidy warnings or formatting issues
9. ✅ All unit and integration tests pass with `QT_QPA_PLATFORM=offscreen ctest`
10. ✅ Manual visual verification confirms:
    - Consistent corner radius across all controls
    - Muted blue-gray borders on inactive controls (no cyan)
    - Elevated list/text field backgrounds
    - No visual regressions in focused/pressed/active states
11. ✅ Build completes cleanly: `cmake --build build -j$(nproc)` with zero warnings
12. ✅ Scope limited to `palette/palette.cpp` and `style/holonightstyle.cpp`
