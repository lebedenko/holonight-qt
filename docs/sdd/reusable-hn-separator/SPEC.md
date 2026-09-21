# SPEC: HnSeparator physical geometry and connected boundaries

Status: Accepted redesign; automated verification in progress, native acceptance pending.
Date: 2026-09-21. Supersedes the previous semantic-width/centerOpacity/edgeOpacity contract.

## Public requirements

- REQ-F-001: `HnSeparator` remains in `Holonight.Controls`, a noninteractive Item with
  `Accessible.ignored: true` and no input or focus handlers.
- REQ-F-002: `orientation` defaults to Qt.Horizontal; Qt.Vertical swaps the axes. Invalid
  orientation falls back to horizontal. Caller dimensions, anchors or layouts own major length.
- REQ-F-003: `thickness` is an integer physical-pixel count, default 1. Nonpositive values
  suppress paint and default minor occupancy. Callers never divide thickness by DPR.
- REQ-F-004: `color` defaults to reactive `HoloniightPalette.borderPassive`. Inherited opacity
  defaults to 1 and composes with color alpha, ancestor opacity and the fade. No legacy opacity aliases.
- REQ-F-005: `fadeMode` defaults to Solid. Solid has stop strengths (1,1,1), FadeStart (0,1,1),
  FadeEnd (1,1,0), FadeBoth (0,1,0), at positions 0/.5/1. Invalid modes use Solid. Start/end mean
  local left/right or top/bottom; signed transforms mirror the whole line and its gradient.
- REQ-F-006: `crossAxisAlignment` supports Leading, Center, Trailing, default Leading. It positions
  the fixed-thickness stroke within the available minor slot; explicit dimensions never stretch it.
  Bottom/right boundary owners use Trailing, top/left use Leading. Invalid alignment uses Leading.
- REQ-F-007: Default actual and implicit minor size equals thickness / (effective DPR * absolute
  minor-axis scale). The major implicit size is zero. Caller bindings replace defaults; layouts may
  round their allocated slots independently. Both anchor-only orientations paint by default.

## Geometry requirements

- REQ-G-001: Compute a complete local painted rectangle using scene position, both signed scales,
  slot size, effective render-target DPR, integer thickness and alignment in one calculation.
- REQ-G-002: Snap both major endpoints with floor(physical + .5), including negative coordinates.
  The difference determines length; zero-length quantized strokes do not paint.
- REQ-G-003: Leading/trailing snap the selected local boundary and extend inward by exactly the
  requested pixel count. Center rounds the desired leading edge and preserves the count.
- REQ-G-004: Preserve axis-aligned signed built-in scaling and mirroring; suppress degenerate,
  nonfinite or unsupported non-axis-aligned geometry and recover when valid.
- REQ-G-005: Update event-driven on attachment, reparenting, ancestor x/y/width/height/scale/origin,
  window screen and DPR notifications. Use effectiveDevicePixelRatio, including redirected targets.
  No frame polling or private APIs. Native screen changes remain a manual acceptance item.
- REQ-G-006: Paint a unit rectangle scaled on both axes using Qt's normal three-stop gradient.
  Arbitrary rotation/shear/custom transform-object invalidation are outside this guarantee.
- REQ-G-007: Consumers must not clip away snapped boundaries. A logical fractional clip may round
  differently from painted geometry; boundary-only clips should be removed, with content elision
  or clipping owned separately. An ancestor clip still constrains all Qt Quick primitives.

## Integration and verification

- REQ-I-001: Migrate shared controls/gallery and audit Files, Shell, AI, Package Manager, Viewer.
  Remove legacy opacity properties, caller DPR thickness conversions and diagnostic two-pixel overrides.
- REQ-I-002: Files restores HnHeaderBar's divider. Each connected boundary has one owner; column
  dividers use column-header coordinates and end at the bottom rule's leading boundary.
- REQ-V-001: Render physical thickness 1/2/3, both orientations, all fades, fractional positions,
  alpha and opacity, each alignment, signed transforms and runtime changes at DPR 1/1.25/1.5/1.75/2.
- REQ-V-002: Assert renderer identity; run software and real OpenGL tests. Examine full stroke bounds
  and junction regions, including adjacent coverage, clipping and translucent overlap, not only midpoints.
- REQ-V-003: Exercise anchors/layouts, explicit slots, lifecycle, zero recovery and DPR-change events.
- REQ-V-004: Run focused tests, full provider tests, QML policy/lint, formatting, gallery/demo startup
  and packaging. Validate consumers against an explicitly rebuilt/staged dirty provider.
- REQ-V-005: Record actual Files runtime imports, provider hashes, backend and effective DPR. Run
  wide/narrow Files regressions across the DPR matrix; keep native visual confirmation pending until observed.

## Non-goals

No custom scene-graph renderer unless standard primitives prove insufficient; no compatibility layer,
new dependencies, publication, submodule-pin changes, application behavior, or arbitrary transform support.
Historical investigations remain in RENDERING-AUDIT.md and TASKS.md; they are not acceptance of this redesign.
