# SPEC — Semantic Border & Surface Tokens

## 1. Goal

Refactor HoloNight's palette to eliminate visual confusion between passive (inactive) and active UI states by:
- Introducing five new semantically-named border tokens (`borderPassive`, `borderHover`, `borderFocus`, `borderActive`, `borderUrgent`)
- Realigning surface elevation tokens to match the design system while maintaining WCAG AA contrast
- Deprecating generic `outline` and `outlineVariant` tokens
- Updating all ~30 usage sites in QStyle and QML components
- Documenting any design-to-implementation deviations

## 2. Non-Goals

- Border radius changes
- Animation or motion effects
- New QML components or component restructuring
- Changes to font, spacing, or iconography
- Refactoring the `ColorTokens` struct itself (only additions and deprecation markers)

## 3. Requirements

### 3.1 Border Token Definitions

**REQ-F-001: borderPassive token shall be defined in ColorTokens struct**

**Statement:** The `palette/holonight/palette.h` `ColorTokens` struct shall include a `borderPassive` member initialized to `0x565f89` (muted gray-blue).

**Acceptance criteria:**
- `ColorTokens::borderPassive` is declared in the struct
- `darkTokens()` initializes `borderPassive` to hex value `0x565f89`
- The color value is documented as "passive frame border — for inactive controls, list views, separators"
- Build completes without warnings
- `palette_test.cpp` can read `borderPassive` from `darkTokens()`

---

**REQ-F-002: borderHover token shall be defined in ColorTokens struct**

**Statement:** The `ColorTokens` struct shall include a `borderHover` member initialized to `0x7dcfff` at approximately 30% opacity.

**Acceptance criteria:**
- `ColorTokens::borderHover` is declared in the struct
- `darkTokens()` initializes `borderHover` to hex value `0x7dcfff` with alpha channel ≈ 77 (30% of 255)
- Documentation clarifies this is a semi-transparent overlay color
- Contrast test can extract the alpha value for validation

---

**REQ-F-003: borderFocus token shall be defined in ColorTokens struct**

**Statement:** The `ColorTokens` struct shall include a `borderFocus` member with the same value as the existing `focusRing` token.

**Acceptance criteria:**
- `ColorTokens::borderFocus` is declared in the struct
- `darkTokens()` initializes `borderFocus` to the same value as `focusRing` (hex value `0x00e0ff`)
- A comment in the code documents that `borderFocus == focusRing` by design
- No duplicate definition of the color value

---

**REQ-F-004: borderActive token shall be defined in ColorTokens struct**

**Statement:** The `ColorTokens` struct shall include a `borderActive` member initialized to `0x7aa2f7` (blue accent).

**Acceptance criteria:**
- `ColorTokens::borderActive` is declared in the struct
- `darkTokens()` initializes `borderActive` to hex value `0x7aa2f7`
- Documentation describes this as the "selected/active state border"
- The value is distinct from `primary` and `focusRing`

---

**REQ-F-005: borderUrgent token shall be defined in ColorTokens struct**

**Statement:** The `ColorTokens` struct shall include a `borderUrgent` member initialized to `0xf7768e` (red accent).

**Acceptance criteria:**
- `ColorTokens::borderUrgent` is declared in the struct
- `darkTokens()` initializes `borderUrgent` to hex value `0xf7768e`
- Documentation describes this as "critical/destructive/urgent border"
- The value is visually distinct from `error` token (if they differ)

---

### 3.2 Border Token Wiring — QStyle

**REQ-F-010: HoloniightStyle shall use borderPassive for inactive frame borders**

**Statement:** When rendering a frame or control border in the inactive state, `HoloniightStyle` shall use `tok.borderPassive` instead of `tok.outline`.

**Acceptance criteria:**
- Grep confirms all instances of `tok.outline` used for inactive frame borders are replaced with `tok.borderPassive`
- At least 15 replacements are made in `style/holonightstyle.cpp`
- Unit tests for frame rendering verify the correct color is applied
- Builds without clang-tidy warnings

---

**REQ-F-011: HoloniightStyle shall use borderHover for hovered control borders**

**Statement:** When rendering a control border in the hovered state, `HoloniightStyle` shall use `tok.borderHover` instead of a hardcoded or derived color.

**Acceptance criteria:**
- Code sites for hover-state borders are identified in `holonightstyle.cpp`
- All hover border renderings use `tok.borderHover`
- Style tests verify hover state applies the correct semi-transparent border
- No hardcoded `0x7dcfff` values exist outside the token definition

---

**REQ-F-012: HoloniightStyle shall use borderFocus for keyboard focus rings**

**Statement:** When rendering a keyboard focus indicator, `HoloniightStyle` shall use `tok.borderFocus`.

