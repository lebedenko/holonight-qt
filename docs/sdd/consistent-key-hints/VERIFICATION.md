# Provider verification — 2026-09-20

## Environment

Baseline: `8095d49eff12f6dc7742ae1590b860debb90ed20` on `main`.
Qt 6.11.2, GNU C++ 16.2.1, Debug, tests/demo/gallery enabled.
Clean acceptance directory: `/tmp/holonight-key-hints-acceptance`.
Configuration provider: `holonight-config@fe69a59e6b73167fd5349223a4d265d75386c139`.
Its clean worktree/build reports no pending compilation; the installed and build-tree
`libholonight_config.so` SHA-256 hashes match. Existing GCC Debug provider artifacts
were reused from `/tmp/holonight-config-prefix`.

## Commands and results

```sh
cmake -S holonight-qt -B /tmp/holonight-key-hints-acceptance -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON -DBUILD_DEMO=ON \
  -DBUILD_CONTROLS_GALLERY=ON -DCMAKE_PREFIX_PATH=/tmp/holonight-config-prefix
cmake --build /tmp/holonight-key-hints-acceptance -j 6
QT_QPA_PLATFORM=offscreen QT_QPA_PLATFORMTHEME= QT_QUICK_BACKEND=software \
  QT_QUICK_CONTROLS_STYLE=Holonight \
  /tmp/holonight-key-hints-acceptance/tests/holonight_runtime_composite_tests \
  --gtest_filter='QmlSmoke.Controls_KeyHint*:SharedRendering.KeyHint*'
QT_QPA_PLATFORM=offscreen ctest --test-dir /tmp/holonight-key-hints-acceptance \
  --output-on-failure -j 4
```

- Clean build passed; no compiler warnings. Existing Qt private-module/QTP0004 configure notices remain.
- Four focused key-hint tests passed. These cover semantic names and precedence, punctuation,
  all symbol labels, literal compatibility, wrapping, runtime font updates, disabled appearance,
  noninteractive defaults, and distinct rendered symbols in two fonts at 8/12/18 pt.
- Initial full CTest: 84/89 passed; the gallery exposed an invalid Flow implicitHeight assignment.
  Removing that assignment resolved the four gallery startup cases and package-install test.
  Affected rerun: 8/8 passed, including both runtime styles and installed semantic/rendering tests.
- Final metric calibration and lint corrections were followed by the focused tests and an affected
  acceptance rerun: 20/20 passed (package install, both examples in all override modes,
  smoke/composites, both styles at DPR 1/1.25, and all three QML source policy checks).
  A final test-only lint cleanup was followed by the four focused tests under both
  Holonight and Fusion; all eight executions passed. Unaffected palette/platform tests were not repeated.
- Controls and gallery qmllint targets completed without diagnostics in changed QML files.
  Existing unqualified-access warnings remain in HnTextArea, HnLoadingState, HnColorPicker
  and gallery SettingsPage, plus existing unused-import information.
- C++ formatting and `git diff --check` passed.
- `reuse lint` passed (415/415 files including final docs and the screenshot); sandbox blocked its multiprocessing
  socket initially, then the authorized unsandboxed run succeeded.

## Static analysis

The CMake `tidy` target initially reported `No checks enabled` because its external
build-directory working directory could not discover the repository configuration.
The equivalent run with explicit configuration completed successfully:

```sh
run-clang-tidy -quiet -hide-progress -j 6 \
  -p /tmp/holonight-key-hints-acceptance/tidy \
  -config-file=/home/andrii/Projects/pet/holonight/holonight-qt/.clang-tidy \
  '/home/andrii/Projects/pet/holonight/holonight-qt/.*\.cpp$'
```

Existing repository/Qt-header diagnostics remain; new test findings were corrected
and checked again. One linear state test locally suppresses cognitive-complexity
noise caused by GTest macro expansion; its behavior is unchanged. A direct C++ header lint of `qml/keyhintnames.h` also passed,
with only Qt macro diagnostics and the standalone-header pragma notice.

## Visual review

Offscreen render matrix: DejaVu Sans Mono and DejaVu Sans, 8/12/18 pt,
DPR 1, 1.25 and 1.5. Inspected symbol ink, capital alignment, stroke consistency,
badge height, disabled state, mixed combinations and wrapping. No clipping in the
representative matrix. See `visual-review-1.25.png`. The gallery offers live family
and size controls for reproduction. No desktop pointer or focus interaction was automated.

Metrics follow the [Qt FontMetrics contract](https://doc.qt.io/qt-6/qml-qtquick-fontmetrics.html).
Vector stroke coordinates include optical insets; individual keys remain atomic when wrapping.

## Remaining integration work

Provider publication and umbrella pin authorization are pending. Shell, Viewer and Files
implementation must follow the published/pinned provider handoff. Native ecosystem review,
consumer activation regressions and final integration acceptance remain pending. This provider
verification does not mark the cross-repository initiative Integrated.
