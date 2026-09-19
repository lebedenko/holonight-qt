# DESIGN: Consistent key hints

## Public contract

Extend `HnKeyHint` in place with `keyGroups` (ordered arrays of `Qt.Key_*` integers),
`wrap` (false by default), and read-only `accessibleText`. Nonempty `keyGroups`
take precedence over literal `text`. Assign a new array when updating groups.
The component is descriptive and never installs shortcuts or takes keyboard focus.

## Rendering and layout

Private QML paths use a common 16 × 12 grid and proportional rounded strokes.
Shift is outlined; Tab/Backtab have stops; Return/Enter share a bent arrow;
Backspace and Delete point in opposite directions; Space uses a spacebar; all four
arrows share proportions. Optical insets preserve the capital region. Text and
symbols share a baseline, derived from the resolved font's text baseline and
`FontMetrics.capitalHeight`. Padding scales with font height and separators use
font advances. The existing raised surface, passive border, appearance-controlled
rounding, and enabled/disabled foregrounds remain shared defaults.

The private layout computes natural width independently of constrained width.
It moves alternatives to the next line before splitting long combinations between
keys. Separators remain with their following keys so punctuation stays unambiguous.
Individual keys are atomic: consumers must allow the longest key plus separator
when constraining width. Literal text uses plain Text wrapping without parsing.
FontMetrics invokable advances explicitly depend on the resolved font, including
text-only combinations and runtime family/point-size/pixel-size changes.

## Names and accessibility

Common keys and joining words use translatable QML strings. A private
`Holonight.impl.KeyHintNames` singleton delegates the remaining Qt key vocabulary
to `QKeySequence::NativeText`, including Unicode, function and media keys. This
uses the existing Qt dependency; no font or icon-theme dependency is added.
The badge exposes a static-text name; all decorative children are ignored.
Consumers can combine `accessibleText` with their translated action labels.

## Files and verification

- `qml/controls/HnKeyHint.qml`: public contract and private renderer/layout.
- `qml/keyhintnames.h`, `qml/CMakeLists.txt`: private Qt key-name fallback.
- `examples/controls-gallery/pages/SurfacesPage.qml`: every symbol, mixed groups,
  alternatives, wrapping, disabled state and runtime family/size controls.
- `tests/test_qml_smoke.cpp`: compatibility, semantic names, punctuation, wrapping,
  font changes and disabled presentation.
- `tests/test_shared_rendering.cpp`: rendered symbols at 8/12/18 pt in two fonts,
  exercised by the existing style, fractional-scale and installed-package matrix.
- `docs/shared-controls-usage-guide.md`: consumer API and sizing guidance.

See `VERIFICATION.md` for acceptance evidence and remaining ecosystem work.
