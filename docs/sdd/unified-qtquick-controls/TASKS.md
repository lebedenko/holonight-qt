# Unified Qt Quick Controls — discovery task ledger

Assignment baseline: `dd8eef45f92baf77efe6680e4ccaaa40b3c20030`, published `origin/main` confirmed 2026-09-05.
Continuation baseline: `063a752b8a391e7e5212bdad884b5f4525bfed3e`; umbrella `a7329dd8c0bc7819e2da39fcfb9a302a0b9a0cdf`, confirmed 2026-09-06.
Only UQC-001 is assigned. No product implementation changes belong to this handoff.

| Task | State | Result / remaining work |
|---|---|---|
| UQC-001-A provider dependencies | Done | DESIGN and INVENTORY enumerate 18 templates, Core/composite edges, fixed-style imports and Fusion property blockers. |
| UQC-001-B consumer inventory | Done | Five repositories inspected; executable/resource ownership, all tracked QML imports, attached properties/enums, policy scripts and settings test contradiction recorded. |
| UQC-001-C real applications | Done | Corrected logs establish staged-provider loading in all three desktop applications; Haruna backtraces reach the event loop on ordinary/private buses. Matching NeoChat/Tokodon source retrieved. Subsequent manual observations and isolated authentication are recorded below; unobserved states remain explicit integration gates. |
| UQC-001-D coverage proposal | Done | User approved nine additions, indicator sizing fixes, application palette support and explicit composition limitations on 2026-09-07; see REVIEW. |
| UQC-001-E verification design | Done | Installed-prefix isolation, rendering assertions, override precedence/competing imports, negative fixtures, activation paths and manual matrix specified. |
| UQC-001-F documentation checkpoint | Done | Source inventory, verification design and explicit evidence limitations prepared and checked for publication. UQC-001 remains In Progress until C/D evidence is complete and reviewed. |
| UQC-002 contract review | Planned | Review coverage, property/API compatibility, Core dependency boundary and verification feasibility with the user; initiative stays Draft. |

## Verification performed

- Read tracked source, CMake/resource registration, policy scripts, local instructions and existing SDDs in provider
  and all five consumer repositories. INVENTORY is generated from `git ls-files` at the full recorded baselines.
- Verified canonical publication and checkpointed existing work separately; see umbrella BASELINE.md.
- Provider rebuilt and staged; 28 existing CTest entries passed. Focused avatar loading test passed; software backend
  skips shader masking. No new product tests are appropriate for this documentation-only package.
- Existing consumer baseline tests/builds are recorded in umbrella BASELINE.md. They are not UQC runtime acceptance.
- Queried installed package versions; inspected installed module metadata and agent service/linkage. Three empty-XDG
  offscreen application probes produced timeout/portal evidence, not successful rendering evidence.
- Reviewed official Qt selection/custom-style documentation and versioned Haruna/hyprpolkitagent sources.
- Documentation whitespace, relative links and source inventory consistency checked before the handoff commit.

## Resume prerequisites

Collect module trace evidence while the user operates logged-out Haruna, NeoChat and Tokodon surfaces. Use the corrected stderr launcher; the bounded Haruna backtraces now reach the event loop. Obtain an isolated desktop/authentication session for the
surveyed Qt agent, never the active desktop's authorization service. Record every required visible control and its
actual implementation; complete C/D, publish the final audit, then review UQC-002 before any implementation.

## Continuation verification — 2026-09-06

- Confirmed exact requested checkouts; provider product diff since baseline is empty. Unrelated package-manager
  mockups remain untouched. Requeried application/Qt package versions; reused completed dependency inventory.
- Staged existing verified provider build with `cmake --install build --prefix /tmp/uqc-20260906-prefix`.
- Configured/built the standalone audit consumer; `check-fixture.py` passed all three modes, 18 type-origin checks
  and staged-library checks. The executable has an embedded default; no provider source/build QML path is used.
- Ordinary/private-bus Haruna GDB runs reach the event loop, bounded exit 124; NeoChat/Tokodon empty-profile runs
  record QML loading, exit 124. Forced stderr logging corrects the evidence collection gap.
- Retrieved matching release startup and QML sources, with hashes and source URLs preserved in audit/EVIDENCE.md.
- Read logind identity, running agent and authorization policy without changing services or requesting a challenge.
  Only the active user login is available. Authentication remains unexecuted.
- Checked Python syntax, relative documentation links and whitespace. No unrelated repository suites were rerun;
  this change contains discovery documentation and its collection fixture only.

Stop for joint scope review after publication. C/D remain In Progress and UQC-002 Planned; manual observations and
an isolated authentication login are required to finish UQC-001. See the reproducible checklist for exact next steps.

## Manual continuation — 2026-09-07

Haruna user-operated checks report no issues; NeoChat settings reveal checkbox/radio label overlap and inconsistent
colors after theme changes. APPLICATIONS records evidence paths, the indicator implicit-size source finding, and
the unresolved palette-source investigation. Provider build/staged install and all three fixture modes passed.
UQC-001 C/D remain In Progress; include these compatibility defects in UQC-002 review. Product code is unchanged.

