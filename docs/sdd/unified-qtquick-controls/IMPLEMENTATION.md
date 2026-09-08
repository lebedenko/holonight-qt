# UQC-101 — provider implementation

Status: Done (provider acceptance completed 2026-09-08). Assigned repository: holonight-qt.
Exact published upstream baseline: `033d6001fd088a96ac6e4ff936b6bafcf6ab5d4c` (origin/main, checked 2026-09-07).
Umbrella acceptance checkpoint: `8764bc8`. REVIEW contains the user-approved scope; DESIGN contains the audited
contracts and verification requirements. Only provider files are changed in this work package.

## Implementation order and requirements

1. Correct CheckBox/RadioButton/Switch indicator-only implicit sizing and mirrored layout. Preserve labeled
   controls, size roles, checked/disabled geometry and trailing-card containment. Add behavioral regressions.
2. Give standard controls application/control palette authority while preserving HoloNight session defaults.
   Map text, surfaces, highlights, disabled groups, borders and hover/pressed states; preserve Core/composite
   appearance authority. Verify default-theme parity and black/white/black application-palette changes.
3. Add the nine accepted Templates implementations and installed metadata: ApplicationWindow, Label, ToolButton,
   ToolBar, ToolSeparator, MenuSeparator, Popup, MenuBar and MenuBarItem. Preserve Basic and explicit Fusion fallback.
4. Migrate composites to runtime Controls, preserving public error/sizing/corner APIs and scaled ComboBox geometry.
   HnLabel uses Templates.Label to keep Core free of selected-style dependencies.
5. Migrate demo/gallery, embed root qtquickcontrols2.conf defaults, retain explicit overrides, update import policy,
   guides and installed/negative/rendering fixtures. Do not turn discovery defect assertions into acceptance tests.
6. Run focused checks first, then the provider suite and installed consumers. Record limitations and publish the
   verified provider before requesting an umbrella handoff. Consumer migration and ecosystem acceptance are separate.

## Progress

| Slice | State | Verification |
|---|---|---|
| Indicator geometry | Done | Original code fails the new regression; fixed code passes all five focused tests and all 28 CTest entries (2026-09-07). |
| Application palette support | Done | Black/white/black and reload regressions reproduced before fixes; all 46 CTest entries pass, including 16 schemes, offscreen rendering, hybrid and installed-prefix fixtures (2026-09-08). |
| Nine new controls | Done | All nine installed origins, palette/layout/overlay/state acceptance and all 46 provider CTest entries pass (2026-09-08). |
| Composite runtime migration and Core isolation | Done | All 26 public composites load; HoloNight/Fusion origins, behavior, shared popup geometry and installed-prefix acceptance pass (2026-09-08). |
| Executable defaults | Done | Both actual examples pass embedded/default, environment, command-line and configuration overrides in build and installed-prefix runs (2026-09-08). |
| Policy, documentation and installed verification | Done | Ten isolated cases, Qt-default reference, final contract review and all 61 provider CTest entries pass (2026-09-08); see final acceptance record below. |

Application-painted sliders and the license-popup composition remain accepted boundaries. Other catalog types
remain fallback controls. No full manual application/state or two-compositor integration pass is implied here.

Geometry files: qml/CheckBox.qml, RadioButton.qml, Switch.qml and tests/test_qml_smoke.cpp.
Switch thumb placement follows visualPosition for mirrored state. Full build and CTest passed; installed-package
smoke is included. UQC-101 remains In Progress pending the other implementation slices.

## Application palette support — 2026-09-08

Palette slice baseline: published `22ded7815727ce483fd91e82a9cc04bfe252ec3b`.
The nine new controls, runtime-import migration and consumer changes are subsequent slices.

The custom `QQuickStylePlugin` retains generated registration and the installed `Holonight` identity. It reads
appearance synchronously without constructing a watcher on the QML loader thread. The GUI-thread reader updates
System defaults and sends `ApplicationPaletteChange` to existing Quick windows. The Quick plugin never calls
`QGuiApplication::setPalette()`. Configuration palettes resolve against appearance defaults, then explicit
application roles resolve against that result. Qt's selected-style/fallback gate prevents this plugin from
installing defaults alongside explicit Fusion or a Haruna-style Fusion fallback.

`Holonight.impl.ControlPalette` is implementation-only, not a supported palette-selection API. It observes the
mutable effective Qt Quick palette, the control's Active/Inactive/Disabled group and appearance tokens. Core's
palette, typography, icons and appearance configuration remain unchanged. All seventeen painted standard controls
use this resolver; ScrollView has no direct palette painting and is covered through attached scrollbar inheritance.

