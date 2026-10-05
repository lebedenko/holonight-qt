# Decoration-aware Viewer toolbar titles

Status: In Progress

Work package: I-001

Upstream baseline: `43cae7b9e2b04d10882e2319caf25b85af8cacc2`

## Requirements and design

`HnWindowDecoration` observes a guarded QQuickWindow without changing flags, requesting decoration, creating protocols or supplying controls. Qt 6.11 native Wayland xdg toplevels only: unsupported or unconfigured shells report Unknown; explicit frameless/fullscreen reports Undecorated; an active Qt decoration reports ToolkitClientSide; a configured normal toplevel reports Undecorated when Qt wants unavailable decoration, otherwise ServerSide. Suspended shells conservatively report Unknown.

The adapter uses existing shell `surfaceRole`, `isExposed`, `wantsDecorations`, and window `decoration`/state. Qt private dependencies are implementation-only. BUILD_WAYLAND=OFF keeps the QML API with Unknown. Refresh at GUI dispatcher awake/aboutToBlock and after surface, exposure, visibility and state events, with no timer. Surface destruction immediately clears stale state. Notify only on changes. HnApplicationWindow defaults remain unchanged.

## Files and tasks

- qml/hnwindowdecoration*: public QML helper, private boundary and Wayland adapter.
- qml/CMakeLists.txt: registration and conditional private dependency.
- tests/test_window_decoration.cpp and tests/CMakeLists.txt: boundary transitions, notifications, replacement/destruction and surface recreation.
- docs/window-decoration.md: supported API and limits.

## Verification

Automated verification on 2026-10-05 with Qt 6.11.2 and GCC 16.2.1:

- Focused `holonight_window_decoration_tests`: 3 tests pass, covering transitions, changed-only notifications, replacement/destruction, dispatcher boundaries, surface recreation and actual QML registration.
- `cmake --build build/test -j 2`, followed by `ctest --test-dir build/test --output-on-failure -j 2`: all 98 CTests pass, including `holonight_package_install_test` and example startup checks.
- `task qml-lint`, `task format-check`, `task qml-import-check`, `task qmltypes-check`: pass.
- Direct clang-tidy on both new implementation sources and the new test with repository configs: pass.
- `BUILD_WAYLAND=OFF` in `build/decoration-no-wayland`: full provider build passes; focused CTest passes, including QML Unknown fallback.
- Additional OFF package-install attempt: fails because the existing fixture unconditionally requires the intentionally disabled `HolonightQt::Wayland` component. The normal Wayland-enabled package-install test passes. No fixture checks were weakened.
- `task check`: stops during tidy on 12 existing errors in unchanged `tests/isolated_style_probe.cpp` (complexity, braces, pointer conversions, qualified auto and trailing comma). Remaining acceptance stages ran separately as recorded above. Full acceptance is not claimed.

Logs: `/tmp/hn-provider-check.log`, `/tmp/hn-provider-ctest.log`, `/tmp/hn-provider-qml.log`, `/tmp/hn-provider-format.log`, `/tmp/hn-provider-import.log`, `/tmp/hn-provider-types.log`, `/tmp/hn-new-tidy.log`, `/tmp/hn-test-tidy.log`, `/tmp/hn-no-wayland-final.log`, `/tmp/hn-no-wayland-all.log`, `/tmp/hn-no-wayland-ctest.log`.

Native Wayland SSD, Qt CSD, undecorated, fullscreen entry/exit and surface recreation require user interaction; offscreen tests do not establish native detection correctness. The user requested that native checks remain pending. Publication and pins remain pending authorization.