**Acceptance criteria:**
- Focus ring rendering in `drawComplexControl()` or `drawPrimitive()` uses `tok.borderFocus`
- Unit test verifies focus state border color matches `darkTokens().borderFocus`
- Focus ring remains visually distinct from hover state

---

**REQ-F-013: HoloniightStyle shall use borderActive for selected/active state borders**

**Statement:** When rendering a control border in the selected or active state, `HoloniightStyle` shall use `tok.borderActive`.

**Acceptance criteria:**
- Selection/active state borders in menus, list views, and tabs use `tok.borderActive`
- At least 3 separate code sites updated in `holonightstyle.cpp`
- Unit tests verify selected state applies `borderActive`

---

**REQ-F-014: HoloniightStyle shall use borderUrgent for critical/error state borders**

**Statement:** When rendering a control border for error, warning, or destructive states, `HoloniightStyle` shall use `tok.borderUrgent`.

**Acceptance criteria:**
- Error/destructive state borders are identified and updated to use `tok.borderUrgent`
- At least 1 site in `holonightstyle.cpp` updated
- Unit test verifies error state border color is correct

---

### 3.3 Border Token Wiring — QML

**REQ-F-020: QML components shall use borderPassive for inactive element borders**

**Statement:** When a QML component renders an inactive border (Button in default state, Frame, Separator), it shall reference `token.borderPassive` instead of `token.outline`.

**Acceptance criteria:**
- Grep finds all `token.outline` references in `qml/*.qml`
- At least 10 replacements with `token.borderPassive` are made
- QML files that do not reference borders are not modified
- Build completes; QML syntax is valid (verified by `cmake --build`)

---

**REQ-F-021: QML components shall use borderHover for hovered element borders**

**Statement:** When a QML component enters the hovered state, its border shall reference `token.borderHover`.

**Acceptance criteria:**
- Hover state bindings in Button, delegate, and interactive elements use `token.borderHover`
- At least 3 components updated
- Visual inspection (via demo app or snapshot) shows hover border is semi-transparent cyan

---

**REQ-F-022: QML components shall use borderFocus for keyboard focus indicators**

**Statement:** When a QML component has keyboard focus, its focus ring/border shall reference `token.borderFocus`.

**Acceptance criteria:**
- QML focus state styling uses `token.borderFocus`
- Focus indicator is visually distinct from hover state
- At least 2 components (e.g., Button, TextField) updated

---

**REQ-F-023: QML components shall use borderActive for selected element borders**

**Statement:** When a QML component is selected or active, its border shall reference `token.borderActive`.

**Acceptance criteria:**
- Selected state in CheckBox, ComboBox, and custom delegates uses `token.borderActive`
- At least 3 components updated
- Selected items visually differ from default state

---

**REQ-F-024: QML components shall use borderUrgent for error/destructive state borders**

**Statement:** When a QML component indicates an error or destructive state, its border shall reference `token.borderUrgent`.

**Acceptance criteria:**
- Error indicators and destructive buttons (if any) use `token.borderUrgent`
- At least 1 component updated
- Red border is visually applied in demo or tests

---

### 3.4 Surface Elevation Alignment

**REQ-F-030: Surface tokens shall be realigned to design system target values**

**Statement:** The `ColorTokens` struct shall be updated to align surface elevation tokens with the design system, subject to WCAG AA contrast requirements:
- `background` target: `#10131f` (or closest passing value)
- `surface` target: `#161925` (or closest passing value)
- `surfaceElevated` target: `#1a1b26` (or closest passing value)
- `panel` target: `#24283b` (or closest passing value)

**Acceptance criteria:**
- `darkTokens()` is updated with the target values or justified fallbacks
- Each surface token is accompanied by a comment documenting the design system target
- If a target value fails WCAG AA contrast, the actual value is noted (see REQ-F-050)
- Build completes; no palette tests fail due to token value changes

---

**REQ-F-031: Surface elevation tokens shall maintain progressive lightness hierarchy**

**Statement:** The surface tokens shall be ordered by lightness such that `background < surface < surfaceElevated < panel`, reflecting visual hierarchy.

**Acceptance criteria:**
- Luminance values of the four tokens are extracted (via HSL or relative brightness)
- Verify ordering: `lum(background) < lum(surface) < lum(surfaceElevated) < lum(panel)`
- A comment in `palette.h` explicitly documents this ordering constraint

---

### 3.5 QStyle Surface Audit

**REQ-F-040: HoloniightStyle shall use surface for window and base element backgrounds**

**Statement:** When rendering the background of a window or base control (QWidget), `HoloniightStyle` shall use `tok.surface` (not `surfaceVariant` or other elevation tokens).

