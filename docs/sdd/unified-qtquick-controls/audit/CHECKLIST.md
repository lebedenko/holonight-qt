# Reproducible collection checklist

Run from the provider root. No product source changes are needed. Use a new prefix for a new provider revision.
The 2026-09-06 run installed the existing verified build at provider `063a752b8a391e7e5212bdad884b5f4525bfed3e`.
For another checkout, build that checkout before installing. Never infer the build revision from the prefix name.

```sh
cmake --install build --prefix /tmp/uqc-20260906-prefix
cmake -S docs/sdd/unified-qtquick-controls/audit -B /tmp/uqc-discovery-build -G Ninja
cmake --build /tmp/uqc-discovery-build
python3 docs/sdd/unified-qtquick-controls/audit/check-fixture.py /tmp/uqc-20260906-prefix /tmp/uqc-discovery-build/uqc-discovery
```

The fixture embeds a default and asserts six type URLs and loaded library origins in three fresh processes.
It neither displays a window nor validates application rendering. It retains system Qt modules but rejects a
system HoloNight library masking the staged provider in successful runs. Missing-module negative isolation belongs
to UQC-101; this fixture is not that negative test.

## Startup diagnosis

```sh
python3 docs/sdd/unified-qtquick-controls/audit/launch.py --prefix /tmp/uqc-20260906-prefix --mode ordinary --backtrace -- /usr/bin/haruna
python3 docs/sdd/unified-qtquick-controls/audit/launch.py --prefix /tmp/uqc-20260906-prefix --mode isolated --backtrace -- /usr/bin/haruna
```

Both modes create empty XDG config/data/cache/state directories. Ordinary mode keeps the desktop session bus;
isolated mode uses a private bus with no activation services. Both use offscreen/software rendering and a 12-second
limit. Runtime directory and home remain the session's: this protects application profiles but is not a security
sandbox or authentication isolation. Logs include selected environment, executable hash, UTC date and exit status.
Qt stderr routing is forced, debug/info rules are explicit, and GDB debuginfod downloads are disabled. A timeout of
124 is not a startup failure if the backtrace shows the event loop after successful QML loading. Locate the first
failing stage before changing timeout duration. Do not attribute earlier empty stderr logs to a missing module.

## Guided Hyprland observations — user operates the applications

Run one at a time from a terminal inside Hyprland. Close each audited app before starting the next. Do not reuse
an existing instance of that application: single-instance forwarding could bypass the temporary profile.

```sh
python3 docs/sdd/unified-qtquick-controls/audit/launch.py --prefix /tmp/uqc-20260906-prefix --mode manual -- /usr/bin/haruna
python3 docs/sdd/unified-qtquick-controls/audit/launch.py --prefix /tmp/uqc-20260906-prefix --mode manual -- /usr/bin/neochat
python3 docs/sdd/unified-qtquick-controls/audit/launch.py --prefix /tmp/uqc-20260906-prefix --mode manual -- /usr/bin/tokodon
```

Manual mode keeps the compositor/platform settings and has no timeout. Only the user moves the pointer, clicks,
changes focus, types or captures screenshots. Record the printed evidence directory and close normally to capture
exit status. Inspect logs before sharing; publish selected type/library lines and observations, not entire profiles.

| Application | User-operated sequence | Required observation |
|---|---|---|
| Haruna | Open a disposable local media clip; seek and adjust volume; open menus and settings | Toolbar and separators; seek/volume track and handles; menu popup, selection and disabled actions; settings editing/navigation/scrolling |
| NeoChat | Welcome → add account/login → server form; return and open reachable navigation/dialogs | Labels and buttons; edit/select/delete disposable server text; validation/disabled state; Tab traversal, back navigation, scroll and popup behavior |
| Tokodon | Welcome/onboarding → server/account flow; return and open reachable scrolling/dialogs | Labels, search/server text editing, selection, navigation, scrolling, dialogs and disabled/loading states |

Do not sign in or enter secrets. Use `example.invalid` for disposable server text; stop before browser authorization.
If a surface needs network data or an account, record that exact prerequisite. For each surface record: app/package
and Qt versions, local date, state, screenshot reference if available, relevant component and loaded-library URL,
user action and outcome, geometry/scale, and classification (HoloNight, Basic fallback, application-owned, or blocked).
Record Fusion fallback as a concrete compatibility boundary; do not relabel it Basic. Resolution traces include
preloaded/lazy components: pair them with the actual observed page before claiming reachable coverage.

## Separate authentication login — not available in this collection

Observed logind has only user session 4 (uid 1000, Wayland, seat0, tty1, active), plus greeter/manager sessions.
`holonight-polkit-agent` is running. No separate test login is prepared. Do not stop it, alter its service, or run
hyprpolkitagent in session 4. A private D-Bus session does not change the system-bus Polkit subject.

1. The user prepares a separate graphical login, preferably a disposable Unix user with an empty profile and a
   minimal compositor session that does not autostart an authentication agent. The agent and challenge must belong
   to this login. Do not import test variables into `systemctl --user` or D-Bus activation environment.
2. In that session record `loginctl list-sessions` and `loginctl show-session "$XDG_SESSION_ID" -p Id -p User -p Type
   -p Class -p Leader -p Seat -p TTY -p Active`. If XDG_SESSION_ID is absent, resolve the shell PID through logind
   (`org.freedesktop.login1.Manager.GetSessionByPID`) before proceeding; do not assume the displayed seat is identity.
3. Inspect agent processes, their owning logind sessions/cgroups and Polkit registration journal entries. Verify
   that none serves the test session. A process-name list alone does not prove absence of a competing registration.
   If registration ownership cannot be established, leave the task open. Never change the active desktop agent.
4. Confirm `pacman -Q hyprpolkitagent` is the surveyed Qt `0.1.3-10` and its executable links Qt6. Launch
   `/usr/lib/hyprpolkitagent/hyprpolkitagent` directly from the test login with process-local
   `QT_QUICK_CONTROLS_STYLE=Holonight`, the staged `QML_IMPORT_PATH`/`QT_PLUGIN_PATH`, `QML_IMPORT_TRACE=1`,
   `QT_DEBUG_PLUGINS=1`, `QT_FORCE_STDERR_LOGGING=1` and explicit debug/info logging rules. Capture its successful
   registration, PID/session identity and prompt-loading output. This must not use the general launcher, which
   refuses the surveyed authentication executable. Record date, hash and final exit status as for desktop runs.
5. Inspect `pkaction --action-id org.freedesktop.policykit.exec --verbose` in the test session. The surveyed host's
   policy requires `auth_admin` for active/inactive/other subjects. Use a session without cached authorization.
   From another terminal in that same login, the user runs `pkexec --disable-internal-agent /usr/bin/true` and
   manually cancels the graphical challenge. This command performs no change even if unexpectedly authorized;
   an absent prompt does not count as evidence. Do not add a policy rule merely to force a prompt.
6. Without entering/recording any password, record masked-field presentation, focus, labels, buttons, window
   geometry, component/library origins and cancellation exit status. Stop only the directly launched test agent
   using its own terminal after the challenge. No active-session agent/service changes are permitted.

Missing separate login, unverifiable registration ownership, or no suitable challenge keeps authentication evidence
open. Full Hyprland/Sway activation and acceptance remain UQC-201 work.
