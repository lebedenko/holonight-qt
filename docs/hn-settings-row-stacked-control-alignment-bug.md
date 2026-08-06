# `HnSettingsRow` stacked control alignment bug

**Status:** Fixed
**Date:** 2026-07-28
**Reported by:** `holonight-shell` shared-controls adoption checkpoint CP-S-006
**Affected revision:** `6ffdae297caa665cb9a26fb611837035174eeb31`

## Problem

`HnSettingsRow.stacked` does not give the loaded control the full content width below the title.
The control loader is placed in the second column of a two-column `GridLayout`:

```qml
columns: root.stacked ? 2 : 4

Loader {
    id: controlLoader
    Layout.column: root.stacked ? 1 : 2
    Layout.row: root.stacked ? 1 : 0
    Layout.fillWidth: root.stacked
}
```

In stacked mode, column 0 retains the width of the current row's title column. The loaded control
therefore starts after that title-dependent width rather than at the shared row origin. Two rows
with different title lengths place otherwise identical controls at different x coordinates.

This contradicts the Controls gallery description that a stacked control receives the full
content width below its title. Compound standard controls also need a defined common vertical
alignment; the downstream switch and slider appeared offset within the same loaded row.

## Reproduction

Create two full-width stacked settings rows with different title lengths and identical compound
controls:

```qml
import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import Holonight.Controls

ColumnLayout {
    width: 960

    Repeater {
        model: ["Override base radius", "Override base chamfer"]

        HnSettingsRow {
            required property string modelData

            titleText: modelData
            stacked: true
            Layout.fillWidth: true

            control: Component {
                RowLayout {
                    Switch {}
                    Slider { Layout.fillWidth: true }
                    Label {
                        text: "0 px"
                        Layout.preferredWidth: 52
                    }
                }
            }
        }
    }
}
```

Compare the global x coordinates of each loaded `Switch` and `Slider`. Also compare the vertical
centers of the switch and slider within each row.

The downstream reproduction is in the attempted CP-S-006 migration of shape scale and the
base-radius/base-chamfer override rows in
`holonight-shell/apps/settings/qml/AppearancePage.qml`.

## Expected behavior

- In stacked mode, the title occupies the first row.
- The loaded control occupies the next row and spans the complete available content width.
- Its x origin is independent of title or description width.
- Identical control compositions align horizontally across sibling settings rows.
- Standard controls in a compound loaded layout share a consistent vertical center.
- The row retains responsive sizing; consumers do not provide fixed widths or compensating
  margins.

## Actual behavior

- `controlLoader` remains in column 1 without spanning both stacked columns.
- Column 0 width is derived from the current row's title.
- Loaded-control x positions differ between rows with different title lengths.
- Switches and sliders in the compound control appear vertically offset.
- The nominally fill-width slider fills only the remaining title-dependent grid cell.

## Evidence

- User review capture:
  `/tmp/codex-clipboard-EMHKNp.png`.
- Downstream checkpoint capture:
  `/tmp/holonight-shared-controls-review.eVnLYi/cp-s-006-post-current-s1.0-default.png`.
- Affected source: `qml/controls/HnSettingsRow.qml`.
- Durable downstream finding:
  `holonight-shell/docs/sdd/shared-controls-adoption/UPSTREAM-NOTES.md`, UP-003.

The temporary screenshots are diagnostic evidence and are not intended to be committed.

## Suggested correction

In stacked mode, place `controlLoader` on the row below the title/description and span the full
grid width. The control geometry must not depend on the title's implicit width.

Possible implementation details should be selected according to the existing layout structure,
but the resulting contract should be equivalent to:

- title/description content spanning the stacked row where necessary;
- control loader beginning at column 0;
- control loader spanning all stacked columns;
- full-width size negotiation for the loaded control;
- explicit vertical centering for compound control content.

Do not solve this by imposing a fixed title width or fixed control width. Those approaches would
make the control fragile at different window sizes, translations, fonts, and scale factors.

## Focus compatibility

`HnSettingsRow` currently forwards row focus to the root item loaded in `controlLoader`. This is
useful for a single control, but a compound root such as `RowLayout` is not itself an actionable
control. The geometry fix must preserve existing focus forwarding for single-control consumers.

Add coverage or document the supported composition for compound controls. If compound controls
remain supported, the row should not introduce a dead Tab stop before the focusable children.
Avoid breaking the natural Tab order of `Switch`, `Slider`, and other standard controls.

## Regression coverage

Add a focused rendered or geometry-level QML test with:

1. two full-width stacked rows;
2. different title lengths;
3. identical compound switch/slider/value controls;
4. a long translated title;
5. compact and normal semantic size roles;
6. enabled and disabled controls.

Verify:

- both loaded controls have the same global x origin;
- both loaded controls receive the same available width;
- the trailing value edges align;
- switch and slider vertical centers align within tolerance;
- resizing the parent preserves alignment;
- title wrapping does not shift the control origin;
- non-stacked geometry is unchanged;
- single-control focus forwarding is unchanged;
- compound controls retain usable keyboard Tab order.

Include a Controls gallery example or extend the existing stacked example so the intended
full-width geometry is visually obvious.

## Acceptance criteria

- Stacked controls begin at a title-independent shared x origin.
- Stacked controls span the complete available row width.
- Identical controls align horizontally across rows with different title lengths.
- Switches and sliders in a compound control align vertically.
- Geometry remains responsive under long text, dark/light themes, and scale factors 1.0 and 1.25.
- Existing public properties, slot aliases, size roles, implicit heights, and non-stacked layout
  remain compatible.
- Single-control focus forwarding remains correct.
- Compound controls do not gain a dead keyboard focus stop.
- Focused regression tests and the Controls gallery pass.
- `task build`, `task lint`, and `task test` pass in `holonight-qt`.

## Downstream retry

After the fix is committed and installed:

1. record the fixing `holonight-qt` commit;
2. rebuild and install it to `/tmp/holonight-qt-prefix`;
3. return to `holonight-shell` checkpoint CP-S-006;
4. change UP-003 to `Fixed upstream; awaiting downstream retry`;
5. retry only the three shape scale/override rows with `HnSettingsRow`;
6. run the focused Settings behavior/geometry test, `task qml-lint`, `task qmltypes-check`, live
   Settings instantiation, and `git diff --check`;
7. capture a fresh post-change screenshot and repeat the CP-S-006 visual-review gate.

## Resolution

`HnSettingsRow` now places stacked controls at column 0 with a two-column span,
so their geometry is independent of title width and optional leading content.
Stacked trailing content also spans the complete third row. The row forwards
keyboard focus only when the loaded root is itself tab-focusable; compound
layout roots therefore do not add a dead Tab stop, while their controls remain
reachable in natural order.

Automated smoke coverage verifies responsive stacked geometry, compact and
normal size roles, enabled and disabled rows, leading content, inline
compatibility, and single/compound keyboard focus behavior. The Controls gallery
contains matching compact and disabled normal compound examples for visual
inspection.
