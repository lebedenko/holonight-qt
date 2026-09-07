# Isolated Qt authentication check as tux

Prepared 2026-09-07 for UQC-001 discovery. This procedure does not change either user's normal desktop configuration,
services, passwords or Polkit rules. It tests the surveyed `hyprpolkitagent 0.1.3-10`, not HoloNight's own agent.
The helper refuses other users, resolves its actual login through logind, and requires the same active local tux
session for collection, agent and challenge. The helper verifies the registration request/reply and its current bus/PID/session ownership before any challenge.
Journal output is supplementary; missing registration messages alone do not block this procedure.

## 1. Before leaving this chat

The self-contained kit is at **`/var/tmp/uqc-auth-test-20260907`**. It includes this file, `auth-test.py`, `registration.py`, a minimal
`hyprland.conf`, its `terminal.sh` launcher, and the staged provider in `prefix/`. This directory is outside Andrii's private home so tux can
read it. It survives ordinary reboot, although system temporary-file cleanup may eventually remove it.
Results go to **`/home/tux/uqc-auth-evidence/<UTC timestamp>`**, which persists across reboot.

Write down these two commands, or open these instructions on another device:

```sh
less /var/tmp/uqc-auth-test-20260907/README.md
Hyprland --config /var/tmp/uqc-auth-test-20260907/hyprland.conf
```

Save your work. You can log out normally, or lock your current desktop and leave it running while using a spare
virtual terminal. You do not have to keep the chat visible. Do not reboot unless necessary; if packages change
between preparation and testing, rebuild/recheck the staged provider before testing.

## 2. Enter a real tux login

1. Press **Ctrl+Alt+F3** manually to reach a spare text console. If F3 is occupied, use another spare console.
2. Log in directly as **tux**, using its existing password. Do not use `sudo -u tux`, `su`, SSH, a nested compositor,
   or a private D-Bus session as a substitute for a separate login. If you cannot log in, return to your desktop;
   no password changes are part of this procedure.
3. Do not start tux's normal HoloNight session. Start only the supplied minimal compositor:

   ```sh
   Hyprland --config /var/tmp/uqc-auth-test-20260907/hyprland.conf
   ```

A Kitty terminal running a clean Bash shell should open. **Super+Enter** opens another terminal. **Super+Shift+E** exits this test compositor.
No panel is expected. The configuration starts only the supplied terminal launcher and does not source normal configuration or start an
agent. Existing tux user services may still run; the next step checks for that. A TTY-originated login can remain
`Type=tty` in logind even while the compositor is running; record it honestly rather than changing session metadata.

If startup fails, preserve the visible error and return to Andrii's desktop. Do not stop the main desktop or greeter.
Hyprland's config behavior is documented at https://wiki.hypr.land/0.53.0/Configuring/Keywords/;
`Hyprland --help` on this host confirms `--config` and `--verify-config`.

## 3. Collect and review the login before starting an agent

Inside the new compositor terminal:

```sh
/usr/bin/python3 /var/tmp/uqc-auth-test-20260907/auth-test.py preflight
```

It prints an evidence directory. Set a convenience variable to that **exact printed path** in each terminal you use:

```sh
UQC_RUN=/home/tux/uqc-auth-evidence/REPLACE_WITH_PRINTED_TIMESTAMP
```

Review these files with `less` (press `q` to leave):

```sh
cat "$UQC_RUN/identity.txt"
less "$UQC_RUN/processes.txt"
less "$UQC_RUN/user-services.txt"
less "$UQC_RUN/registration-before.txt"
cat "$UQC_RUN/policy.txt"
```

Required checks:

- Identity says User=1001, Active=yes, Remote=no, and gives the test session ID. The helper gets the identity of its
  actual process from logind, rather than trusting an inherited XDG_SESSION_ID.
- Review authentication-agent processes and their cgroups/session ownership, including `holonight-polkit-agent`,
  `hyprpolkitagent`, KDE/GNOME/LXQt agents and `polkit-gnome-authentication-agent-1`. An agent for Andrii's other
  session is expected and must remain untouched. A process-name list alone cannot prove registration ownership.
