# CA-002 implementation

- [x] Inspect existing implementation and preserve public contracts.
- [x] Implement repository-owned scope from README.
- [x] Add focused behavioral regression coverage.
- [x] Run focused checks, clean acceptance and installed consumer checks.
- [ ] Review final diff, commit, publish and hand off exact revision.

Installed-package follow-up passed (74.75 seconds) after selecting the accepted Config runtime. All five affected
checks passed across the follow-up runs; the incremental build and metadata/policy/format checks passed.

## CA-002a

- [x] Reproduce missing-target recreation against the published watcher.
- [x] Watch physical target parents using the accepted Config resolver; make GTest async assertions reliable.
- [x] Verify focused runtime and clean acceptance.
- [ ] Publish and hand off.

2026-10-06 local: all 10 AppearanceReader cases passed with GTest-visible asynchronous assertions. Clean `task ci` passed all 98 CTest registrations, installed-package consumption, provider/example checks, full source tidy/format and licensing (478/478); logs `build/ci/20261005T214228Z-rfxhylz6/` reviewed in full. Focused host tidy of watcher and changed test file also passed. No actionable build/static diagnostics. Config provider `733781607124fc9bec0820c880e7467d08b34a50`.