| Painted value | Effective palette roles | Override behavior |
|---|---|---|
| Background / input / elevated / button surfaces | Window / Base / AlternateBase / Button | Use the supplied role directly. |
| Labels / editable text / button text | WindowText / Text / ButtonText | Use the effective group; SpinBox arrows use ButtonText separately from its editor. |
| Muted and placeholder text | PlaceholderText | Use the supplied role directly. |
| Accent and contrasting foreground | Highlight / HighlightedText | Use supplied RGBA, including transparent colors. |
| Passive / focus / active borders | Mid / Highlight / Highlight | Keep appearance error borders as the explicit `hasError` extension. |
| Tooltip surface and text | ToolTipBase / ToolTipText | Preserve the old raised surface and primary text when roles match defaults. |
| Hover | Relevant fill and foreground | Blend fill 10% toward foreground. |
| Pressed accent | Highlight and Base | Blend Highlight 20% toward Base. |
| Quiet selected surface | Base and Highlight | Blend Base 12% toward Highlight, or 20% while hovered. |
| Subtle border | Mid and Base | Blend 50%. |

Each painted color compares only its relevant roles against `buildPalette(currentTokens)`. Matching roles return
the original semantic token exactly, including tooltip and translucent selection compatibility. Derived colors
blend RGBA channels deterministically. No desktop accent is substituted into an overridden palette. Disabled
background opacity remains only for default background/border roles; explicit Disabled roles, including colors
equal to defaults, do not receive another opacity reduction.

Composite-owned standard-control boundaries supply an appearance palette through the internal helper. The existing
search-field disabled background opacity is retained. ComboBox's owned popup inherits through Qt resolve masks,
so its own explicit roles survive changes to the ComboBox palette. Ordinary menus/popups follow Qt's window-palette
inheritance contract. Attached scrollbars and popup delegates receive effective inherited palettes.

The QWidget reload path resolves the current application palette against new style defaults and restores its
resolve mask. The Qt 5 discovery build uses the equivalent Qt 5 mask API. QWidget painting and widget polish are
unchanged. Application palette selection never writes appearance configuration.

New private dependencies are confined to the QML implementation/plugin and fixture targets. The private components
are found at exactly the installed Qt version (verified with Qt 6.11.2); none are added to exported consumer targets.

### Verification record

The new black → white → black regression failed before standard-control adoption for background, text, placeholder,
selection and selected text. The new QWidget regression separately reproduced overwritten explicit colors and a
broadened resolve mask before the reload fix. The discovery characterization runner remains unchanged.

Acceptance coverage includes partial parent inheritance, child overrides, sibling isolation, role reset,
Active/Inactive/Disabled groups, explicit default-valued Disabled colors, alpha, all eighteen standard controls,
menu/window inheritance, popup-local overrides, attached scrollbars, composite authority, and appearance changes
while an application override remains active. Sixteen isolated scheme fixtures verify exact semantic defaults;
software/offscreen rendering checks hover, pressed, selected and disabled colors by changing isolated control state
directly, without mouse movement, clicking or desktop focus automation. Focus-border token mapping is checked
without desktop focus interaction.

Installed-prefix fixtures cover HoloNight defaults, explicit Fusion, Haruna-style Fusion fallback, configuration
palette overrides and application-over-configuration precedence. QML context URLs identify the runtime types;
`/proc/self/maps` confirms that Holonight, Core, Controls and impl plugins come from the staged prefix. The complete
palette acceptance fixture also runs against that prefix. Both QGuiApplication and QApplication are exercised;
the hybrid fixture loads the actual HoloNight QWidget style. Repeated style reloads preserve explicit role masks.

Commands (from the umbrella root):

```sh
cmake --build holonight-qt/build -j 6
QT_QPA_PLATFORM=offscreen QT_QPA_PLATFORMTHEME= QT_STYLE_OVERRIDE= \
  holonight-qt/build/tests/holonight_tests \
  --gtest_filter=StyleSmoke.ReloadPreservesApplicationPaletteRolesAndResolveMask
ctest --test-dir holonight-qt/build \
  -R 'holonight_control_palette_tests|holonight_quick_palette_(dark|hybrid)$|holonight_package_install_test' \
  --output-on-failure
QT_QPA_PLATFORM=offscreen ctest --test-dir holonight-qt/build --output-on-failure -j 4
git -C holonight-qt diff --check
```

Final results (2026-09-08): full build passed; the focused reload, palette and installed fixtures passed;
all 46 provider CTest entries passed (5.82 seconds). Formatting checks for changed C++ files and whitespace checks
passed. No NeoChat named-scheme observation,
Hyprland/Sway manual acceptance or UQC-201 ecosystem integration check is implied by these provider tests.

## Nine standard controls — 2026-09-08

Slice baseline: published `bc3ed4a0f8e6b9b0d3a3568b07c8c2905332e955`, umbrella checkpoint `8099678`.
UQC-101 remains In Progress; composite runtime imports, executable defaults and consumer migration are subsequent work.

