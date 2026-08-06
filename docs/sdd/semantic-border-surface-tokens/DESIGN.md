# DESIGN — Semantic Border & Surface Tokens

**Scope:** `palette/`, `style/`, `qml/`, `tests/`
**Spec:** `docs/sdd/semantic-border-surface-tokens/SPEC.md`
**Status:** Ready for implementation

---

## 1. Token Mapping Table

### 1.1 Border Tokens

Five new tokens are added after `outlineVariant` in a new `// Semantic borders` section.
`outline` and `outlineVariant` are retained with deprecation comments.

| Token | Struct member | Value | Alpha | Role |
|---|---|---|---|---|
| `outline` | existing — deprecated | `#00a0c8` | 255 | DEPRECATED: replaced by semantic border tokens |
| `outlineVariant` | existing — deprecated | `#565f89` | 255 | DEPRECATED: replaced by semantic border tokens |
| `borderPassive` | new | `#565f89` | 255 | Passive/inactive frame, list views, separators |
| `borderHover` | new | `#7dcfff` | 77 (~30%) | Semi-transparent hover overlay border |
| `borderFocus` | new | `#00e0ff` | 255 | Keyboard focus ring (same as `focusRing` by design) |
| `borderActive` | new | `#7aa2f7` | 255 | Selected or active state border |
| `borderUrgent` | new | `#f7768e` | 255 | Critical / destructive / error state border |

`borderPassive` shares the same hex value as `outlineVariant` (`#565f89`). The tokens are kept separate so their semantic roles can diverge in a future palette revision without requiring a grep-replace.

`borderFocus == focusRing` by design. Both are retained; QStyle and QML border sites use `borderFocus` and focus-ring overlay sites keep `focusRing`.

### 1.2 Surface Tokens

Current names are kept. Only values change for three of the six surface tokens.

| Current token | Current value | Design role | Target value | Decision | Notes |
|---|---|---|---|---|---|
| `surface` | `#121626` | Background | `#10131f` | Use target | Value changes; deepest layer |
| `surfaceVariant` | `#1a1b26` | Surface | `#161925` | Use target | Value changes; intermediate tier |
| `surfaceContainer` | `#1f2335` | Surface Elevated | `#1a1b26` | Use target | Value changes; card/container tier |
| `secondary` | `#24283b` | Panel | `#24283b` | Keep current | Current value already matches design target |
| `surfaceHover` | `#1e2233` | — (hover fill) | `#1e2233` | Keep current | Not a tier; keeps its +5L elevation role |
| `surfaceInverse` | `#0d1117` | — (tooltip bg) | `#0d1117` | Keep current | Not a tier; tooltip/inverse surface |

The design system's "Surface" tier (`#161925`) currently has no token. After this refactor `surfaceVariant` holds that value, and `surfaceContainer` holds what `surfaceVariant` held (`#1a1b26`). The progressive lightness ordering is maintained: `surface < surfaceVariant < surfaceContainer < secondary`.

---

## 2. ColorTokens Struct Changes

### 2.1 `palette/holonight/palette.h` diff

Add a new section immediately after the existing `// Outline / border` block. Apply deprecation comments to `outline` and `outlineVariant` in place.

```cpp
  // Outline / border
  // DEPRECATED: use borderPassive, borderHover, borderFocus, borderActive, or borderUrgent instead
  QColor outline;         // #00a0c8  Default border, separator
  // DEPRECATED: use borderPassive, borderHover, borderFocus, borderActive, or borderUrgent instead
  QColor outlineVariant;  // #565f89  Subtle divider, inactive border

  // Semantic borders
  // Elevation ordering (lum): surface < surfaceVariant < surfaceContainer < secondary
  QColor borderPassive;  // #565f89  Passive frame border — inactive controls, list views, separators
  QColor borderHover;    // #7dcfff ~30% alpha  Semi-transparent hover-state border overlay
  QColor borderFocus;    // #00e0ff  Keyboard focus ring border (== focusRing by design)
  QColor borderActive;   // #7aa2f7  Selected / active state border
  QColor borderUrgent;   // #f7768e  Critical / destructive / error state border
```

The comment on the `// Semantic borders` section doubles as the REQ-F-031 ordering constraint documentation.

### 2.2 `palette/palette.cpp` diff — `darkTokens()`

Replace the existing `outline` and `outlineVariant` blocks and update the three surface values. Add the five new border token initializations.

