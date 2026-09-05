# Unified Qt Quick Controls — discovery task ledger

Assignment baseline: `dd8eef45f92baf77efe6680e4ccaaa40b3c20030`, published `origin/main` confirmed 2026-09-05.
Continuation baseline: `063a752b8a391e7e5212bdad884b5f4525bfed3e`; umbrella `a7329dd8c0bc7819e2da39fcfb9a302a0b9a0cdf`, confirmed 2026-09-06.
Only UQC-001 is assigned. No product implementation changes belong to this handoff.

| Task | State | Result / remaining work |
|---|---|---|
| UQC-001-A provider dependencies | Done | DESIGN and INVENTORY enumerate 18 templates, Core/composite edges, fixed-style imports and Fusion property blockers. |
| UQC-001-B consumer inventory | Done | Five repositories inspected; executable/resource ownership, all tracked QML imports, attached properties/enums, policy scripts and settings test contradiction recorded. |
| UQC-001-C real applications | In Progress | Corrected logs establish staged-provider loading in all three desktop applications; Haruna backtraces reach the event loop on ordinary/private buses. Matching NeoChat/Tokodon source retrieved. Manual surface/state and separate-session authentication evidence remain open; see audit/EVIDENCE.md and audit/CHECKLIST.md. |
| UQC-001-D coverage proposal | In Progress | Nine exact provisional additions proposed, with deferred candidates, observed Basic/Fusion boundaries and application-owned painting. Final required coverage awaits manual reachability/state evidence and authentication; no additions accepted. |
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
