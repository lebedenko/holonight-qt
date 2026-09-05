# Unified Qt Quick Controls — discovery task ledger

Assignment baseline: `dd8eef45f92baf77efe6680e4ccaaa40b3c20030`, published `origin/main` confirmed 2026-09-05.
Only UQC-001 is assigned. No product implementation changes belong to this handoff.

| Task | State | Result / remaining work |
|---|---|---|
| UQC-001-A provider dependencies | Done | DESIGN and INVENTORY enumerate 18 templates, Core/composite edges, fixed-style imports and Fusion property blockers. |
| UQC-001-B consumer inventory | Done | Five repositories inspected; executable/resource ownership, all tracked QML imports, attached properties/enums, policy scripts and settings test contradiction recorded. |
| UQC-001-C real applications | In Progress | Installed versions, binary imports, Haruna/agent startup sources and attempted launch conditions recorded. Desktop probes timed out before QML trace output; isolated authentication still required. |
| UQC-001-D coverage proposal | In Progress | Source-evidenced ApplicationWindow/Label gaps and application-owned agent surfaces recorded. Exact reachable desktop control additions await successful probes/manual observations. |
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

Collect module trace evidence while the user operates logged-out Haruna, NeoChat and Tokodon surfaces. Diagnose the
isolated offscreen startup stall independently if useful. Obtain an isolated desktop/authentication session for the
surveyed Qt agent, never the active desktop's authorization service. Record every required visible control and its
actual implementation; complete C/D, publish the final audit, then review UQC-002 before any implementation.
