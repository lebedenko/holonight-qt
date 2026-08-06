# SPEC: Reusable HnSeparator

**Feature:** Add a reusable, device-pixel-aligned separator to `Holonight.Controls`.

**Component:** `qml/controls/HnSeparator.qml`

**Status:** Implemented

**Date:** 2026-07-29

## Context

HoloNight applications currently implement separators as local `Rectangle` items with a height or
width of `HoloniightPalette.separatorWidth`. At fractional display scale factors, a one-logical-pixel
rectangle can cover different physical-pixel rows depending on its scene position. Adjacent
separators can therefore appear to have different thicknesses despite using the same QML dimensions
and palette token.

`holonight-shell/qml/HoloNight/Components/ContentSeparator.qml` already provides reusable horizontal
and vertical separators with four useful gradient profiles. Its implementation is application-local,
requires callers to supply a fixed length, defaults to one logical pixel, and does not align its
painted geometry to the device-pixel grid. `HnSeparator` shall retain the reusable orientation and
gradient behavior while adopting layout-friendly sizing, semantic palette defaults, and
fractional-DPR-safe rendering.

## Goals

- Provide one canonical separator for HoloNight QML applications and shared controls.
- Render separators with consistent physical thickness at integer and fractional display scaling.
- Support both ordinary solid dividers and the directional fades proven by `ContentSeparator`.
- Work naturally with anchors and Qt Quick Layouts without requiring a caller-supplied length.
- Keep the component visual-only, lightweight, and independent of application-domain behavior.

## Functional requirements

### REQ-F-001: Public component and module

The system shall provide a public `HnSeparator` QML type from the `Holonight.Controls` module.

**Acceptance criteria:**

- `import Holonight.Controls` makes `HnSeparator` available.
- The source and install trees both expose the type through the existing QML module packaging.
- The component follows the repository's SPDX and `pragma ComponentBehavior: Bound` conventions.

### REQ-F-002: Orientation

The component shall support horizontal and vertical separators through an `orientation` property
using the standard `Qt.Horizontal` and `Qt.Vertical` values. The default shall be `Qt.Horizontal`.

**Acceptance criteria:**

- A horizontal separator paints along its width and uses its height as the minor-axis thickness.
- A vertical separator paints along its height and uses its width as the minor-axis thickness.
- Changing `orientation` at runtime updates implicit sizing and gradient direction.
- Invalid orientation values normalize to `Qt.Horizontal` or produce a deterministic warning and
  safe horizontal rendering.

### REQ-F-003: Layout-driven length

The component shall not require a numeric length or assign its explicit `width` and `height` from a
required `size` property. The parent, anchors, or a Qt Quick Layout shall determine the major-axis
length.

**Acceptance criteria:**

- `Layout.fillWidth: true` produces a horizontal separator spanning the available width.
- `Layout.fillHeight: true` produces a vertical separator spanning the available height.
- Explicit `width` and `height` bindings remain under caller control.
- The component supplies only the minor-axis implicit size needed for layout participation.

### REQ-F-004: Semantic color

The component shall expose a `color` property whose default is
`HoloniightPalette.borderSubtle`. Callers may select another semantic palette color, such as
`HoloniightPalette.borderPassive`, when a stronger boundary is required.

**Acceptance criteria:**

- Default construction uses `HoloniightPalette.borderSubtle`.
- Theme changes update the default color dynamically.
- The implementation contains no hardcoded RGB, hexadecimal, or named visual colors.

### REQ-F-005: Thickness

The component shall expose a positive `thickness` property representing the requested separator
thickness in units of the semantic `HoloniightPalette.separatorWidth` metric. Its default shall be
`HoloniightPalette.separatorWidth`.

**Acceptance criteria:**

- Default construction produces the theme's standard separator thickness.
- A caller can request a larger thickness without replacing the component.
- Zero or negative values produce no painted separator and no negative implicit dimension.
- The public requested thickness remains distinct from the internal device-pixel-aligned painted
  thickness.

