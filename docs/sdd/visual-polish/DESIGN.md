# DESIGN: Visual Polish — Corner Radius, Border De-Cyanification, Surface Elevation

**Feature:** Visual polish pass on the HoloNight Qt6 theme  
**Date:** 2026-05-20  
**Status:** Ready for implementation  
**Spec:** `docs/sdd/visual-polish/SPEC.md`

---

## 1. Files Changed

| File | Change |
|------|--------|
| `style/holonightstyle.cpp` | Define `kRadiusControl` and `kRadiusTight`; replace all radius literals in draw functions; update `standardPalette()` Mid/Dark/Shadow; update `PE_PanelLineEdit` and `PE_PanelItemViewRow` fills |
| `palette/palette.cpp` | Change Mid/Dark/Shadow to `tok.borderPassive`; change Base to `tok.surfaceVariant` in Active and Disabled groups |
| `tests/test_palette_qpalette.cpp` | Add `TEST_F(PaletteTest, BorderRolesAreBorderPassive)` and `TEST_F(PaletteTest, BaseIsSurfaceVariant)` per REQ-F-032 |

No other files are modified. `palette/holonight/palette.h` is untouched.

---

## 2. Workstream A: Corner Radius

### Constant definition

Add immediately after the Qt/system includes (before the first function definition) in `style/holonightstyle.cpp`:

```cpp
namespace {
constexpr qreal kRadiusControl = 6.0;  // unified control corner radius
constexpr qreal kRadiusTight   = 2.0;  // tight radius for item-view selection/focus rings
}  // namespace
```

The anonymous namespace keeps these out of the header (REQ-F-001, REQ-C-010) and prevents ODR violations if the TU is ever included transitively. They are not `inline constexpr` at file scope to avoid polluting any future linkage.

### Exhaustive call-site inventory

Every `drawRoundedRect` call that uses a shape-defining radius (not a track groove or scrollbar detail):

| Function | Current literal | Replace with | Line (approx.) |
|---|---|---|---|
| `drawPushButtonBevelImpl` | `4.0` | `kRadiusControl` | 283 |
| `drawPanelButtonImpl` | `4.0` | `kRadiusControl` | 450 |
| `drawCheckBoxImpl` (checked fill) | `3.0` | `kRadiusControl` | 304 |
| `drawCheckBoxImpl` (unchecked border) | `3.0` | `kRadiusControl` | 311 |
| `drawItemViewItemImpl` (selected row) | `3.0` | `kRadiusControl` | 499 |
| `drawItemViewItemImpl` (hovered row) | `3.0` | `kRadiusControl` | 502 |
| `drawItemViewItemImpl` (focus ring) | `3.0` | `kRadiusTight` | 561 |
| `drawPanelItemViewImpl` (selected) | `2.0` | `kRadiusTight` | 468 |
| `drawPanelItemViewImpl` (hovered) | `2.0` | `kRadiusTight` | 471 |
| `drawPanelItemViewImpl` (focus ring) | `2.0` | `kRadiusTight` | 476 |
| `PE_PanelLineEdit` | `4.0` | `kRadiusControl` | 746 |
| `PE_FrameGroupBox` | `4.0` | `kRadiusControl` | 756 |
| `PE_FrameDefaultButton` | `5.0` | `kRadiusControl` | 583 |
| `CC_ComboBox` | `4.0` | `kRadiusControl` | 808 |
| `CC_SpinBox` | `4.0` | `kRadiusControl` | 986 |
| `drawGroupBoxImpl` | `4.0` | `kRadiusControl` | 1058 |
| `drawToolButtonImpl` (bg) | `4.0` | `kRadiusControl` | 1029 |
| `drawToolButtonImpl` (focus ring) | `4.0` | `kRadiusControl` | 1034 |
| `CE_MenuItem` highlight | `4.0` | `kRadiusControl` | 150 |
| `CE_MenuBarItem` highlight | `4.0` | `kRadiusControl` | 163 |

### Intentional exceptions (not replaced)