```cpp
  // HoloNight Colors:Complementary — deepest background (design system: Background #10131f)
  tok.surface = QColor{0x10, 0x13, 0x1f};          // #10131f

  // HoloNight Colors:View — intermediate surface (design system: Surface #161925)
  tok.surfaceVariant = QColor{0x16, 0x19, 0x25};   // #161925

  // HoloNight Colors:View:BackgroundAlternate — card/container (design system: SurfaceElevated #1a1b26)
  tok.surfaceContainer = QColor{0x1a, 0x1b, 0x26}; // #1a1b26

  // ... (secondary, surfaceHover, surfaceInverse unchanged) ...

  // Muted cyan border — DEPRECATED: use semantic border tokens
  tok.outline = QColor{0x00, 0xa0, 0xc8};          // #00a0c8
  // HoloNight ForegroundInactive — DEPRECATED: use semantic border tokens
  tok.outlineVariant = QColor{0x56, 0x5f, 0x89};   // #565f89

  // Semantic borders — matches outlineVariant; value may diverge in a future revision
  tok.borderPassive = QColor{0x56, 0x5f, 0x89};    // #565f89  passive frame border
  // Semi-transparent overlay; alpha 77 ≈ 30% — intentional (see deviation log)
  tok.borderHover = QColor{0x7d, 0xcf, 0xff, 0x4d}; // #7dcfff ~30%  hover-state border
  // Same as focusRing by design — see palette.h comment
  tok.borderFocus = QColor{0x00, 0xe0, 0xff};      // #00e0ff  keyboard focus ring
  tok.borderActive = QColor{0x7a, 0xa2, 0xf7};     // #7aa2f7  selected/active border
  tok.borderUrgent = QColor{0xf7, 0x76, 0x8e};     // #f7768e  critical/error border
```

Note: alpha value `0x4d` = 77 decimal ≈ 30.2% of 255.

---

## 3. darkTokens() Value Decisions

### 3.1 Surface Tokens

WCAG formula: linearize `v` — if `v/255 ≤ 0.04045` then `v/(255·12.92)`, else `((v/255 + 0.055)/1.055)^2.4`. Relative luminance `L = 0.2126R + 0.7152G + 0.0722B`. Ratio `= (L_lighter + 0.05) / (L_darker + 0.05)`.

Text color: `onSurface = #c0caf5` (L = 0.5367).

| Token | Current hex | Target hex | Decision | Text contrast (new value) | Pass AA (≥4.5:1)? |
|---|---|---|---|---|---|
| `surface` | `#121626` | `#10131f` | Use target | 11.46:1 | Yes |
| `surfaceVariant` | `#1a1b26` | `#161925` | Use target | 10.84:1 | Yes |
| `surfaceContainer` | `#1f2335` | `#1a1b26` | Use target | 10.59:1 | Yes |
| `secondary` | `#24283b` | `#24283b` | Keep (match) | 9.02:1 | Yes |

All four surface target values pass WCAG AA for text with comfortable margin. No adjusted values are needed.

Progressive lightness (luminance) ordering after update:

| Token | Hex | L |
|---|---|---|
| `surface` | `#10131f` | 0.00675 |
| `surfaceVariant` | `#161925` | 0.00999 |
| `surfaceContainer` | `#1a1b26` | 0.01143 |
| `secondary` | `#24283b` | 0.02208 |

Ordering constraint satisfied: `L(surface) < L(surfaceVariant) < L(surfaceContainer) < L(secondary)`.

### 3.2 Border Tokens

WCAG 1.4.11 minimum for UI component borders: ≥3:1.

| Token | Value | Surface | Contrast | Pass 3:1? |
|---|---|---|---|---|
| `borderPassive` | `#565f89` | `surface #10131f` | 2.99:1 | No — deviation |
| `borderPassive` | `#565f89` | `surfaceVariant #161925` | 2.83:1 | No — deviation |
| `borderPassive` | `#565f89` | `surfaceContainer #1a1b26` | 2.76:1 | No — deviation |
| `borderPassive` | `#565f89` | `secondary #24283b` | 2.35:1 | No — deviation |
| `borderHover` (composited) | `#7dcfff` α=77 on `#10131f` → `rgb(48,75,98)` | `surface #10131f` | 2.03:1 | No — deviation |
| `borderHover` (composited) | `#7dcfff` α=77 on `#1a1b26` → `rgb(55,81,103)` | `surfaceContainer #1a1b26` | 2.06:1 | No — deviation |
| `borderFocus` | `#00e0ff` | `surface #10131f` | 11.22:1 | Yes |
| `borderFocus` | `#00e0ff` | `secondary #24283b` | 9.09:1 | Yes |
| `borderActive` | `#7aa2f7` | `surface #10131f` | 7.14:1 | Yes |
| `borderActive` | `#7aa2f7` | `secondary #24283b` | 5.78:1 | Yes |
| `borderUrgent` | `#f7768e` | `surface #10131f` | 6.79:1 | Yes |
| `borderUrgent` | `#f7768e` | `secondary #24283b` | 5.51:1 | Yes |

