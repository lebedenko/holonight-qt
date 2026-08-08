# TASKS: Cross-toolkit semantic appearance export

- [x] **SEM-001:** Define producer ownership, semantic mappings, version policy, failure behavior, and adapter
  boundaries.
- [x] **SEM-002:** Add and install the versioned `SemanticAppearance` value API in `HolonightQt::Theme`.
- [x] **SEM-003:** Implement deterministic projection through `ThemeResolver`, including disabled-surface source-over
  compositing.
- [x] **SEM-004:** Cover exact mappings, metadata, typography and theme identifiers, deterministic equality, and all
  built-in scheme/accent combinations with unit tests.
- [x] **SEM-005:** Extend the installed-prefix consumer to request `Theme`, include the public header, and resolve a
  snapshot.
- [x] **SEM-006:** Complete local build, full CTest, format, clang-tidy, package/install, and QML policy verification.
- [x] **SEM-007:** Publish the focused implementation commit and record green remote CI for umbrella CTV-003 handoff.

## Local verification

Completed on 2026-08-08:

- `cmake --build build -j$(nproc)`: passed.
- `QT_QPA_PLATFORM=offscreen ctest --test-dir build --output-on-failure`: 20/20 targets passed.
- `cmake --build build --target format-check`: passed.
- `cmake --build build --target tidy`: passed with the repository's existing non-fatal diagnostics.
- `git diff --check`: passed.

The full CTest run includes the installed-prefix consumer, QML import policy, QML source policy, theme resolver unit
suite, and QML smoke suite.

## Published handoff

- Semantic appearance implementation: `e69d81e` (`feat(theme): export semantic appearance snapshot`).
- Package/CI follow-ups: `dde2e9d`, `277a95f`.
- Published verified revision: `holonight-qt@277a95f`.
- GitHub Actions CI run `31280216547`: passed on 2026-08-09; build and full CTest completed successfully.