| Location | Value | Reason |
|---|---|---|
| `CE_ProgressBarGroove` | `3.0` | Thin groove; not a control shape. Keep as-is or use a new `kRadiusGroove = 3.0` constant. |
| `CE_ProgressBarContents` | `3.0` | Same as above. |
| `CE_ScrollBarSlider` | `6.0` | Already matches `kRadiusControl`; replace with the constant anyway for consistency. |
| `drawScrollBarImpl` track | `2.0` | Scrollbar track grooves; visual scale differs from control shapes; keep with a comment. |
| `drawScrollBarImpl` handle | `4.0` | Scrollbar handle; not a control — can be `kRadiusControl` or kept separate. |
| `drawSliderImpl` grooves | `2.0` | Slider track grooves; same logic as scrollbar track. |
| `drawSliderImpl` handle | `2.0` | Small slider handle; tighter radius fits 14×14 size — keep as `kRadiusTight`. |

### Post-implementation verification (REQ-F-008)

```bash
grep -n "drawRoundedRect" style/holonightstyle.cpp | grep -v "kRadius"
```
Must return zero matches.

---

## 3. Workstream B: Border De-Cyanification

### Root cause

`QPalette::Mid` and `QPalette::Dark` are set to `tok.outline` (`#00a0c8`, cyan) in `buildPalette()`. Many Qt native controls (scrollbars, separators, some frames) read `Mid` for their border stroke directly from the palette without going through `drawPrimitive`. Replacing with `tok.borderPassive` (`#565f89`) removes the cyan bleed.

### Exact lines in `palette/palette.cpp` to change

**Active group (lines 98–101):**

```cpp
// Before
pal.setColor(QPalette::Active, QPalette::Mid,    tok.outline);
pal.setColor(QPalette::Active, QPalette::Dark,   tok.outline);
...
pal.setColor(QPalette::Active, QPalette::Shadow, tok.outlineVariant);

// After
pal.setColor(QPalette::Active, QPalette::Mid,    tok.borderPassive);
pal.setColor(QPalette::Active, QPalette::Dark,   tok.borderPassive);
...
pal.setColor(QPalette::Active, QPalette::Shadow, tok.borderPassive);
```

The Inactive group is a full copy of Active (lines 108–111) so no Inactive-specific change is needed; it inherits automatically.

**Disabled group (lines 119–122):**

```cpp
// Before
pal.setColor(QPalette::Disabled, QPalette::Mid,    tok.outline);
pal.setColor(QPalette::Disabled, QPalette::Dark,   tok.outline);
...
pal.setColor(QPalette::Disabled, QPalette::Shadow, tok.outlineVariant);

// After
pal.setColor(QPalette::Disabled, QPalette::Mid,    tok.borderPassive);
pal.setColor(QPalette::Disabled, QPalette::Dark,   tok.borderPassive);
...
pal.setColor(QPalette::Disabled, QPalette::Shadow, tok.borderPassive);
```

### Exact lines in `standardPalette()` / `polish(QWidget*)` to verify

`HoloniightStyle::standardPalette()` delegates entirely to `buildPalette(darkTokens())` (line 17), so no separate change is required there — the palette function is the single source of truth.

`polish(QWidget*)` (lines 41–42) already sets Mid and Shadow to `tok.borderPassive`. After the `buildPalette` fix they are consistent. `Dark` is not set in `polish()`; it falls back to the palette already carrying `tok.borderPassive` — acceptable.

### `tok.outline` retained (REQ-F-014)

`palette/holonight/palette.h` is not modified. `tok.outline` remains in `ColorTokens`. After this workstream `grep "tok\.outline" palette/palette.cpp` returns zero assignment matches (the token exists but is no longer used in role assignments). A deprecation comment already exists on both `outline` and `outlineVariant` in the header.

---

## 4. Workstream C: Surface Elevation

### The four sites that must change together

All four reference the "input/list background" semantic and must agree on `surfaceVariant` (`#161925`):

