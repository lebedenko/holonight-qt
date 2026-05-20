# HoloNight Qt/QML Theme Rendering Audit

## Issue Summary

The checkbox and radio button indicators show corrupted borders on the left edge when rendered under fractional scaling (1.5x monitor scaling).

The same visual artifacts may also appear on:
- radio buttons
- bordered controls
- rounded rectangles
- input borders
- focus rings

This is most likely a rendering/alignment issue related to:
- sub-pixel painting
- pen alignment
- fractional scaling
- clipped borders
- odd indicator sizes

---

# Root Causes

## 1. Sub-pixel Painting at Fractional Scaling

At scaling factor `1.5`, logical coordinates frequently land between physical pixels.

Example:

```cpp
QRectF r = option->rect;
painter->drawRoundedRect(r, 3, 3);
```

The border stroke becomes partially rendered between pixels, causing:
- blurred edges
- clipped borders
- asymmetric rendering
- corrupted left/top edges

---

# Recommended Fix

Inset the painted rect by half the pen width.

Recommended:

```cpp
const qreal penWidth = 1.0;

QRectF r = option->rect;

r.adjust(
    penWidth / 2.0,
    penWidth / 2.0,
    -penWidth / 2.0,
    -penWidth / 2.0
);

QPen pen(borderColor, penWidth);

painter->setPen(pen);

painter->drawRoundedRect(r, 3, 3);
```

This prevents the stroke from being clipped by the control boundary.

---

# 2. Border Painted Exactly on Edge

Qt paints strokes centered on geometry edges.

That means:
- half the border is inside
- half the border is outside

If the rect touches the edge of the available area,
part of the border gets clipped.

Bad:

```cpp
painter->drawRoundedRect(option->rect, 3, 3);
```

Better:

```cpp
QRectF r = option->rect.adjusted(
    0.5,
    0.5,
    -0.5,
    -0.5
);

painter->drawRoundedRect(r, 3, 3);
```

---

# 3. Fractional Indicator Sizes

Checkboxes/radio buttons often break visually when their final physical size becomes fractional.

Example:

```text
13 px * 1.5 = 19.5 px
```

This frequently produces:
- blurry borders
- asymmetric antialiasing
- broken corners

---

# Recommended Indicator Sizes

Prefer:

```text
16x16
18x18
20x20
```

Avoid:

```text
13x13
15x15
17x17
```

Recommended implementation:

```cpp
PM_IndicatorWidth  = 16;
PM_IndicatorHeight = 16;
```

---

# 4. Antialiasing + Pen Interaction

For tiny controls, cosmetic pen settings and antialiasing can interact badly.

Recommended:

```cpp
painter->setRenderHint(
    QPainter::Antialiasing,
    true
);

QPen pen(borderColor);

pen.setWidthF(1.0);
pen.setCosmetic(false);

painter->setPen(pen);
```

---

# 5. Indicator Rect Too Tight

If:

```cpp
subElementRect(SE_CheckBoxIndicator)
```

returns a very tight rect,
the border may be clipped.

Recommended:

```cpp
QRect indicator = ...;

indicator.setSize(QSize(16, 16));
```

Then inset before painting:

```cpp
QRectF r(indicator);

r.adjust(
    0.75,
    0.75,
    -0.75,
    -0.75
);
```

---

# Recommended Rendering Template

Suggested implementation pattern:

```cpp
QRectF r = indicatorRect;

r.adjust(
    0.5,
    0.5,
    -0.5,
    -0.5
);

QPen pen(borderColor, 1.0);

pen.setCosmetic(false);

painter->setRenderHint(
    QPainter::Antialiasing,
    true
);

painter->setPen(pen);
painter->setBrush(fillColor);

painter->drawRoundedRect(
    r,
    3.0,
    3.0
);
```

---

# Radio Buttons

The same issue affects radio buttons.

Do NOT draw ellipses directly on the edge rect.

Bad:

```cpp
painter->drawEllipse(rect);
```

Better:

```cpp
QRectF r(rect);

r.adjust(
    0.5,
    0.5,
    -0.5,
    -0.5
);

painter->drawEllipse(r);
```

---

# High DPI Recommendations

Ensure proper High DPI rounding policy testing:

```cpp
QGuiApplication::setHighDpiScaleFactorRoundingPolicy(
    Qt::HighDpiScaleFactorRoundingPolicy::PassThrough
);
```

Also test:
- Round
- Floor
- RoundPreferFloor

---

# Most Likely Immediate Fix

The most likely solution is:

1. inset painted rects
2. avoid odd indicator sizes
3. align borders to pixel centers
4. slightly enlarge indicator metrics

That should fix:
- corrupted checkbox edges
- radio border artifacts
- inconsistent rounded borders
- asymmetric rendering at fractional scaling
