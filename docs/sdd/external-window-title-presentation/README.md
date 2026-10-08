# External window title presentation

Archived: replaced by [Decoration-independent Viewer](../decoration-independent-viewer/README.md). The requirements below describe historical work, not the current API.

Work package: I-003. Baseline: `aa26e2edbda52e1d96771b0b6931944a564f72f2`.

See the umbrella initiative for settled contracts and scope. Implementation remains local; publication and integration are pending.

## Requirements

Register HnWindowPresentation in Holonight.Core. Preserve raw HnWindowDecoration. Combine active Qt CSD evidence with SSD compositor observations. Frameless/fullscreen report Absent; unsupported or pending evidence stays Unknown. Clear observations on native surface or window invalidation and reject stale callbacks.

## Implementation

Holonight.Core registers HnWindowPresentation with writable window and read-only externalTitleBarState. Raw HnWindowDecoration remains independent. Active supported Qt CSD supplies Present; SSD uses compositor observations. Frameless/fullscreen supplies Absent; unavailable evidence supplies Unknown. Surface/window generation guards reject stale snapshots; lifecycle events request refresh without a periodic title timer. Private Qt/Wayland APIs remain within Qt.

The QML implementation privately links Compositor without imposing a new exported Core consumer dependency. Dependency bootstrap supplies SystemServices before Qt. Documentation replaces the previous SSD-implies-title recommendation.

## Verification — 2026-10-05

- Clean enabled build passed; full CTest passed 95/96 initially, then the corrected package-install consumer passed, covering all 96 checks.
- Enabled and disabled presentation/raw decoration suites passed after final corrections. Install-tree Core/QML consumers passed.
- Format, focused presentation clang-tidy, QML lint/import/types and REUSE checks passed.
- `task check` stops on 12 existing lint diagnostics in unchanged `tests/isolated_style_probe.cpp`.
- Qt CSD, fullscreen and native surface recreation checks remain manual and pending. Offscreen fixtures establish lifecycle/composition behavior only.