| Site | File | Current value | New value |
|---|---|---|---|
| `buildPalette()` Active Base (line 89) | `palette/palette.cpp` | `tok.surface` | `tok.surfaceVariant` |
| `buildPalette()` Disabled Base (line 115) | `palette/palette.cpp` | `tok.surface` | `tok.surfaceVariant` |
| `PE_PanelLineEdit` fill (line 737) | `style/holonightstyle.cpp` | `tok.surface` | `tok.surfaceVariant` |
| `PE_PanelItemViewRow` default row (line 725) | `style/holonightstyle.cpp` | `tok.surface` | `tok.surfaceVariant` |

`polish(QWidget*)` line 34 sets `Base = tok.surface` on every polished widget. This also needs updating to `tok.surfaceVariant` for consistency, otherwise any polished widget resets the value set by `buildPalette`.

### Exact diffs

**`palette/palette.cpp` — Active group line 89:**
```cpp
// Before
pal.setColor(QPalette::Active, QPalette::Base, tok.surface);
// After
pal.setColor(QPalette::Active, QPalette::Base, tok.surfaceVariant);
```

**`palette/palette.cpp` — Disabled group line 115:**
```cpp
// Before
pal.setColor(QPalette::Disabled, QPalette::Base, tok.surface);
// After
pal.setColor(QPalette::Disabled, QPalette::Base, tok.surfaceVariant);
```

**`style/holonightstyle.cpp` — `polish(QWidget*)` line 34:**
```cpp
// Before
palette.setColor(QPalette::Base, tok.surface);
// After
palette.setColor(QPalette::Base, tok.surfaceVariant);
```

**`style/holonightstyle.cpp` — `PE_PanelLineEdit` line 737:**
```cpp
// Before
painter->setBrush(tok.surface);
// After
painter->setBrush(tok.surfaceVariant);
```

**`style/holonightstyle.cpp` — `PE_PanelItemViewRow` default branch line 725:**
```cpp
// Before
painter->setBrush(tok.surface);
// After
painter->setBrush(tok.surfaceVariant);
```

Note: `AlternateBase` is already `tok.surfaceVariant` in both `buildPalette()` and `polish()`. After this change, `Base` and `AlternateBase` are the same color — this is intentional for the current design; alternating row stripes can be revisited in a subsequent pass if needed.

---

## 5. Tests

### Right file: `tests/test_palette_qpalette.cpp`

This file already exercises `buildPalette(darkTokens())` via a `PaletteTest` fixture (line 10). REQ-F-032 assertions belong here as new `TEST_F` cases.

### Exact assertions to add

Append after the existing `HighlightIsHolonightPrimary` test:

```cpp
TEST_F(PaletteTest, BorderRolesAreBorderPassive) {
  const Holonight::ColorTokens tok = Holonight::darkTokens();
  EXPECT_EQ(palette_.color(QPalette::Active, QPalette::Mid),    tok.borderPassive)
      << "Mid should be borderPassive (#565f89), not cyan outline";
  EXPECT_EQ(palette_.color(QPalette::Active, QPalette::Dark),   tok.borderPassive)
      << "Dark should be borderPassive (#565f89)";
  EXPECT_EQ(palette_.color(QPalette::Active, QPalette::Shadow), tok.borderPassive)
      << "Shadow should be borderPassive (#565f89)";
  // Inactive mirrors Active
  EXPECT_EQ(palette_.color(QPalette::Inactive, QPalette::Mid),    tok.borderPassive);
  EXPECT_EQ(palette_.color(QPalette::Inactive, QPalette::Dark),   tok.borderPassive);
  EXPECT_EQ(palette_.color(QPalette::Inactive, QPalette::Shadow), tok.borderPassive);
  // Disabled also uses borderPassive (not dimmed — border tokens are structural, not content)
  EXPECT_EQ(palette_.color(QPalette::Disabled, QPalette::Mid),    tok.borderPassive);
  EXPECT_EQ(palette_.color(QPalette::Disabled, QPalette::Dark),   tok.borderPassive);
  EXPECT_EQ(palette_.color(QPalette::Disabled, QPalette::Shadow), tok.borderPassive);
}

TEST_F(PaletteTest, BaseIsSurfaceVariant) {
  const Holonight::ColorTokens tok = Holonight::darkTokens();
  EXPECT_EQ(palette_.color(QPalette::Active,   QPalette::Base), tok.surfaceVariant)
      << "Base should be surfaceVariant (#161925) to elevate list views and text fields";
  EXPECT_EQ(palette_.color(QPalette::Disabled, QPalette::Base), tok.surfaceVariant)
      << "Disabled Base should also be surfaceVariant";
  // Inactive mirrors Active
  EXPECT_EQ(palette_.color(QPalette::Inactive, QPalette::Base), tok.surfaceVariant);
}
```

