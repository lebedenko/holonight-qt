# Configuration architecture: holonight-qt

Work package: CA-002. Upstream baseline: `c434d6821140d5269550cb2388820f9cbfb2e163`.

## Scope

Reusable Qt watching and appearance reads.

Follow the accepted [shared contract](../../../../docs/initiatives/configuration-architecture/README.md).
Keep this repository independently buildable; do not modify another repository in its implementation commit.

## Design and acceptance

Each application owns its configuration schema, file, settings UI and behavior. Global appearance is shared;
application preferences are separate. Files and Viewer must remain usable without Shell or Settings. AI and Packages
retain their own configuration. Infrastructure requires neither Shell, Settings nor a running daemon.

Snapshots retain original bytes, parsed typed values, override presence, source spans and content revisions.
Paths are vectors of key segments, including quoted keys containing dots. Schemas declare typed defaults,
constraints, descriptions and reload policy, with domain validators for related values and dynamic collections.
Edit batches carry set/remove operations and baseline values/presence. Save outcomes distinguish success,
per-key conflicts, invalid documents/edits, unsupported patches, pre-replacement storage failures and
post-replacement durability failures.

Use toml++ and a TOML-aware lexical editor; never serialize an existing document wholesale or substitute by regex.
Preserve unrelated bytes, comments, ordering, whitespace and unknown fields. Reset removes an assignment and retains
comments/sections. Arrays and arrays of tables are whole conflict values. Unsupported safe patches fail unchanged.
Reparse and validate every candidate. Establish preservation fixtures before consumer adoption.

Merge baseline, pending and current values: unrelated edits merge, identical edits converge, different changes to the
same value conflict. Lock a stable sibling file for cooperating writers; read/patch under the lock and recheck the
revision immediately before replacement. Arbitrary editors do not participate in the lock: a race remains between
the final check and rename. Follow existing symlinks, abort on retargeting, preserve existing permissions, create new
files as 0600, sync a same-directory temporary file, rename, and sync the directory.

Appearance v2 uses sparse defaults, rejects invalid known fields and warns about preserved unknown fields. Retain the
v1 decoder and explicit v1 serialization APIs. Document version is metadata, separate from the effective appearance
model. First successful Settings save upgrades valid v1 by changing only version and requested values. New editing
documents use v2; unsupported versions are read-only. Enable GUI v2 writes only after readers/adapters pass compatibility.

Shell owns defaults/validation in its exported configuration package. Preserve paths and meanings. Reads never create
files or write defaults. Missing overrides use defaults; reset removes the override. Reject invalid known values.

Settings retains Save/Discard, tracks baseline/pending edits, refreshes untouched controls on external changes and
retains pending edits. Expose baseline/disk/pending values and per-value keep-pending/accept-external resolution;
recheck on save. Show default/override status and diagnostics. Discard loads latest disk. Invalid external documents
block saves and running consumers retain last valid values; startup errors use defaults with diagnostics. Missing
files use defaults without writes. Watch files and nearest existing parents through replacement/deletion/recreation;
publish only differing effective values. Domain saves have independent outcomes. Rollback is conditional on the staged
revision still being current; concurrent changes survive and must not be reported successfully applied.

- [ ] Preservation fixtures cover comments, unknown fields, quoted/dotted keys, inline tables, multiline strings, Unicode, CRLF, arrays/AoT, insertion/reset and rejected patches.
- [ ] Merge, convergence, conflicts/reset, cooperating locks and revision-change aborts pass.
- [ ] Unreadable files, permissions, interrupted writes, replacement failures, symlink retargeting and durability outcomes pass.
- [ ] v1 behavior remains compatible; sparse v2/reset and surgical first-save upgrades pass; unsupported versions cannot be overwritten.
- [ ] Runtime invalid/startup/missing/delete/recreate and unchanged-signal scenarios pass.
- [ ] Settings Save/Discard, external updates, per-value resolution, partial saves and adapter/rollback concurrency pass.
- [ ] Each repository passes required clean acceptance and installed-package checks at accepted provider revisions.
- [ ] Files and Viewer pass standalone checks without Shell or Settings installed.
- [ ] Every participating submodule is clean and pinned to a canonical published implementation commit.
- [ ] Dependency-order integration and user-operated concurrent-edit/appearance checks are recorded with dates and revisions.

## CA-002 implementation and verification — 2026-10-05

The exported Appearance component now provides `DocumentWatcher`. It watches the file and nearest existing parent,
rearms after replacement/deletion, and also watches a symlink target's parent. It exposes neutral snapshots without
writing configuration. AppearanceReader adopts sparse v2 decoding, preserves its effective-model contract and
retains the last valid appearance after invalid external changes. Missing files use defaults without diagnostics
or creation; unknown v2 fields produce warnings. Equal effective values do not emit appearance change signals.

`HoloNight::Config` is a public dependency because the watcher header exposes its document types. The clean CI lane
fetches published Config `7e83cacde8911452741420a29abbfe4465b7d52b` and the already accepted System Services
`398804a7cce5a57f9f6870c4e7ec99e9b1f3ddaa`; the latter was missing from the baseline lane despite being required.
The installed-example probe uses the same accepted Config runtime path as the other installed-package probes,
avoiding accidental loading of an older host installation. No CI triggers or required checks were removed.

Local clean `task ci` passed build-test (98/98 tests, installed-package isolation, format and full source tidy)
and licensing (478/478): `build/ci/20261005T200830Z-52q88p52/`. Complete logs were reviewed. An actionable QML registrar
warning was then resolved by spelling the window-decoration enum's existing 8-bit base as `unsigned char`, which
the registrar recognizes. The follow-up incremental full build, QML import policy, qmltypes check and format check
passed without that warning. Focused clang-tidy passed for appearance, reader, watcher, runtime tests and decoration.
Runtime tests cover sparse v2 warnings/invalid values, deletion and nested-parent recreation, and metadata-only
v1/v2 changes without effective-value signals. The affected bootstrap, QML smoke, isolation and decoration tests
passed after the enum spelling change. Installed-package follow-up results are recorded in TASKS.

## CA-002a: symlink recovery

Baseline `1067d0a717f0b24eaa399693a855d687d30e1f71`; Config provider `733781607124fc9bec0820c880e7467d08b34a50`. The published watcher misses recreation in the physical target directory after deletion. Regression reproduced with a GTest assertion around `QTest::qWaitFor`; Qt-only QTRY macros were replaced in this reader suite because they did not mark GTest failures. Watch both alias and physical target nearest existing parents, even when the target is absent. Existing invalid dangling-link reads retain diagnostics and last valid values.
