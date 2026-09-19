# SPEC: Consistent key hints

Approved scope: user-supplied implementation plan, 2026-09-20.
Work package KH-001; upstream baseline `8095d49eff12f6dc7742ae1590b860debb90ed20`.

## Requirements

- When semantic key groups are supplied, hints shall use the shared renderer and preserve shortcut activation.
- When displaying symbols, the provider shall scale and align vector ink with resolved-font capital metrics.
- When space is constrained, shared wrapping shall prefer alternatives, then keys.
- When assistive technology reads hints, it shall receive translated semantic names without decorative duplicates.
- When disabled, hints shall retain the shared disabled palette.
- Literal text shall remain supported without parsing.

## Owned scope

Shared semantic renderer, wrapping, typography, accessibility and gallery.

## Symbol and API details

`keyGroups: [[Qt.Key_Control, Qt.Key_Return], [Qt.Key_Tab]]` displays two alternatives
inside one badge. An empty `keyGroups` array uses the literal `text` compatibility API; display strings
are never parsed. `wrap: true` enables group-first, then key-boundary wrapping.
`accessibleText` is a read-only translated semantic label for parent composition.
Shift, Tab, Backtab, Enter/Return, Backspace, Space, Delete and directional arrows use
private vector paths. Other keys remain textual. The palette, border, monospace defaults
and appearance-controlled rounding are retained. No shortcut bindings change.

## Acceptance

Focused regressions, repository acceptance and QML checks; provider gallery startup and install tests; visual review at 8/12/18 pt, multiple fonts and fractional scales. Manual ecosystem checks remain required. Publication and umbrella pins need explicit authorization.