**Acceptance criteria:**
- Audit `holonightstyle.cpp` PE_Widget, PE_FrameWindow, and base backgrounds
- At least 3 sites confirmed or updated to use `tok.surface`
- A code comment documents the surface token choice for each site
- No `surfaceVariant` is used for base backgrounds

---

**REQ-F-041: HoloniightStyle shall use surfaceElevated for elevated control backgrounds**

**Statement:** When rendering the background of an elevated control (card, container, button), `HoloniightStyle` shall use `tok.surfaceElevated`.

**Acceptance criteria:**
- Audit control backgrounds in `drawControl()` (buttons, frames, panels)
- At least 5 sites identified or updated
- Visual hierarchy shows elevated controls are lighter than base surface
- Contrast test passes for text on `surfaceElevated`

---

**REQ-F-042: HoloniightStyle shall use panel for popup and tooltip backgrounds**

**Statement:** When rendering popup (menu, dropdown, combobox popup), tooltip, or other floating surface backgrounds, `HoloniightStyle` shall use `tok.panel`.

**Acceptance criteria:**
- Audit `PE_PanelMenu`, tooltip rendering, and popup backgrounds
- At least 3 sites identified or updated to use `tok.panel`
- Popups are visually distinct from base windows
- Contrast test verifies text readability on `panel`

---

### 3.6 Contrast & WCAG Compliance

**REQ-NF-001: All new border tokens shall meet WCAG AA contrast minimums**

**Statement:** Each of the five new border tokens (`borderPassive`, `borderHover`, `borderFocus`, `borderActive`, `borderUrgent`) shall achieve a contrast ratio of at least 3:1 when used as a border on the most common surface backgrounds (window, surface, surfaceElevated, panel).

**Acceptance criteria:**
- `tests/test_palette_contrast.cpp` includes contrast checks for all five border tokens
- Contrast ratio is computed for each border token against each surface token using WCAG 2.0 formula
- All checks pass with ratio ≥ 3:1 for UI component borders
- Test output reports the actual contrast ratio for each pair

---

**REQ-NF-002: All updated surface tokens shall meet WCAG AA contrast minimums for text**

**Statement:** Text rendered on each surface token (background, surface, surfaceElevated, panel) in conjunction with standard text colors (primary text, secondary text, disabled text) shall achieve a contrast ratio of at least 4.5:1.

**Acceptance criteria:**
- `test_palette_contrast.cpp` verifies contrast between each surface token and text color tokens
- All checks pass with ratio ≥ 4.5:1
- Disabled text contrast requirement is noted separately (if applicable)
- Test reports all passing values

---

**REQ-NF-003: Semi-transparent border tokens shall not degrade contrast below minimums**

**Statement:** When `borderHover` (30% opacity) is composited over a surface background, the resulting effective color shall maintain contrast ratio ≥ 3:1 with foreground elements.

**Acceptance criteria:**
- Contrast calculation accounts for alpha blending (`C_effective = C_overlay * alpha + C_background * (1 - alpha)`)
- Test includes composited contrast verification for `borderHover`
- If the minimum is not met, actual ratio is logged and deviation documented (see REQ-F-050)

---

### 3.7 Deviation Logging

**REQ-F-050: Any color deviation from design system targets shall be documented**

**Statement:** If a color value diverges from its design system target (due to WCAG AA contrast requirements or other constraints), `docs/holonight-design-deviations.md` shall record the deviation with a reason.

**Acceptance criteria:**
- File `docs/holonight-design-deviations.md` is created (if it does not exist) or updated
- Each deviation entry includes: target value, actual value, reason, WCAG ratio (if applicable)
- Format is consistent and human-readable (Markdown table or list)
- Entry for each of the four surface tokens (if any differ from design system target)
- Entry for any border token if contrast requirement forced a different value

---

**REQ-F-051: Deviations file shall be reviewed and approved**

**Statement:** The `docs/holonight-design-deviations.md` file shall be committed to the repository as part of this refactor and serve as the source of truth for palette differences.

**Acceptance criteria:**
- File is tracked in git
- File is linked or referenced from the main design system documentation
- File is reviewed in code review before merge

---

### 3.8 Deprecation

**REQ-C-001: outline and outlineVariant tokens shall be marked deprecated**

**Statement:** The `ColorTokens` struct members `outline` and `outlineVariant` shall be retained but annotated with deprecation comments explaining that new code shall use the semantic border tokens (`borderPassive`, `borderHover`, `borderFocus`, `borderActive`, `borderUrgent`).

**Acceptance criteria:**
- Both `outline` and `outlineVariant` remain in the struct (not removed)
- Each has a C++ comment: `// DEPRECATED: use borderPassive, borderHover, borderFocus, borderActive, or borderUrgent instead`
- `darkTokens()` function includes the same deprecation comment for each initialization
- No new code introduced in this refactor uses `outline` or `outlineVariant`

