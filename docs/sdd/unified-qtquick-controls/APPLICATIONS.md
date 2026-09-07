# Installed-application evidence

## Manual observations — 2026-09-07 (Europe/Kyiv)

Provider checkout `4d63b29c708a8876daa30b738f91aef68800d3de` rebuilt and installed to
`/tmp/uqc-20260907-prefix`; standalone collection fixture passed default, Fusion and Haruna Fusion-fallback
modes. These observations continue discovery and do not accept implementation scope.

- Haruna: user reports no rendering or interaction issues after the requested playback, seek/volume, menu and
  settings check. `/tmp/uqc-audit-fh2hcji_/metadata.json` records manual mode and exit 0. Its trace loads the staged
  provider and resolves ToolButton/ApplicationWindow to Fusion. Visual success does not imply all controls are
  HoloNight implementations.
- NeoChat: user reached Appearance and General settings and reports most controls render correctly. Checkbox and
  radio labels overlap indicators; resizing does not resolve it. Theme switching, especially dark/light transitions,
  leaves inconsistent colors. No other issues reported. `/tmp/uqc-audit-m1kndzv4/launch.log` confirms staged platform,
  Widgets and Quick style loading; the completed manual run records exit status 0. Supplied screenshots show
  Appearance with light surfaces/dark controls and General with indicator/label overlap. Session-local screenshot
  references: `/tmp/codex-clipboard-B1UivN.png` and `/tmp/codex-clipboard-Jd94UV.png`; these and raw logs are temporary,
  while this observation summary is durable. Exact theme transition sequence and scale were not recorded.

### Checkbox/radio layout finding

Installed Kirigami Addons `FormCheckDelegate.qml` and `FormRadioDelegate.qml` embed standard controls with
`contentItem: null`, zero padding and zero spacing, alongside separate labels. Provider `qml/CheckBox.qml` and
`qml/RadioButton.qml` compute implicit dimensions only from background and content, omitting the 16-pixel indicator.
With the content removed and no background, their implicit dimensions collapse, explaining why the adjacent label
can overlap the still-visible indicator. Treat this as a provider compatibility defect for scope review. The required
regression should exercise indicator-only controls in a layout with adjacent labels, including height and mirrored
layout; wider windows alone cannot repair the missing size reservation. No product fix is included in discovery.

### Theme-switching investigation

`qml/holoniightpalette.cpp` resolves tokens from the appearance reader and responds to that reader's palette changes;
CheckBox/RadioButton colors bind to these tokens. The screenshot demonstrates divergent colors after using NeoChat's
selector. Separate application/Kirigami and HoloNight palette sources are a plausible mechanism, not yet a complete
runtime diagnosis. Compare initial launch and dark-to-light/light-to-dark transitions with explicit selected schemes,
record both palette sources, and separate application-owned colors from provider colors before assigning the fix.
Do not count dynamic application theme switching as verified.

Tokodon observations are recorded below; isolated authentication remains pending. NeoChat's report does not establish individual
login/server-form or keyboard-navigation checks beyond the surfaces explicitly reported.

## Current checkpoint — 2026-09-06 (Europe/Kyiv)

UQC-001 remains In Progress. Baselines match umbrella `a7329dd8c0bc7819e2da39fcfb9a302a0b9a0cdf` and provider
`063a752b8a391e7e5212bdad884b5f4525bfed3e`; no provider implementation changed. The application and Qt packages
remain at the versions below. Reuse the dependency inventory. New durable [evidence](audit/EVIDENCE.md) includes
executable hashes, timestamps, selected environment, staged-library loading and surface/type URL pairs.
The [launcher and manual checklist](audit/CHECKLIST.md) provide reproduction and separate-session prerequisites.

### Startup diagnosis and collection fixture

Haruna's 12-second ordinary-bus and isolated-bus GDB runs both loaded application QML and the staged provider,
then reached QCoreApplication::exec. Exit 124 means the bounded run ended while the event loop was running.
There is no demonstrated failing initialization stage in these corrected runs. Earlier empty stderr was an
insufficient diagnostic: explicit QT_FORCE_STDERR_LOGGING and logging rules expose module output. Initial GDB
attempts spent time downloading symbols; corrected runs disable debuginfod. Do not repeat longer timeouts.
This does not retrospectively explain every earlier portal warning or establish successful compositor rendering.

