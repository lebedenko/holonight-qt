# Unified Qt Quick Controls — audit findings and verification design

Audit date: 2026-09-05. See [INVENTORY.md](INVENTORY.md) for every tracked QML file's relevant imports and usage,
[APPLICATIONS.md](APPLICATIONS.md) for external evidence, and [TASKS.md](TASKS.md) for remaining gates.

## Provider dependency inventory

`qml/CMakeLists.txt` exports exactly 18 standard implementations: Button, CheckBox, ComboBox, ItemDelegate, Menu,
MenuItem, ProgressBar, RadioButton, ScrollBar, ScrollView, Slider, SpinBox, Switch, TabBar, TabButton, TextArea,
TextField and ToolTip. They are template implementations with shared Core palette/metrics; the generated Holonight
qmldir imports Basic. A module name and dark palette do not prove which implementation a surface instantiates.

Core exports HnLabel, HnIcon, metrics, typography/control-size singletons, palette and appearance APIs. HnLabel
currently derives from Basic.Label. Converting that import mechanically to runtime Controls would put a style
consumer inside a dependency of the style. Proposed resolution: preserve the public HnLabel API using an internal
Template Label implementation; verify typography, accessibility and sizing before replacing its Basic dependency.
Core must not import Holonight or Holonight.Controls. HnApplicationWindow is a Window-based composite, not the
standard ApplicationWindow style implementation, and retains its own frame API.

The direct `Holonight as H` composite users are HnSearchField (TextField), HnIconComboBox (ComboBox and ItemDelegate),
HnTextArea (ScrollView and TextArea), HnColorPicker (ScrollView) and HnLoadingState (ProgressBar). Their Basic imports
and the Basic imports in the other composites are enumerated in INVENTORY. HnIconButton, HnSelectableDelegate,
HnSegmentedControl and other template/primitive-owned visuals require API preservation, not blind type substitution.

### Override blockers

| Dependency | Concrete risk | Proposed acceptance resolution |
|---|---|---|
| TextField/TextArea `hasError` | Fusion has no such property; HnTextArea binds it onto H.TextArea; AI credential forms inherit it | Keep error state on the composite/application wrapper; render its frame there using public control hooks. Preserve inherited public HnSearchField error API explicitly. Decide exact implementation at acceptance. |
| HnIconComboBox `delegateHeight`, inherited `maximumVisibleItems` | Custom properties disappear when base changes to Controls.ComboBox | Preserve existing composite API locally; own required geometry through public popup/delegate hooks. Do not silently remove inherited APIs. |
| ItemDelegate corner radii and `popup.background.semanticRadius` | Fusion delegate lacks four radius properties and its background has no semanticRadius contract | Composite-owned delegate/frame calculation; no access to another style's internal background properties. |
| Greeter FooterSelector `delegateHeight: height` | Direct Hn.ComboBox use is coupled to scaled popup behavior | Resolve how owned selector preserves size and placement under runtime selection before greeter migration. |
| Basic attached ScrollBar and direct H.ScrollView | Mixed selected and fixed style controls in HnColorPicker/HnTextArea | Use the same runtime namespace for both instances and attached properties; exercise overflow and wrap modes. |

The existing ComboBox contract is mandatory: popup parent is the window Overlay.overlay, scene mappings refresh on
aboutToShow after ancestor layout, uniform translation/scaling supports 0.78/1.0/1.25 and both edges, bounded scrolling
and keyboard selection remain intact. Rotation, shear and non-uniform scale remain unsupported with diagnostics.
Check actual rendered bounds, not only formula values. Reference: [existing geometry SDD](../combobox-popup-geometry/SPEC.md).

## Owned executable/resource inventory

| Repository | Graphical executables and embedded QML | Startup and migration boundary |
|---|---|---|
| Qt | holonight_demo; holonight_controls_gallery | Both main.cpp files call setStyle(Holonight); replace with embedded defaults in UQC-101. Theme-generator/probe binaries are command-line/test tools. |
| Shell | holonight-shell (`/HolonightShell`); holonight-polkit-agent and holonight-askpass (`/qt-project.org/imports/Holonight/Authentication`) | Authentication module is linked as static whole archive; embed config in each executable, including askpass test variant. sudo/ssh askpass names are launch modes/links, not independent QML modules. No settings executable exists in current shell checkout. |
| Settings | holonight-settings (`/qt/qml/HolonightSettings`, assets `/HolonightSettings`) | No imperative Quick style selection; AudioPage already uses runtime alias. AppearancePage imports Basic as QQC2. |
| AI | holonight-chat (`/HolonightChat`) | Workspace, quick panel and settings surfaces share one executable and config; direct H imports and unqualified controls remain. |
| Packages | holonight-packages (`/HolonightPackages`) | Direct style imports and attached scrollbars must migrate; independent launch cannot rely on shell variables. |
| Greeter | holonight-greeter (`/qt/qml/Holonight/Greeter`) | LoginPanel/Main use runtime unqualified imports; FooterSelector uses direct Hn style. holonight-greeter-session is a non-graphical session helper. Default must work before desktop environment exists. |

