# Shared Selection and Composition Controls — Design

## Palette

Each scheme derives the two quiet selection fills from its authored `primary`
token at 14% and 20% opacity. `selectionIndicator` aliases the opaque scheme
primary. Derivation happens during palette construction, so QML consumers only
select semantic roles.

## Selection composition

Internal `HnSelectableDelegate` subclasses the Qt Quick Templates item delegate.
Its `selected` property combines `highlighted`, `checked`, and the attached
`ListView.isCurrentItem`. Its background resolves state in this order:

1. pressed overlay;
2. selected-hover surface;
3. selected surface;
4. hover surface;
5. transparent.

Keyboard focus independently adds `borderFocus`. Selection styles are fill,
accent edge, and outline. Public rich delegates inherit this behavior and replace
only `contentItem`.

## Slots and inheritance

Slots are `Component` properties loaded with `Loader.active`. Public read-only
item aliases make ownership testable without exposing loader machinery.
Navigation delegates use an accent edge, list delegates use fill, and card
delegates use an elevated outlined frame. Action delegates constrain the list
pattern to icon/title/description/chevron.

## Supporting controls

Status colors map to existing semantic palette roles. Key hints are static,
accessible shortcut labels. Panel headers and action bars use layouts with
optional component slots. Segments are native buttons in a row; choice cards are
native check delegates so standard focus, activation, and `ButtonGroup`
exclusivity remain available.

## Integration

All public files are compiled, installed, and type-described by the canonical
`Holonight.Controls` 1.0 QML module. `HnSelectableDelegate` is marked as an
internal QML type but is installed because public compiled components depend on
it. Package validation checks the installed artifacts and constructs every
public type.
