# TASKS: Shared-controls compatibility migration

- [x] Record ownership, compatibility, singleton-identity, packaging, and non-goal contracts.
- [x] Document the acyclic Core → style → Controls design and lowercase compatibility strategy.
- [x] Move palette, theme, appearance, shape, enum, and icon-provider registration to `Holonight.Core`.
- [x] Make `HnIcon` Core-owned and retain a thin lowercase compatibility subtype.
- [x] Export the maintained `HnSurfaceFrame` and `HnApplicationWindow` implementations from `Holonight.Controls`.
- [x] Re-export Core through `Holonight` and preserve its frame/window and styled-control surface.
- [x] Update internal imports, plugin linkage, build ordering, metadata generation, and install lists.
- [x] Add canonical Core contracts and canonical/legacy singleton identity tests.
- [x] Add canonical Controls frame/window creation, defaults, slots, and content-ownership tests.
- [x] Extend install-tree validation for canonical plugins, metadata, QML files, and compatibility imports.
- [x] Run all three QML lint targets and focused migration smoke tests.
- [x] Run package-install validation and the complete headless CTest suite.
- [x] Launch the controls gallery at scales 1.0 and 1.25 and record completion evidence.

## Completion evidence

- `holonight_core_qml_qmllint`, `holonight_qml_qmllint`, and `holonight_controls_qml_qmllint` completed; the
  legacy `ComboBox.qml` `Window` attached-property warnings remain unchanged.
- Focused migration and compatibility smoke tests passed.
- Installed Core/Controls plugin, metadata, QML artifact, CMake target, canonical import, legacy import, and
  lowercase import validation passed.
- Complete offscreen CTest passed: 16/16 tests.
- The offscreen controls gallery remained running until the three-second smoke timeout at scale factors 1.0 and
  1.25, with no startup errors.

Downstream adoption in `holonight-shell`, `holonight-ai`, and `holonight-pkg-manager` is complete. The follow-up
pipeline removes the temporary compatibility surface: `docs/sdd/shared-controls-compatibility-removal/`.
