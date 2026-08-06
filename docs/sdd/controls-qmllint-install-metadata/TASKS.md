# TASKS: Controls qmllint install metadata

- [x] Reproduce and record build-tree versus installed-prefix `qmllint` behavior.
- [x] Inspect generated Controls `qmldir`, qmltypes, resource, and type-registration artifacts.
- [x] Compare Controls generation with the working `Holonight` and `Holonight.Core` modules.
- [x] Test whether the generated `prefer` directive affects installed-source type discovery.
- [x] Confirm and document the Qt/CMake root cause and supported metadata mechanism.
- [x] Establish a canonical public Controls inventory and separate internal/resource inventories.
- [x] Implement the smallest generated upstream metadata or discovery correction.
- [x] Keep `HnSelectableDelegate` installed and internal.
- [x] Add an isolated installed-prefix `qmllint` consumer covering every public Controls type.
- [x] Ensure the lint test cannot fall back to source/build-tree or ambient QML import paths.
- [x] Assert consistency among the public inventory, installed sources, `qmldir`, and qmltypes metadata.
- [x] Run the minimal `HnNavigationDelegate` reproduction.
- [x] Run Controls lint plus focused runtime and package-install tests.
- [x] Run `task build`, `task lint`, and `task test`.
- [x] Run `git diff --check`.
- [x] Hand off the downstream CP-S-001 retry to the repository owner using this implementation commit.

## Completion evidence

- Plain `qmllint -I <prefix>` reproduced unresolved newer types by selecting stale modules installed under
  `~/.local` and `/usr`; `--bare -I <prefix> -I <qt-qml-root>` resolved all 20 public types.
- Removing the generated Controls `prefer` entry from a diagnostic install did not affect the failure.
- The empty generated Controls qmltypes file is correct for its QML-only public surface; the generated `qmldir`
  and installed QML sources provide those type declarations.
- `cmake --build build --target holonight_qml_qmllint holonight_core_qml_qmllint
  holonight_controls_qml_qmllint -j4` passed. The two pre-existing `ComboBox.Window` warnings remain unchanged.
- `holonight_package_install_test` passed with runtime construction and hermetic lint coverage for every public
  Controls type.
- `task build`, `task lint`, and `task test` passed; the full offscreen suite passed 16/16 tests.
- `git diff --check` passed.
- The repository owner will perform the downstream CP-S-001 retry using this implementation commit.
