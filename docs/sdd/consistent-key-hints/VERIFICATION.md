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

## KH-006 refinement acceptance — 2026-09-20

The earlier KH-001 results above are retained as historical evidence.
Baseline: `8fe24ff83f8108631c2b7b0351994f7e513acfcd` on `main`.
Qt 6.11.2, GNU C++ 16.2.1, Debug, tests/demo/gallery enabled.
Clean acceptance directory: `/tmp/holonight-kh006-acceptance`.
Reused config provider `fe69a59e6b73167fd5349223a4d265d75386c139` is clean,
GCC Debug with tests disabled; build-tree and `/tmp/holonight-config-prefix`
library SHA-256 hashes match (`8c6d870814a9ab238e2a837ffc8dda47343503de72ade4bd16acb322e546acc8`).

```sh
cmake -S holonight-qt -B /tmp/holonight-kh006-acceptance -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON -DBUILD_DEMO=ON \
  -DBUILD_CONTROLS_GALLERY=ON -DCMAKE_PREFIX_PATH=/tmp/holonight-config-prefix
cmake --build /tmp/holonight-kh006-acceptance -j 6
QT_QPA_PLATFORM=offscreen ctest --test-dir /tmp/holonight-kh006-acceptance \
  --output-on-failure -j 4
```

- Seven focused tests passed under both Holonight and Fusion. Existing coverage
  still verifies every symbol, semantic names, literal compatibility, wrapping,
  font changes and disabled presentation. New coverage checks the standalone
  label's parity, full-font binding and explicit color, no frame/padding/focus,
  centered compact single keys at 8/12/18 pt and explicit zero-radius appearance.
- Initial square-corner test used an invalid zero shape scale; corrected the
  fixture to the supported explicit `base_radius = 0` setting and reran successfully.
- Clean build and final incremental correction build passed without compiler
  diagnostics. Complete logs were reviewed; only pre-existing Qt private-module
  and QTP0004 configure notices remain.
- Final full CTest passed **89/89**, including provider/demo/gallery QML source
  policy checks, gallery/demo startup overrides, both runtime styles, fractional
  rendering and staged package install. No binding-loop diagnostics occurred.
- Controls and gallery qmllint targets passed. The new label's default font was
  corrected to the theme's UI body font after lint identified unsupported
  `Qt.application.font` metadata. No changed-file QML diagnostics remain;
  existing unqualified-access warnings in other controls and SettingsPage remain.
- C++ formatting and diff whitespace checks passed. REUSE passed after allowing
  its multiprocessing socket outside the sandbox; the final documentation and
  visual evidence check passed (417/417 files).

Visual review: captured R, 1, Ctrl+0, mixed symbols, frameless sequences, disabled
badges and constrained wrapping at 8/12/18 pt with DejaVu Sans Mono and DejaVu Sans,
scales 1, 1.25 and 1.5. Short keys are centered in wider-than-tall compact badges;
no clipping was observed. See [compact matrix at 1.25](compact-visual-review-1.25.png).
The gallery includes these short-key and frameless examples with live size/family
controls. Captures used offscreen Qt rendering; no desktop interaction was automated.

KH-007 Viewer menu/footer/help acceptance and KH-008 Shell/Files compatibility
remain pending the corrected provider's explicit publication and pin handoff.
Native ecosystem checks and final KH-005 integration remain pending. These
provider results do not establish consumer or integrated acceptance.

Final full clang-tidy completed successfully with the external build database and
explicit repository configuration:

```sh
cmake -P /tmp/holonight-kh006-acceptance/strip_tidy_flags.cmake
run-clang-tidy -quiet -hide-progress -j 6 \
  -p /tmp/holonight-kh006-acceptance/tidy \
  -config-file=/home/andrii/Projects/pet/holonight/holonight-qt/.clang-tidy \
  '/home/andrii/Projects/pet/holonight/holonight-qt/.*\.cpp$'
```

Existing source and Qt/GTest-header diagnostics remain. New-test precedence
warnings were corrected; the geometry matrix follows the existing focused
suppression for cognitive complexity caused by GTest macro expansion. No
clang-tidy diagnostics remain in the added tests.
