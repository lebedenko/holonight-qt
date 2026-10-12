# External dependency management — holonight-qt

Work package: EDM-006
Audit baseline: `680d709de4d1d8e8697d6a69a99f15ef891ef7b2`
Implementation baseline: `2a6fcb0ef29d20e3413f425914ec1cf171b82306`
Config provider: `0ccd25f323662f50a47bb048be636160f2b7c135`
Umbrella handoff: `522ca7c`; public-component declaration correction: `21a641e`
Umbrella audit: `38146d8f474317f3f8d72699f60e6d16a71dd165`
Status: Locally verified; publication and umbrella pinning require later approval.

## Requirements

- Configuration and normal build commands shall never acquire dependencies or prepare source providers.
- `task deps` shall explicitly prepare supplied local providers and check installed external prerequisites.
- Internal providers shall resolve only from approved prefixes; external packages shall resolve from an explicit
  external prefix or normal system locations.
- Enabled dependency-dependent features shall require their dependencies; minimal profiles shall disable them explicitly.
- Where production components use Qt, discovery shall require >=6.11; private Qt components shall match
  the selected public version exactly.
- Where tests use GTest, consumers shall require installed GTest >=1.14 CONFIG and GTest::gtest. This module does not require GMock.
- Existing public APIs, component switches, install-tree consumers and acceptance coverage shall be preserved.

## Design

Adopt canonical `tooling/modules/holonight-qt.json` and `.cmake`, then synchronize bundle 2.0.0 into this repository.
The copied metadata is owned by this standalone checkout. CMake performs authoritative discovery; shared probes
provide prerequisite diagnostics and record resolved versions/paths. Incompatible caches and local provider install
trees are archived before reuse. Package acquisition occurs only in explicit environment preparation. CI switches
build/runtime/release lanes together to qualified immutable images and runs acceptance offline after preparation.

Add BUILD_SYNTAX_THEMES default ON; require KF6SyntaxHighlighting when enabled. Match all private Qt modules exactly to selected public Qt. Preserve Wayland and Qt5 probe switches.

## Implementation and verification tasks

- [x] Confirm the exact baseline and canonical handoff before implementation.
- [x] Adopt declarations, prefix helpers, explicit preparation and compatibility signatures.
- [x] Apply the repository-specific discovery changes above.
- [x] Update Task, CI preparation/entrypoints, Dockerfiles, release lanes and contributor instructions.
- [x] Run declaration/CMake/profile consistency checks and offline missing-package regressions.
- [x] Run narrow regressions, clean acceptance, installed-package consumers and required quality checks.
- [x] Inspect complete logs; record commands, versions, features, warnings and manual checks.
- [x] Make a repository-local commit only after local verification; publish only when authorized.

## Implementation decisions

- Copy canonical module declarations and generated provider helper; synchronize bundle 2.0.0 without local edits.
- Qt production and installed exports require >=6.11. GuiPrivate, QuickPrivate, QuickControls2Private,
  QuickTemplates2Private and test-only QmlPrivate explicitly require the selected Qt6 version exactly.
- Require installed GTest >=1.14 CONFIG and GTest::gtest; GMock remains unnecessary.
- BUILD_SYNTAX_THEMES defaults ON and requires KF6SyntaxHighlighting. OFF skips discovery, generation and installation.
- Normal workflow and demo/gallery commands validate installed prerequisites; task deps is the sole provider preparation command.
- Explicit CI preparation acquires immutable images and archives the exact Config commit, then invokes task deps
  in the qualified image. Its source-content, compiler, external resolution and options signatures are retained
  in build/ci-prepared/provider-state.json; manifest.json ties the artifact to the canonical revision/image.
- All acceptance containers use --pull=never --network=none, isolated HOME/runtime directories, read-only inputs
  and rootless passwd/group mappings. Acceptance never fetches Config or downloads supplemental packages.
- Full Debug and static enable tests, examples, Wayland, syntax and Qt5 probes; minimal retains tests and
  installed-package checks with those features off. Release uses production defaults without tests/examples/probes.
- Preserve production APIs and QML contracts. Font-sensitive button/install fixtures now use a fixed pixel size;
  installed QML execution times out after 30 seconds instead of hanging after a diagnostic.

## Verification (2026-10-12)

Preparation: `python3 scripts/ci/run.py --prepare --config-source ../holonight-config` passed;
log: `build/ci/20261012T183316Z-b29z9bcj/prepare.log`.
Qualified build image: `ghcr.io/lebedenko/holonight-deps@sha256:37ca051c39d1688d6413c47d94b8c0f221152dac6981ec67d155f2c43ea17b40`.
Licensing image: `docker.io/fsfe/reuse@sha256:85462a75c0f8efda09ddd190b92816b70e7662577c8427429e11e1b9f25a992e`.
Toolchain: Qt 6.11.2, GCC 16.2.1, CMake 4.4.3, Ninja 1.13.2, Python 3.14.7.

