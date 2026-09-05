# Installed-application evidence — 2026-09-05

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