These will **fail before** the implementation and **pass after** — they serve as the acceptance gate for workstreams B and C.

The existing `DisabledFillRolesUnchanged` test asserts `palette_.color(QPalette::Disabled, Base) == palette_.color(QPalette::Active, Base)` which remains true after the change (both become `surfaceVariant`), so no conflict.

---

## 6. Key Rationale

### Why `kRadiusControl` in an anonymous namespace (not a header)

REQ-C-010 and REQ-C-011 restrict changes to `.cpp` files only. An anonymous namespace constant has internal linkage — it does not leak into the plugin's ABI, cannot be odr-used from other TUs, and will not appear in future headers by accident. Defining it in a header would require modifying `holonightstyle.h`, which is out of scope.

### Why `Mid`/`Dark`/`Shadow` → `borderPassive` resolves the cyan issue

Qt's Fusion base style (which HoloniightStyle wraps via `QProxyStyle`) reads `QPalette::Mid` for many frame and border primitives that fall through to the base implementation — scrollbar grooves, frame borders, separator lines — without calling `drawPrimitive`. Because `standardPalette()` delegates to `buildPalette()` and `polish()` propagates the palette to every widget, fixing Mid/Dark/Shadow in `buildPalette()` is the single change that eliminates cyan from all native-drawn controls simultaneously, with no need to override every fallback path individually.

### Why all four Base-related sites must change together

`buildPalette()` sets the application-level palette used by `QApplication::setPalette`. `polish(QWidget*)` overwrites the palette on every individual widget it touches (line 34). If only `buildPalette()` is fixed, any widget processed by `polish()` silently reverts `Base` back to `tok.surface`. `PE_PanelLineEdit` and `PE_PanelItemViewRow` paint directly to the device context using `tok.surface` rather than reading from the palette, so they must also be updated independently. All four must be synchronized or the rendered background will be visually inconsistent.

---

## 7. Known Risks

### `QPalette::Mid` used for non-border purposes

`Mid` is read by `CE_MenuItem` separator drawing (line 141):
```cpp
painter->setPen(option->palette.color(QPalette::Mid));
```
After the change `Mid` is `borderPassive` (`#565f89`), which is the correct muted blue-gray for a separator line. This is a semantic improvement, not a regression.

Fusion base style also uses `Mid` for some trough/groove fills (e.g. progress bar groove trough). `CE_ProgressBarGroove` is fully overridden in HoloniightStyle (line 173) and does not read from the palette — no impact.

`QAbstractScrollArea` and `QFrame` read `Mid` for their frame border color via `PE_Frame`. HoloniightStyle overrides `PE_Frame` (line 589) using `tok.borderPassive` directly, so the palette value is not consulted — safe.

### `QPalette::Dark` currently unused in draw paths

`Dark` is set in `buildPalette()` but is not explicitly read by any `drawPrimitive` or `drawControl` override. Fusion base style may read it for 3D shading effects on non-overridden controls (combo popup borders, some scrollbar elements). The change from cyan to muted blue-gray is a visual improvement in those edge cases.

### `AlternateBase == Base` after elevation

After the change both `Base` and `AlternateBase` are `tok.surfaceVariant` (`#161925`). Alternating row zebra-striping will disappear in list views that enable `setAlternatingRowColors(true)`. This is a known side-effect and is acceptable for the current visual direction. If striping is desired later, `AlternateBase` can be moved to `tok.surfaceContainer` in a follow-up.

