# UQC-101 — provider implementation

Status: In Progress. Assigned repository: holonight-qt.
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
| Nine new controls | Planned | — |
| Composites and executable defaults | Planned | — |
| Policy, documentation and installed verification | Planned | — |

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
