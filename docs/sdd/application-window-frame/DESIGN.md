# DESIGN: HnApplicationWindow

**Spec:** `docs/sdd/application-window-frame/SPEC.md`
**Status:** Implemented
**Date:** 2026-07-23

## Structure

`HnApplicationWindow` derives from `Window`, preserving compositor decorations and normal platform window
semantics. It paints only the client background and never adds another perimeter border. Structural surfaces
inside the application continue to use `HnSurfaceFrame`, but the top-level client rectangle does not pretend
to be a second window decoration.

The optional application header uses three items: leading and trailing slots size to their children, while the
center slot fills the remaining width. A default title label remains visible only while the center slot is
empty. The header is hidden by default and is application navigation space, not a draggable title bar.

## Public API

- `applicationHeaderVisible`, `applicationHeaderHeight`, `applicationHeaderColor`, `contentPadding`
- read-only `effectiveApplicationHeaderHeight` and `effectiveContentPadding`
- read-only `applicationHeader` and `clientContentItem` aliases
- list aliases `applicationHeaderLeadingData`, `applicationHeaderCenterData`, and
  `applicationHeaderTrailingData`
- default `contentData` list alias for application content

Negative layout values are normalized to zero. Background and header colors follow live palette changes.

## Deliberate omissions

Client-side decoration is not part of this component. Dragging, system menus, double-click maximize, resize
edges, and platform buttons remain compositor-owned. A future opt-in frameless component would be a separate
contract requiring explicit cross-compositor behavior and testing.

## Verification

QML smoke tests cover compositor-decoration flags, public defaults, content parenting, slot parenting, title
fallback, non-negative normalization, application-header collapse, and both import spellings.
