# Shared Selection and Composition Controls — Specification

## Purpose

`Holonight.Controls` provides reusable selection, status, header, choice, and
action-bar patterns without taking ownership of application navigation or domain
behavior.

## Requirements

- `surfaceSelected`, `surfaceSelectedHover`, and `selectionIndicator` are
  available from every built-in `ColorTokens` scheme and `HoloniightPalette`.
  They are theme-authored semantic values; consumers do not derive accent alpha.
- `Holonight.ItemDelegate` treats `highlighted`, `checked`, and
  `ListView.isCurrentItem` as equivalent selection inputs. Press has visual
  precedence, selected hover follows, selected follows, hover follows, and the
  normal surface is transparent. Keyboard focus adds `borderFocus` without
  replacing the state surface. Selected text remains `textPrimary`.
- Rich delegates expose owned component slots and read-only loaded-item handles.
  Inactive slots do not instantiate content.
- Interactive controls are keyboard reachable, preserve the native
  `AbstractButton` activation contract, expose meaningful accessible names and
  descriptions, and visually distinguish disabled state.
- `HnSegmentedControl` supports array and object models, configurable text/value
  roles, synchronized `currentIndex`/`currentValue`, arrow navigation, and
  `activated(index, value)`.
- `HnChoiceCard` remains a checkable delegate and composes with the standard
  `ButtonGroup`.
- Existing controls remain source-compatible. `HnSettingsRow` adds
  `dividerVisible` and `trailingContent`; `HnSectionHeader` adds `compact`.
- Public controls remain in `Holonight.Controls` version 1.0. The common
  selectable-surface implementation is internal.

## Ownership boundary

The shared module owns visual state, slots, semantic sizing, keyboard activation,
and accessibility defaults. Applications own domain commands, routing,
master/detail policy, three-pane shells, AI message layouts, network state, HUD
decoration, and responsive policy.

## Compatibility

No existing property is removed or renamed. The only intentional visual change
is the quieter standard `ItemDelegate` selection treatment.