- `python3 scripts/ci/test_launcher.py`: five launcher regressions pass, including absent preparation without image acquisition.
- `python3 scripts/ci/test_dependencies.py`: twelve discovery/cache regressions pass locally and in the image.
  Cover old/missing Qt/GTest, exact private Qt mismatch, enabled syntax/Wayland prerequisites,
  disabled-feature isolation, explicit prefixes with spaces, provider fallback rejection and incompatible artifact archival.
- Umbrella `python3 -m unittest discover -s tooling -p 'test_*.py'`: 32 tests pass.
- `python3 tooling/generate-install-profile.py --check`, bundle drift, canonical declaration equality and diff checks pass.
- Initial full failures exposed font-dependent fixture assumptions and a hung installed QML probe; corrected.
  Qt5 has no standalone GuiPrivate CONFIG package; retain its established GuiPrivate target discovery.
- Compatible static audit: 76 owned translation units, 24 initial failures. Mechanical fixes, explicit casts,
  std::array fixtures, named coordinates, scoped annotations for GTest shared state and exhaustive matrices,
  and extraction of isolated origin collection preserve production APIs and behavior. The vacuous metrics
  test now asserts distinct dark/light tokens with an accurate name. All affected units pass focused tidy.
- Qt private include paths copied through target properties are explicitly SYSTEM includes in Wayland and
  its coordinate test. Owned headers remain checked; Qt/generated dependency headers are not edited.
- Eight actionable QML warnings are resolved by bound popup component context, explicit palette/model references
  and a scoped annotation for background properties erased to QQuickItem in static metadata.
  The original direct popup-radius binding is retained. QML import URIs and installed targets remain unchanged.
- Disposable CI source uses the real module directory name, so canonical QML policy provider exemptions apply.
  Static initializes only disposable Git metadata for tracked-source enumeration.
- Production QML modules consume the tooling import-path hint; the prior unused-variable warning is resolved.
  Expected private Qt version warnings and intentional gallery QTP0004 OLD deprecation remain documented.
- Final clean five-lane command: `JOBS=8 python3 scripts/ci/run.py`.
  Logs: `build/ci/20261012T200458Z-kx8vjcg5/`; Static and Licensing pass.
  Its full/minimal runtime checks found a teardown TypeError introduced by the temporary background alias.
  The alias was removed, restoring the exact original binding with a scoped missing-property annotation.
  Focused QML lint and installed/palette tests verify the correction; affected clean lanes are rerun below.
  C++ tidy, format and licensing results are independent of this QML-only correction and pass in that run.
  Static covered all 76 owned translation units; no owned-code tidy errors/warnings remained. System-header
  warning counts are suppressed by the canonical check configuration and are not project diagnostics.
  The final popup expression differs from baseline only in comments; focused all_qmllint and installed/palette
  regressions pass after restoration, as do canonical and repository QML import policies.
  Corrected clean full/minimal/Release rerun: `JOBS=8 python3 scripts/ci/run.py --lane full --lane minimal --lane release`;
  logs: `build/ci/20261012T202258Z-m0owdjsc/`; Full 97/97, Minimal 86/86 and Release build/install pass.
  All CTest entries, installed consumers, example startup tests and Qt5 probes pass in their applicable lanes.
  Complete Full/Minimal logs contain no actionable compiler or QML runtime diagnostics.
  Durable identities, lane outcomes and log hashes: [VERIFICATION.json](VERIFICATION.json).
  Final review corrects the extra-small control fixture variable name; focused tidy, build and QML smoke
  pass (`build/tidy-audit/source/build/final-naming-{tidy,build,test}.log`). Formatting remains clean.
  Earlier acceptance/audit evidence is retained for diagnosis, without claiming it verifies later corrections.

CTest entries pass while existing individual GTest cases skip shader masking under software rendering,
optional Kirigami Addons FormCard imports absent from the qualified image, and HoloNight-specific rendering
assertions in explicit Fusion runs. No skip policy was added or broadened.

Interactive visual assessment, manual ecosystem acceptance (EDM-022) and Debian qualification remain deferred.
No implementation push, umbrella pin update or CI success is claimed.

Provider identity: Config archive content SHA-256
`0329c0f5df87b1fecde6253a521ad2984a40c2ca89c91c5d72e8926887ec8a05`,
Debug, `BUILD_TESTING=OFF`, with the same qualified GCC/CMake/Ninja/Qt environment.
Source is an exact Git archive, so bundle source signatures label it unversioned; the preparation manifest
records the canonical Config revision. KF6SyntaxHighlighting is 6.30.0 and Wayland is 1.26.0.
Logs for incremental correction checks are under `build/tidy-audit/`; retained artifacts share the qualified
image, /work/source build paths, full features and /prepared provider prefix. They are focused verification,
not final clean acceptance evidence. No module-owned Dockerfile is needed for digest-based qualified images.
