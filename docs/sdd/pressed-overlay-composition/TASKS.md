# SDD Tasks — pressed-overlay-composition

- [x] T-001: Reproduce the selected pressed-state color mismatch.
- [x] T-002: Use `HnSegmentedControl` as the pressed-state reference.
- [x] T-003: Resolve `HnChoiceCard` down state to opaque `surfaceElevated`.
- [x] T-004: Apply the same state precedence to `HnCardDelegate`.
- [x] T-005: Apply the same state precedence to `HnSelectableDelegate`.
- [x] T-006: Apply the same state precedence to `Holonight.ItemDelegate`.
- [x] T-007: Add structural state coverage for all four implementations.
- [x] T-008: Add active-scheme offscreen rendering coverage.
- [x] T-009: Perform manual Controls gallery verification.
- [x] T-010: Run focused and repository-wide verification.
- [x] T-011: Close the downstream handoff as consumer-owned.

## Completion evidence

- Pressed fills use the same opaque `surfaceElevated` token as the List/Grid switcher.
- Selection overlays are suppressed while down and restored on release.
- Tests cover choice, card, navigation, and styled item delegates.
- Offscreen rendering verifies an ancestor-independent pressed pixel against the active palette token.
- Focused tests, qmllint targets, `task test`, `task lint`, and `git diff --check` pass.
- The user visually verified the corrected pressed states in the Controls gallery.
- The downstream consumer installation and adoption retry are explicitly user-owned.
