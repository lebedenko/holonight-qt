# SPEC.md: Subpixel Border Rendering Fixes

**Feature:** Fix sub-pixel border rendering artifacts in `HoloniightStyle` checkbox and radio button indicators.

**Context:** At fractional display scaling (1.5×, 1.75×, etc.), bordered and rounded controls exhibit clipped or corrupted edges because border strokes are centered on rect edges (causing 50% clipping) and cosmetic pens do not scale with DPR. This SDD cycle addresses rendering correctness in `drawCheckBoxImpl()` and `drawRadioButtonImpl()` without adding API surface or changing palette tokens.

**Exclusions:**
- Visual polish (radius increase, cyan reduction, surface elevation): deferred to future SDD.
- `drawPushButtonBevelImpl()` / `drawPanelButtonImpl()`: already use correct inset pattern; no changes needed.
- QML/platform theme indicator drawing: out of scope (this SDD is QStyle plugin only).

---

## Functional Requirements

### REQ-F1: Checkbox indicator size shall use pixelMetric instead of hardcoded literal

**Ubiquitous:** The checkbox indicator in `drawCheckBoxImpl()` shall be sized using `style()->pixelMetric(PM_IndicatorWidth)` instead of the hardcoded value `14`.

**Acceptance Criterion:** A code review of `style/holonightstyle.cpp` `drawCheckBoxImpl()` confirms all indicator width assignments call `pixelMetric(PM_IndicatorWidth)` (which returns 16 for this theme) and contain no literal `14` or other hardcoded dimension.

### REQ-F2: Checkbox border rect shall be inset by half pen width before drawing

**Event-driven:** When `drawCheckBoxImpl()` is called with a checkbox option, the border rect shall be converted to `QRectF`, inset on all sides by 0.5 logical pixels, then passed to `drawRoundedRect()`.

**Acceptance Criterion:** The painted checkbox border in a fractional-DPR test image (or manual visual inspection at 1.5× scaling) shows no clipped edge or stroke overflow beyond the indicator rect bounds.

### REQ-F3: Checkbox border pen shall be non-cosmetic with explicit width

**State-driven:** When drawing the checkbox border, a pen with explicit width `1.0` (non-cosmetic) shall replace any `QPen(QColor)` constructor call.

**Acceptance Criterion:** A code review of `drawCheckBoxImpl()` confirms all border pens are constructed via `QPen(color, 1.0)` and no cosmetic pens (`width = 0` or default-constructed) are used for visible strokes.

### REQ-F4: Checkbox indicator drawing shall enable Antialiasing hint

**State-driven:** The `QPainter` used to draw the checkbox indicator shall have `QPainter::Antialiasing` render hint enabled before any `drawRoundedRect()` call.

**Acceptance Criterion:** A code review of `drawCheckBoxImpl()` shows `painter->setRenderHint(QPainter::Antialiasing, true)` is called before the `drawRoundedRect()` that paints the border.

### REQ-F5: Radio button indicator size shall use pixelMetric instead of hardcoded literal

**Ubiquitous:** The radio button indicator in `drawRadioButtonImpl()` shall be sized using `style()->pixelMetric(PM_ExclusiveIndicatorWidth)` instead of any hardcoded literal dimension.

**Acceptance Criterion:** A code review of `style/holonightstyle.cpp` `drawRadioButtonImpl()` confirms all indicator width assignments call `pixelMetric(PM_ExclusiveIndicatorWidth)` and contain no hardcoded numeric constants for indicator size.

### REQ-F6: Radio button border rect shall be inset by half pen width before drawing

**Event-driven:** When `drawRadioButtonImpl()` is called with a radio button option, the border rect shall be converted to `QRectF`, inset on all sides by 0.5 logical pixels, then passed to `drawEllipse()`.

**Acceptance Criterion:** The painted radio button border in a fractional-DPR test image (or manual visual inspection at 1.5× scaling) shows no clipped edge or stroke overflow beyond the indicator rect bounds.

### REQ-F7: Radio button border pen shall be non-cosmetic with explicit width

**State-driven:** When drawing the radio button border, a pen with explicit width `1.0` (non-cosmetic) shall replace any `QPen(QColor)` constructor call.

**Acceptance Criterion:** A code review of `drawRadioButtonImpl()` confirms all border pens are constructed via `QPen(color, 1.0)` and no cosmetic pens are used for visible strokes.

### REQ-F8: Radio button indicator drawing shall enable Antialiasing hint

**State-driven:** The `QPainter` used to draw the radio button indicator shall have `QPainter::Antialiasing` render hint enabled before any `drawEllipse()` call.

**Acceptance Criterion:** A code review of `drawRadioButtonImpl()` shows `painter->setRenderHint(QPainter::Antialiasing, true)` is called before the `drawEllipse()` that paints the border.

### REQ-F9: Existing checkbox state rendering shall be preserved

**Conditional:** If a checkbox option contains a checkmark, focus ring, or hover/pressed state overlay, these shall continue to be painted after the border fix with unchanged visual semantics.

**Acceptance Criterion:** All existing unit tests in `tests/test_style_checkbox.cpp` (if present) or integration tests pass without modification, and manual inspection confirms checkmark position and state overlays are visually unchanged.

### REQ-F10: Existing radio button state rendering shall be preserved

**Conditional:** If a radio button option contains a dot fill, focus ring, or hover/pressed state overlay, these shall continue to be painted after the border fix with unchanged visual semantics.

**Acceptance Criterion:** All existing unit tests in `tests/test_style_radio_button.cpp` (if present) or integration tests pass without modification, and manual inspection confirms dot position and state overlays are visually unchanged.

### REQ-F11: No change to indicator color tokens

**Unwanted behaviour:** The fix shall not introduce new palette tokens or modify existing color token definitions in `palette/holonight/palette.h`.

