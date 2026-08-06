# DESIGN: Consistent pressed surfaces

**Spec:** `docs/sdd/pressed-overlay-composition/SPEC.md`
**Status:** Implemented
**Date:** 2026-07-28

## Corrected diagnosis

The original replacement bug was real: using translucent `pressedOverlay` as the entire background exposed
the ancestor. The first correction removed that dependency but retained the selected accent layer beneath
the pressed layer. That composition produced another accent-mixed pressed color and did not match the
already-correct List/Grid switcher.

`HnSegmentedControl` establishes the intended precedence:

```qml
if (segment.down)
    return HoloniightPalette.surfaceElevated
```

The affected controls now follow that same rule. Press is a resolved, opaque visual state rather than an
additional accent layer.

## State resolution

Each background keeps a base rectangle and a selected-state rectangle so existing idle selection rendering
is unchanged:

```text
base while down: surfaceElevated
selected layer while down: hidden
```

When released, existing semantics resume:

- cards use `surfaceElevated` as their foundation and apply their selected overlay;
- selectable and styled item delegates retain a transparent unselected idle base;
- hover surfaces, borders, focus rings, and selection indicators retain their existing precedence.

`HnNavigationDelegate` and `HnListDelegate` inherit the correction from `HnSelectableDelegate`.

## Test strategy

Structural tests inspect the base and selected layers across choice, card, navigation, and styled item
delegates. They verify `surfaceElevated` while down and selection restoration after release.

An offscreen test presses a checked `HnChoiceCard`, samples its center over two unrelated opaque ancestor
colors, and compares the result to the active scheme's `surfaceElevated`.

## Files changed

| File | Change |
| --- | --- |
| `qml/controls/HnChoiceCard.qml` | Resolve down to `surfaceElevated`; suppress selection fill while down |
| `qml/controls/HnCardDelegate.qml` | Apply the same pressed precedence |
| `qml/controls/HnSelectableDelegate.qml` | Apply the same precedence to navigation/list delegates |
| `qml/ItemDelegate.qml` | Apply the same precedence to styled item delegates |
| `tests/test_qml_smoke.cpp` | Add structural and rendered regressions |

No palette, generated data, install metadata, or public API changes are required.
