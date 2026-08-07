# SDD Tasks — appearance-configuration-foundation

Implementation started after umbrella ACF-004 became `Ready`, on repository baseline `767cf3d1640e6957455b0146d28f6ce233ff5f51`
and exact provider revision `holonight-config@81b01d3ae8433f3a4b017db2feb588a1ee62b714`.

- [x] ACF4-01: Add `HoloNight::Config` package discovery and restructure the local configuration/catalog targets.
- [x] ACF4-02: Implement the reusable Qt appearance projection, catalog validation, diagnostics, and read-only loader.
- [x] ACF4-03: Implement canonical file/directory watching with atomic publication and last-known-good rollback.
- [x] ACF4-04: Route QML appearance, theme, and palette projections through one engine-local state and precise change
  signals.
- [x] ACF4-05: Route style and platform-theme loading through the shared Qt reader semantics.
- [x] ACF4-06: Introduce installed `MetricTokens` and exact unit tests for scalar and size-role values.
- [x] ACF4-07: Replace `HnControlMetrics` and palette metric properties with the single `HnMetrics` QML facade.
- [x] ACF4-08: Migrate all QML, C++, demo, gallery, test, documentation, and install/package call sites.
- [x] ACF4-09: Remove INI/JSON schema code, writers, legacy watchers, KDE selection fallback, persisted mode and
  transparency, field environment overrides, duplicated defaults, and obsolete tests.
- [x] ACF4-10: Add startup/reload/rollback/QML/package regression tests and perform full automated verification.
- [ ] ACF4-11: Perform manual Qt/QML dark/light, accent, typography, icon, layout, and shape checks; record results and
  publish the verified commit for umbrella handoff.

## Completion evidence

Automated verification on 2026-08-07:

- Toolchain: CMake 4.4.2, Ninja 1.13.2, Clang 22.1.8, Qt 6.11.1.
- `cmake --build build --parallel`: passed.
- `ctest --test-dir build --output-on-failure`: 20/20 test targets passed, including 247 C++ unit/smoke tests,
  111 QML smoke tests, canonical watcher/rollback regressions, package install, and QML policy checks.
- `task lint`: passed.
- `git diff --check`: passed.

ACF4-11 remains open until the interactive visual matrix is completed and the resulting commit is published. A
local or unpublished commit is not a repository handoff.