### REQ-F-006: Device-pixel-aligned rendering

The component shall account for the effective device pixel ratio of the window containing it. Its
painted minor-axis thickness and scene position shall align to physical-pixel boundaries.

**Acceptance criteria:**

- A default separator occupies exactly one physical pixel when
  `HoloniightPalette.separatorWidth` is `1`.
- At DPR values `1.0`, `1.25`, `1.5`, `1.75`, and `2.0`, two separators at different logical
  positions render with the same physical thickness.
- The painted line does not alternate between one and two physical rows or columns as its parent
  position changes.
- Horizontal and vertical separators satisfy the same alignment guarantee.
- Moving the component between screens with different DPR values updates its geometry without
  requiring recreation.
- Alignment is based on the effective window/screen DPR and the separator's scene position; using a
  constant logical height alone is insufficient.

### REQ-F-007: Fade modes

The component shall support four major-axis opacity profiles derived from
`ContentSeparator.qml`:

- `Solid`: equal emphasis at both ends and the center.
- `FadeBoth`: low opacity at both ends and full separator opacity at the center.
- `FadeStart`: low opacity at the start and full separator opacity at the end.
- `FadeEnd`: full separator opacity at the start and low opacity at the end.

The default shall be `Solid`, matching ordinary divider behavior.

**Acceptance criteria:**

- Horizontal gradients progress from left to right.
- Vertical gradients progress from top to bottom.
- Layout mirroring does not silently reverse the documented geometric start/end meaning.
- Changing the fade mode at runtime updates the painted separator.
- Public enum names describe their visual behavior; the ambiguous legacy names `Center`, `Start`,
  `End`, and `None` are not required for compatibility because `ContentSeparator` is not a public
  `Holonight.Controls` type.

### REQ-F-008: Opacity controls

The component shall expose `centerOpacity` and `edgeOpacity` properties for gradient profiles.
`centerOpacity` shall default to `1.0`; `edgeOpacity` shall default to `0.0`.

**Acceptance criteria:**

- `Solid` uses `centerOpacity` across the full separator.
- Fade modes use `centerOpacity` for their emphasized stops and `edgeOpacity` for faded stops.
- Effective opacity values are clamped to the inclusive range `[0.0, 1.0]`.
- Opacity is composed with the supplied semantic color without changing its RGB channels.
- The component-level `opacity` property continues to compose normally with stop opacity.

### REQ-F-009: Visual-only accessibility behavior

The separator shall be ignored by accessibility tooling by default.

**Acceptance criteria:**

- `Accessible.ignored` is `true`.
- The component does not accept focus, pointer input, keyboard input, or expose an interactive
  accessibility role.

### REQ-F-010: Visibility and zero-length behavior

The component shall handle hidden, zero-length, and not-yet-window-associated states without
warnings or invalid geometry.

**Acceptance criteria:**

- A separator with zero major-axis length paints nothing.
- A separator that has not yet been associated with a `Window` uses safe geometry and recomputes
  alignment after association.
- Hiding and showing the separator does not retain stale DPR or scene-position calculations.

## Integration requirements

### REQ-I-001: Canonical internal reuse

New shared-control implementations that need a divider shall use `HnSeparator` instead of declaring
an equivalent raw `Rectangle`.

**Acceptance criteria:**

- At least one existing shared control is migrated as an integration proof.
- The migrated control preserves its public sizing and `dividerVisible` behavior.
- Consumers can still override separator strength where their existing visual hierarchy requires
  `borderPassive` rather than the default `borderSubtle`.

### REQ-I-002: Focused initial migration

The initial implementation shall not require a repository-wide replacement of all divider
rectangles. Migration may proceed incrementally after the component contract is verified.

**Acceptance criteria:**

- The implementation change remains reviewable and separates component creation from broad visual
  migration.
- Existing controls not migrated in the initial change preserve their current behavior.

### REQ-I-003: Application adoption