NeoChat and Tokodon also load staged HoloNight/Core libraries and welcome QML on isolated 12-second runs (exit 124).
All three are offscreen observations without human input. Type resolution can include preloaded components; it
must not be reported as visual reachability, instantiated state coverage or interaction success.

A separate consumer built only against system Qt uses the installed provider QML prefix. Three fresh processes
pass actual URL/library assertions: embedded Holonight default; explicit Fusion; and Holonight with programmatic
Fusion fallback. Button/TextField resolve to staged HoloNight in the first/third cases; ApplicationWindow, Label,
ToolButton and Dialog resolve to Basic in the first, Fusion in the other two. Explicit Fusion loads no HoloNight
library. This fixture validates collection, not application evidence or missing-module negative isolation.

### Surface classifications

Each origin below has a concrete type-resolution reference in [EVIDENCE.md](audit/EVIDENCE.md). “Blocked” identifies
missing observation or a competing requested style; it does not mean the application cannot start.

| Application surface | Resolved implementation / ownership | Observed state and remaining evidence |
|---|---|---|
| Haruna Header/Footer toolbar | Fusion ToolBar/ToolButton; Header ToolSeparator also Fusion | Loading observed; blocked as HoloNight coverage by requested Fusion fallback. Manual hover, disabled actions and layout pending. |
| Haruna seek and volume | HoloNight Slider base; application-owned hidden handles and Rectangle backgrounds in HProgressBar/VolumeSlider | Loading plus versioned source evidence; manual media/seek/volume behavior and painting pending. Do not count the painted track as HoloNight. |
| Haruna hamburger/menu surfaces | HoloNight Menu/MenuItem; Fusion MenuSeparator/Popup/ToolButton | Loading observed; opening, selection, geometry and cancellation pending. |
| Haruna settings | HoloNight ItemDelegate resolves in SettingsWindow | Loading only; individual settings pages, editing and scrolling not yet observed. |
| Haruna main window/labels | Fusion ApplicationWindow and Label | Loading observed; exact window/label visual requirements remain candidates. |
| NeoChat welcome | Basic Label/ToolButton/Control, HoloNight ToolTip | Loading observed; labels/buttons may belong to conditional account delegates; manual visibility/states pending. |
| NeoChat server/login | FormCard.FormTextFieldDelegate in versioned Homeserver source | Blocked: no manually reached server form/type-origin pair. FormCard painting cannot be assumed to match its base control. |
| NeoChat navigation/dialogs | Kirigami/FormCard and application composition | Blocked: no user-operated navigation/dialog observations. |
| Tokodon welcome | Basic Label/AbstractButton; application-owned AbstractButton content; FormCard actions | Loading and versioned source observed; manual welcome/onboarding states pending. |
| Tokodon server/account forms | Kirigami.SearchField and Addons RoundedItemDelegate; explicit Rectangle header background | Source only; blocked pending reachable form origins/editing. Header painting is application-owned. |
| Tokodon scrolling/dialogs | Kirigami/Addons and application composition | Blocked: no user-operated scroll/dialog observations. |
| hyprpolkitagent field/buttons | Source candidates TextField/Button, implemented by provider | Blocked: separate user login and confirmed exclusive authentication registration missing. |
| hyprpolkitagent window/labels | ApplicationWindow/Label candidates; fixture Basic does not establish agent fallback | Blocked: prompt not launched. Explicit label overrides remain application-owned. |
| hyprpolkitagent separators/error/geometry | Application-owned Rectangle, explicit error styling and fixed constraints | Versioned source only; prompt presentation and cancellation pending. |

Matching NeoChat/Tokodon startup and welcome/server QML were retrieved at v26.08.0, with URLs and hashes in
[EVIDENCE.md](audit/EVIDENCE.md). NeoChat calls KirigamiAppDefaults::apply after QApplication; Tokodon uses
KirigamiApp::App/KirigamiApp. Neither inspected main.cpp directly calls QQuickStyle::setStyle. Helper policy is
not inferred from that absence; the corrected runtime traces establish selected origins for the sampled run.
The earlier NeoChat URL failed because main.cpp is under src/app. Source inspection explains possible composition,
not unobserved rendering. Haruna's versioned seek/volume source explicitly replaces standard visual hooks.

### Exact provisional coverage proposal — not accepted

