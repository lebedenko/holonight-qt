# DESIGN: Cross-toolkit semantic appearance export

## Ownership and data flow

`holonight-qt` owns canonical palette meanings and scheme/accent resolution, so the producer lives in the existing
`HolonightQt::Theme` target beside `ThemeResolver`:

```text
validated ResolvedAppearance -> ThemeResolver -> SemanticAppearance value
```

The function is a pure in-memory projection. Consumers decide whether and how to serialize, publish, monitor, or
translate the result into toolkit settings.

## Public representation

`SemanticAppearance` is an aggregate of Qt value types with defaulted equality. Its metadata preserves the canonical
scheme ID, accent ID, and `ColorMode`; it does not infer identifiers back from colors. Typography uses point sizes to
match the validated appearance contract. Only UI and monospace typography are included because title and display
families are HoloNight application roles rather than general desktop toolkit defaults.

The public `kSemanticAppearanceContractVersion` constant and each default-constructed snapshot use version `1`.
Field additions are optional v1 extensions. Breaking semantic changes require a new major contract and an explicit
migration rather than silently changing the meaning of an existing field.

## Role mapping

| Export role | Canonical source |
|---|---|
| Accent / foreground | `primary` / `onPrimary` |
| Window / view | `background` / `surface` |
| Elevated / raised / hover | `surfaceElevated` / `surfaceRaised` / `surfaceHover` |
| Strong selection / foreground | `primary` / `onPrimary` |
| Subtle selection / hover / foreground | `surfaceSelected` / `surfaceSelectedHover` / `textPrimary` |
| Primary / secondary / disabled / inverse text | Corresponding canonical text roles |
| Passive / active / focus / destructive border | `borderPassive` / `borderActive` / `borderFocus` / `borderUrgent` |
| Success / warning / error / error foreground | `success` / `warning` / `error` / `onError` |

The disabled surface uses integer 8-bit source-over compositing of `disabledOverlay` over the opaque view surface,
with nearest-integer rounding. Publishing the composited opaque color avoids requiring adapters to reproduce Qt
layering behavior.

## Failure behavior and downstream expectations

`ResolvedAppearance` is already validated by the canonical appearance layer. The resolver therefore cannot fail and
does not repeat scheme/accent validation. Downstream code must not use this function as a parser or validation
boundary. Adapters may omit unsupported roles only when they document the native fallback, and must own diagnostics
for their own serialization and toolkit application failures.
