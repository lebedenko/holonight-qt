# DESIGN: Legacy Radius Token Cleanup

Existing controls use lightweight `Rectangle` backgrounds, so this cycle introduces a narrow adapter rather than
changing their renderer. `HnAppearance.roundedRadius(role, width, height, revision)` delegates to `ShapeResolver` with
an explicit rounded style. The role selects the token, dimensions preserve clamping, and the revision dependency makes
live configuration changes observable to QML bindings.

This separates ownership cleanly: `ShapeResolver` owns geometry values, `HnAppearance` owns live configuration, and
each control owns only its semantic role. The old palette radius fields remain ABI/API compatibility data but are no
longer theme implementation inputs.

Chamfer and hybrid rendering will be introduced by migrating suitable backgrounds to `HnSurfaceFrame`; pretending a
`Rectangle` supports those shapes would make configuration behavior misleading.
