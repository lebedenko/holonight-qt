# `HnActionBar` trailing-slot alignment bug

**Status:** Fixed
**Date:** 2026-07-29
**Reported by:** `holonight-shell` shared-controls adoption checkpoint CP-S-011
**Affected revision:** `4003d45f65a7b7103d5b98cbbebda13a776aade4`

## Problem

`HnActionBar` does not right-align `trailingContent` when `centerContent` is unset. Its
`RowLayout` relies on the center loader to consume the remaining width:

```qml
Loader {
    id: centerLoader
    active: root.centerContent !== null
    visible: active
    sourceComponent: root.centerContent
    Layout.fillWidth: true
    Layout.alignment: Qt.AlignHCenter
}
```

When the optional center slot is null, the loader is inactive and invisible. `RowLayout` ignores
that item, so there is no stretch cell between the leading and trailing loaders. The trailing
loader's `Layout.alignment: Qt.AlignRight` only aligns within its implicit-width layout cell; it
does not make that cell consume the remaining row width.

This breaks a common action-bar composition with status/context content at the left and primary
actions at the right. Supplying an empty fake center component activates the spacer, but makes an
optional content slot effectively required and pushes shared geometry responsibility onto every
consumer.

## Reproduction

Create a wide action bar with leading and trailing slots and no center slot:

```qml
import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import Holonight.Controls

HnActionBar {
    width: 1000
    height: 56
    dividerVisible: false

    leadingContent: RowLayout {
        Label { text: "Shell is running" }
        Label { text: "v0.1.0" }
    }

    trailingContent: RowLayout {
        Button { text: "Discard Changes" }
        Button { text: "Apply" }
        Button { text: "Save & Apply" }
    }
}
```

Render the component and compare the trailing item's right edge with the action bar's right edge.
Repeat after adding an empty `centerContent: Item {}`. The empty center component causes the
trailing group to move to the expected right edge, demonstrating that slot activation rather
than available width controls the geometry.

The downstream reproduction is the attempted migration of
`holonight-shell/apps/settings/qml/FooterBar.qml` at CP-S-011.

## Expected behavior

- All three content properties remain independently optional.
- Leading content aligns to the available row's left edge.
- Trailing content aligns to the available row's right edge.
- Center content, when present, is centered in the available action-bar width.
- Leading and trailing alignment does not depend on whether center content exists.
- Slot groups retain their implicit size unless available width requires normal layout
  compression.
- Divider visibility changes vertical composition only; it does not affect horizontal slot
  alignment.

The HoloNight Settings mockup illustrates the intended downstream composition: shell status and
version remain at the footer's left, while Discard Changes, Apply, and Save & Apply form a
right-aligned action group. The mockup is alignment context only; redesigning the Settings window
is outside this fix.

## Actual behavior

- With leading and trailing content and no center content, both groups are packed from the left.
- The trailing group begins immediately after the leading group and the layout's normal spacing.
- `Layout.alignment: Qt.AlignRight` on `trailingLoader` has no visible effect because the loader's
  layout cell owns only its implicit width.
- Adding an otherwise meaningless empty center component activates `Layout.fillWidth` and restores
  the expected separation.

## Evidence

- Intended Settings alignment reference:
  `holonight-docs/10-settings/settings-window-mockup.png`.
- Rejected downstream checkpoint capture:
  `/tmp/holonight-shared-controls-review.eVnLYi/cp-s-011-post-current-s1.0-footer.png`.
- Downstream baseline:
  `/tmp/holonight-shared-controls-review.eVnLYi/cp-s-010-post-current-s1.0-footer.png`.
- Temporary pixel diff:
  `/tmp/holonight-shared-controls-review.eVnLYi/cp-s-011-diff.png`.
- Affected source: `qml/controls/HnActionBar.qml`.
- Durable downstream finding:
  `holonight-shell/docs/sdd/shared-controls-adoption/UPSTREAM-NOTES.md`, UP-004.

