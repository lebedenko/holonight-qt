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

## KH-006 refinement — 2026-09-20

Upstream baseline: `8fe24ff83f8108631c2b7b0351994f7e513acfcd`.
The KH-001 scope and verification above remain historical acceptance evidence.

- Extract the shared content into frameless `HnKeySequenceLabel` in Holonight.Controls.
  Expose keyGroups, literal text, font, color, wrap and read-only accessibleText;
  preserve symbols, translated readable names, separators and wrapping.
- Reuse it in HnKeyHint without changing its public API or monospace defaults.
- Scale vertical padding at line height × 2/22, horizontal padding at × 6/22.
  Center short content in a natural minimum width of 1.2 × single-line badge height.
- Cap appearance-derived rounding at 2 logical pixels; preserve square settings.
  Compute minimum width independently of constrained multiline height.
- Document and demonstrate framed and frameless components in the gallery.
- Verify parity, typography, compact geometry, centering, wrapping and runtime font
  changes, plus the 8/12/18 pt and fractional-scale visual matrix and provider acceptance.

Publication and pinning remain an explicit handoff; KH-007 and KH-008 depend on it.
