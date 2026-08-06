# DESIGN: Shared input controls

**Spec:** `docs/sdd/shared-input-controls/SPEC.md`

## Module design

All three QML files live in the existing `Holonight.Controls` target and canonical install directory. They compose
the styled `Holonight` controls and import semantic sizing from `Holonight.Core`; this preserves the current module
boundary and avoids new runtime dependencies.

## Icon combo box

`HnIconComboBox` subclasses the styled combo box so selection, editable behavior, popup navigation, roles, signals,
and replacement of `delegate` or `contentItem` stay owned by Qt Quick Controls. A small role lookup handles indexed
JavaScript objects and models exposing `get()`, while the popup delegate also reads its adaptor-model role object.
The default selected and popup presentations use `HnIcon`. Their visibility drives padding, so an absent or failed
source collapses both the image and its spacing.

## Text area

`HnTextArea` is a `FocusScope` with one shared semantic surface. Its internal styled text area has a transparent
background and remains the sole text editor, preserving Qt's input method, selection, undo, and keyboard behavior.
The scroll view height clamps editor content height between normalized line-derived bounds. Vertical overflow
enables its scrollbar; horizontal policy follows wrap mode. A conditional loader owns footer content below the
editor inside the same surface.

The wrapper deliberately forwards only stable editor properties and methods. It does not expose the internal
editor object.

## Form field

`HnFormField` uses a `Loader` for its single component and a compact column for label, control, and supporting text.
Conditional visibility collapses unused label and support rows. Error display is selected from explicit
`hasError`; the loader never discovers or mutates a child `hasError` property.

## Risks and mitigations

- Model shapes differ: use both public indexed lookup and the delegate adaptor role object, and cover each supported
  shape in contracts.
- Text metrics vary by font: derive bounds from the active font/content rather than fixed pixels.
- Wrapper focus can obscure editor behavior: forward focus directly to the loaded control/editor and test it.
- Custom presentation could regress: retain inherited replaceable `delegate` and `contentItem` properties.