### `PE_FrameDefaultButton` radius increase (5.0 → 6.0)

The default-button outer glow ring expands by 1pt. The adjusted rect is already inflated by `(-1,-1,1,1)`, so the visual difference is minor. No regression expected.

---

## 8. Diff Sketch

### Workstream A — `style/holonightstyle.cpp` (representative, not exhaustive)

```diff
+namespace {
+constexpr qreal kRadiusControl = 6.0;
+constexpr qreal kRadiusTight   = 2.0;
+}  // namespace
+
 ...
 // drawPushButtonBevelImpl
-  painter->drawRoundedRect(..., 4.0, 4.0);
+  painter->drawRoundedRect(..., kRadiusControl, kRadiusControl);

 // drawCheckBoxImpl
-  painter->drawRoundedRect(borderRect, 3.0, 3.0);
+  painter->drawRoundedRect(borderRect, kRadiusControl, kRadiusControl);

 // drawItemViewItemImpl — selection/hover rows
-  painter->drawRoundedRect(opt->rect.adjusted(1,1,-1,-1), 3.0, 3.0);
+  painter->drawRoundedRect(opt->rect.adjusted(1,1,-1,-1), kRadiusControl, kRadiusControl);

 // drawItemViewItemImpl — focus ring
-  painter->drawRoundedRect(opt->rect.adjusted(1,1,-1,-1), 3.0, 3.0);
+  painter->drawRoundedRect(opt->rect.adjusted(1,1,-1,-1), kRadiusTight, kRadiusTight);

 // PE_PanelLineEdit
-  painter->drawRoundedRect(..., 4.0, 4.0);
+  painter->drawRoundedRect(..., kRadiusControl, kRadiusControl);

 // PE_FrameGroupBox
-  painter->drawRoundedRect(..., 4.0, 4.0);
+  painter->drawRoundedRect(..., kRadiusControl, kRadiusControl);
```

### Workstream B — `palette/palette.cpp`

```diff
-  pal.setColor(QPalette::Active,   QPalette::Mid,    tok.outline);
-  pal.setColor(QPalette::Active,   QPalette::Dark,   tok.outline);
-  pal.setColor(QPalette::Active,   QPalette::Shadow, tok.outlineVariant);
+  pal.setColor(QPalette::Active,   QPalette::Mid,    tok.borderPassive);
+  pal.setColor(QPalette::Active,   QPalette::Dark,   tok.borderPassive);
+  pal.setColor(QPalette::Active,   QPalette::Shadow, tok.borderPassive);
 ...
-  pal.setColor(QPalette::Disabled, QPalette::Mid,    tok.outline);
-  pal.setColor(QPalette::Disabled, QPalette::Dark,   tok.outline);
-  pal.setColor(QPalette::Disabled, QPalette::Shadow, tok.outlineVariant);
+  pal.setColor(QPalette::Disabled, QPalette::Mid,    tok.borderPassive);
+  pal.setColor(QPalette::Disabled, QPalette::Dark,   tok.borderPassive);
+  pal.setColor(QPalette::Disabled, QPalette::Shadow, tok.borderPassive);
```

### Workstream C — both files

```diff
 // palette/palette.cpp
-  pal.setColor(QPalette::Active,   QPalette::Base, tok.surface);
+  pal.setColor(QPalette::Active,   QPalette::Base, tok.surfaceVariant);
 ...
-  pal.setColor(QPalette::Disabled, QPalette::Base, tok.surface);
+  pal.setColor(QPalette::Disabled, QPalette::Base, tok.surfaceVariant);

 // style/holonightstyle.cpp — polish(QWidget*)
-  palette.setColor(QPalette::Base, tok.surface);
+  palette.setColor(QPalette::Base, tok.surfaceVariant);

 // style/holonightstyle.cpp — PE_PanelLineEdit
-  painter->setBrush(tok.surface);
+  painter->setBrush(tok.surfaceVariant);

 // style/holonightstyle.cpp — PE_PanelItemViewRow default branch
-  painter->setBrush(tok.surface);
+  painter->setBrush(tok.surfaceVariant);
```
