# SDD Tasks — visual-polish

- [x] T-001: Define corner radius constants in anonymous namespace
  - REQs: REQ-A1
  - Check: `kRadiusControl = 6.0` and `kRadiusTight = 2.0` are declared after includes in `style/holonightstyle.cpp`.

- [x] T-002: Replace radius literals in menu and button drawing functions
  - REQs: REQ-A2
  - Check: `CE_MenuItem`, `CE_MenuBarItem`, `drawPushButtonBevelImpl`, and `drawPanelButtonImpl` all use `kRadiusControl` instead of hardcoded `4.0`.

- [x] T-003: Replace radius literals in checkbox and selection list drawing
  - REQs: REQ-A3
  - Check: `drawCheckBoxImpl` (checked fill and unchecked border), `drawItemViewItemImpl` (selected/hovered rows and focus ring), and `drawPanelItemViewImpl` (selected/hovered and focus ring) use `kRadiusControl` or `kRadiusTight` correctly.

- [x] T-004: Replace radius literals in frame and input widget drawing
  - REQs: REQ-A4
  - Check: `PE_FrameDefaultButton`, `PE_PanelLineEdit`, `PE_FrameGroupBox`, and `CC_ComboBox` all use `kRadiusControl` instead of hardcoded `4.0` or `5.0`.

- [x] T-005: Replace radius literals in spinbox, groupbox, and toolbutton drawing
  - REQs: REQ-A5
  - Check: `CC_SpinBox`, `drawGroupBoxImpl`, and `drawToolButtonImpl` (background and focus ring) use `kRadiusControl` or `kRadiusTight` correctly.

- [x] T-006: Verify all radius literals are replaced with constants
  - REQs: REQ-A6
  - Check: `grep -n "drawRoundedRect" style/holonightstyle.cpp | grep -v "kRadius"` returns zero matches (excluding track/slider groove comments).

- [x] T-007: Update border colors in Active color group
  - REQs: REQ-B1
  - Check: `buildPalette()` in `palette/palette.cpp` assigns Mid, Dark, and Shadow roles to `tok.borderPassive` in the Active group.

- [x] T-008: Update border colors in Disabled color group
  - REQs: REQ-B2
  - Check: `buildPalette()` in `palette/palette.cpp` assigns Mid, Dark, and Shadow roles to `tok.borderPassive` in the Disabled group.

- [x] T-009: Verify border token references are correct
  - REQs: REQ-B3
  - Check: `grep "tok\.outline" palette/palette.cpp` returns no matches in role-assignment lines (struct definition untouched).

- [x] T-010: Update Active Base to surfaceVariant in palette
  - REQs: REQ-C1
  - Check: `buildPalette()` in `palette/palette.cpp` assigns Base role to `tok.surfaceVariant` in the Active group.

- [x] T-011: Update Disabled Base to surfaceVariant in palette
  - REQs: REQ-C2
  - Check: `buildPalette()` in `palette/palette.cpp` assigns Base role to `tok.surfaceVariant` in the Disabled group.

- [x] T-012: Update polish() method Base color assignment
  - REQs: REQ-C3
  - Check: `polish(QWidget*)` in `style/holonightstyle.cpp` assigns `tok.surfaceVariant` to `QPalette::Base` role.

- [x] T-013: Update PE_PanelLineEdit fill brush
  - REQs: REQ-C4
  - Check: `PE_PanelLineEdit` painter brush in `style/holonightstyle.cpp` uses `tok.surfaceVariant` instead of `tok.surface`.

- [x] T-014: Update PE_PanelItemViewRow default fill brush
  - REQs: REQ-C5
  - Check: `PE_PanelItemViewRow` default branch painter brush in `style/holonightstyle.cpp` uses `tok.surfaceVariant` instead of `tok.surface`.

- [x] T-015: Add border role assertion test
  - REQs: REQ-C6
  - Check: `TEST_F(PaletteTest, BorderRolesAreBorderPassive)` exists in `tests/test_palette_qpalette.cpp` and asserts Mid/Dark/Shadow == borderPassive for Active, Inactive, and Disabled groups.

- [x] T-016: Add Base role assertion test
  - REQs: REQ-C7
  - Check: `TEST_F(PaletteTest, BaseIsSurfaceVariant)` exists in `tests/test_palette_qpalette.cpp` and asserts Base == surfaceVariant for Active, Inactive, and Disabled groups.

- [x] T-017: Build compilation verification
  - REQs: REQ-V1
  - Check: `cmake --build build -j$(nproc)` completes with zero compilation errors or warnings.

- [x] T-018: Clang-tidy verification
  - REQs: REQ-V2
  - Check: `cmake --build build --target tidy` reports zero new warnings (Qt6's `-mno-direct-extern-access` flag ignored).

- [x] T-019: Run test suite verification
  - REQs: REQ-V3
  - Check: `QT_QPA_PLATFORM=offscreen ctest --test-dir build --output-on-failure` passes all tests including T-015 and T-016.

- [x] T-020: Verify palette header is unchanged
  - REQs: REQ-V4
  - Check: `git diff palette/holonight/palette.h` is empty (no ColorTokens struct modifications).