**Acceptance Criterion:** A `git diff` between the baseline and fixed state shows no changes to `palette/holonight/palette.h` and no new token enumerations in color selection logic.

---

## Non-Functional Requirements

### REQ-NF1: Code shall adhere to C++23 and project style rules

**Ubiquitous:** All modified code shall conform to C++23 language rules, 120-column line limit, 2-space indentation, and naming conventions (Classes: `CamelCase`, methods: `camelBack`, private members: `lower_case_`).

**Acceptance Criterion:** `cmake --build build --target tidy` runs with zero warnings on the modified files, and `clang-format --dry-run` shows no formatting changes needed.

### REQ-NF2: Build shall complete without errors

**Ubiquitous:** The modified style plugin code shall compile cleanly with `CMAKE_CXX_STANDARD=23` and `CMAKE_CXX_EXTENSIONS=OFF`.

**Acceptance Criterion:** `cmake --build build -j$(nproc)` exits with status 0 and all object files are generated without compiler errors or warnings.

### REQ-NF3: All tests shall pass

**Event-driven:** When the test suite runs after the fix is applied, all unit tests and integration tests shall pass.

**Acceptance Criterion:** `QT_QPA_PLATFORM=offscreen ctest --test-dir build --output-on-failure` exits with status 0 and reports 0 failing tests.

### REQ-NF4: No public API changes

**Ubiquitous:** The fix shall not add, remove, or modify any public method or class declarations in `HoloniightStyle` or related headers.

**Acceptance Criterion:** A comparison of public header files (`style/holonightstyle.h`, etc.) before and after the fix shows no signature or declaration changes.

---

## Constraints

### REQ-C1: Fix location scope

**Ubiquitous:** Only `drawCheckBoxImpl()` and `drawRadioButtonImpl()` functions in `style/holonightstyle.cpp` shall be modified.

**Acceptance Criterion:** A code review confirms no other functions in the style plugin source tree have been changed, and `git diff` shows changes only to these two function bodies.

### REQ-C2: Qt6 API compatibility

**Ubiquitous:** The fix shall use only Qt6 public APIs; deprecated Qt5 APIs (e.g. `QStyle::State_Up`) and non-standard extensions are forbidden.

**Acceptance Criterion:** No Qt5-specific headers or enums appear in the modified code, and the code compiles with `QT_VERSION_CHECK(6, ...)` guards if any version-conditional logic is needed (it is not).

### REQ-C3: DPR-aware rendering

**State-driven:** All geometric calculations for borders and insets shall account for fractional display scaling (1.5×, 1.75×, etc.) via the use of `QRectF` and explicit (non-cosmetic) pen widths.

**Acceptance Criterion:** Manual testing at 1.5× DPR (or code review of QRectF usage and pen construction) shows no hardcoded integer rect operations or cosmetic pens in the border-drawing paths.

### REQ-C4: Qt pixel metric compliance

**Ubiquitous:** Indicator dimensions shall be determined via `style()->pixelMetric(PM_IndicatorWidth)` or `style()->pixelMetric(PM_ExclusiveIndicatorWidth)` rather than theme-specific magic numbers.

**Acceptance Criterion:** A code review of `drawCheckBoxImpl()` and `drawRadioButtonImpl()` confirms all indicator size calculations use `pixelMetric()` calls and no literal constants like `14` or `16` appear as dimensions.

---

## Implementation Notes

### Drawing Pattern

The corrected pattern for indicator borders is:

```cpp
QRectF r(option->rect);
r.adjust(0.5, 0.5, -0.5, -0.5);
QPen pen(borderColor, 1.0);  // non-cosmetic, width=1.0
painter->setPen(pen);
painter->setRenderHint(QPainter::Antialiasing, true);
painter->drawRoundedRect(r, 3.0, 3.0);  // or drawEllipse(r)
```

### Unmodified Functions

- `drawPushButtonBevelImpl()`: uses `adjusted(borderWidth, ...)` inset — no changes.
- `drawPanelButtonImpl()`: uses `adjusted(borderWidth, ...)` inset — no changes.
- Platform theme indicator code: out of scope.

### Test Expectations

- No pixel-level assertions in existing tests (they test compilation and high-level behavior).
- All existing tests shall pass without modification.
- Rendering artifacts at 1.5× DPR shall be validated via manual inspection or an optional future visual regression test suite.

---

## Acceptance Checklist

- [ ] `drawCheckBoxImpl()` uses `pixelMetric(PM_IndicatorWidth)` for indicator size.
- [ ] `drawCheckBoxImpl()` insets indicator rect by 0.5px before `drawRoundedRect()`.
- [ ] `drawCheckBoxImpl()` uses `QPen(color, 1.0)` for all visible borders.
- [ ] `drawCheckBoxImpl()` sets `QPainter::Antialiasing` before drawing.
- [ ] `drawRadioButtonImpl()` uses `pixelMetric(PM_ExclusiveIndicatorWidth)` for indicator size.
- [ ] `drawRadioButtonImpl()` insets indicator rect by 0.5px before `drawEllipse()`.
- [ ] `drawRadioButtonImpl()` uses `QPen(color, 1.0)` for all visible borders.
- [ ] `drawRadioButtonImpl()` sets `QPainter::Antialiasing` before drawing.
- [ ] `cmake --build build -j$(nproc)` compiles cleanly.
- [ ] `cmake --build build --target tidy` passes with zero warnings.
- [ ] `QT_QPA_PLATFORM=offscreen ctest --test-dir build --output-on-failure` passes all tests.
- [ ] No changes to `palette/holonight/palette.h`.
- [ ] No public API surface added or removed.
- [ ] Manual visual inspection at 1.5× DPR confirms no border clipping.

