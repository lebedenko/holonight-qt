# Local CI rehearsal

Baseline: 79ab555a886af469c9d5fd91b455c1e391008e0c. Umbrella package CI-003.

Preserve the original combined build/test/static job and the separate licensing job,
push/PR triggers, Release/Ninja options, demo/gallery and Qt5 compatibility probes.
Each lane copies a read-only current working-tree snapshot into a fresh tmpfs;
revision/dirty state, complete logs, image identities and results go to ignored build/ci.
Docker is preferred, with Podman fallback when Docker is absent; linux/amd64 is fixed.
The task and workflows call the same repository-owned launcher and lane script.

The published immutable image supplies Qt6 6.11.1 and compiler tools. Versioned,
SHA-256-pinned official Arch archives supply Qt5 5.15.19, ripgrep and patchelf in a
disposable prefix, without changing the host or compiler image. Qt5 probes receive
their own qt.conf; Qt6 executables retain their existing configuration. Licensing
uses immutable REUSE 6.2.0. All supplements and tool versions are logged.

Provider contract: clone and verify canonical holonight-config revision
fe69a59e6b73167fd5349223a4d265d75386c139 (the existing workflow revision), build it
fresh in Release and use its explicit installed prefix in both local/remote lanes.
Reject global configuration headers that can hide missing target dependencies.
Keep all existing CTest cases and full source tidy; no required check may be skipped.
No publication or submodule pin updates are included. Local acceptance passed on 2026-10-02; source fixes are checkpointed in a2b2c2d.

The first clean run exposed environment assumptions: Qt requires a UTF-8 locale,
a private XDG runtime directory, and the canonical Config library prefix on the
runtime search path for installed examples. The lane now supplies those settings;
all 97 CTest cases passed in the corrected environment before static acceptance.

Existing source diagnostics also fail the original required tidy target. Resolve
those findings before accepting the rollout, including host clang-tidy 23 checks.
Keep enum representations and Qt instance APIs stable; use narrowly documented
exceptions only for ABI or meta-object requirements. Anchor the header filter to
owned repository headers so third-party Qt implementation headers are not linted.
Extract existing painting, SVG recoloring and cache handling into focused helpers;
retain their decisions, ordering, locking and rendering behavior. Existing rendering,
interaction, installed-package, generated-scheme and Qt5 tests provide acceptance.
The gallery declares the existing QTP0004 OLD behavior explicitly, preserving its
current directory imports while resolving the developer policy warning.


Verification: `task ci` passed both lanes from a fresh snapshot in
`build/ci/20261002T200026Z-kpkidjh2/`: full Release build, 97 CTest cases, format-check,
source tidy and REUSE 6.2.0. Launcher regressions covered edits, deletions, new and
ignored files, spaces, executable modes, symlinks, missing runtimes and every failed
lane. The native build, format-check, clang-tidy 23.1.1 source check and six focused
CTest registrations also passed. Before/after hashes, modes and timestamps confirmed
1,644 source/development-build files stayed unchanged during container verification.
Complete logs were reviewed. The frozen Qt private-ABI warnings are expected; Vulkan
and KF6 theme-generator support remain optional as in the original job. Explicitly
preserving QTP0004 OLD leaves its future-major-version deprecation warning; changing
gallery import behavior is outside this compatibility-preserving rollout.


Rootless Podman uses `--userns=keep-id` so preserved private file modes remain
readable under the requested UID/GID. A fake-runtime launcher regression verifies
user mapping and the read-only input mount. Podman is not installed on this host;
its real-runtime integration is unverified. The verified Docker path is unchanged.