Recommend reviewing these **nine** missing standard implementations first: **ApplicationWindow, Label, ToolButton,
ToolBar, ToolSeparator, MenuSeparator, Popup, MenuBar and MenuBarItem**. Haruna resolves the first eight from Fusion
(MenuBarLoader resolves MenuBar); MenuBarItem is its standard dependent implementation. ApplicationWindow and Label
remain candidates until their default appearance is manually assessed. Require a surface/state evidence pair for
each accepted addition; reject neither Basic nor Fusion fallback merely from a palette screenshot.

Hold **Dialog, DialogButtonBox, BusyIndicator, Page, Pane, Frame, RoundButton and RadioDelegate** for reachability
review. Dialog's fallback is verified in the fixture only; Haruna preloads RadioDelegate in HProgressBar, which does
not prove its popup was shown. The earlier binary-only ContextMenu reference is not an accepted standard-control
requirement. Nonvisual Action/ActionGroup/ButtonGroup and abstract Control/AbstractButton are not proposed visual
implementations by themselves. Extend this exact provisional list only after the remaining manual surfaces identify
additional visible standard gaps. The complete required coverage proposal remains open.

Respect application-owned painting and Haruna's explicit Fusion fallback. Its fallback is a concrete exception to
the initiative's Basic assumption, requiring joint review; do not silently force Basic or count Fusion as HoloNight.
Preserve all public composite APIs using the recommendations in [DESIGN.md](DESIGN.md).

### Outstanding prerequisites and stopping point

- User-operated Hyprland surface/state evidence: editing, navigation, scrolling, popup geometry and visual states on
  all reachable required pages. No signed-in profiles required; record network/account prerequisites if encountered.
- Separate graphical login with confirmed logind identity and no competing agent, plus a harmless cancellable
  authorization challenge. Only active user session 4 exists; the existing HoloNight agent remains untouched.
- Final surface-by-surface addition decisions after these observations. UQC-001 C/D cannot be Done yet.

Publish this as a partial discovery handoff and retain Draft/UQC-002 Planned. Full Hyprland/Sway activation and
integration acceptance belong to UQC-201. No product source or public API changes are included.

## Historical first-pass record — 2026-09-05

The following preserves the original probe results. Its timeout and missing-source limitations are superseded by
the corrected loading/source evidence above; its unperformed visual/authentication checks remain open.

## Environment and reproducibility

`pacman -Q haruna neochat tokodon hyprpolkitagent qt6-base qt6-declarative kirigami qqc2-desktop-style` reports:

| Package | Installed version |
|---|---|
| haruna | 1.8.1-2 |
| neochat | 26.08.0-1 |
| tokodon | 26.08.0-1 |
| hyprpolkitagent | 0.1.3-10 |
| qt6-base | 6.11.2-3 |
| qt6-declarative | 6.11.2-1 |
| kirigami | 6.29.0-1 |
| qqc2-desktop-style | 6.29.0-2 |

`pacman -Ql` identifies `/usr/bin/{haruna,neochat,tokodon}` and
`/usr/lib/hyprpolkitagent/hyprpolkitagent`; ldd confirms Qt6 libraries for the surveyed agent.
The provider used for attempted probes was staged from the published audit baseline into a temporary prefix.
Package versions are an observation snapshot, not a minimum-supported-version contract.

Three desktop probes used separate empty temporary XDG_CONFIG_HOME, XDG_DATA_HOME, XDG_CACHE_HOME and XDG_STATE_HOME
profiles, offscreen Qt/software Quick rendering, explicit Quick style Holonight and QML_IMPORT_TRACE=1. They supplied
the installed temporary prefix's QML path, not source-tree paths. No credentials were supplied and no manual input
was automated. No screenshots or successful rendering observations were obtained.

The first 12-second launches on private standard session buses stalled in portal startup, including unavailable
`org.freedesktop.impl.portal.desktop.holonight`. A private bus with no activation services was then tested:

```xml
<busconfig>
  <type>session</type>
  <listen>unix:tmpdir=/tmp</listen>
  <policy context="default">
    <allow user="*"/>
    <allow send_destination="*"/>
    <allow receive_sender="*"/>
    <allow own="*"/>
  </policy>
</busconfig>
```