All nine additions use their matching `QtQuick.Templates` types in the existing `Holonight` module. Build resources,
installed QML files, generated qmldir registration and tooling metadata retain the existing module identity.
The palette resolver is a child object addressed by QML id; these controls add no HoloNight-specific public properties.
No custom input handlers or window management are introduced. Core HnLabel and the separate HnApplicationWindow
frame API are unchanged.

| Controls | Visual and role contract |
|---|---|
| ApplicationWindow | Window background and HoloNight body typography; Qt owns content, header/footer, menu-bar and safe-area behavior. |
| Label | Effective WindowText and direct Link, including alpha and disabled groups; standard text layout and accessibility. |
| ToolButton | Transparent idle; Button/ButtonText hover and Button pressed surfaces; Base/Highlight quiet checked surface; Highlight/HighlightedText highlighted surface; Highlight focus border. IconLabel preserves action/icon/display/mirroring behavior. |
| ToolBar / MenuBar | Button raised surface, normal control-height minimum, Qt safe-area padding and content-driven width. MenuBar uses standard MenuBarItem delegates and a mirrored row of its content model. |
| ToolSeparator / MenuSeparator | Mid/Base subtle blend, existing separator thickness; Basic orientation-sensitive padding and 30-pixel toolbar length. Menu separator defaults to 180 pixels overall; explicit padding changes implicit size. |
| Popup | Button raised surface, Mid passive border, Menu radius, 12-pixel padding, no minimum content size. Padding zero and content/background replacement remain authoritative. Qt owns placement, lifecycle and close policies. |
| MenuBarItem | Transparent idle, MenuItem-compatible hover/highlight/pressed treatment, Highlight focus border, normal control-height minimum, menu padding and IconLabel. |

The existing role-local semantic compatibility branches preserve all sixteen appearance schemes. Explicit Disabled
roles are painted without an additional opacity reduction. Link is returned directly; overlay RGB comes directly
from effective Shadow and alpha is multiplied by 0.5 (modal) or 0.12 (modeless), including transparent Shadow.
Ordinary popups follow Qt window palette inheritance rather than inheriting an intervening control's palette.
No full appearance palette is copied into these standard controls.

Acceptance extends `test_quick_palette.cpp`, `test_control_palette.cpp`, `quick_style_selection.cpp` and the staged
package fixture. It covers exact semantic defaults under all sixteen schemes, application overrides and reloads,
local override/reset and sibling isolation, color groups and alpha, generated menu-bar delegates, mirrored order,
icon-only/text-only and long-label sizing, explicit padding and visual replacements, header/footer placement,
popup content and direct open/close, and both overlay components. The ToolButton zero-padding regression exposed
and removed a separate horizontal-padding default that would have prevented callers from clearing all padding.

Software offscreen pixels cover idle, hover, checked, highlighted/pressed and disabled states. Focus-border color
bindings are inspected without desktop focus changes. The reload test observes an open popup, following Qt's
window-palette propagation lifecycle. Overlay components are instantiated using their QML creation context.
Installed origins are checked for every new type and generated delegates in separate HoloNight, explicit Fusion,
Haruna-style Fusion fallback, and HoloNight-with-Fusion-fallback processes; Dialog remains the Basic/Fusion fallback
sentinel. Configuration and application-over-configuration palette fixtures also remain covered.

Verification commands from the umbrella root:

```sh
cmake --build holonight-qt/build -j 6
ctest --test-dir holonight-qt/build \
  -R 'holonight_control_palette_tests|holonight_quick_palette_(dark|hybrid)$|holonight_package_install_test' \
  --output-on-failure
QT_QPA_PLATFORM=offscreen ctest --test-dir holonight-qt/build --output-on-failure -j 4
clang-format --dry-run --Werror holonight-qt/qml/controlpalette.cpp \
  holonight-qt/tests/test_control_palette.cpp holonight-qt/tests/test_quick_palette.cpp \
  holonight-qt/tests/quick_style_selection.cpp
git -C holonight-qt diff --check
```

Historical discovery characterization is unchanged. These are provider acceptance checks, not real-application
acceptance. NeoChat named-scheme observations and manual Hyprland/Sway acceptance remain ecosystem integration gates.

Final results (2026-09-08, Qt 6.11.2): full build passed; all four focused palette/hybrid/installed checks passed;
all 46 provider CTest entries passed (7.92 seconds), including indicator geometry regressions. Changed C++ formatting
and whitespace checks passed. The source-policy inventory now includes the nine additions and recognizes Core
HnLabel's existing explicit Basic import; this does not migrate Core or change the historical discovery runner.

Provider commit sequence: `675240e` (window, label, toolbar and separators), `7d0c988` (popup and menu bar), followed
by the installed acceptance/documentation commit containing this record and the padding regressions.


## Composite runtime migration — 2026-09-08