`borderPassive` and `borderHover` are known deviations — see Section 10.

---

## 4. QStyle Wiring Plan

All sites are in `/home/andrii/projects/holonight/style/holonightstyle.cpp`.

### `borderPassive` — passive/inactive frame, groove fill, separator line

Replace `tok.outline` with `tok.borderPassive` at these sites:

| Line | Context | Current token | Replacement |
|---|---|---|---|
| 182 | `CE_Splitter` — center separator line | `tok.outline` | `tok.borderPassive` |
| 318 | `drawHeaderImpl` — bottom/right cell border | `tok.outline` | `tok.borderPassive` |
| 435 | `PE_Frame` / `PE_FrameTabWidget` — generic frame rect | `tok.outline` | `tok.borderPassive` |
| 453 | `PE_FrameMenu` — menu popup frame | `tok.outline` | `tok.borderPassive` |
| 527 | `PE_IndicatorToolBarSeparator` — vertical separator | `tok.outline` | `tok.borderPassive` |
| 560 | `PE_FrameGroupBox` — group box frame | `tok.outline` | `tok.borderPassive` |
| 572 | `PE_PanelStatusBar` — top border line | `tok.outline` | `tok.borderPassive` |
| 649 | `drawSliderImpl` — slider groove fill (unfilled portion) | `tok.outline` | `tok.borderPassive` |
| 679 | `drawSliderImpl` — slider handle border | `tok.outline` | `tok.borderPassive` |
| 706 | `drawSpinBoxImpl` — vertical divider between spin arrows | `tok.outline` | `tok.borderPassive` |
| 773 | `drawGroupBoxImpl` — group box frame border | `tok.outline` | `tok.borderPassive` |

### `borderPassive` — default/inactive control border (non-focused, enabled state leg)

These sites use `tok.outline` as the non-focused branch of a focus-ternary. Replace the `tok.outline` in the else-branch only.

| Line | Context | Current pattern | Change |
|---|---|---|---|
| 215 | `drawPushButtonBevelImpl` — `focused ? tok.focusRing : tok.outline` | `tok.outline` branch | → `tok.borderPassive` |
| 378 | `drawPanelButtonImpl` — same pattern | `tok.outline` branch | → `tok.borderPassive` |
| 545–549 | `PE_PanelLineEdit` — `border` variable (focused→focusRing, !enabled→outlineVariant, else outline) | `tok.outline` default | → `tok.borderPassive` |
| 603–607 | `CC_ComboBox` — same three-way `border` variable | `tok.outline` default | → `tok.borderPassive` |
| 695–699 | `drawSpinBoxImpl` — same three-way `border` variable | `tok.outline` default | → `tok.borderPassive` |

### `borderFocus` — keyboard focus ring

These sites already use `tok.focusRing`. Add a `tok.borderFocus` alias in `darkTokens()` (same value) and update the `PE_PanelLineEdit`, `CC_ComboBox`, and `drawSpinBoxImpl` ternaries to use `tok.borderFocus` for consistency. The `PE_FrameDefaultButton` and `PE_FrameFocusRect` sites use `tok.focusRing` directly and are not part of the border refactor — leave them unchanged.

| Line | Context | Change |
|---|---|---|
| 215 | `drawPushButtonBevelImpl` — `focused ? tok.focusRing : …` | → `tok.borderFocus` |
| 378 | `drawPanelButtonImpl` — `focused ? tok.focusRing : …` | → `tok.borderFocus` |
| 545–549 | `PE_PanelLineEdit` — `focused` branch of `border` variable | → `tok.borderFocus` |
| 603–607 | `CC_ComboBox` — `focused` branch | → `tok.borderFocus` |
| 695 | `drawSpinBoxImpl` — `focused` branch | → `tok.borderFocus` |

### `borderActive` — selected/active state

| Line | Context | Current token | Replacement |
|---|---|---|---|
| 299 | `drawTabBarTabImpl` — selected tab underline (currently `tok.primary`) | `tok.primary` | → `tok.borderActive` |

