# SPEC: Shared input controls

## Goal

Complete the second shared-controls cycle in `Holonight.Controls` with reusable icon selection, bounded multiline
editing, and structural form layout. Existing modules, imports, targets, and public APIs remain compatible.

## Requirements

### `HnIconComboBox`

- Derives from the styled `Holonight.ComboBox`.
- Exposes `sizeRole` (Normal by default, with invalid values normalized) and `iconRole` (`"icon"` by default).
- Retains the inherited model, role, editable, selection, keyboard, signal, delegate, and content-item contracts.
- Its defaults display URL-compatible icon role data for JavaScript object arrays, `ListModel`, and item models.
- Empty or failed icons reserve no space. Control, icon, spacing, and delegate dimensions use semantic metrics.

### `HnTextArea`

- Is a focus-scope wrapper around the styled editor and an internal scroll view.
- Exposes `sizeRole`, `minimumVisibleLines` (3), `maximumVisibleLines` (8), `hasError`, and optional
  `footerContent`.
- Normalizes bounds to at least one line and keeps the maximum no lower than the minimum.
- Grows between its normalized line bounds and scrolls after the upper bound. Wrapping disables horizontal
  scrolling; no-wrap content may scroll horizontally.
- Exposes curated text, placeholder, read-only, wrap, length, input method, cursor, selection, font, and
  accessibility properties, plus common editing operations.
- Keeps editor behavior, focus, IME, selection, disabled/read-only/error visuals, and optional footer composition.
- Does not provide submit behavior, validation rules, chat actions, or a character counter.

### `HnFormField`

- Exposes label, helper, error, required, and error-state properties.
- Owns one default `Component` control and exposes its loaded item read-only.
- Shows error support text in the error state and helper text otherwise, without empty reserved rows.
- Shows a translated required marker as presentation only.
- Leaves validation propagation explicit at the call site.

## Compatibility and acceptance

The module remains version 1.0 and the existing `HolonightQt::Controls` component remains intact. No lowercase
alias, package component, C++ type, third-party dependency, or downstream migration is introduced. Acceptance
requires focused QML contracts, install-tree import coverage, a representative gallery, and the existing headless
test suite.
