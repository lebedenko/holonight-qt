# SPEC: Cross-toolkit semantic appearance export

## Objective

Provide an installed C++ API that projects a validated canonical `ResolvedAppearance` into a stable, versioned set
of semantic values suitable for non-Qt adapter implementations. The projection is a toolkit-neutral description even
though its producer API uses Qt value types as part of `holonight-qt`.

## Requirements

- **SEM-001:** `HolonightQt::Theme` shall install `holonight/theme/semanticappearance.h` and expose contract version
  `1`, scheme and accent identifiers, and dark/light `ColorMode` metadata.
- **SEM-002:** The snapshot shall expose the resolved accent and contrasting foreground; window, view, elevated,
  raised, hover, and disabled surfaces; strong and subtle selection roles; primary, secondary, disabled, and inverse
  text; passive, active, focus, and destructive borders; success, warning, and error roles; UI and monospace
  typography; and icon, fallback-icon, and cursor theme identifiers.
- **SEM-003:** Strong selection shall map to `primary` and `onPrimary`. Subtle selection and its hover state shall map
  to `surfaceSelected` and `surfaceSelectedHover`, with `textPrimary` as their foreground.
- **SEM-004:** The disabled surface shall be the result of source-over compositing the resolved `disabledOverlay`
  onto the resolved normal view `surface`.
- **SEM-005:** `resolveSemanticAppearance(const ResolvedAppearance&)` shall reuse `ThemeResolver`, perform no I/O,
  produce deterministic equal values for equal input, and require no failure channel for validated input.
- **SEM-006:** Every exported color shall be valid for every built-in scheme and supported accent. Strong and subtle
  selections shall remain distinct and retain their specified mappings.
- **SEM-007:** An installed-prefix consumer shall be able to include the public header, request and link
  `HolonightQt::Theme`, and resolve a snapshot.

## Compatibility policy

Contract version 1 may gain optional fields without changing its major version. Removing or renaming a field, or
reinterpreting an existing semantic role, requires a new major contract. Because the producer is supplied as a
static C++ package, compiled consumers rebuild when updating the package; the contract version describes semantic
compatibility and does not promise binary compatibility between independently replaced archives.

## Exclusions

Serialization, generated files, live-reload orchestration, GTK mappings, portals, toolkit-specific fallbacks, and
adapter diagnostics remain downstream responsibilities. Shape, layout scale, metrics, terminal colors, and
application-specific roles are not part of semantic export version 1.