Tokodon manual run also completed with exit 0: radio overlap, switches overflowing cards, and clipped license-popup
text. Source review extends the indicator-size defect to Switch; license ScrollView geometry remains unresolved.
See APPLICATIONS for observations and evidence. No product implementation or acceptance-state change.

## Authentication preparation — 2026-09-07

Added audit/auth-test instructions, minimal Hyprland config and a manual collection helper for existing tux UID 1001.
Published a local self-contained kit at `/var/tmp/uqc-auth-test-20260907` with provider `4d63b29` staged from the current
build. This is a local test kit, not a canonical Git publication. Evidence goes to `/home/tux/uqc-auth-evidence`.
Helper checks actual logind identity and refuses the primary user; journal/registration review remains manual.
Verification: provider build/install, Python AST parsing, expected wrong-user refusal, `Hyprland --verify-config`,
three fixture modes against the final kit path, and `git diff --check` passed. Live tux login and authentication are
unexecuted; no account, existing service, normal desktop configuration or product implementation was changed.

### First tux preflight correction — 2026-09-07

User saved `/home/tux/auth-check/log-auth-test-py.txt`: GetSessionByPID failed before agent launch because the helper
PID had no known logind session. Installed Kitty moves child shells into user systemd scopes; revised test config
uses terminal.sh to deny only Kitty's user-bus connection and restore the real bus to its clean Bash child. Keep
the real process-identity gate; do not substitute an environment session ID. Updated persistent kit and instructions.
Shell/Python syntax, Hyprland parser, whitespace and the installed Kitty scope-function refusal outside the sandbox
passed. No GUI or authentication launched. Actual tux session association remains to be verified after restarting
the test compositor. The helper now explains this failure without a Python traceback.

### Registration evidence correction — 2026-09-07

Saved tux Polkit journal and privileged process-tagged query contain only startup messages. Reviewed Polkit 127's
session matching/duplicate-registration rejection and agent v0.1.3 session-subject startup. Replace the mandatory
journal gate with agent-local GLib registration request/reply capture and live authority/bus-PID/logind correlation.
A known competing tux agent still blocks the attempt; a duplicate reply blocks the challenge without replacement.
Updated kit permits read-only `registration` verification and repeats it immediately before the manual challenge.
No agent or challenge launched. Journal silence is neither failure nor evidence of absence.

Verification: agent-local G_DBUS_DEBUG request/reply format observed with a read-only GetId query; Python syntax,
two accepted registration formats and seven rejection scenarios pass; whitespace passes and copied kit matches
source. Real tux registration verification remains unexecuted.

### Unique-name registration correction — 2026-09-07

The live tux log records RegisterAuthenticationAgent serial 9 targeting authority unique name `:1.24`, session 5,
expected agent endpoint; method-return replies to agent connection `:1.3644`. The initial verifier incorrectly
required the well-known destination name. Accept the well-known name or the exact replying authority unique name;
retain the live authority/PID/session checks and add an explicit Authority object-path check. Observed exchange,
well-known form and eight rejection cases pass. Copied the verifier into the test kit. Registration exchange is
successful; live verifier completion and prompt/cancellation remain pending. No second agent or challenge launched.

### Authentication results review — 2026-09-07

Registration verification completed and test agent stopped (exit -15). User reports correct prompt rendering,
masking/focus/navigation/buttons and geometry; trace pairs observed prompt with staged HoloNight Button/TextField
and Basic window/labels. challenge.txt records exit 0, while the log shows two authentication submissions and a later
negative result; exact cancellation/command correlation needs clarification. APPLICATIONS records the evidence
without marking command-level cancellation passed. Documentation whitespace check passed.

## Controlled compatibility follow-up — 2026-09-07

User clarified final nonzero cancellation after successful authentication attempts; isolated authentication discovery
is complete with the exact cancelled numeric exit unretained. Added standalone installed-provider layout/palette
characterization and six-case runner. All three styles lose indicator-only width; HoloNight additionally loses
height. All three overflow unconstrained license-like content; viewport constraints restore wrapping. HoloNight
TextField ignores application palette changes while Kirigami/Fusion/Basic respond. Retrieved matching NeoChat selector
sources and hashes. APPLICATIONS contains measured results; REVIEW contains proposed scope decisions. UQC-001 remains
In Progress pending coverage/scope review and publication; no product code changed and UQC-002 is not accepted.

## Approved discovery handoff — 2026-09-07

User reviewed and approved REVIEW in full. C/D discovery gates are complete with recorded limitations; this does
not certify unobserved application surfaces or final ecosystem integration. All nine additions and the palette
boundary are accepted, alongside indicator geometry fixes and the DESIGN API-preservation resolutions.
UQC-002 is umbrella-owned; publication and its acceptance checkpoint precede UQC-101 assignment.

Standalone audit configure/build and all six layout/palette characterization runs passed during scope-review
preparation. CMake warned about unavailable Kirigami plugin link targets, but runtime comparison succeeded.