The component shall be suitable for replacing the header dividers in `holonight-ai` without
application-specific properties or logic.

**Acceptance criteria:**

- Both `QuickPanelHeader.qml` and `ChatHeader.qml` can express their current full-width,
  `borderPassive` separators with `HnSeparator`.
- No application code needs to perform DPR calculations.

## Non-functional requirements

### REQ-NF-001: QML-only implementation

The component should be implemented in QML unless testing demonstrates that public Qt QML APIs
cannot reliably obtain the effective DPR and align scene geometry. Any C++ helper requires a
separate design justification and shall not expose application-specific behavior.

### REQ-NF-002: Public Qt API

The implementation shall use supported Qt 6 public APIs and shall not depend on private scene-graph
or platform APIs.

### REQ-NF-003: Binding stability

DPR and pixel-alignment bindings shall not create binding loops, repeated warnings, or continuous
polish/layout cycles when ancestors move or resize.

### REQ-NF-004: Rendering cost

Each separator shall use constant-size visual structure. It shall not allocate canvases, images, or
per-frame JavaScript objects and shall not animate or schedule frames while static.

### REQ-NF-005: API clarity

Public properties shall use standard Qt terminology where available (`orientation`, `Qt.Horizontal`,
and `Qt.Vertical`). Compatibility aliases for the shell-local `direction`, `type`, and required
`size` API are out of scope.

## Verification requirements

### REQ-V-001: QML smoke tests

Automated QML tests shall cover:

- public type availability from the source and install-tree modules;
- default property values;
- horizontal and vertical implicit sizing;
- runtime orientation changes;
- all four fade profiles;
- opacity clamping;
- zero and negative thickness handling;
- zero-length behavior; and
- accessibility exclusion.

### REQ-V-002: DPR geometry tests

Automated rendering or geometry tests shall exercise DPR values `1.0`, `1.25`, `1.5`, `1.75`, and
`2.0`, including separators placed at multiple logical minor-axis offsets.

**Acceptance criteria:**

- Captured or inspected output confirms equal physical thickness at every tested position.
- Tests cover both orientations.
- Tests fail for the current raw one-logical-pixel `Rectangle` behavior at a fractional DPR and pass
  for `HnSeparator`, demonstrating that the test detects the original regression.

If the offscreen platform cannot provide deterministic fractional-DPR rendering, the repository
shall include a focused geometry test for the alignment calculation plus a documented manual visual
check using a real fractional-scale display.

### REQ-V-003: Existing verification

`task build`, `task test`, and the `holonight_controls_qml_qmllint` target shall pass after
implementation and packaging changes.

### REQ-V-004: Visual gallery

The controls gallery should demonstrate:

- horizontal and vertical separators;
- solid, fade-both, fade-start, and fade-end profiles;
- default subtle and stronger passive colors; and
- default and increased thickness.

## Constraints and non-goals

- Do not add new palette tokens solely for this component.
- Do not copy `ContentSeparator` into `Holonight.Controls` unchanged.
- Do not require callers to know or bind a display scale factor.
- Do not make the separator responsible for surrounding margins, layout spacing, labels, or
  section-header behavior.
- Do not introduce application-specific imports or dependencies.
- Do not promise cross-window drawing; the separator renders only within its containing Qt Quick
  window.
- Migrating every existing raw divider in `holonight-qt`, `holonight-shell`, and `holonight-ai` is
  outside the initial implementation scope.

## Implementation review notes

The implementation design should explicitly resolve these details before coding:

1. How the effective DPR is obtained from the separator's actual window and updated after screen
   changes.
2. How the painted minor-axis coordinate is snapped using the separator's scene position, rather
   than only rounding its local height or width.
3. How layout occupancy is preserved while an internal painted line uses a potentially smaller
   physical-pixel-aligned thickness.
4. How gradient stop alpha composes with colors that already contain alpha.
5. Whether the chosen Qt Quick primitive renders hard-edged one-physical-pixel geometry
   deterministically with antialiasing disabled or avoided.
