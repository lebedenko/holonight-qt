# SPEC: Consistent pressed surfaces

**Status:** Implemented
**Date:** 2026-07-28
**Source:** `docs/hn-choice-card-pressed-overlay-composition-bug.md`

## Goal

Choice cards, card delegates, selectable delegates, and the styled item delegate shall use the same pressed
surface semantics as `HnSegmentedControl`. Pressed feedback must be an opaque palette surface, must not expose
an ancestor color, and must not retain a selected accent tint while the control is held.

## Requirements

- While `down`, each affected control shall render `HoloniightPalette.surfaceElevated` as its fill.
- Selected overlays shall be hidden while `down` and restored on release.
- Existing idle, hover, selected, border, focus, and accent-edge behavior shall remain unchanged.
- Disabled controls shall not remain down.
- No palette values or public QML APIs shall change.
- `HnChoiceCard` shall retain its current geometry and `ButtonGroup` behavior.
- `HnNavigationDelegate` and `HnListDelegate` shall receive the behavior through `HnSelectableDelegate`.

## Regression coverage

Focused QML smoke tests shall cover the four affected background implementations. Tests shall verify the
opaque pressed fill for selected and unselected controls, suppression and restoration of the selected layer,
disabled behavior, real pointer press/release behavior, and ancestor independence.

An offscreen pixel test shall compare the pressed center pixel to the active scheme's `surfaceElevated`, within
a small render tolerance, matching `HnSegmentedControl`. Existing palette tests cover the token across schemes.

## Acceptance criteria

- All controls shown in the Controls gallery use the same coherent pressed fill as the List/Grid switcher.
- No selected control produces an accent-mixed or ancestor-dependent pressed color.
- Existing APIs, geometry, interaction, and selection chrome are preserved.
- Focused QML tests, qmllint, repository tests, and `git diff --check` pass.

## Out of scope

Changing palette tokens, redesigning non-pressed selection colors, downstream installation, and downstream
`holonight-shell` migration are outside this change.