- Journal messages are useful if available, but this host's journal only reports Polkit startup. Do not infer
  registration state from missing messages. After process/service review, one registration attempt by the test
  agent is permitted: Polkit rejects an existing registration for the same session without replacing it.
- `policy.txt` must show `org.freedesktop.policykit.exec` requires `auth_admin` for this login, as in the surveyed
  baseline. Do not create a policy override. No prompt can also mean cached authorization or local policy changes.

If processes/services show a known competing agent for tux, stop and preserve the evidence. If the journal is
unavailable or contains only startup messages, continue to step 4 after the other checks. The actual registration
reply, rather than journal absence, will establish whether the test agent acquired the session registration.

## 4. Start only the test agent

In the first terminal, using the evidence directory from step 3:

```sh
/usr/bin/python3 /var/tmp/uqc-auth-test-20260907/auth-test.py agent --run "$UQC_RUN"
```

Type `ISOLATED` only if the step 3 checks passed. The script checks the exact surveyed version and Qt 6 linkage,
sets process-local HoloNight paths, uses empty per-run XDG application directories, and logs into `agent.log`.
It prints the child PID. It does not change systemd/D-Bus activation environment. Keep this terminal running.

In a second terminal (Super+Enter), set `UQC_RUN` again and collect registration evidence:

```sh
journalctl -b -u polkit.service --no-pager > "$UQC_RUN/registration-after.txt" 2>&1
cat "$UQC_RUN/agent.json"
less "$UQC_RUN/registration-after.txt"
less "$UQC_RUN/agent.log"
ps -eo uid,pid,ppid,cgroup,comm > "$UQC_RUN/processes-after.txt"
```

Run the new read-only registration verification in the second terminal:

```sh
/usr/bin/python3 /var/tmp/uqc-auth-test-20260907/auth-test.py registration --run "$UQC_RUN"
```

It must print **PASS**. It parses the agent's own GLib D-Bus trace, matches the registration request's exact session
and endpoint to its successful reply, and checks the current Polkit owner plus the reply recipient's live PID and
logind session. It writes `registration.json`. Merely finding the process or an exported object is insufficient.
If the reply has not arrived yet, wait briefly and repeat this read-only command. If registration fails (including
an existing-agent error), the helper stops before any challenge. Stop only the test agent with Ctrl+C and return
with logs. No service restart, global debug change, bus-wide monitor or sudo journal access is required.

## 5. Show and cancel one harmless challenge

In the second terminal, after confirming registration:

```sh
/usr/bin/python3 /var/tmp/uqc-auth-test-20260907/auth-test.py challenge --run "$UQC_RUN"
```

The challenge step repeats registration verification. Type `REGISTERED` when ready to inspect and cancel. It runs exactly:

```sh
pkexec --disable-internal-agent /usr/bin/true
```

`true` performs no operation if unexpectedly authorized. A missing prompt is not a successful UI check.

When the graphical prompt appears, manually inspect:

- Window fully visible, readable labels and buttons, sensible padding and geometry.
- Password field presentation and focus ring. Optionally type a few disposable characters to check masking,
  then erase them; never type a real password or press the authentication/submit button.
- Tab/Shift+Tab navigation and button hover/focus, without submitting.
- Cancel using the prompt's Cancel button (or Escape if supported).

The helper records challenge output and exit status in `challenge.txt`. Record whether a graphical prompt appeared;
a nonzero exit alone does not prove successful cancellation. Take a screenshot manually if useful, with the field
empty. Save it inside the evidence directory if possible. Do not automate pointer movement or focus.

## 6. Finish and preserve observations

Press Ctrl+C in the **first terminal**. This stops only the helper's directly launched test agent and records its
exit status in `agent.json`. Do not close that terminal before stopping the helper. If the agent has already exited,
preserve its log and status.

In the second terminal:

```sh
journalctl -b -u polkit.service --no-pager > "$UQC_RUN/registration-final.txt" 2>&1
cat > "$UQC_RUN/observations.txt"
```

Type notes in this format, then press Ctrl+D to save:

```text
Prompt appeared: yes/no
Theme and colors:
Field/masking/focus:
Tab and Shift+Tab:
Buttons/hover/cancellation:
Geometry/clipping (configured scale is 1):
Screenshot filename, if any:
Other issues or blocked steps:
```

