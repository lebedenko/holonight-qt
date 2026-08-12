# Scaled ComboBox Popup Geometry Bug

**Status:** Ready for implementation

**Date:** 2026-08-12

**Shared-control baseline:** `holonight-qt@faba3dae39d4e07722fd21497191edec81b9b14e`

**Greeter reference supplied by HGR-104:** `holonight-greeter@39a38da387e57077e7ebbd1aeb1edec81b9b14e`, plus the current HGR-104 working changes

**Validated environment:** Qt 6.11.1; installed `HolonightQt 0.1.0`

> **Baseline note:** The supplied greeter reference is not present in the inspected checkout. The checkout used to
> confirm this plan was `39a38da387e57077e7ebbd1aeb1edb81e2952e7f`, with uncommitted HGR-104 changes. Before
> implementation, the greeter owner must resolve which exact baseline is authoritative; do not silently substitute
> one revision for the other.

## Summary

The footer ComboBox popup in the greeter does not match the rendered bounds or visual scale of its collapsed control
when `LoginPanel` is uniformly scaled. Popup typography, row height, padding, offsets, and corner radii are also
rendered at the wrong scale. The local workaround introduced to correct the apparent width replaced the shared
control's established popup and delegate behavior and caused further rendering and interaction regressions.

The correction belongs first in `holonight-qt`. The shared `Holonight.ComboBox` must compensate for a uniformly
scaled ancestor while retaining its existing delegate model, list view, placement, scrolling, keyboard navigation,
and edge-radius behavior. The greeter must then remove its popup reimplementation and consume the corrected shared
control explicitly.

## Confirmed root cause

`LoginPanel` is scaled to `1.25` in the failing configuration. A Qt Quick Controls popup whose popup type is
`Popup.Item` reparents its visual content into the window overlay. The overlay is not below the scaled panel in the
item tree, so it does not inherit the panel's transform. Equal logical widths therefore do not imply equal rendered
scene widths: the collapsed selector is rendered at the panel scale while the popup remains at overlay scale.

The same transform discontinuity affects every popup-space value, including font size, delegate height, padding,
attachment offset, and radius. Adjusting only `popup.width` cannot produce a geometrically consistent result.