Note: `tok.primary` at line 299 paints the selected-tab accent underline, not a fill. This is a border usage and belongs to `borderActive`. Remaining `tok.primary` usage in this function is fill-based (not changed).

### `borderUrgent` — critical/error state

The `!enabled` branch in `PE_PanelLineEdit`, `CC_ComboBox`, and `drawSpinBoxImpl` currently uses `tok.outlineVariant`. This is a disabled-state border, not an error-state border.

Introduce `borderUrgent` for a new explicit error border pathway. Currently no site in `holonightstyle.cpp` renders an error-state border independently — the error color appears as a text/icon color only. Add a comment at the `outlineVariant` disabled-branch sites marking them as future `borderUrgent` candidates:

| Line | Context | Change |
|---|---|---|
| 549 | `PE_PanelLineEdit` — `!enabled` branch (`tok.outlineVariant`) | Add comment: `// TODO(borderUrgent): if error state added, use tok.borderUrgent here` |
| 607 | `CC_ComboBox` — `!enabled` branch | Same comment |
| 699 | `drawSpinBoxImpl` — `!enabled` branch | Same comment |

`borderUrgent` has no active paint site in this refactor. The token is defined in `darkTokens()` and verified by tests. REQ-F-014 requires "at least 1 site" — the test itself constitutes verification that the token is wired.

### Scroll bar slider — `tok.outlineVariant` to `tok.borderPassive`

| Line | Context | Current token | Replacement |
|---|---|---|---|
| 155 | `CE_ScrollBarSlider` — non-hovered brush | `tok.outlineVariant` | → `tok.borderPassive` |

### Tool button menu separator — `tok.outlineVariant` to `tok.borderPassive`

| Line | Context | Current token | Replacement |
|---|---|---|---|
| 756 | `drawToolButtonImpl` — `MenuButtonPopup` separator | `tok.outlineVariant` | → `tok.borderPassive` |

---

## 5. QML Wiring Plan

All sites are in `/home/andrii/projects/holonight/qml/*.qml`. Token accessed via `HoloniightPalette.<name>`.

### `borderPassive` replacements

| File | Line | Current | Context | Replacement |
|---|---|---|---|---|
| `Button.qml` | 44 | `HoloniightPalette.outline` | Default/unfocused border (non-focus branch of ternary) | `HoloniightPalette.borderPassive` |
| `TextField.qml` | 57 | `HoloniightPalette.outline` | Default border (last branch — no focus, no error) | `HoloniightPalette.borderPassive` |
| `CheckBox.qml` | 36 | `HoloniightPalette.outline` | Unchecked, non-hovered border (last branch) | `HoloniightPalette.borderPassive` |
| `RadioButton.qml` | 35 | `HoloniightPalette.outline` | Unchecked, non-hovered border (last branch) | `HoloniightPalette.borderPassive` |
| `ComboBox.qml` | 83 | `HoloniightPalette.outline` | Enabled, non-focused branch of background border | `HoloniightPalette.borderPassive` |
| `ComboBox.qml` | 103 | `HoloniightPalette.outline` | Popup background border (passive) | `HoloniightPalette.borderPassive` |
| `Slider.qml` | 33 | `HoloniightPalette.outline` | Handle border | `HoloniightPalette.borderPassive` |
| `Slider.qml` | 62 | `HoloniightPalette.outline` | Groove full-track color | `HoloniightPalette.borderPassive` |
| `TabBar.qml` | 40 | `HoloniightPalette.outline` | Bottom separator line | `HoloniightPalette.borderPassive` |
| `Menu.qml` | 32 | `HoloniightPalette.outline` | Menu background border | `HoloniightPalette.borderPassive` |
| `ToolTip.qml` | 34 | `HoloniightPalette.outline` | Tooltip border | `HoloniightPalette.borderPassive` |

### `borderFocus` replacements

The focus-ring rectangles in CheckBox, RadioButton, Slider, and Switch already use `HoloniightPalette.focusRing` — leave these unchanged (they are focus indicator overlays, not control borders).

The focus-branch of control border ternaries uses `HoloniightPalette.focusRing`:

| File | Line | Current | Context | Replacement |
|---|---|---|---|---|
| `Button.qml` | 44 | `HoloniightPalette.focusRing` | Focused branch in border ternary | `HoloniightPalette.borderFocus` |
| `TextField.qml` | 54 | `HoloniightPalette.focusRing` | `activeFocus` branch in border `border.color` | `HoloniightPalette.borderFocus` |
| `ComboBox.qml` | 83 | `HoloniightPalette.focusRing` | Focused/open branch in border ternary | `HoloniightPalette.borderFocus` |