The attempted migration passed downstream QML instantiation, focused action-routing tests,
`task qml-lint`, and `task qmltypes-check`. The failure is rendered geometry rather than type
resolution or service behavior. The temporary screenshots are diagnostic evidence and are not
intended to be committed.

## Suggested correction

Keep a layout-managed stretch region between the leading and trailing slots regardless of whether
center content is present. The implementation may retain the current loaders, but horizontal
placement must not rely on an inactive optional loader participating in `RowLayout`.

Possible approaches include:

- an always-active stretch item that owns the remaining width, with center content centered
  independently inside it; or
- a three-region layout whose left and right regions remain edge-aligned while the center loader
  is optional.

Do not require consumers to provide empty slot components, compensating margins, fixed widths, or
external spacer items. Avoid changing the public slot API merely to correct internal geometry.

Centering semantics need explicit treatment when leading and trailing groups have different
widths. `centerContent` should remain centered relative to the complete action bar, not merely
centered in the residual gap between asymmetric side groups, unless the documented contract is
intentionally changed.

## Resolution

`HnActionBar` now positions the leading and trailing loaders against opposite edges of an
always-present content region. The optional center loader is anchored to that region's midpoint,
so asymmetric side content cannot shift it. When the available width is smaller than the slots'
combined implicit width, the loaders receive constrained widths while retaining their edge and
center alignment.

Focused QML smoke coverage exercises all slot combinations, asymmetric true centering, runtime
slot changes, resizing, divider visibility, and the loaded-item aliases. The Controls gallery
also includes the original no-center status/version and multi-button action composition.

## Regression coverage

Add focused rendered or geometry-level QML coverage for these slot combinations:

1. leading only;
2. center only;
3. trailing only;
4. leading and trailing, with center unset;
5. leading and center;
6. center and trailing;
7. all three slots;
8. no slots.

For applicable cases, verify:

- leading global x equals the content row's left edge;
- trailing global right edge equals the content row's right edge;
- center midpoint equals the action bar's horizontal midpoint within tolerance;
- asymmetric leading/trailing implicit widths do not shift true center content;
- resizing wider and narrower preserves edge alignment and valid size negotiation;
- toggling a slot component between null and non-null recomputes geometry correctly;
- `dividerVisible` true and false preserve horizontal geometry;
- compact labels, long translated labels, disabled buttons, and dark/light themes do not alter the
  alignment contract;
- `leadingItem`, `centerItem`, and `trailingItem` aliases still expose the loaded objects;
- accessibility continues to report the root as a toolbar.

Include or extend a Controls gallery example with status/version content at the left and a
multi-button action group at the right so the no-center composition is visually exercised.

## Acceptance criteria

- Trailing-only content is right-aligned.
- Leading and trailing content occupy opposite edges when center content is null.
- Center content remains truly centered when present with asymmetric side groups.
- Consumers do not need empty components or spacer workarounds.
- All slot combinations respond correctly to runtime component changes and parent resizing.
- Existing public properties, loaded-item aliases, divider behavior, implicit sizing, and
  accessibility role remain compatible.
- Focused geometry regressions and the Controls gallery pass.
- `task build`, `task lint`, and `task test` pass in `holonight-qt`.

## Downstream retry

After the fix is committed and installed:

1. record the fixing `holonight-qt` commit;
2. rebuild and install it to `/tmp/holonight-qt-prefix`;
3. return to `holonight-shell` checkpoint CP-S-011;
4. change UP-004 to `Fixed upstream; awaiting downstream retry`;
5. retry only the `FooterBar.qml` composition migration to `HnActionBar`;
6. preserve the existing shell-status/version content, themed footer buttons, dirty/saving
   enablement, load/save routing, and save-error dialog behavior;
7. run the focused Settings footer test, `task qml-lint`, `task qmltypes-check`, live Settings
   instantiation, and `git diff --check`;
8. capture a fresh post-change screenshot and repeat the CP-S-011 visual-review gate.