Slice baseline: published provider `24220e95a4ca612700e2fd2b0395f59a481144b6`, umbrella
`b44340c169dd63073740efd088b76913b1353bd1`. UQC-101 remains In Progress; consumers remain Planned.

All fixed-style imports in `qml/controls/` now use `QtQuick.Controls as C`, including attached ToolTip/ScrollBar
properties and enums. Existing Templates-based primitives and application-owned frames remain intact. The Controls
module declares QtQuick.Controls, Core and impl imports; its build dependency no longer requires the selected style.
Core HnLabel derives directly from Templates.Label, retaining its typography roles, prefix formatting, text/color/font
caller overrides, wrapping, sizing and accessible StaticText interface. `linkColor: palette.link` preserves the former
Basic Label's effective Link behavior without loading runtime Controls.

HnSearchField declares hasError locally and owns the prior semantic error/focus frame and disabled opacity.
HnTextArea uses runtime ScrollView, TextArea and attached scrollbars; its outer error frame remains authoritative,
and the editor no longer receives the style-specific hasError property. Both preserve editing/selection APIs,
size roles, slots, clear behavior, maximum length and wrapping/overflow. Search, editor and icon ComboBox explicitly
retain HoloNight typography under Fusion, with caller overrides tested. Search and ComboBox body frames retain
palette-role resolution at the existing appearance boundary.

HnIconComboBox owns maximumVisibleItems (default 8), delegateHeight (size-role default), popup and delegate frames.
Its runtime ComboBox, Popup, ItemDelegate and ScrollBar implementations follow the selected style. Semantic delegate
corners are calculated from popup dimensions/appearance directly, without reading another style's background.
The intentional composite visuals and appearance palette remain present under Fusion.

`qml/ComboBoxPopupGeometry.qml`, registered and installed in `Holonight.impl`, shares the former standard ComboBox
calculations with HnIconComboBox. This namespace is implementation-only; the helper is not a supported public API.
It imports only QtQuick and Templates and takes a template control/popup plus the visible-item limit. Existing standard
ComboBox geometry properties remain forwarded, including the popup geometryRevision refresh and the observable
popupTransformSupported diagnostic. Overlay parenting, uniform scene scaling, above/below placement, margins,
item limits, scrolling and highlighted-item visibility retain their behavior. Unsupported transforms continue to
report false and use the existing scale fallback; support for rotated/nonuniform transforms is not added.

Verification additions are in `tests/RuntimeComposites.cmake`, `test_runtime_composites.cpp`, the shared smoke tests,
strict diagnostic handler, package-install fixture and import-policy fixtures. The runtime runner reuses existing
behavior tests rather than duplicating them. Each HoloNight/Fusion process runs 59 tests, including all 26 public
composite creations and the two-variant geometry suite; a separate Core-only process checks that no runtime Controls
or HoloNight style/composite plugin loads. Installed runs override every fixture's import root and check plugin
mappings against the staged prefix. Actual base, editor, popup, delegate, progress and scrollbar context URLs are
asserted. The Fusion/Haruna selection fixture now requires the HoloNight style plugin to remain unloaded.

Geometry covers both variants at 0.78, 1.0 and 1.25 near each horizontal and vertical edge, reopening after ancestor
movement, empty and 100-item models, default/custom/invalid limits, constrained windows, selected-item containment,
and unsupported transforms. Direct editing tests cover selection, maximum length, undo/redo, removal, slots, visual
replacements, wrapping/overflow, disabled treatment and appearance reload. Existing offscreen rendering and interaction
regressions run under both styles without desktop pointer/focus automation. A historical pressed-state comparison now
supplies the same explicit appearance palette to its standard delegate as the compared composites.

The source policy rejects fixed-style imports, missing/wrong runtime namespaces, unqualified control instances,
enums and attached properties inside the migrated scope, and Controls/style imports inside Core or the geometry
helper. Positive and individual negative fixtures enforce these rules. Standard style templates, demo/gallery defaults
and historical compatibility fixture imports remain explicitly outside this slice's new rules.

Commands from the provider root:

```sh
cmake --build build -j 6
ctest --test-dir build -R 'holonight_(runtime_composites|core_isolation|package_install|qml_.*policy)' --output-on-failure
QT_QPA_PLATFORM=offscreen ctest --test-dir build --output-on-failure -j 4
clang-format --dry-run --Werror tests/test_runtime_composites.cpp tests/test_qml_smoke.cpp tests/quick_palette_main.cpp tests/quick_style_selection.cpp
git diff --check
```

Results: focused and installed acceptance passed; full provider build and all 49 CTest entries passed (11.65 seconds)
with Qt 6.11.2. Changed C++ formatting and whitespace passed. The first slice separately passed 44 focused tests;
the second passed 21 search/text/geometry tests in each selected style and installed acceptance before its commit.

