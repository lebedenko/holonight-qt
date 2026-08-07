# SDD Tasks — appearance-configuration-foundation

All tasks remain unchecked until umbrella ACF-004 is `Ready`. Implement on the exact provider revision pinned by the
accepted initiative.

- [ ] ACF4-01: Add `HoloNight::Config` package discovery and restructure the local configuration/catalog targets.
- [ ] ACF4-02: Implement the reusable Qt appearance projection, catalog validation, diagnostics, and read-only loader.
- [ ] ACF4-03: Implement canonical file/directory watching with atomic publication and last-known-good rollback.
- [ ] ACF4-04: Route QML appearance, theme, and palette projections through one engine-local state and precise change
  signals.
- [ ] ACF4-05: Route style and platform-theme loading through the shared Qt reader semantics.
- [ ] ACF4-06: Introduce installed `MetricTokens` and exact unit tests for scalar and size-role values.
- [ ] ACF4-07: Replace `HnControlMetrics` and palette metric properties with the single `HnMetrics` QML facade.
- [ ] ACF4-08: Migrate all QML, C++, demo, gallery, test, documentation, and install/package call sites.
- [ ] ACF4-09: Remove INI/JSON schema code, writers, legacy watchers, KDE selection fallback, persisted mode and
  transparency, field environment overrides, duplicated defaults, and obsolete tests.
- [ ] ACF4-10: Add startup/reload/rollback/QML/package regression tests and perform full automated verification.
- [ ] ACF4-11: Perform manual Qt/QML dark/light, accent, typography, icon, layout, and shape checks; record results and
  publish the verified commit for umbrella handoff.

## Completion evidence

Record exact commands, toolchain versions, test results, manual observations, and the published commit here before
requesting ACF-004 `Done`. A local or unpublished commit is not a repository handoff.
