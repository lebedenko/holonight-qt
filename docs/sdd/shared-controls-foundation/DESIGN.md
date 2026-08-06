# DESIGN: Shared controls foundation

**Spec:** `docs/sdd/shared-controls-foundation/SPEC.md`
**Status:** Implemented
**Date:** 2026-07-27

## Boundaries

`Holonight.Core` owns dependency-light design-system primitives. In this cycle it contains singleton semantic
size values and metric lookups only. `Holonight.Controls` owns rich reusable controls and explicitly depends on
both Core and the existing `Holonight` style module. Deriving from `Holonight.TextField` guarantees the HoloNight
visual contract even when the host selects another process-wide QQC2 style.

No existing type is moved. Physical ownership changes for palette, appearance, shapes, icons, surfaces, and
application windows are deferred to a compatibility-specific SDD.

## Sizing

Both size and metrics are stateless singleton APIs. Metrics normalize input before lookup, making an invalid
integer indistinguishable from `Normal`. `HnSearchField` uses the resolved height, padding, icon size, and spacing
while inheriting its font and all editing behavior.

## Search-field composition

The field retains the styled text field as its root. Overlay items at the leading and trailing edges own optional
slot instances through loaders; their measured widths feed the inherited text padding. The packaged search icon
uses the existing `HnIcon` semantic recoloring path. The trailing row independently hosts custom content and the
clear action, so neither replaces the other.

Escape handling runs before the inherited editor: non-empty input is cleared and accepted, while empty input is
left unaccepted. Pointer activation of clear explicitly restores focus to the editor.

## Packaging and examples

Both QML backing/plugin targets join the existing `HolonightQtTargets` export with `Core` and `Controls` export
names. Package component validation maps these to `HolonightQt::Core` and `HolonightQt::Controls`. Generated QML
metadata, source files, assets, and plugins are installed under canonical `Holonight/Core` and
`Holonight/Controls` import paths.

The gallery is opt-in through `BUILD_CONTROLS_GALLERY`, separate from the existing theme demo.
