# DESIGN.md: Subpixel Border Rendering Fixes

**Spec:** `docs/sdd/subpixel-border-fixes/SPEC.md`
**Scope:** `style/holonightstyle.cpp` — `drawCheckBoxImpl()` and `drawRadioButtonImpl()` only.

---

## Component Affected

**File:** `style/holonightstyle.cpp`

### `drawCheckBoxImpl()` (line ~287)

Today: computes a 14×14 `QRect indicator` using a hardcoded literal `14`. Draws the unchecked border with a cosmetic pen (`QPen{pal.color(QPalette::Mid)}` — no explicit width). Sets `QPainter::Antialiasing` before the draw calls, but the integer rect is never inset, so half the 1px stroke falls outside the allocated rect and gets clipped by Qt's painter clip region.

After fix: indicator size comes from `pixelMetric(PM_IndicatorWidth)` (returns 16). The border rect is widened to `QRectF` and inset by 0.5px. The unchecked-border pen is `QPen(color, 1.0)` (non-cosmetic). Antialiasing is set explicitly before the border draw call (currently set once at the top of the function — fine, but made explicit near the draw call for clarity). Checkmark line offsets are updated to stay visually centered in the now-16px indicator.

### `drawRadioButtonImpl()` (line ~318)

Today: indicator is `QRect{..., 16, 16}` — size is already 16 but hardcoded. Border pen is `QPen{pal.color(QPalette::Mid)}` (cosmetic). The integer `QRect` is passed directly to `drawEllipse()` without inset. `QPainter::Antialiasing` is set at the top of the function.

After fix: size comes from `pixelMetric(PM_ExclusiveIndicatorWidth)`. `QRectF` + 0.5px inset before `drawEllipse()`. Pen is `QPen(color, 1.0)`. Antialiasing confirmed set before the draw call.

---

## Call Path

```
Qt widget paint event
  └─ QStyle::drawControl(CE_CheckBox / CE_RadioButton, ...)
       └─ HoloniightStyle::drawControl()          [holonightstyle.cpp:116]
            ├─ CE_CheckBox  → drawCheckBoxImpl()  [holonightstyle.cpp:287]
            └─ CE_RadioButton → drawRadioButtonImpl() [holonightstyle.cpp:318]
```

Qt never calls `drawPrimitive(PE_IndicatorCheckBox)` for a standalone `QCheckBox` — that path is used when a `QStyleOptionViewItem` embeds a checkbox (e.g., a `QTreeView` column). `CE_CheckBox` is the full-widget path and owns the indicator rect computation.

---

## Key Design Decisions

### 1. `QRectF` + `adjust(0.5, 0.5, -0.5, -0.5)` fixes stroke clipping

QPainter strokes a path by centering the pen width on the path boundary. For a 1px pen on a `QRect`, the outer half of the stroke (0.5px) falls outside the rect. Qt's painter clip is set to `option->rect`, so that outer half is clipped, producing a faint or missing edge.

Converting to `QRectF` and shrinking by 0.5px on each side moves the path boundary inward. The stroke now spans from 0px to 1px relative to the original edge — fully inside the clip region. The visual control is the same allocated size; only the stroke placement changes.

### 2. Non-cosmetic pen `QPen(color, 1.0)` instead of `QPen(color)`

`QPen(QColor)` produces a cosmetic pen (width = 0). A cosmetic pen is always 1 physical pixel wide regardless of DPR. At 1.5× DPR, 1 physical pixel = 0.667 logical pixels — the stroke appears thinner than intended and does not benefit from the 0.5px inset (the inset is in logical pixels).

`QPen(color, 1.0)` is non-cosmetic: width is 1 logical pixel, which scales to 1.5 physical pixels at 1.5× DPR. The 0.5 logical-px inset then correctly lands the stroke fully inside the clip region at any DPR.

### 3. Antialiasing must be set before the draw call

