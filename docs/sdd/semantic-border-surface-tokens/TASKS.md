# SDD Tasks — semantic-border-surface-tokens

## Implementation Order

- [x] T-001: Add new border token members to ColorTokens struct
  - REQs: REQ-F-001, REQ-F-002, REQ-F-003, REQ-F-004, REQ-F-005, REQ-C-001
  - Check: `palette/holonight/palette.h` contains `borderPassive`, `borderHover`, `borderFocus`, `borderActive`, `borderUrgent` members with documentation comments, and `outline`/`outlineVariant` are marked deprecated.

- [x] T-002: Initialize new border tokens and updated surface tokens in darkTokens()
  - REQs: REQ-F-001, REQ-F-002, REQ-F-003, REQ-F-004, REQ-F-005, REQ-F-030
  - Check: `palette/palette.cpp` darkTokens() function initializes all five border tokens to exact hex values (`#565f89`, `#7dcfff`, `#00e0ff`, `#7aa2f7`, `#f7768e`) with correct alpha values, and surface/surfaceVariant/surfaceContainer are updated to `#10131f`, `#161925`, `#1a1b26`.

- [x] T-003: Add semantic border token properties and accessors to QML singleton
  - REQs: REQ-F-020, REQ-F-021, REQ-F-022, REQ-F-023, REQ-F-024
  - Check: `qml/holoniightpalette.h` contains five new `Q_PROPERTY` declarations and corresponding `[[nodiscard]]` accessor methods for borderPassive/Hover/Focus/Active/Urgent.

- [x] T-004: Update QStyle holonightstyle.cpp — borderPassive sites (passive borders)
  - REQs: REQ-F-010, REQ-C-002
  - Check: All 11 sites (lines 182, 318, 435, 453, 527, 560, 572, 649, 679, 706, 773) that render passive/inactive frame or separator borders replace `tok.outline` with `tok.borderPassive`.

- [x] T-005: Update QStyle holonightstyle.cpp — borderPassive sites (default state in ternaries)
  - REQs: REQ-F-010, REQ-C-002
  - Check: Five ternary expressions (lines 215, 378, 545, 603, 695) replace the `tok.outline` in the else-branch (default/unfocused state) with `tok.borderPassive`.

- [x] T-006: Update QStyle holonightstyle.cpp — borderFocus sites (keyboard focus rings)
  - REQs: REQ-F-012
  - Check: Five focus-branch replacements (lines 215, 378, 545, 603, 695) change `tok.focusRing` to `tok.borderFocus` in control border ternaries.

- [x] T-007: Update QStyle holonightstyle.cpp — borderActive site (selected tab underline)
  - REQs: REQ-F-013
  - Check: Line 299 `drawTabBarTabImpl` selected-tab underline changes from `tok.primary` to `tok.borderActive`.

- [x] T-008: Update QStyle holonightstyle.cpp — borderUrgent TODO comments and outlineVariant→borderPassive
  - REQs: REQ-F-014, REQ-C-002
  - Check: Lines 549, 607, 699 add comment `// TODO(borderUrgent): if error state added, use tok.borderUrgent here`; lines 155, 756 replace `tok.outlineVariant` with `tok.borderPassive`.

- [x] T-009: Update QStyle holonightstyle.cpp — PE_PanelMenu surface token
  - REQs: REQ-F-042
  - Check: Line 463 PE_PanelMenu popup background changes from `tok.surfaceVariant` to `tok.secondary` to match Panel tier design system classification.

- [x] T-010: Update QML components — borderPassive replacements (11 sites across 11 files)
  - REQs: REQ-F-020, REQ-C-002
  - Check: Button.qml:44(else), TextField.qml:57, CheckBox.qml:36(else), RadioButton.qml:35(else), ComboBox.qml:83(else), ComboBox.qml:103, Slider.qml:33, Slider.qml:62, TabBar.qml:40, Menu.qml:32, ToolTip.qml:34 all replace `HoloniightPalette.outline` with `HoloniightPalette.borderPassive`.

