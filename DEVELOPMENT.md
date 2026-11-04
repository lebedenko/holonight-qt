# Development

`task build` and `task test` call CMake and CTest directly. Install dependencies first;
CMake reports missing packages and never downloads them. Use `cmake --preset dev
-DCMAKE_PREFIX_PATH=/path/to/prefix` for an additional installed provider prefix.
The development preset enables tests; `release` stages optimized builds.
clangd uses `build/dev/compile_commands.json`.

`task format` uses the pinned image. `task check` is the acceptance gate: fresh provider
builds, build, tests, format, static analysis, applicable QML checks and REUSE.
`task check:integration` adds Release staging and project runtime checks. GitHub invokes
the same launcher with `integration`; no additional validation is hidden in the workflow.
The only host launcher prerequisites are Python 3, Git, tar and Docker, plus Task for aliases.
Builds and clang-tidy use all available CPUs by default (`nproc` inside the container).
Set `JOBS=4 task check` to override the job count. Native component builds use Ninja
defaults; use `CMAKE_BUILD_PARALLEL_LEVEL=4 task build` to limit those.
Image, tool versions and the selected job count appear in the acceptance log. Logs and fresh builds stay under
ignored `build/acceptance/`; downloaded sources are reused under `build/sources/`.

Standalone checks automatically acquire the exact sources in `scripts/ci/providers.json`.
For simultaneous provider/consumer development, explicitly run
`task check -- --siblings /path/to/umbrella`. This uses current sibling files, including
edits. Final umbrella integration uses its gitlinks; never substitute sibling mode for it.
Provider sources are built with ordinary CMake into a disposable prefix. QMarkdown is
an independently installed package in the shared image and is not owned by our installer.

Routine fixes need tests and a commit explanation. Substantial designs use one SDD;
cross-project work uses one umbrella contract note. Keep execution logs out of Git.


## Dolphin selection and icon refresh

Focused host verification uses `holonight_tests`, `holonight_style_primitive_tests` and
`holonight_platform_palette_{gui,fusion,holonight,quick,override}`. The rendering regression covers
light/dark schemes, selected, hovered-selected, focused-selected and disabled items through primitive
and delegate paths, single compositing when the row primitive is also called, readable text and an
accent-colored folder tab. The palette regression uses a derived KDE-like container with a graphics
view, scene and graphics item; editor highlight roles stay solid. The standalone style reload regression
checks pixmap invalidation without a HoloNight platform theme, explicit palette roles, no-op reloads and
typography-only changes.

The Qt-only platform consumer retains a theme icon across cyan → blue → cyan updates and compares
both it and fresh icons against live Highlight and Accent roles. It also compares cold palette snapshots,
checks cached pixmap eviction, preserves no-op caches and exercises an explicit application Accent override.
Existing platform icon tests cover explicit primary icon-theme overrides and live icon-theme hints.

Native Dolphin verification on 2026-11-04 used host-built plugins, a private D-Bus session, disposable
configuration/cache directories and plain, Downloads and SVG-preview folders. `/proc/<pid>/maps` confirmed
both build-tree HoloNight plugins and KDE's `KIconEnginePlugin.so`. Captures and an isolated KDE startup
probe are retained under the ignored `build/dev/dolphin-check/` directory. The probe resolved
`/usr/share/icons/HoloNight/places/32/folder.svg` after explicitly configuring KDE's icon theme to HoloNight.
It observed startup Accent `#5ea2ff`, followed by theme name `KIconEngine` and application Accent/Highlight
`#3daee9` after KDE startup callbacks. This is an application palette and icon-engine override, beyond the
Qt-only platform-consumer path. KDE's [startup implementation](https://invent.kde.org/frameworks/kiconthemes/-/blob/master/src/kicontheme.cpp)
installs that engine and initializes its color-scheme manager.

Selected Downloads now has readable text and an intact silhouette in the native icon-view capture;
Places labels also remain readable. Graphics items require the scene palette as well as the view palette.
Ordinary folders still use KDE's cyan in cold blue startup and after a live cyan → blue change; the SVG
preview folder follows HoloNight's blue on cold startup and preserves its authored preview colors. Thus
full Dolphin folder-color acceptance remains **unmet**. The attempted compositor shortcuts did not change
view modes, so detail-view interaction and forced preview toggling are not claimed as verified.

Preserving explicit application palettes and icon-theme overrides prevents silently taking ownership of
KDE's injected engine here. Resolving that path needs a separate integration decision. No KDE dependency,
Dolphin patch, disk-cache deletion, desktop-wide refresh or icon-artwork change is included.
The separate native KDE Accent-only cache regression in `holonight-icons` retains its reported failure
status; it does not explain this startup override, and it was not rerun as part of this change.
