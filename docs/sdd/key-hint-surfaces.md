# Transparent key hints on host surfaces

## Intent

Footer hints keep their neutral border and glyph colors while showing the host
surface through the badge. Hints within buttons or selected rows show that same
surface and use the enclosing border color for both badge outline and content.

## Interfaces and decisions

- HnKeyHint fill is always transparent. Its public borderColor defaults to
  HoloniightPalette.borderPassive; foregroundColor defaults to enabled
  textSecondary / disabled textDisabled. The sequence renderer uses that foreground
  for literal text, separators and vector glyphs. Explicit overrides remain
  authoritative when disabled, matching HnKeySequenceLabel's color contract.
- HnSelectableDelegate exposes read-only outlineColor, inherited by HnListDelegate.
  The existing outline renderer and consumer hints share this resolved color;
  keyboard focus uses borderFocus and otherwise selectionIndicator.
- HnLabeledKeyHint inherits transparent fill and exposes customization through its
  existing hintItem. Geometry, fonts, wrapping, accessibility and shortcuts remain
  unchanged. Frameless menu sequence labels are unchanged.

## Consumers and delivery

Shell binds its selected launcher-row hint to outlineColor and authentication
button hints to their actual background border color. Shell footers, Viewer and
Files inherit transparency without local fill overrides. See the
[umbrella contract](../../../docs/initiatives/key-hint-surfaces.md) and
[Shell design](../../../holonight-shell/docs/sdd/key-hint-surfaces.md).
Verify against an installed development provider; publish this provider before
consumer commits reference its revision and update umbrella pins afterwards.

## Acceptance

Provider smoke checks cover default disabled colors, transparent fill, live color
bindings and explicit overrides for text, separators and vector glyphs. Rendering
checks verify the host surface is visible inside the badge. Gallery examples show
neutral footer, selected-row and accented-button hints. Run QML lint/policy,
demo/gallery startup and package-install checks, then focused Shell launcher and
authentication checks under Holonight and Fusion. Compare the launcher,
authentication and Viewer footer visuals with the supplied screenshots.