This behavior and the required popup transform compensation are described by Qt's
[Popup scaling guidance](https://doc.qt.io/qt-6/qml-qtquick-controls-popup.html#showing-a-popup-as-an-item).

The in-progress greeter workaround replaces the shared popup with a local `T.Popup`, `Column`, and `Repeater`, then
manually sets `currentIndex`, emits `activated()`, and closes the popup. That bypasses the tested
`root.delegateModel` and `ListView` path in `Holonight.ComboBox`, including scrolling, current-item containment,
above/below placement, keyboard navigation, `maximumVisibleItems`, and first/last delegate edge radii. The later
missing-row, hover/click, placement, and keyboard inconsistencies are consequences of this replacement rather than
separate defects.

## Scope and ownership

The work has two ordered parts:

1. `holonight-qt` owns scale-aware popup geometry and the reusable ComboBox contract. Implement and publish this
   part first as `HolonightQt 0.1.1`.
2. `holonight-greeter` owns footer composition and visual customization. Integrate only after the corrected module is
   rebuilt and installed, and require version `0.1.1`.

Deterministic process-wide HoloNight style selection is a related dependency concern, but broader application style
activation is outside this defect. The extracted selector must derive explicitly from `Holonight.ComboBox`, so its
behavior is independent of `QT_QUICK_CONTROLS_STYLE`.

Automatic compensation is limited to uniform scale and translation. Rotation, shear, and non-uniform scaling are
unsupported. This limitation must be documented in the public control contract and covered by a focused guard or
diagnostic test as appropriate; the implementation must not claim correct geometry for unsupported transforms.

## Shared-control implementation

Extend `Holonight/ComboBox.qml` without replacing its current popup or delegate architecture.

### Effective scale and popup transform

- Derive the effective uniform scene scale from mapped control coordinates rather than from a particular ancestor's
  `scale` property. This must account for nested uniform scales and translation.
- Expose the resolved value as a read-only property so tests and downstream diagnostics can inspect it.
- Apply the effective scale to the `Popup.Item` visual content in the overlay.
- Choose the transform origin from the attachment edge: the popup's bottom edge when it opens above and its top edge
  when it opens below. Scaling must not create a gap or move the attached edge away from the control.
- Keep the popup's rendered horizontal bounds equal to the control's mapped scene bounds. Raw logical `width`
  equality is not the acceptance measurement.

### Placement and height

- Map the control bounds into scene/window coordinates and calculate space above and below in that coordinate system.
- Select above/below placement using rendered popup demand and scene-space availability.
- Convert the selected available scene height back into logical popup units before constraining
  `implicitHeight` and visible rows.
- Preserve window margins and the control-edge offset at the effective scale.
- Keep overflow clipping and scrolling functional when either available space or `maximumVisibleItems` limits the
  list.

### Existing interaction architecture

Retain all of the following:

- `ListView.model: root.delegateModel`;
- `ListView.currentIndex: root.highlightedIndex`;
- `positionViewAtIndex(..., ListView.Contain)` on index changes and popup opening;
- clipping, interactivity, and the vertical scrollbar for overflow;
- `maximumVisibleItems` and its current minimum-of-one normalization;
- Qt ComboBox's standard mouse and keyboard activation path.

Do not use a `Repeater`, manually assign `currentIndex`, manually emit `activated()`, or implement a second selection
path.

### Delegate geometry and radii

- Add a configurable `delegateHeight` property with a default of `28` logical pixels.
- Bind the standard delegate height to `root.delegateHeight`.
- Continue deriving the outer popup radius from the actual popup background.
- Continue deriving the inner delegate radius as `max(0, outer radius - popup padding)`.
- Apply outer rounded corners only to the first row's top corners and the last row's bottom corners. Middle rows must
  remain square at shared edges.
- Ensure font, delegate height, padding, radii, and borders all receive the same effective rendered scale.

### Versioning

Bump the package version from `HolonightQt 0.1.0` to `0.1.1`. The corrected behavior is a downstream dependency, not
an application-local workaround.

## Greeter integration

After `HolonightQt 0.1.1` is available to the greeter build and runtime:

- Extract the inline footer component from `LoginPanel.qml` into `FooterSelector.qml`.
- Derive it explicitly from `Holonight.ComboBox`.
- Remove the local `T.Popup`, `Column`, `Repeater`, fixed popup-height calculation, manual `currentIndex`, manual
  `activated()`, popup closing, and shared-radius workarounds.
- Keep both footer cells at 66 px and preserve their equal-width layout.
- Inset the actual selector item by 5 px on all sides of its cell. Its item geometry and painted surface geometry must
  then be identical, allowing the inherited popup width to follow the selector naturally.
- Customize only the collapsed icon, text, chevron, hover/focus background, font, and `delegateHeight: height`.
- Preserve all existing session and keyboard models, roles, selection synchronization, activation handlers, enabled
  conditions, and key-navigation bindings.
- Update CMake to require `HolonightQt 0.1.1`.

The inspected `/usr/lib` QML module is not package-owned and uses embedded resources. Editing the sibling
`holonight-qt` source tree does not update runtime behavior. Rebuild and install the module, then verify both the
CMake package version and the QML artifacts actually loaded by the greeter before judging the integration.

## Verification plan

### `holonight-qt`

Add focused ComboBox coverage using translated scaled parents at `0.78`, `1.0`, and `1.25`.

For each scale:

- compare the mapped scene-space left and right bounds of the collapsed control and popup;
- verify the exposed effective scale;
- verify rendered font size, delegate height, padding, popup radius, inner delegate radius, and attachment offset;
- verify correct above/below placement and containment within the window;
- verify every delegate is created and visible after repeated open/close cycles;
- traverse every row with pointer movement and verify the last row can be hovered and clicked;
- verify Up, Down, and Enter selection;
- verify mouse activation produces the same `currentIndex`, `currentValue`, and `activated()` behavior as keyboard
  activation;
- verify reopening positions the list at the selected item;
- verify overflow scrolling and `maximumVisibleItems` with a long model.

Preserve and rerun the five existing HoloNight ComboBox popup tests:

- popup instantiation/opening;
- first/middle/last delegate corner radii;
- model and `maximumVisibleItems` height bounds;
- window-space constraint and overflow interaction;
- semantic delegate height through `HnIconComboBox`.

Run the repository build, QML compilation, `qmllint`, focused tests, the full CTest suite, formatting checks, and
`git diff --check`.

### `holonight-greeter`

Add a QML integration test around `FooterSelector.qml` using four role-based session entries. Exercise translated
parents at scales `0.78` and `1.25`, and verify:

- selector and popup mapped scene widths match;
- all four rows render immediately and remain visible and pointer-interactive;
- the last row can be hovered and selected;
- keyboard and mouse activation update the same selection state;
- closing and reopening retains and reveals the selected item;
- the session and keyboard footer cells remain equal width and 66 px high;
- the selector surface is inset by exactly 5 px on every side;
- the explicit HoloNight type is used even when `QT_QUICK_CONTROLS_STYLE` names another style.

Run the greeter build, QML compilation, `qmllint`, focused integration test, full CTest suite, formatting checks, and
`git diff --check`. Confirm the rebuilt `HolonightQt 0.1.1` installation is the module resolved by both build tooling
and the running demo.

### Manual verification

Run the real greeter demo at compact, normal, and 1.25-scale sizes. For both session and keyboard selectors, inspect
every dropdown row and confirm width, typography, row geometry, padding, radii, control-edge attachment, hover,
click, keyboard selection, scrolling, and window-edge placement.

## Acceptance criteria

- Popup and collapsed selector have identical rendered horizontal bounds at every supported panel scale.
- Font, row height, padding, offset, border, and radius scale consistently with the collapsed selector.
- All rows render immediately and remain pointer-interactive, including the final row.
- Mouse and keyboard selection use the standard ComboBox path and produce identical state and signals.
- Popup placement remains inside the window and scrolling works for long models.
- First, last, and middle row radii remain concentric with the popup border and correct at shared edges.
- No local popup reimplementation remains in the greeter.
- Session and keyboard cells remain equal width; their existing bindings and navigation behavior are preserved.
- `HolonightQt 0.1.1` is installed, resolved at runtime, and required explicitly by the greeter.
- Uniform scale and translation are supported; unsupported rotation and non-uniform transforms are documented.

## Change boundary for this planning task

This planning task adds only this document. It does not implement, version, build, install, or modify either
repository's product code. Implementation must be performed as ordered, reviewable changes in each owning repository.