Provider sequence: `f32f5bb` (Core and straightforward imports), `da470c5` (compatibility and shared geometry), followed
by this acceptance/policy/implementation-record commit, including typography and frame-palette preservation found
in final review. Demo/gallery defaults, broader guide alignment, remaining provider negative/final acceptance,
consumer migration and manual Hyprland/Sway ecosystem acceptance remain subsequent work. No UQC-201 checks ran.

## Demo/gallery runtime defaults — 2026-09-08

Slice baseline: published `bd5f2f0f6844ec809b3fa80182a61493cf724c0a`, umbrella `8e3cc74`.
UQC-101 remains In Progress; final isolated negative fixtures and provider acceptance are subsequent work.

Both graphical examples embed `:/qtquickcontrols2.conf` with `Style=Holonight` and remove imperative startup style
selection. Application QML qualifies standard controls, fallback types, attached properties and enums using
`QtQuick.Controls as Controls`. Their CMake imports declare runtime Controls and the explicit Core/composite APIs.
The demo's error examples use HnSearchField/HnTextArea. Gallery Switch sizeRole assignments are conditional on the
selected implementation exposing that extension: HoloNight keeps semantic track sizes; Fusion keeps its native
sizing. The initial migrated gallery failed under Fusion with a nonexistent sizeRole diagnostic before this fix.

Installed executables locate QML relative to their own prefix and do not add the configured build-tree import path.
Build executables retain that path only when running from their configured build directory. Public composites and
standard-control implementations are unchanged in this slice. No process-global appearance writes are added.

Changed files: demo and gallery QML, startup sources, generated-header templates, CMake definitions and new config
resources; source-policy script and positive/negative fixtures; executable startup runner, CTest and installed test
registration; CI configuration; README and selection/shared-control/frame guides. The ignored local AGENTS.md is
also aligned with the application namespace, embedded defaults and focused verification commands.

The runtime checker starts the actual executables offscreen in four separate-process modes: no style overrides,
environment Fusion, command-line Fusion overriding environment HoloNight, and external Fusion configuration.
It checks resolved Controls.Button URLs, all loaded HoloNight module paths, absence of the HoloNight style plugin
under Fusion, no QML diagnostics and no appearance file writes. All gallery pages instantiate eagerly. Each process
is observed for three seconds and terminated; this is bounded startup coverage, not a desktop event-loop readiness,
visual, editing or pointer/focus interaction claim. The same checks run on installed binaries with inherited QML paths
cleared. Existing provider fixtures continue to verify detailed palette, rendering, composite and geometry behavior.

Source policy now covers both application directories, including fallback catalog types, with positive and individual
negative namespace/import/instance/enum/attached-property cases. CI enables both examples so this coverage is active.
The selector guide replaces historical blanket selection claims with the accepted override, coverage, palette and
module-discovery boundaries. Historical discovery characterization remains unchanged.

Verification commands from the provider root:

```sh
cmake -S . -B build -DBUILD_TESTS=ON -DBUILD_DEMO=ON -DBUILD_CONTROLS_GALLERY=ON
cmake --build build -j 6
ctest --test-dir build -R 'startup_|qml_.*policy|package_install' --output-on-failure -j 4
QT_QPA_PLATFORM=offscreen ctest --test-dir build --output-on-failure -j 4
clang-format --dry-run --Werror demo/main.cpp examples/controls-gallery/main.cpp
bash -n scripts/check-qml-import-policy.sh
git diff --check
```

Results: full build and all 59 provider CTest entries passed (43.15 seconds, Qt 6.11.2); thirteen focused entries
including all installed startup modes passed. A final focused rerun covers the checker runtime-directory isolation
and relative source-policy paths. C++ formatting, Python syntax and whitespace checks pass. Configuration reports
existing Qt private-API and gallery QTP0004 warnings. No UQC-201 integration or human-operated ecosystem checks ran.

Remaining provider acceptance: isolated absent-module/plugin/dependency and bad-case diagnostics; explicit competing
Basic/Fusion imports, imperative style precedence and platform-theme-only negative fixtures; final contract review and
published provider handoff. Consumer repositories remain unassigned until UQC-101 completes.

## Final isolated provider acceptance — 2026-09-08

Handoff slice baseline: published `82ccb126c2ad8f364f32e0ae3b1551040a6949cb`; umbrella
`a68e524da790314eb55c64ee7b14bfb99ab4e5e5`. This final review supersedes the open provider gates in the
historical slice records above. No production APIs or style behavior changed in this slice.

Changed files: `tests/isolated_style_probe.cpp`, `tests/check_isolated_style.py`, `tests/CMakeLists.txt`,
`tests/test_package_install.cmake.in`, `tests/test_runtime_composites.cpp`, `tests/test_quick_palette.cpp`,
`.github/workflows/ci.yml`, and this record.

### Isolation and acceptance evidence