`dbus-run-session --config-file=<bus.conf> -- dbus-send --session --print-reply
--dest=org.freedesktop.DBus / org.freedesktop.DBus.ListNames` passed. App launches on that bus timed out at 15 and
55 seconds without QML import output. A repeat selecting a generic platform theme and Widgets Fusion also produced
no trace. Haruna `--version` with the generic platform theme succeeded. A limited connect/poll syscall trace of Haruna also observed graphics-socket connections despite its offscreen
setting; no pointer/focus input was sent. This is not a pure installed-consumer isolation test. The precise app startup blocker remains
undiagnosed; neither a missing HoloNight module nor successful selection can be inferred from these results.
Raw session-local logs are `/tmp/uqc-app-audit/<app>/{launch,isolated,generic}.log`; the durable result is this summary.

For a repeat, create fresh empty XDG profile directories, export the variables above, unset QML2_IMPORT_PATH,
QT_QUICK_CONTROLS_CONF and QT_QUICK_CONTROLS_FALLBACK_STYLE, and launch:

```sh
timeout 55s dbus-run-session --config-file=<bus.conf> -- <application>
```

For actual compositor evidence, the user must open logged-out surfaces with QML_IMPORT_TRACE=1 and capture the
resolved control URLs together with screenshots and interaction outcomes. Do not reuse a signed-in profile for
this audit. Both Hyprland and Sway matrices remain unexecuted.

## Application findings and coverage

The following classifications are source-based expectations unless explicitly stated. A blocked row is not a claim
that the application cannot be themed; it identifies why the required observation is unavailable in this audit.

| Target surface | Current classification / evidence | Required follow-up |
|---|---|---|
| Haruna main media controls and settings | Blocked: no QML loading evidence before probe timeout. Installed binary contains runtime Controls and Kirigami imports. Versioned startup honors nonempty Quick style environment. | Capture playback toolbar, seek/volume, menus and settings on a logged-out/no-account profile; inspect actual URLs and fallback origin. |
| NeoChat welcome/server/account forms, navigation and dialogs | Blocked: no QML loading evidence before probe timeout. Installed binary contains runtime Controls, Kirigami and Kirigami Addons imports. | Confirm startup style policy and collect welcome/server form plus drawer/dialog traces without credentials. |
| Tokodon onboarding/account forms and scrolling | Blocked: same timeout; binary contains runtime Controls, Kirigami and Addons imports. | Confirm startup style policy and collect onboarding forms, scrollable areas and dialogs without credentials. |
| hyprpolkitagent password field and action buttons | HoloNight candidate: standard TextField and Button exist in provider and versioned QML uses runtime Controls. Runtime status blocked by absence of isolated authentication session. | In an isolated desktop session, request/cancel a synthetic authorization and inspect actual module origins. Never register a second agent in the active session. |
| hyprpolkitagent window and labels | Basic fallback expected: ApplicationWindow and Label are absent from provider. QML explicitly supplies some label colors/fonts. | Decide whether standard default window/label gaps must be implemented; account for application overrides. |
| hyprpolkitagent separators, error label styling and fixed window geometry | Application-owned: inline Rectangle separators, explicit error color and size constraints in versioned QML. | Record as application-owned; do not promise replacement via a style module. |

Versioned primary sources:

- [Haruna main.cpp](https://github.com/KDE/haruna/blob/v1.8.1/src/main.cpp): conditionally selects KDE desktop style
  only with an empty style environment, and explicitly requests Fusion fallback. Observe whether provider-declared
  Basic fallback remains effective; do not assume this from the environment.
- [hyprpolkitagent Agent.cpp](https://github.com/hyprwm/hyprpolkitagent/blob/v0.1.3/src/core/Agent.cpp): selects its own
  style only with empty Quick style environment; creates prompt after a real in-progress agent session.
- [hyprpolkitagent main.qml](https://github.com/hyprwm/hyprpolkitagent/blob/v0.1.3/qml/main.qml): evidence for the three
  authentication rows above. Source inspection is not a live authentication/rendering result.

Installed-binary strings additionally identify NeoChat WelcomePage.qml, Login.qml, LoginRegister.qml and
NeoChatSettingsView.qml, and references to QQC2 ToolButton, ToolBar, BusyIndicator, Dialog, DialogButtonBox,
RoundButton and ContextMenu. Tokodon's binary also references these types except ContextMenu. These are candidate
coverage gaps beyond the provider's 18 controls, not evidence that a logged-out surface instantiated them.

NeoChat/Tokodon versioned startup source could not be retrieved through the browser in this session. Binary import
strings establish toolkit/import usage only; they cannot establish reachable controls or style selection precedence.

## Proposed additions and acceptance limitations

`ApplicationWindow` and `Label` are the two exact source-evidenced standard gaps in the authentication matrix.
Propose implementing them if this agent's default window/labels are required HoloNight surfaces. Explicit label
colors remain application-owned. Preserve HnApplicationWindow separately; it is not the standard style control.

Do not accept a larger exact control list from toolkit membership alone. ToolButton, ToolBar, Dialog, DialogButtonBox,
Popup, Frame, BusyIndicator, Page, Pane and menu separators are candidates to inspect in the three desktop apps,
not approved requirements. Their reachability and current implementation must be recorded first. Additional gaps
found on required visible standard surfaces must be implemented or explicitly resolved at UQC-002.

Required outstanding evidence: successful module-loading traces for all four applications; exact reachable desktop
control list; rendering states; user-operated editing/navigation/scrolling/popups; isolated third-party authentication.
Until then the complete coverage proposal and UQC-001 exit gate remain incomplete. Do not mark UQC-002 Accepted.

## Tokodon manual observations — 2026-09-07

User reports three issues on reachable settings/about surfaces and no others:

- Network Proxy radio labels overlap their indicators, matching NeoChat's FormRadioDelegate finding.
- Appearance switches extend beyond the right edge of the card. Provider `qml/Switch.qml` has the same
  indicator-omitting implicit-size calculation as CheckBox/RadioButton. Installed `FormSwitchDelegate.qml`
  sets the embedded Switch content to null and all padding to zero. This supplies a concrete source explanation
  for the layout reserving insufficient space for the visible track. Include Switch in the indicator-only layout
  regression proposal, covering right-edge containment and its supported size roles.
- About Tokodon license popup text reaches/clips at the right edge, reported as missing right padding.
  The trace resolves AboutPage's QQC2.ScrollView to `qrc:/qt/qml/Holonight/ScrollView.qml`.
  Installed Kirigami Addons AboutPage explicitly sets the license MessageDialog's four paddings to zero and
  places a SelectableLabel with a grid-unit textMargin inside that ScrollView. Therefore adding blanket popup
  padding is not yet a justified fix: investigate label width/wrapping, horizontal scrolling and viewport geometry,
  comparing the same composition under Fusion. Root cause and ownership remain unresolved.

Completed manual run: `/tmp/uqc-audit-8u37qiwb/metadata.json`, exit status 0. Raw trace is `launch.log` in that
folder. User screenshots (temporary): `/tmp/codex-clipboard-dQJbQS.png` (radio),
`/tmp/codex-clipboard-W9kNGC.png` (switch), `/tmp/codex-clipboard-1TYrGp.png` (license).
This durable summary records the observations; temporary screenshots/logs may disappear on reboot. Exact scale
and individual onboarding/server-field/keyboard checks were not reported. These findings establish reachable
settings and license-dialog surfaces, not complete application-state coverage. Discovery and scope review remain open.

## Isolated authentication observations — 2026-09-07

Tux run `/home/tux/uqc-auth-evidence/20260907T111246Z` records logind session 5, surveyed agent 0.1.3-10,
and provider `/var/tmp/uqc-auth-test-20260907/prefix`. `registration.json` records request serial 9, authority
`:1.24`, agent connection `:1.3644`; the verifier completed request/reply and live ownership checks.
`agent.json` records PID 114381 and final exit -15 after stopping the directly launched child.

User's observations.txt reports prompt appearance, correct theme/colors, field masking/focus, Tab/Shift+Tab,
buttons/hover/cancellation and unclipped geometry; no screenshot or other issue. Log lines 16356/16483 load the
staged Quick/Core libraries. Prompt main.qml resolves Button/TextField to HoloNight (16450/16535) and
ApplicationWindow/Label to Basic (16409/16571), pairing the actual observed prompt with implementation origins.

Outcome correlation needs clarification: `challenge.txt` contains exit_status=0, indicating successful command
execution rather than cancelled authorization. The agent log contains two redacted authentication-submit outcomes
(lines 26116/29571) and a later `fail` outcome (40953), each following prompt creation. This supports multiple prompt
interactions, including a later negative outcome, but does not identify the command exit associated with each one.
Do not overwrite the successful visual/loading evidence or mark command-level cancellation verified from exit 0.
User clarification requested; cancellation acceptance remains open pending correlation or a separately recorded
cancel-only challenge. No password values are included in the evidence summary.

### Authentication outcome clarification

User confirms multiple attempts: successful authentication returned 0, followed by another invocation explicitly
cancelled with a nonzero exit. This explains the saved successful exit and the later agent `fail` result. Accept
prompt appearance/interaction and cancellation based on the user-operated observation plus the loaded-control
trace; the final cancelled command's exact numeric status was not retained. Authentication discovery is complete,
not final UQC-201 ecosystem acceptance. No repeat is required for this discovery gate.

## Controlled follow-up — 2026-09-07

The standalone layout/palette probe and six-mode runner are in `audit/layout-probe.cpp`, `layout-probe.qml`, and
`check-layout.py`. They use system Qt/Kirigami and the installed provider at `/var/tmp/uqc-auth-test-20260907/prefix`.
All six headless runs passed their characterization assertions. These assertions describe the current defects and
comparison cases; they must not become regression acceptance assertions preserving broken behavior.

| Reduced composition | HoloNight | Fusion | Basic |
|---|---|---|---|
| Indicator-only CheckBox/RadioButton width × height | 0 × 0 | 0 × 14 | 0 × 28 |
| Indicator-only Switch width × height | 0 × 0 | 0 × 16 | 0 × 28 |
| Visible indicator widths: checkbox/radio/switch | 16 / 16 / 34 | 14 / 14 / 40 | 28 / 28 / 56 |
| Unconstrained 400-pixel ScrollView: label width / lines | 19852.25 / 1 | 19852.25 / 1 | 19852.25 / 1 |
| Viewport-constrained label and content: width / lines | 400 / 56 | 400 / 56 | 400 / 56 |
| TextField follows application black/white/black palette | No | Yes | Yes |

The initial indicator diagnosis was incomplete: null content and zero padding collapse width under all three
styles. HoloNight additionally omits indicator height and anchors the indicator at the left rather than centering
it like the compared styles. This supports a HoloNight compatibility improvement, not a claim that only HoloNight
can fail this Kirigami composition. Verify the real FormCard delegates after the eventual fix, including trailing
switch containment, mirrored layout, disabled/checked states and size roles.

The popup probe reproduces the license content composition, not the complete application dialog. Its long ordinary
text expands to intrinsic width under every style despite WordWrap. Bounding both label width and ScrollView
contentWidth to availableWidth produces 56 wrapped lines and a full-size horizontal scrollbar (no horizontal
scroll range) under every style. Kirigami's explicit zero dialog padding and unconstrained SelectableLabel explain
a cross-style composition problem; blanket HoloNight popup padding is not justified. Full application comparison
would still be needed before assigning an upstream fix. Do not alter third-party sources within this initiative.

Palette probe: changing QGuiApplication palette black → white → black changes Kirigami text white → black → white.
HoloNight TextField background stays #131a24 and text stays #e7edf5; Fusion/Basic follow the application palette.
The experiment changes only the temporary process palette and does not modify any desktop appearance configuration.
Matching NeoChat v26.08.0 `src/settings/ColorScheme.qml` calls ColorSchemer.apply; `colorschemer.cpp` delegates to
KColorSchemeManager.activateScheme. This establishes the selector path and reproduces separate palette authority;
it does not replay every named NeoChat scheme or establish that every control has the same role mapping.

Sources retrieved 2026-09-07:
- https://github.com/KDE/neochat/blob/v26.08.0/src/settings/ColorScheme.qml
  SHA-256 `f47f623367bb09c91e3957564b12c26e60ced9b48e0a70504b2239ca0a12731f`
- https://github.com/KDE/neochat/blob/v26.08.0/src/settings/colorschemer.cpp
  SHA-256 `21204f5a8fd6c409968dbe144ffe673119d4f6d4a09c21f2b722dc71ee6175b0`

The recommended scope decisions are in [REVIEW.md](REVIEW.md). Earlier provisional ownership assertions are
superseded by these comparison results; earlier unobserved login/server-form cases remain unclaimed.