- [x] T-011: Update QML components — borderFocus replacements (3 sites)
  - REQs: REQ-F-022
  - Check: Button.qml:44(if), TextField.qml:54, ComboBox.qml:83(if-focus) change focus-branch bindings from `HoloniightPalette.focusRing` to `HoloniightPalette.borderFocus`.

- [x] T-012: Update QML components — borderActive replacements (2 sites)
  - REQs: REQ-F-023
  - Check: CheckBox.qml:36(hover), RadioButton.qml:35(hover) change hovered state borders from `HoloniightPalette.primary` to `HoloniightPalette.borderActive`.

- [x] T-013: Update QML components — borderUrgent and remaining outlineVariant→borderPassive
  - REQs: REQ-F-024, REQ-C-002
  - Check: TextField.qml:55 error-state border changes from `HoloniightPalette.error` to `HoloniightPalette.borderUrgent`; ComboBox.qml:83(disabled) changes `HoloniightPalette.outlineVariant` to `HoloniightPalette.borderPassive`.

- [x] T-014: Add token value assertions to tests/test_palette.cpp
  - REQs: REQ-F-060, REQ-F-061
  - Check: New test case `DarkTokens_BorderTokenValues` asserts exact hex values for all five border tokens and alpha value for borderHover; test case `DarkTokens_SurfaceValues` asserts updated surface values.

- [x] T-015: Add surface luminance ordering test to tests/test_palette.cpp
  - REQs: REQ-F-031
  - Check: New test case `DarkTokens_SurfaceLuminanceOrdering` verifies lum(surface) < lum(surfaceVariant) < lum(surfaceContainer) < lum(secondary) with explicit comparison messages.

- [x] T-016: Extend opaque color tests in tests/test_palette.cpp
  - REQs: REQ-F-060
  - Check: Existing test `DarkTokens_OpaqueColorsAreOpaque` extended to assert alpha=255 for borderPassive/Focus/Active/Urgent; borderHover excluded with a comment.

- [x] T-017: Add contrast tests for new border tokens to tests/test_palette_contrast.cpp
  - REQs: REQ-NF-001
  - Check: Six new test cases (BorderFocusVsSurface, BorderFocusVsSecondary, BorderActiveVsSurface, BorderActiveVsSecondary, BorderUrgentVsSurface, BorderUrgentVsSecondary) each assert contrastRatio() >= 3.0.

- [x] T-018: Add contrast tests for updated surface tokens to tests/test_palette_contrast.cpp
  - REQs: REQ-NF-002
  - Check: Three new test cases (OnSurfaceVsSurfaceNew, OnSurfaceVsSurfaceVariantNew, OnSurfaceVsSurfaceContainerNew) each assert contrastRatio() >= 4.5 for text on new surface values.

- [x] T-019: Add WCAG exemption comments to tests/test_palette_contrast.cpp
  - REQs: REQ-NF-003, REQ-F-050
  - Check: Comment block added documenting borderPassive and borderHover contrast deviations (2.35–2.99:1 and 2.03–2.06:1 respectively), with rationale for exemption and reference to deviation log.

- [x] T-020: Create design deviations documentation file
  - REQs: REQ-F-050, REQ-F-051
  - Check: File `docs/holonight-design-deviations.md` is created with two deviation entries (borderPassive and borderHover), each including target value, actual value, reason, and WCAG ratio; format is consistent markdown.

- [x] T-021: Build, clang-tidy, and test verification
  - REQs: REQ-F-062, REQ-F-063
  - Check: `cmake --build build -j$(nproc)` completes without warnings, `run-clang-tidy -p build` exits with zero warnings, and `QT_QPA_PLATFORM=offscreen ctest --test-dir build --output-on-failure` reports all tests passing.

- [x] T-022: Create git commit with conventional message
  - REQs: All
  - Check: Commit message is `feat: semantic border & surface tokens` or similar feat-type, includes all changes from T-001 through T-020, and git log shows clean commit with no uncommitted files remaining.
