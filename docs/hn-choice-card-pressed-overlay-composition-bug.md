# `HnChoiceCard` pressed-overlay composition bug

**Status:** Fixed and verified
**Date:** 2026-07-28
**Reported by:** `holonight-shell` shared-controls adoption checkpoint CP-S-004
**Affected revision:** `dbbccd045063b8063512e946da8593f7f048dd8c`

## Problem

`HnChoiceCard` uses the translucent `pressedOverlay` token as its complete background whenever
the control is down:

```qml
color: root.down ? HoloniightPalette.pressedOverlay
                 : (root.checked ? HoloniightPalette.surfaceSelected
                                 : (root.hovered ? HoloniightPalette.surfaceHover
                                                 : HoloniightPalette.surfaceElevated))
```

The palette documents `pressedOverlay` as a state layer over the current surface. Assigning it
as the complete background removes the card's opaque normal or selected surface and composites
the translucent token directly over the ancestor background.

This makes pressed feedback depend on the surface behind the card. It is especially visible in
editors such as HoloNight Settings, where draft theme/accent values can differ from the currently
applied global `HoloniightPalette`. During adoption for the accent-choice row, pressing Cyan
produced a muddy green fill instead of coherent pressed feedback.

## Reproduction

Create a minimal control with an opaque parent:

```qml
import QtQuick
import QtQuick.Controls.Basic
import Holonight.Core
import Holonight.Controls

Rectangle {
    width: 320
    height: 120
    color: HoloniightPalette.background

    ButtonGroup {
        id: choices
    }

    HnChoiceCard {
        anchors.centerIn: parent
        width: 220
        title: "Cyan"
        checked: true
        ButtonGroup.group: choices
    }
}
```

Press and hold the card. Repeat with:

- `checked: false`;
- `checked: true`;
- a different opaque parent color;
- at least one dark and one light theme.

The same defect is visible in the downstream Settings reproduction:

1. Migrate the five accent choices in
   `holonight-shell/apps/settings/qml/AppearancePage.qml` to `HnChoiceCard`.
2. Select a draft theme/accent combination that differs from the currently applied palette.
3. Press and hold an accent choice.
4. Observe that the translucent pressed token replaces the card surface and blends with the
   Settings page.

## Expected behavior

- A pressed card uses the same opaque `surfaceElevated` fill as `HnSegmentedControl`.
- Selection tint is suppressed while pressed and restored on release.
- The pressed result does not change merely because an unrelated opaque color is placed behind
  the card.
- The control continues to own its interaction-state colors; consumers do not calculate or
  override pressed colors.

## Actual behavior

- `root.down` replaces the complete background with a translucent color.
- The normal `surfaceElevated`, hover `surfaceHover`, or checked `surfaceSelected` base is lost.
- The ancestor surface participates directly in the pressed result.
- In the downstream capture, a pixel within the muddy pressed fill was
  `sRGB(82,101,33)`.

## Evidence

- User capture: `/tmp/scr.png` in the reporting `holonight-shell` session.
- Downstream checkpoint capture:
  `/tmp/holonight-shared-controls-review.eVnLYi/cp-s-004-post-current-s1.0-default.png`.
- Affected source: `qml/controls/HnChoiceCard.qml`.
- Token contract: `docs/theme-colors-usage.md` describes `pressedOverlay` as a pressed layer over
  the current surface.

The temporary screenshots are diagnostic evidence and are not intended to be committed.

## Final correction

Resolve `down` to the opaque `surfaceElevated` token, matching the already-correct
`HnSegmentedControl`, and suppress the selected surface layer until release. Do not change the
global `pressedOverlay` token because other consumers rely on its documented alpha semantics.

Audit the same pattern in related shared controls, including `HnCardDelegate`,
`HnSelectableDelegate`, and the styled `ItemDelegate`, because they also select
`pressedOverlay` as a complete background color.

## Regression coverage

Add focused QML or C++-driven QML tests that inspect/render `HnChoiceCard` in these states:

1. unchecked and idle;
2. unchecked and pressed;
3. checked and idle;
4. checked and pressed;
5. hovered and pressed;
6. disabled.

For pressed states, verify:

- the rendered fill equals `HoloniightPalette.surfaceElevated`;
- changing the ancestor background does not affect the card fill;
- the selected layer is suppressed until release;
- pointer, keyboard, focus, accessibility, and `ButtonGroup` behavior remain unchanged.

If implementation details are intentionally not exposed, use deterministic offscreen rendering
and pixel sampling at the card center with animations disabled.

Add equivalent regression coverage for any related controls changed by the audit.

## Acceptance criteria

- `HnChoiceCard` resolves its pressed fill to opaque `surfaceElevated`.
- Pressing a checked or unchecked card no longer exposes the ancestor background.
- Dark and light theme captures show coherent pressed feedback.
- The public `HnChoiceCard` API and geometry remain unchanged.
- Existing Controls gallery behavior remains correct.
- New pressed-state regression tests pass.
- `task build`, `task lint`, and `task test` pass in `holonight-qt`.

## Downstream retry

Downstream adoption is owned by the consumer and is not part of this repository's completed pipeline.

After the fix is committed, the consumer may:

1. record the fixing `holonight-qt` commit;
2. rebuild and install it to `/tmp/holonight-qt-prefix`;
3. return to `holonight-shell` checkpoint CP-S-004;
4. change UP-002 to `Fixed upstream; awaiting downstream retry`;
5. retry the accent-choice migration to `HnChoiceCard`;
6. run the focused Settings interaction test, `task qml-lint`, `task qmltypes-check`, live
   Settings instantiation, and `git diff --check`;
7. capture a new post-change screenshot and repeat the CP-S-004 visual-review gate.