---

**REQ-C-002: Existing outline and outlineVariant usage outside this refactor scope shall not be modified**

**Statement:** Usage of `outline` or `outlineVariant` in components or sites not identified in REQ-F-010 through REQ-F-024 shall be left unchanged to minimize risk.

**Acceptance criteria:**
- Grep confirms only the ~30 identified sites are updated
- Any remaining `outline` usage is documented (e.g., "future refactor" comment)
- No unintended changes to out-of-scope code

---

### 3.9 Test Coverage

**REQ-F-060: Palette tests shall verify all new tokens are initialized**

**Statement:** `tests/test_palette.cpp` or `tests/palette_test.cpp` shall include tests that verify each of the five new border tokens is initialized to the correct value by `darkTokens()`.

**Acceptance criteria:**
- Test case exists for each token: `borderPassive`, `borderHover`, `borderFocus`, `borderActive`, `borderUrgent`
- Each test asserts the expected hex value
- Test passes in Release build (`QT_QPA_PLATFORM=offscreen`)

---

**REQ-F-061: Contrast tests shall validate all new and modified tokens**

**Statement:** `tests/test_palette_contrast.cpp` shall be updated to include contrast checks for all five new border tokens and all updated surface elevation tokens.

**Acceptance criteria:**
- Test includes minimum 5 new test cases (one per border token) plus additional for surfaces
- Each test case verifies contrast ratio using WCAG 2.0 formula
- All tests pass (ratio ≥ 3:1 for borders, ≥ 4.5:1 for text on surfaces)
- Test output is human-readable and includes actual ratios (not just pass/fail)

---

**REQ-F-062: QStyle and QML visual smoke tests shall pass**

**Statement:** Existing QStyle and QML smoke tests shall continue to pass after the refactor, confirming no regressions in rendering.

**Acceptance criteria:**
- `ctest --test-dir build --output-on-failure` runs all tests and reports no failures
- If any test fails, the failure is investigated and root cause is fixed or test is updated
- Test run includes at least one QStyle test (e.g., button rendering) and one QML test

---

**REQ-F-063: Clang-tidy checks shall pass**

**Statement:** The refactored code shall pass all clang-tidy checks configured in the project.

**Acceptance criteria:**
- `cmake --build build --target clang-tidy` completes with zero warnings
- Or `run-clang-tidy -p build` exits with code 0
- No new clang-tidy issues are introduced by the refactor

---

## 4. Acceptance Summary

The palette refactor is complete when:

1. **Token Definitions** (REQ-F-001 through REQ-F-005)
   - All five new border tokens are defined in `ColorTokens` and initialized in `darkTokens()`
   - `outline` and `outlineVariant` are marked deprecated

2. **QStyle Wiring** (REQ-F-010 through REQ-F-014)
   - ~20 sites in `holonightstyle.cpp` are updated to use semantic border tokens
   - All passive/inactive/hovered/focused/active/urgent states use the correct token

3. **QML Wiring** (REQ-F-020 through REQ-F-024)
   - ~10 sites in `qml/*.qml` components are updated to use semantic border tokens
   - All component state transitions (default, hover, focus, selected, error) use the correct token

4. **Surface Alignment** (REQ-F-030, REQ-F-031)
   - Surface elevation tokens are realigned to design system targets (or closest passing values)
   - Progressive lightness hierarchy is verified: `background < surface < surfaceElevated < panel`

5. **QStyle Surface Audit** (REQ-F-040 through REQ-F-042)
   - Base windows/controls use `surface`
   - Elevated controls use `surfaceElevated`
   - Popups/tooltips use `panel`

6. **Contrast Compliance** (REQ-NF-001 through REQ-NF-003)
   - All border tokens achieve ≥ 3:1 contrast on surfaces
   - All surface tokens achieve ≥ 4.5:1 contrast with text
   - Semi-transparent `borderHover` maintains contrast after compositing

7. **Deviations Documented** (REQ-F-050, REQ-F-051)
   - `docs/holonight-design-deviations.md` records any color divergences from design system targets
   - Each entry includes reason and WCAG ratio

8. **Tests Pass** (REQ-F-060 through REQ-F-063)
   - Token initialization tests pass
   - Contrast tests pass for all tokens
   - QStyle/QML smoke tests pass
   - Clang-tidy passes (zero warnings)

9. **No Build Warnings**
   - `cmake --build build -j$(nproc)` completes without warnings
   - No undefined references or missing symbols

10. **Git Commit**
    - Changes are committed with conventional commit message (e.g., `feat: semantic border & surface tokens`)
    - All requirements are traceable to commit diffs
