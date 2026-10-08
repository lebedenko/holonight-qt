# Decoration-independent Viewer

Work package: I-002. Exact upstream baseline: `8eadfa36396315da56d734a83b2145b6d71d51cc` (origin/main).

## Requirements and design

Remove decoration helpers and dependencies. Follow the [umbrella contract](../../../../docs/initiatives/decoration-independent-viewer/README.md). No decoration settings or desktop heuristics. Preserve unrelated behavior.

## Implementation and verification

Deleted both public QML helpers, their private probes and dedicated tests/registration. Core no longer links private WaylandClient or SystemServices Compositor; the independent Wayland module and other private Qt dependencies remain. Removed the tooling dependency and CI bootstrap. Current usage documentation no longer instructs decoration detection; historical SDDs are archived.

The package acceptance fixture checks the optional Wayland consumer only when BUILD_WAYLAND is enabled. Fractional-scale input regression tests now retain QPointF global coordinates rather than rounding a stationary event; test lint was corrected without changing production input behavior.

## Verification — 2026-10-08

- Clean Debug configurations in build/decoration-independent (BUILD_WAYLAND=ON, demo/gallery enabled) and build/decoration-independent-off (OFF), explicit Config provider build/deps/prefix, Qt 6.12.0.
- Enabled suite covered 95 checks; disabled suite covered 86. Initially two input-interaction scale-1.25 checks failed in each new build. After the QPointF fixture correction, all four input checks passed in each build. Other passing suites were not unnecessarily repeated.
- Package install consumers passed in both configurations, including Core consumption with Wayland discovery disabled. Enabled Wayland and demo/gallery startup checks passed.
- Core/Controls/style/implementation QML lint, QML source/import policy, metadata and removed-type checks passed. Existing unrelated QML warnings remain in unchanged files. Full clang-format and REUSE passed; focused changed-test clang-tidy passed with GCC-only compile flags filtered.
- Build logs reviewed: expected private-Qt version notices and existing protocol-policy/deprecation notices; no compiler diagnostics in changed production code. Current Core metadata and source/build configuration contain none of the removed APIs.
- Logs: /tmp/decoration-qt{,-off}-tests.log, /tmp/decoration-qt{,-off}-input-final2.log, /tmp/decoration-qt-checks.log and /tmp/decoration-qt-input-tidy-final2.log.

Provider stage: /tmp/holonight-decoration-independent/qt. Publication and pinning remain separate; no hosted CI success is claimed.