The standalone probe links only Qt (including version-matched QmlPrivate for origin inspection), with only the
example's root controls configuration embedded. `ldd` confirms no linked HoloNight libraries. Every selection runs
in a fresh process; the engine's complete import list is asserted to equal exactly the per-case provider fixture
and a filtered Qt tree containing only QtQml and QtQuick. Normal engine host/build/resource import roots are excluded.
Qt embeds its IndirectBasic forwarding qmldir in the Controls plugin: the probe loads that copied Qt plugin and
copies its Qt-owned metadata into the filtered tree, without adding a resource import root or synthesizing metadata.

The runner clears inherited Qt, QML, loader and appearance overrides and uses disposable XDG directories. Only the
configured native configuration-library directory is supplied via LD_LIBRARY_PATH. It copies the staged provider
into distinct per-case trees, omitting the selected module/plugin/dependency during copying. It never removes files
from a host module or the normal staged installation. The native failure rewrites the copied style plugin's actual
configuration-library DT_NEEDED entry to a temporary-fixture-specific missing library name. The real host config
library remains installed and cannot satisfy that name. Patchelf is required only when tests are enabled and is
included in CI's test dependencies; this session obtained 0.19.1 via a temporary uv environment, with no system install.

The probe exits 0 after successful creation, 1 for component-creation failure, and 2 for harness failures. Every
negative requires exactly exit 1 plus its intended component diagnostic. Signals, crashes, other exits and the
20-second subprocess timeout fail acceptance. The orchestrator also has a 240-second timeout. JSON evidence records
effective import paths, complete component errors, Button/TextField context URLs and mapped plugin paths. All loaded
HoloNight modules must come from that case's fixture; success also requires the expected style/Core/impl mappings.
Resource URLs are interpreted together with those mapped plugin origins, never as standalone installation evidence.
The installed runner saves the complete reports to `build/tests/package-install-test/isolated-style.log`.

| Case | Observed result |
|---|---|
| Complete deployment | Exit 0; runtime Button/TextField use Holonight QML; style, Core and impl plugins map from the fixture. |
| Missing Holonight | Exit 1; `module "Holonight" is not installed`; no host provider loads. |
| Missing style plugin | Exit 1; `plugin "holonight_qml" not found`. |
| Missing Core | Exit 1; `module "Holonight.Core" is not installed`. |
| Missing impl | Exit 1; `module "Holonight.impl" is not installed`. |
| Missing native dependency | Exit 1; cannot load copied style plugin, naming `libuqc-isolated-<fixture>_missing_config.so`. |
| Incorrect casing | Exit 1; `module "HoloNight" is not installed`, with correctly cased Holonight available. |
| Explicit Basic/Fusion imports | Exit 0; Basic.Button and Fusion.TextField retain their origins alongside runtime HoloNight Button/TextField. |
| Imperative precedence | Exit 0; setStyle(Fusion) wins over command-line Holonight, environment Holonight and embedded Holonight; no style plugin loads. |
| Platform theme alone | Exit 0; the copied qholonight platform plugin loads, but runtime origins match a separate no-theme/no-selector reference process (Fusion on this Qt build); no HoloNight Quick style plugin loads. |

### Final contract review

Reviewed [SPEC](SPEC.md), [DESIGN](DESIGN.md), [approved REVIEW](REVIEW.md) and every implementation slice above.
SPEC and DESIGN preserve historical discovery findings; the approved REVIEW and subsequent implementation records
settle the implementation contract. Provider requirements have the following evidence:

| Provider requirement | Acceptance evidence |
|---|---|
| Indicator geometry and mirrored containment | Existing indicator and ComboBox geometry regressions, including uniform scales 0.78/1.0/1.25, remain passing. |
| Application palette authority; intentional Core/composite appearance | Sixteen scheme fixtures, palette inheritance/group/alpha/reset/reload tests, hybrid QWidget checks, and installed palette/composite fixtures. |
| Nine approved standard controls; Basic/Fusion fallback | Existing type/delegate origin, layout, state, overlay and installed-selection checks. Unapproved catalog types remain fallback controls. |
| Runtime composite migration and Core independence | All public composites load under HoloNight/Fusion; existing editing, replacement, wrapping, scrolling, popup, disabled-state and Core-only checks. |
| Password echo, input hints and length limits | New runtime fixture verifies masked display with retained/truncated text on standard TextField and HnSearchField, hint flags, zero-length input, and HnTextArea editor hint forwarding and positive/zero limits. It runs under both styles and against installed modules. HnTextArea retains its existing text-change truncation behavior. |
| Rendering at multiple device ratios | Two representative existing state-rendering tests now run at DPR 1.0 and 1.25. Physical sample coordinates use the window DPR because software grabWindow images retain DPR metadata 1. Image dimensions are checked against the physical window size; exact center-pixel palette colors pass. |
| Executable defaults and override hierarchy | Both built and installed example executables pass four startup modes; the isolated imperative and competing-import cases complete precedence coverage. |
| Installation failures and platform/Quick independence | Ten isolated cases plus the Qt-default reference above; no host provider masks an incomplete deployment. |
| Policy and documentation | Provider/Core/composite and both application import-policy checks, positive/negative policy fixtures, C++ formatting, Python/shell syntax, local SDD links and whitespace checks. |

