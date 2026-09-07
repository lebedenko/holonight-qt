# Unified Qt Quick Controls — discovery specification

Status: Discovery reviewed and approved on 2026-09-07; see REVIEW.md. This specification records UQC-001.
The umbrella acceptance checkpoint and UQC-101 implementation SDD govern subsequent product work.

Assignment: `holonight-qt`, published upstream baseline
`dd8eef45f92baf77efe6680e4ccaaa40b3c20030` (`origin/main`, confirmed 2026-09-05).
The baseline contains avatar implementation `2574ea71b02b35517384f4db270d629cbb2c510e` and the
separately preserved provider review. Consumer baselines are recorded in [INVENTORY.md](INVENTORY.md).

## Required discovery outputs

1. Inventory provider templates, Core/composite dependencies, direct-style imports and custom-property coupling.
2. Inventory every production graphical entry point, embedded resources, control imports, attached properties,
   enums, policy scripts and conflicting tests across all five consumer repositories.
3. Inspect the installed Haruna, NeoChat, Tokodon and Qt-based hyprpolkitagent versions. Distinguish source evidence,
   module resolution, rendered states and manual interaction evidence. Do not substitute one for another.
4. Classify surfaces as HoloNight, Basic fallback, application-owned, or concretely blocked. Propose additions with
   surface evidence; do not pre-approve an entire catalog or invent public APIs before acceptance.
5. Design installed-prefix, override, failure, rendering, activation and manual checks for UQC-002 review.

## Contract proposed for review

Owned applications use `import QtQuick.Controls as Controls`, including attached properties and enums, and embed
`:/qtquickcontrols2.conf` with `[Controls]` / `Style=Holonight` before QML loads. Explicit Fusion overrides must load
without invalid-property assignments. Core and composites retain their intentional HoloNight visuals and public APIs.
Basic remains the style fallback. Runtime migration must not introduce Core → selected style → Core recursion.

The environment selects Quick Controls independently of the QPA platform theme. Native dynamically linked Qt 6
applications are in scope; Qt 5, static binaries, Flatpak/AppImage and patching third-party resources are excluded.
Application-owned painting and explicit competing styles are compatibility boundaries, not successful restyling.

## Exit gate

UQC-001 requires reproducible evidence and explicit coverage limitations. Unobserved surfaces remain unverified;
manual interaction is performed by the user, with no automated pointer movement or window focus. Authentication
must be isolated from the active desktop agent. UQC-002 must resolve every scope/override/coverage finding together
with the user before Accepted or provider implementation. A partial audit does not satisfy this gate.