Press **Super+Shift+E** to exit the test compositor, then `exit` at the text console to log tux out. Switch back to
the original graphical VT (usually Ctrl+Alt+F1 on this host), unlock or log in as Andrii, and return to this chat.
Send the evidence-directory path, for example `/home/tux/uqc-auth-evidence/20260907T150000Z`, and your observations.
Do not change home-directory permissions or make logs public. If reading tux's evidence needs elevated access,
we will request that narrowly when reviewing it. Keep the evidence until the audit is recorded durably.

## Preparation verification and limits

The kit's provider is built from the checkout recorded in PROVIDER.txt. Before handoff, its installed-consumer
fixture is checked against this exact kit location in default/Fusion/Haruna-fallback modes. The Python helper is
syntax-checked, its wrong-user refusal is exercised, and the Hyprland config is checked without starting a desktop.
No live agent or authorization challenge is launched during preparation. Tux login, journal visibility, successful
registration and graphical prompt behavior must still be established during the manual procedure.

## Correction after the first preflight attempt

The saved error was `PID 109683 does not belong to any known session`, before agent startup or evidence-directory
creation. The initial configuration used ordinary Kitty. Installed Kitty's `child.py` calls
`systemd_move_pid_into_new_scope` for its shell: this is a likely explanation for losing logind's per-process session
association. See [Kitty's documented scope behavior](https://sw.kovidgoyal.net/kitty/changelog/).

The revised configuration uses `terminal.sh` for both the first terminal and Super+Enter. It gives only Kitty itself
an unavailable user-bus endpoint, preventing its systemd scope move. It restores the real user-bus address before
starting a clean Bash shell. The agent/challenge keep their normal bus, and the script still requires actual
GetSessionByPID success. No private bus or guessed session ID is used. A Kitty diagnostic about failing to open its
own systemd user bus is expected. Use `/usr/bin/python3` to avoid depending on tux's pyenv setup.

To retry, exit the old test compositor with Super+Shift+E, then restart it with the same `Hyprland --config` command
in step 2. Existing terminal processes cannot be repaired by reloading configuration. Run preflight from the new
terminal. If GetSessionByPID still fails, stop and save the new error; the revised launch path still needs live tux
verification. No authentication challenge has been executed by this correction.

## Registration-evidence correction

The supplied `polkit-journal.txt` and a privileged process-tagged journal query contain only startup messages.
Do not repeat journal collection as a prerequisite. Agent-local `G_DBUS_DEBUG=message` now captures its actual
registration exchange. Keep logs private: they can contain authorization metadata, although this procedure never
submits a password. Do not share whole logs publicly.

[Polkit 127 registration source](https://github.com/polkit-org/polkit/blob/127/src/polkitbackend/polkitbackendinteractiveauthority.c)
checks the caller's session and rejects duplicate registrations for the same subject before adding an agent.
[hyprpolkitagent v0.1.3 startup](https://github.com/hyprwm/hyprpolkitagent/blob/v0.1.3/src/core/Agent.cpp)
registers a UnixSessionSubject derived from its PID. Thus a verified successful reply supplies the missing ownership
evidence without replacing an existing agent. This is scoped to the verified login, not an assertion about all users.

If you already have a successful preflight from the corrected terminal and the same login is still active, keep its
UQC_RUN and resume at step 4. If you logged out or have already attempted agent startup, make a fresh preflight run.
The supplied kit has been updated in place; a compositor restart is only needed if still using the old plain-Kitty
configuration. No actual registration or challenge was performed during preparation.

## Empty run variable versus session mismatch

Shell variables are local to each terminal. An unset `UQC_RUN` previously produced the misleading message
"evidence directory must belong to this tux login". The helper now distinguishes an empty argument, wrong parent
path, unreadable session record and actual session-ID mismatch, showing the relevant paths/IDs.
For the current saved run, use this full-path command in the second test terminal:

```sh
/usr/bin/python3 /var/tmp/uqc-auth-test-20260907/auth-test.py registration --run /home/tux/uqc-auth-evidence/20260907T111246Z
```

That run records session 5. Reuse it only while its original test agent/login remains active. If the new error
reports a different session, return with that message; do not change the stored session ID or start another agent.