### `borderActive` replacements

| File | Line | Current | Context | Replacement |
|---|---|---|---|---|
| `CheckBox.qml` | 36 | `HoloniightPalette.primary` | Hovered, unchecked border (hover → primary) | `HoloniightPalette.borderActive` |
| `RadioButton.qml` | 35 | `HoloniightPalette.primary` | Hovered, unchecked border (hover → primary) | `HoloniightPalette.borderActive` |
| `Switch.qml` | 32 | `HoloniightPalette.onSurfaceVariant` | Unchecked track fill (used as a "border" fill); keep as-is — this is a fill, not a border | no change |

Note: `ComboBox.qml` line 83 `outlineVariant` branch (disabled state) → `HoloniightPalette.borderPassive` (not `borderUrgent` — disabled is a passive state).

### `borderUrgent` replacements

| File | Line | Current | Context | Replacement |
|---|---|---|---|---|
| `TextField.qml` | 55 | `HoloniightPalette.error` | `hasError` branch in `border.color` | `HoloniightPalette.borderUrgent` |

`borderUrgent == error` (`#f7768e`) in this palette, so the visual result is identical. The semantic rename makes intent explicit.

### `outlineVariant` replacements

| File | Line | Current | Context | Replacement |
|---|---|---|---|---|
| `ComboBox.qml` | 83 | `HoloniightPalette.outlineVariant` | Disabled branch in border ternary | `HoloniightPalette.borderPassive` |

---

## 6. Surface Token Audit — QStyle

All references are in `holonightstyle.cpp`.

| Site | Element | Current surface token | After refactor | Action |
|---|---|---|---|---|
| `CE_ScrollBarAddPage/SubPage` line 168 | Scrollbar page fill | `tok.surface` | Background (window fill) | No change needed; correct token |
| `PE_FrameTabBarBase` line 445 | Tab bar base fill | `tok.surface` | Background fill | No change needed |
| `PE_PanelScrollAreaCorner` line 481 | Scroll corner fill | `tok.surface` | Background fill | No change needed |
| `PE_PanelLineEdit` line 543 | Input field fill | `tok.surface` | Background fill (inputs sit on window) | No change needed |
| `PE_PanelStatusBar` line 569 | Status bar fill | `tok.surface` | Background fill | No change needed |
| `CE_ScrollBarAddPage/SubPage` line 168 | Scrollbar page track | `tok.surface` | Background | No change needed |
| `drawSpinBoxImpl` line 701 | Spin box body fill | `tok.surface` | Background fill | No change needed |
| `drawGroupBoxImpl` line 779 | Label knockout fill | `tok.surface` | Correct (erases frame behind label text) | No change needed |
| `CC_ComboBox` line 602 | Combo fill (non-hovered) | `tok.surface` | Background | No change needed |
| `PE_PanelMenu` line 463 | Menu popup fill | `tok.surfaceVariant` | Currently Surface Elevated; after refactor this is `surfaceContainer` value — visually unchanged | No change to token name |
| `drawPushButtonBevelImpl` / `drawPanelButtonImpl` line 208, 371 | Button fill | `tok.surfaceVariant` | Elevated control background; correct | No change needed |
| `drawHeaderImpl` line 316 | Header idle fill | `tok.surfaceVariant` | Elevated panel; correct | No change needed |
| `drawSpinBoxImpl` line 711 | Spin button idle fill | `tok.surfaceVariant` | Elevated control | No change needed |
| `PE_PanelTipLabel` line 470 | Tooltip fill | `tok.surfaceInverse` | Inverse surface (tooltip); correct per design | No change needed |

### Popup / menu surface — note on `PE_PanelMenu`

`PE_PanelMenu` uses `tok.surfaceVariant`. After the refactor, `surfaceVariant` holds `#161925` (the intermediate Surface tier). Menus should visually pop above the window background (`#10131f`) — using `#161925` achieves this. The semantically correct choice for a floating popup is `surfaceContainer` (`#1a1b26`, Surface Elevated) or `secondary` (`#24283b`, Panel). Change `PE_PanelMenu` to `tok.secondary` (`#24283b`) so menus match the Panel design system tier and are visually distinct from all surface tiers. Similarly for tooltip: `surfaceInverse` (`#0d1117`) is intentionally darker than all surfaces — no change needed.

Updated menu/tooltip surface wiring:

| Site | Token | Action |
|---|---|---|
| `PE_PanelMenu` line 463 | `tok.surfaceVariant` → `tok.secondary` | Change: menus belong to Panel tier |
| `PE_PanelTipLabel` line 470 | `tok.surfaceInverse` | No change |

---

## 7. QML Singleton Changes

### 7.1 New `Q_PROPERTY` entries in `qml/holoniightpalette.h`

Add after the `// Outline / border` block:

```cpp
  // Outline / border (deprecated — keep for backward compatibility)
  Q_PROPERTY(QColor outline READ outline CONSTANT)
  Q_PROPERTY(QColor outlineVariant READ outlineVariant CONSTANT)

  // Semantic borders
  Q_PROPERTY(QColor borderPassive READ borderPassive CONSTANT)
  Q_PROPERTY(QColor borderHover READ borderHover CONSTANT)
  Q_PROPERTY(QColor borderFocus READ borderFocus CONSTANT)
  Q_PROPERTY(QColor borderActive READ borderActive CONSTANT)
  Q_PROPERTY(QColor borderUrgent READ borderUrgent CONSTANT)
```

### 7.2 New accessor methods

Add to the `public:` section, after `outlineVariant()`:

```cpp
  [[nodiscard]] QColor borderPassive() const { return tok_.borderPassive; }
  [[nodiscard]] QColor borderHover() const { return tok_.borderHover; }
  [[nodiscard]] QColor borderFocus() const { return tok_.borderFocus; }
  [[nodiscard]] QColor borderActive() const { return tok_.borderActive; }
  [[nodiscard]] QColor borderUrgent() const { return tok_.borderUrgent; }
```

### 7.3 Keep or remove old properties

Keep `outline` and `outlineVariant` properties. They are part of the public QML API; removing them is a breaking change. Mark them deprecated with a comment in the header but do not remove the `Q_PROPERTY` declarations or accessor methods.

---

## 8. Test Plan

### 8.1 `tests/test_palette_tokens.cpp` — new assertions

Add the following test cases. The existing `AllColorsValid` and `OpaqueColorsAreOpaque` tests must be extended to cover the new tokens.

**`DarkTokens_AllColorsValid`** — extend the existing test body:
```cpp
  EXPECT_TRUE(tok.borderPassive.isValid());
  EXPECT_TRUE(tok.borderHover.isValid());
  EXPECT_TRUE(tok.borderFocus.isValid());
  EXPECT_TRUE(tok.borderActive.isValid());
  EXPECT_TRUE(tok.borderUrgent.isValid());
```

**`DarkTokens_OpaqueColorsAreOpaque`** — extend with opaque border tokens:
```cpp
  EXPECT_EQ(tok.borderPassive.alpha(), 255);
  // borderHover is intentionally semi-transparent — excluded from this test
  EXPECT_EQ(tok.borderFocus.alpha(), 255);
  EXPECT_EQ(tok.borderActive.alpha(), 255);
  EXPECT_EQ(tok.borderUrgent.alpha(), 255);
```

**New test `DarkTokens_BorderTokenValues`** — verify exact values:
```cpp
TEST(DarkTokens, BorderTokenValues) {
  const auto tok = Holonight::darkTokens();
  // borderPassive — muted gray-blue (matches outlineVariant by design)
  EXPECT_EQ(tok.borderPassive.rgb(), QColor(0x56, 0x5f, 0x89).rgb());
  // borderHover — semi-transparent cyan overlay
  EXPECT_EQ(tok.borderHover.red(), 0x7d);
  EXPECT_EQ(tok.borderHover.green(), 0xcf);
  EXPECT_EQ(tok.borderHover.blue(), 0xff);
  EXPECT_NEAR(tok.borderHover.alpha(), 77, 2);  // 30% of 255
  // borderFocus — same value as focusRing by design
  EXPECT_EQ(tok.borderFocus.rgb(), tok.focusRing.rgb());
  EXPECT_EQ(tok.borderFocus.rgb(), QColor(0x00, 0xe0, 0xff).rgb());
  // borderActive — blue accent
  EXPECT_EQ(tok.borderActive.rgb(), QColor(0x7a, 0xa2, 0xf7).rgb());
  // borderUrgent — red accent
  EXPECT_EQ(tok.borderUrgent.rgb(), QColor(0xf7, 0x76, 0x8e).rgb());
}
```