No tracked owned resource embeds qtquickcontrols2.conf. No consumer startup setStyle call was found. Relevant
CMake registration files are apps/*/CMakeLists.txt, shell qml/Authentication/CMakeLists.txt and greeter CMakeLists.txt.
Config must be at the resource root, not underneath a QML module URI. Verify through QFile at runtime.

### Policy and tests

- Provider `scripts/check-qml-import-policy.sh` permits unqualified Holonight/Controls and knows only the current
  18-type list. Its CMake test fixture must change with the policy; add namespace/attached-property negative cases.
- AI `scripts/check-canonical-qml-imports.sh` rejects only a particular unaliased Basic/direct-style collision;
  align it with runtime imports, its CTest registration and semantic-styling checker.
- Settings `tests/test_acf005_qml_contract.cpp:55` requires Basic as QQC2, contrary to AGENTS.md. Correct this in
  UQC-103 alongside AppearancePage, retaining observable settings behavior tests.
- Shell QML ownership smoke tests, check-qmltypes, architecture checks and authentication harness need aligned
  expectations. Packages' installed-view tests exercise keyboard selection and scroll layout. Greeter lint must
  use staged Core/Controls metadata; retain existing geometry and controller regressions.
- Every import/attached-property occurrence, including test-only Basic fixtures, is recorded in INVENTORY. Test
  exceptions must be fixture-specific. Do not exempt an entire tests directory from the policy.

## Reconciliation of the preserved provider review

Qt documents runtime precedence as setStyle, command line, environment, then resource configuration. Thus the
review's embedded default is overridable, not an unconditional guarantee. Direct competing imports bypass runtime
selection; QQuickStyle::name alone is insufficient. The platform theme and module discovery remain separate.
The declared fallback must be tested against an application's setFallbackStyle(Fusion), especially Haruna.
Source: [Qt style selection](https://doc.qt.io/qt-6/qtquickcontrols-styles.html), accessed 2026-09-05.
The review's earlier screenshot/install observations are historical; this audit does not re-certify them.

## Verification design for acceptance

1. Build/install provider to a fresh temporary prefix. Compile a separate consumer against only that prefix and
   system Qt. Remove source/build QML paths and inherited QML2_IMPORT_PATH. Inspect engine import paths and plugin
   origins; system-installed Holonight must not mask a missing prefix. Use an isolated deployment root for negatives.
2. Each executable embeds the resource; launch fresh processes with STYLE, FALLBACK_STYLE, CONF and command-line
   overrides cleared, then environment Fusion, command-line Fusion, and conflicting environment Holonight plus
   command-line Fusion. Add a fixture with setStyle to prove its higher precedence and one with competing imports.
3. Assert resolved component URL/plugin origin and representative object structure, then render enabled, disabled,
   focus, hover, pressed, checked/selected states. Compare geometry and palette-role pixels with tolerances at
   multiple device ratios; a style-name assertion or arbitrary custom property alone is not rendering evidence.
4. Exercise every composite under Holonight and Fusion. Fail on QML invalid-property/binding-loop warnings. Cover
   text edits, selection, password echo, input-method hints, maximum length, errors, wrapping, scrolling, delegates,
   keyboard navigation and scaled popups. Keep automated input confined to isolated test windows/harnesses.
5. Installed negative fixtures: absent Holonight module, absent plugin/dependency, bad case, explicit Basic/Fusion
   imports, and platform theme enabled without a Quick selector. Missing-module cases must actually exclude the
   system copy, and assert failure diagnostics rather than allowing accidental fallback discovery.
6. Activation: terminal, desktop entry, D-Bus and systemd launches in Hyprland and Sway; capture only relevant
   variables plus actual module origins for the launched process. Current shell scripts export QML_IMPORT_PATH,
   QT_PLUGIN_PATH, platform theme and Quick selector through dbus-update-activation-environment and systemctl.
   The polkit session wrapper's narrower whitelist does not carry QML_IMPORT_PATH/QT_PLUGIN_PATH: test custom-prefix
   authentication discovery and stale user-manager environment. Diagnostics currently report configured variables.
7. User-operated manual matrix: logged-out Haruna media/settings, NeoChat welcome/server forms, Tokodon onboarding,
   settings/scrolling; record app/Qt versions, theme, actual module evidence, screenshots and interaction outcomes.
   Test keyboard traversal, editing, hover/disabled/selected states, scrolling and popup edge/scale placement. Repeat
   on both compositors. Private authentication session only; do not replace or trigger the active desktop agent.

Provider → settings → AI → packages → greeter → shell is the proposed execution sequence. Publish and pin provider
before any consumer implementation. Do not start UQC-201 until every repository package is Done.