`QPainter::Antialiasing` is a per-call render hint. The painter state comes from the caller (Qt's widget paint machinery), which may or may not have antialiasing enabled. Relying on inherited state is fragile. Setting it explicitly before `drawRoundedRect()` / `drawEllipse()` is required for the 0.5px fractional inset to render smoothly rather than as a jagged half-pixel step.

The current code already calls `painter->setRenderHint(QPainter::Antialiasing)` at the top of both functions (inside `painter->save()` / `painter->restore()`), so this is already correct — the design preserves this placement and makes it visible in the diff.

### 4. Indicator size from `pixelMetric()` not a literal

`pixelMetric(PM_IndicatorWidth)` and `pixelMetric(PM_ExclusiveIndicatorWidth)` both return 16 (see `holonightstyle.cpp:103–110`). Using `pixelMetric()` means the impl function is consistent with what Qt uses to allocate space for the indicator in `subElementRect(SE_CheckBoxIndicator)`. A literal `14` in `drawCheckBoxImpl()` was already mismatched with the declared metric (16), making the indicator render 2px narrower than the allocated space.

---

## Alternatives Considered

**Cosmetic pen (width = 0):** Always 1 physical pixel regardless of DPR. At 2× it looks fine; at 1.5× it is visibly thinner and still requires a 0.5 physical-pixel inset (not 0.5 logical). Using a cosmetic pen with a `QRectF` inset only works correctly at integer DPR. Rejected.

**Integer inset `adjusted(1, 1, -1, -1)`:** The button drawing code (`drawPushButtonBevelImpl`) uses `adjusted(borderWidth, ...)` because it supports variable border widths (1px or 2px for focus). For indicators the border is always 1px, so inset by 1 full pixel wastes 1px of allocated space on each side — the indicator renders 2px narrower and shorter than `pixelMetric()` promises. Already used correctly for buttons; not appropriate for fixed-size indicators.

**Changing `PM_IndicatorWidth` return value:** The value is already 16 — the only mismatch was `drawCheckBoxImpl` using `14` as a literal. No metric change needed.

---

## Known Risks

**Checkmark line offsets after indicator size change (checkbox only)**

The checkmark in `drawCheckBoxImpl` is drawn with hardcoded pixel offsets relative to `indicator.left()` and `indicator.top()`:

```cpp
// Current (14px indicator):
painter->drawLine(indicator.left() + 3,  indicator.top() + 7,
                  indicator.left() + 6,  indicator.top() + 10);
painter->drawLine(indicator.left() + 6,  indicator.top() + 10,
                  indicator.left() + 11, indicator.top() + 4);
```

When the indicator grows from 14px to 16px, these offsets keep the checkmark anchored to the top-left corner, shifting it visually up-left by 1px. The offsets must be updated to re-center the glyph. Approximate corrected offsets for a 16px box:

```cpp
// After (16px indicator):
painter->drawLine(indicator.left() + 4,  indicator.top() + 8,
                  indicator.left() + 7,  indicator.top() + 11);
painter->drawLine(indicator.left() + 7,  indicator.top() + 11,
                  indicator.left() + 12, indicator.top() + 5);
```

Exact values should be verified with visual inspection at 1× and 1.5× DPR.

**Half-pixel inset at 1× integer DPR**

At 1× DPR, 0.5 logical px = 0.5 physical px — a sub-pixel boundary. This requires antialiasing to render cleanly (it will render as a 50% alpha pixel along the edge). Since antialiasing is explicitly enabled, this is by design and produces no regression compared to the clipped border that exists today.

---

## Diff Sketch

### `drawCheckBoxImpl()` — before / after

```cpp
// BEFORE
const int top = option->rect.top() + ((option->rect.height() - 14) / 2);
const QRect indicator{option->rect.left(), top, 14, 14};

painter->save();
painter->setRenderHint(QPainter::Antialiasing);
if (checked) {
  painter->setPen(Qt::NoPen);
  painter->setBrush(pal.color(QPalette::Highlight));
  painter->drawRoundedRect(indicator, 3.0, 3.0);           // QRect, no inset
  painter->setPen(QPen{pal.color(QPalette::HighlightedText), 2.0});
  painter->drawLine(indicator.left() + 3, indicator.top() + 7,   // offsets for 14px
                    indicator.left() + 6, indicator.top() + 10);
  painter->drawLine(indicator.left() + 6, indicator.top() + 10,
                    indicator.left() + 11, indicator.top() + 4);
} else {
  painter->setPen(pal.color(QPalette::Mid));               // cosmetic pen
  painter->setBrush(pal.color(QPalette::Base));
  painter->drawRoundedRect(indicator, 3.0, 3.0);           // QRect, no inset
}

// AFTER
const int side = style()->pixelMetric(PM_IndicatorWidth, option, widget);  // 16
const int top = option->rect.top() + ((option->rect.height() - side) / 2);
const QRect indicator{option->rect.left(), top, side, side};
const QRectF borderRect = QRectF(indicator).adjusted(0.5, 0.5, -0.5, -0.5);

painter->save();
painter->setRenderHint(QPainter::Antialiasing);
if (checked) {
  painter->setPen(Qt::NoPen);
  painter->setBrush(pal.color(QPalette::Highlight));
  painter->drawRoundedRect(borderRect, 3.0, 3.0);          // QRectF, no border stroke needed
  painter->setPen(QPen{pal.color(QPalette::HighlightedText), 2.0});
  painter->drawLine(indicator.left() + 4, indicator.top() + 8,   // offsets re-centered for 16px
                    indicator.left() + 7, indicator.top() + 11);
  painter->drawLine(indicator.left() + 7, indicator.top() + 11,
                    indicator.left() + 12, indicator.top() + 5);
} else {
  painter->setPen(QPen{pal.color(QPalette::Mid), 1.0});    // non-cosmetic, width=1.0
  painter->setBrush(pal.color(QPalette::Base));
  painter->drawRoundedRect(borderRect, 3.0, 3.0);          // QRectF, 0.5px inset
}
```

### `drawRadioButtonImpl()` — before / after

```cpp
// BEFORE
const int top = option->rect.top() + ((option->rect.height() - 16) / 2);
const QRect indicator{option->rect.left(), top, 16, 16};  // literal 16

painter->save();
painter->setRenderHint(QPainter::Antialiasing);
painter->setPen(pal.color(QPalette::Mid));                 // cosmetic pen
painter->setBrush(checked ? pal.color(QPalette::Highlight) : pal.color(QPalette::Base));
painter->drawEllipse(indicator);                           // QRect, no inset
if (checked) {
  painter->setPen(Qt::NoPen);
  painter->setBrush(pal.color(QPalette::HighlightedText));
  painter->drawEllipse(indicator.adjusted(4, 4, -4, -4));
}

// AFTER
const int side = style()->pixelMetric(PM_ExclusiveIndicatorWidth, option, widget);  // 16
const int top = option->rect.top() + ((option->rect.height() - side) / 2);
const QRect indicator{option->rect.left(), top, side, side};
const QRectF borderRect = QRectF(indicator).adjusted(0.5, 0.5, -0.5, -0.5);

painter->save();
painter->setRenderHint(QPainter::Antialiasing);
painter->setPen(QPen{pal.color(QPalette::Mid), 1.0});      // non-cosmetic, width=1.0
painter->setBrush(checked ? pal.color(QPalette::Highlight) : pal.color(QPalette::Base));
painter->drawEllipse(borderRect);                          // QRectF, 0.5px inset
if (checked) {
  painter->setPen(Qt::NoPen);
  painter->setBrush(pal.color(QPalette::HighlightedText));
  painter->drawEllipse(QRectF(indicator).adjusted(4.5, 4.5, -4.5, -4.5));  // inner dot also on QRectF
}
```

Note: the inner dot `adjusted(4, 4, -4, -4)` uses integer `QRect` math today. Changing to `QRectF` with `4.5` inset keeps the dot centered on the inset outer ellipse. This is a cosmetic improvement; `4` vs `4.5` is not required by the spec but is consistent with the fix.