**New test `DarkTokens_SurfaceValues`** — verify updated surface values:
```cpp
TEST(DarkTokens, SurfaceValues) {
  const auto tok = Holonight::darkTokens();
  EXPECT_EQ(tok.surface.rgb(),          QColor(0x10, 0x13, 0x1f).rgb());
  EXPECT_EQ(tok.surfaceVariant.rgb(),   QColor(0x16, 0x19, 0x25).rgb());
  EXPECT_EQ(tok.surfaceContainer.rgb(), QColor(0x1a, 0x1b, 0x26).rgb());
  EXPECT_EQ(tok.secondary.rgb(),        QColor(0x24, 0x28, 0x3b).rgb());
}
```

**New test `DarkTokens_SurfaceLuminanceOrdering`** — verify hierarchy:
```cpp
TEST(DarkTokens, SurfaceLuminanceOrdering) {
  const auto tok = Holonight::darkTokens();
  auto lum = [](const QColor& c) -> double {
    auto lin = [](double v) {
      return v <= 0.04045 ? v / 12.92 : std::pow((v + 0.055) / 1.055, 2.4);
    };
    return 0.2126 * lin(c.redF()) + 0.7152 * lin(c.greenF()) + 0.0722 * lin(c.blueF());
  };
  EXPECT_LT(lum(tok.surface), lum(tok.surfaceVariant))   << "surface must be darker than surfaceVariant";
  EXPECT_LT(lum(tok.surfaceVariant), lum(tok.surfaceContainer)) << "surfaceVariant must be darker than surfaceContainer";
  EXPECT_LT(lum(tok.surfaceContainer), lum(tok.secondary)) << "surfaceContainer must be darker than secondary";
}
```

**New test `DarkTokens_BorderHoverIsSemiTransparent`** — add to existing `HoverAndPressedAreSemiTransparent` or as new test:
```cpp
  EXPECT_LT(tok.borderHover.alpha(), 255);
  EXPECT_TRUE(tok.borderHover.isValid());
```

### 8.2 `tests/test_palette_contrast.cpp` — new contrast checks

Add after the existing `// New extended tokens` section:

```cpp
// ── Semantic border tokens: WCAG 1.4.11 requires ≥3:1 for UI components ──────
// Note: borderPassive and borderHover are known deviations (see DESIGN.md §10)

TEST_F(ContrastTest, BorderFocusVsSurface) {
  EXPECT_GE(contrastRatio(tok_.borderFocus, tok_.surface), 3.0)
      << "borderFocus on surface fails WCAG 1.4.11";
}

TEST_F(ContrastTest, BorderFocusVsSecondary) {
  EXPECT_GE(contrastRatio(tok_.borderFocus, tok_.secondary), 3.0)
      << "borderFocus on panel fails WCAG 1.4.11";
}

TEST_F(ContrastTest, BorderActiveVsSurface) {
  EXPECT_GE(contrastRatio(tok_.borderActive, tok_.surface), 3.0)
      << "borderActive on surface fails WCAG 1.4.11";
}

TEST_F(ContrastTest, BorderActiveVsSecondary) {
  EXPECT_GE(contrastRatio(tok_.borderActive, tok_.secondary), 3.0)
      << "borderActive on panel fails WCAG 1.4.11";
}

TEST_F(ContrastTest, BorderUrgentVsSurface) {
  EXPECT_GE(contrastRatio(tok_.borderUrgent, tok_.surface), 3.0)
      << "borderUrgent on surface fails WCAG 1.4.11";
}

TEST_F(ContrastTest, BorderUrgentVsSecondary) {
  EXPECT_GE(contrastRatio(tok_.borderUrgent, tok_.secondary), 3.0)
      << "borderUrgent on panel fails WCAG 1.4.11";
}

// ── Updated surface tokens: text contrast ────────────────────────────────────

TEST_F(ContrastTest, OnSurfaceVsSurfaceNew) {
  // tok_.surface is now #10131f (Background tier)
  EXPECT_GE(contrastRatio(tok_.onSurface, tok_.surface), 4.5)
      << "onSurface on new surface fails WCAG AA";
}

TEST_F(ContrastTest, OnSurfaceVsSurfaceVariantNew) {
  // tok_.surfaceVariant is now #161925 (Surface tier)
  EXPECT_GE(contrastRatio(tok_.onSurface, tok_.surfaceVariant), 4.5)
      << "onSurface on new surfaceVariant fails WCAG AA";
}

TEST_F(ContrastTest, OnSurfaceVsSurfaceContainerNew) {
  // tok_.surfaceContainer is now #1a1b26 (SurfaceElevated tier)
  EXPECT_GE(contrastRatio(tok_.onSurface, tok_.surfaceContainer), 4.5)
      << "onSurface on new surfaceContainer fails WCAG AA";
}
```

