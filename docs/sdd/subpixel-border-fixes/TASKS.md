# SDD Tasks — subpixel-border-fixes

- [x] T-001: Update checkbox indicator size to use pixelMetric
  - REQs: REQ-F1, REQ-C4
  - Check: `drawCheckBoxImpl()` computes indicator width via `style()->pixelMetric(PM_IndicatorWidth)` and contains no literal `14`.

- [x] T-002: Convert checkbox border rect to QRectF with 0.5px inset
  - REQs: REQ-F2, REQ-C3
  - Check: The unchecked-border `drawRoundedRect()` call in `drawCheckBoxImpl()` receives a `QRectF` operand inset by `adjusted(0.5, 0.5, -0.5, -0.5)` from an integer `QRect`.

- [x] T-003: Replace checkbox border pen with non-cosmetic QPen(color, 1.0)
  - REQs: REQ-F3, REQ-C3
  - Check: The unchecked-border pen in `drawCheckBoxImpl()` is constructed via `QPen(pal.color(QPalette::Mid), 1.0)` with explicit width, not a default cosmetic pen.

- [x] T-004: Verify checkbox antialiasing is set before border draw
  - REQs: REQ-F4
  - Check: `painter->setRenderHint(QPainter::Antialiasing, true)` appears in `drawCheckBoxImpl()` before the unchecked-border `drawRoundedRect()` call.

- [x] T-005: Update checkbox checkmark line offsets for 16px indicator
  - REQs: REQ-F9
  - Check: The two checkmark lines in `drawCheckBoxImpl()` use offsets `(+4, +8)` to `(+7, +11)` and `(+7, +11)` to `(+12, +5)` relative to `indicator.left()` and `indicator.top()`.

- [x] T-006: Update radio button indicator size to use pixelMetric
  - REQs: REQ-F5, REQ-C4
  - Check: `drawRadioButtonImpl()` computes indicator width via `style()->pixelMetric(PM_ExclusiveIndicatorWidth)` and contains no literal hardcoded dimension.

- [x] T-007: Convert radio button border rect to QRectF with 0.5px inset
  - REQs: REQ-F6, REQ-C3
  - Check: The border `drawEllipse()` call in `drawRadioButtonImpl()` receives a `QRectF` operand inset by `adjusted(0.5, 0.5, -0.5, -0.5)` from an integer `QRect`.

- [x] T-008: Replace radio button border pen with non-cosmetic QPen(color, 1.0)
  - REQs: REQ-F7, REQ-C3
  - Check: The border pen in `drawRadioButtonImpl()` is constructed via `QPen(pal.color(QPalette::Mid), 1.0)` with explicit width, not a default cosmetic pen.

- [x] T-009: Verify radio button antialiasing is set before border draw
  - REQs: REQ-F8
  - Check: `painter->setRenderHint(QPainter::Antialiasing, true)` appears in `drawRadioButtonImpl()` before the `drawEllipse()` call for the border.

- [x] T-010: Update radio button inner dot rect to use QRectF with 4.5px inset
  - REQs: REQ-F10
  - Check: The checked-state inner dot in `drawRadioButtonImpl()` is drawn on a `QRectF` with `adjusted(4.5, 4.5, -4.5, -4.5)` inset.

- [x] T-011: Verify code adheres to style rules and compiles
  - REQs: REQ-NF1, REQ-NF2, REQ-C1, REQ-C2
  - Check: Running `cmake --build build -j$(nproc)` produces zero compiler errors/warnings in `style/holonightstyle.cpp`.

- [x] T-012: Pass clang-tidy with zero warnings
  - REQs: REQ-NF1
  - Check: Running `cmake --build build --target tidy` reports zero tidy warnings on the modified code.

- [x] T-013: Pass all unit tests
  - REQs: REQ-NF3, REQ-F9, REQ-F10
  - Check: Running `QT_QPA_PLATFORM=offscreen ctest --test-dir build --output-on-failure` exits with status 0 and all tests pass.

- [x] T-014: Confirm no palette token changes
  - REQs: REQ-F11
  - Check: `git diff palette/holonight/palette.h` is empty (no token additions or modifications).