All new checks remain offscreen, without desktop pointer or focus automation. Existing isolated control-state and
editing harnesses are reused. No system installation, consumer implementation, live authentication challenge or
UQC-201 integration run was performed.

### Final verification commands

From the umbrella root (the patchelf path below is this session's disposable tool environment):

```sh
UV_CACHE_DIR=/tmp/uqc-uv-cache uv run --with patchelf --no-project patchelf --version
cmake -S holonight-qt -B holonight-qt/build -G Ninja -DCMAKE_BUILD_TYPE=Debug \
  -DBUILD_TESTS=ON -DBUILD_DEMO=ON -DBUILD_CONTROLS_GALLERY=ON \
  -DHOLONIGHT_PATCHELF_EXECUTABLE=/tmp/uqc-uv-cache/archive-v0/NBwrEcQZIC2EIFJPI2MRH/bin/patchelf
cmake --build holonight-qt/build -j 6
cmake -DUQC_ISOLATED_ONLY=ON -P holonight-qt/build/tests/test_package_install.cmake
ctest --test-dir holonight-qt/build \
  -R 'holonight_quick_rendering_dpr|holonight_runtime_composites' --output-on-failure
ctest --test-dir holonight-qt/build \
  -R 'package_install|startup_|palette_(dark|hybrid)$|control_palette|runtime_composites|core_isolation|qml_.*policy' \
  --output-on-failure -j 4
QT_QPA_PLATFORM=offscreen ctest --test-dir holonight-qt/build --output-on-failure -j 4
clang-format --dry-run --Werror holonight-qt/tests/isolated_style_probe.cpp \
  holonight-qt/tests/test_runtime_composites.cpp holonight-qt/tests/test_quick_palette.cpp
bash -n holonight-qt/scripts/check-qml-import-policy.sh
git -C holonight-qt diff --check
```

Python `ast.parse` checked both acceptance/startup runners; a local Markdown-link existence check covered SPEC,
DESIGN, REVIEW and IMPLEMENTATION. The probe's `ldd` output was checked for HoloNight linkage (none).
With patchelf already on PATH, the explicit CMake tool override is unnecessary.

Human-operated Hyprland/Sway rendering, editing/navigation/scrolling and activation propagation, real-application
and named-scheme observations, isolated authentication prompt/cancellation, and the final ecosystem matrix remain
UQC-201 gates. Provider completion does not claim any of those gates passed. The next assignment is UQC-103 settings,
starting with its repository-local SDD after the verified provider is published and pinned by the umbrella.

Final results (2026-09-08, Qt 6.11.2): full build including both examples passed; all ten isolated cases and the
separate Qt-default reference passed; four focused composite/DPR entries passed (5.40 seconds); nineteen focused
installed/startup/palette/composite/policy entries passed (44.00 seconds); all 61 provider CTest entries passed
(43.70 seconds). Changed C++ formatting, Python/shell syntax, local documentation links and whitespace passed.
UQC-101 provider acceptance is complete. The umbrella handoff records canonical publication and the authoritative gitlink.

## UQC-107 — clean dependency build repair (2026-09-08)

Assigned baseline: `50c59558bb3817f57a992dd72730dba141db1bc8`; configuration remains
`fe69a59e6b73167fd5349223a4d265d75386c139`. AI CI run `34251906896` fails in both jobs:
`qml/quickstyleplugin.cpp:13:10: fatal error: holonight/config/config.h: No such file or directory`.
The style plugin directly consumes configuration headers, while its private transitive dependency does not
export their include path. Host-installed headers masked this in earlier local acceptance.

Add direct private `HoloNight::Config` linkage to `holonight_qml`. CI stages the pinned configuration in
`build/dependencies/prefix`, rejects configuration headers in system include roots, and builds Release with
provider tests and both examples enabled. No consumer include workaround or public contract changes.
Historical UQC-101 local results remain unchanged.

Verification (Qt 6.11.2): fresh Release build and all 59 enabled CTest entries passed (37.77 seconds),
including installed consumers, isolated negative fixtures and both examples in all startup modes.
Qt 5 probes were not enabled locally; CI retains them. Format check passes; focused quickstyleplugin clang-tidy
completes with existing naming, enum-size and brace warnings. Workflow YAML/shell syntax and whitespace pass.
The first focused installed run overlapped the full suite and failed because both recreated the same staging
prefix; the complete suite subsequently passed. Do not run two CTest invocations sharing that staging directory.

Commands from the umbrella root (logs in `/tmp/uqc107-*.log`):

```sh
cmake -S holonight-config -B holonight-qt/build-uqc107/dependencies/config -G Ninja -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF
cmake --build holonight-qt/build-uqc107/dependencies/config -j 6
cmake --install holonight-qt/build-uqc107/dependencies/config --prefix "$PWD/holonight-qt/build-uqc107/dependencies/prefix"
bwrap --bind / / --tmpfs /usr/include/holonight/config --dev /dev --proc /proc cmake -S holonight-qt -B holonight-qt/build-uqc107 -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="$PWD/holonight-qt/build-uqc107/dependencies/prefix" -DBUILD_TESTS=ON -DBUILD_DEMO=ON -DBUILD_CONTROLS_GALLERY=ON -DHOLONIGHT_PATCHELF_EXECUTABLE=/tmp/uqc-uv-cache/archive-v0/NBwrEcQZIC2EIFJPI2MRH/bin/patchelf
bwrap --bind / / --tmpfs /usr/include/holonight/config --dev /dev --proc /proc cmake --build holonight-qt/build-uqc107 -j 6
QT_QPA_PLATFORM=offscreen ctest --test-dir holonight-qt/build-uqc107 --output-on-failure -j 4
cmake --build holonight-qt/build-uqc107 --target format-check
cmake -P holonight-qt/build-uqc107/strip_tidy_flags.cmake
# From holonight-qt, so the repository clang-tidy configuration is selected:
run-clang-tidy -quiet -j 4 -p build-uqc107/tidy 'qml/quickstyleplugin.cpp$'
```

The host configuration headers were masked only inside a private mount namespace; no host files were modified.
Publication is recorded by the umbrella after canonical remote confirmation.

### Private Qt installation follow-up

Remote CI `34258367972` passed clean Release compilation, then its isolated probe exited 2 loading the copied
Qt Controls plugin. Unlike system Qt, the CI Qt installation needs an explicit native library search directory
after copied plugins lose their original relative RPATH context. Derive that directory from configured Qt6::Core
and pass it alongside the staged configuration library; retain exact filtered QML roots and origin checks.
The probe now reports loader errors before exiting. Focused installed acceptance (including ten isolated cases)
and rebuild pass locally; Python syntax, changed C++ formatting and whitespace pass. Remote confirmation pending.

Remote follow-up `34260895278` confirms the loader correction: all style cases reach their intended outcomes.
The Qt-default reference then falsely classified Qt libraries as HoloNight because CI installs Qt beneath a
`holonight-qt` parent directory. Match the library basename, consistently with the other origin assertions.
The isolated matrix passes locally after this correction; remote confirmation remains required.

## UQC-109: Greeter empty-icon-role compatibility — 2026-09-09

User authorized this scoped provider correction from canonical baseline
`478ef7c40a22c7c3f7ea6f45d9205411b5504834`. Umbrella assignment `22f7298`
was published before the fix. Configuration remains unchanged at `fe69a59e`.

HnIconComboBox now checks that iconRole is nonempty before looking up a delegate
model role. Qt maps the empty role to the entire QVariantMap row; that bypassed
the existing roleValue guard and emitted `Unable to assign QVariantMap to QUrl`.
The runtime regression reproduces the error before the fix and verifies text-only
rows and nonempty icon-role selection afterward. Two palette references in the
same QML file are explicitly qualified to pass zero-warning lint.

Verification from holonight-qt (Qt 6.11.2):

```sh
cmake --build build-uqc107 -j4
ctest --test-dir build-uqc107 -R runtime_composites --output-on-failure
QT_SCALE_FACTOR=1.25 ctest --test-dir build-uqc107 -R runtime_composites --output-on-failure
ctest --test-dir build-uqc107 --output-on-failure -j4
cmake --build build-uqc107 --target format-check
/usr/lib/qt6/bin/qmllint --bare --max-warnings 0 -I build-uqc107/qml -I /usr/lib/qt6/qml qml/controls/HnIconComboBox.qml
run-clang-tidy -quiet -j 4 -p build-uqc107/tidy-uqc109 'tests/test_runtime_composites.cpp$'
reuse lint
git diff --check
```

All 59 CTests pass (36.02 seconds), including installed-provider acceptance.
Both style suites pass at DPR 1 and 1.25. Formatting, QML lint, licensing and
whitespace pass. Focused clang-tidy exits successfully; the pre-existing fixture
has advisory readability/complexity warnings. Analysis uses a temporary compile
database without GCC-only flags. No system installation or desktop interaction.
This provider handoff enables greeter re-verification; it is not UQC-201 integration.

Implementation `7ee28b1a9a4b87e8508428b6d6e0e4b89d9d460c` is published and
confirmed by canonical origin/main. CI `34288292334` and licensing `34288292339`
were running at publication; final remote results are recorded in the umbrella
UQC-109 handoff. Greeter must use the published provider gitlink as prerequisite.