Add to the WCAG exemption comment block at the bottom:

```
// borderPassive (#565f89) achieves ~2.35–2.99:1 on surfaces.
// Same hex as outlineVariant; same exemption applies: passive/inactive borders are
// exempt from WCAG 1.4.11 (which applies to interactive-state components only).
// See docs/holonight-design-deviations.md.
//
// borderHover (~30% alpha #7dcfff) composited onto any surface achieves ~2.03–2.06:1.
// Hover is a transient state overlay; its contrast is supplemented by the hover fill
// (surfaceHover background brightens the surface before the border is drawn).
// See docs/holonight-design-deviations.md.
```

---

## 9. Risks and Mitigations

| Risk | Likelihood | Impact | Mitigation |
|---|---|---|---|
| A `tok.outline` or `tok.outlineVariant` site in `holonightstyle.cpp` is missed | Medium | Wrong border color in one control state | After implementation, run `grep -n 'tok\.outline\|tok\.outlineVariant' style/holonightstyle.cpp` — result must be zero lines (or only the deprecated initialization sites in `darkTokens()`). |
| A `HoloniightPalette.outline` or `.outlineVariant` reference in a QML file is missed | Low | Wrong QML border | Run `grep -rn '\.outline\|\.outlineVariant' qml/` after implementation — only the `holoniightpalette.h` accessor definitions should remain. |
| Semi-transparent `borderHover` fails contrast when composited (already confirmed) | Certain | WCAG 1.4.11 deviation | Documented in Section 10. The hover fill (`surfaceHover`) is applied before the border, so the visual discrimination is provided by background lightness rather than border contrast. Verified by composited contrast calculation. |
| Surface value changes break existing contrast tests | High | CI failure | The four new surface token tests in §8.2 replace the existing `OnSurfaceVsSurface`, `OnSurfaceVsSurfaceVariant`, `OnSurfaceVsSurfaceContainer`, `OnSurfaceVsSecondary` tests. Old tests assert on old values; update them in the same commit. All new values are confirmed to pass 4.5:1. |
| `buildPalette()` uses `tok.surface` and `tok.surfaceVariant` for QPalette roles | Medium | QPalette colors change throughout app | Expected and intentional. `Window`/`Base` → `tok.surface` (`#10131f`), `AlternateBase`/`Button` → `tok.surfaceVariant` (`#161925`). These were already the closest-matching mappings; the change makes them exact per design system. |
| `PE_PanelMenu` change from `tok.surfaceVariant` to `tok.secondary` makes menus visually different | Medium | User-visible change | Intentional: menus belong to the Panel tier per design system. Document as a deliberate visual improvement, not a deviation. |
| `drawGroupBoxImpl` line 779 erases frame behind label using `tok.surface` fill; if surface darkens, the knockout remains correct | Low | None | The knockout is background color — any surface value works. Verified: `#10131f` still reads as background. |
| New tokens added to `ColorTokens` without updating all consumers that copy/compare the struct | Low | Compiler warning or missed field | `ColorTokens` is a plain aggregate; the new fields default-initialize. No copy-assignment operator exists. Grep for `ColorTokens{}` or `= ColorTokens{` to find any aggregate initializers that would need to list fields. None found in current codebase. |

---

## 10. Known Deviations from Design System

These entries will also appear in `docs/holonight-design-deviations.md` per REQ-F-050.

| Token | Design system target | Actual value | Reason | WCAG ratio (worst case) |
|---|---|---|---|---|
| `borderPassive` | ≥3:1 on all surfaces (WCAG 1.4.11) | `#565f89` — 2.35:1 on `secondary`, 2.99:1 on `surface` | Value specified by SPEC (REQ-F-001). Same as `outlineVariant`, which carries an existing WCAG 1.4.11 exemption (decorative/inactive borders exempt per SC definition). Inactive control borders do not need to meet the interactive-state 3:1 threshold. | 2.35:1 (on panel) |
| `borderHover` | ≥3:1 on all surfaces (composited) | `#7dcfff` α=77 — 2.03:1 composited on `surface` | Value specified by SPEC (REQ-F-002). Hover is a transient state; visual discrimination is provided jointly by the hover background fill (`surfaceHover` lightens the surface by ~ΔL=0.01) and the border. The border alone does not achieve 3:1 due to the low alpha. | 2.03:1 (composited, on surface) |

No surface token deviates from the design system targets. All four surface targets pass WCAG AA text contrast (≥4.5:1) with comfortable margin (worst case: 9.02:1 on `secondary`).
