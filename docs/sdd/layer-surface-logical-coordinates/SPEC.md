# UQC-215: layer-surface logical coordinates

State: locally verified (2026-09-15); contract marked Ready before implementation.
Baseline: `638eec25c0934538e747b969b37ab8f63d1722de`, verified canonical main.

## Contract

All public sizes, margins, positive exclusive zones and surface-local input
regions are Qt logical coordinates, matching QWindow/QQuickItem/QScreen callers.
Width/height zero retain anchored compositor allocation. Exclusive zones <= 0
retain protocol semantics. Negative margins remain signed distances.
No public signature changes and no shell compensation; the bar remains 64 units.

At the backend boundary use QHighDpi window-context conversion to/from native
Wayland surface coordinates, including local regions. Window DPR additionally
includes compositor buffer scaling and must not be substituted for this factor.
Round only at the integer protocol boundary using Qt conversions. Keep requested
logical values separately from configure results. Zero configure axes preserve
current Qt size. Repeated configure must not rewrite requests or accumulate
rounding. Refresh requests from logical originals when the Qt conversion factor
changes; output-only buffer-scale changes must not double scale geometry.

## Caller audit

Shell LayerShellManager requests height/exclusive zone kBarHeight (64).
PerMonitorLayerManager uses anchored zero dimensions. StatusPopupSurface,
TooltipSurface, TrayMenuSurface and SidebarSurfacePolicy derive positions from
QScreen/QML logical coordinates; notification/OSD/widget/launcher dimensions
are logical constants or QML measurements. AI buildPanelSurfaceSpec uses
QScreen geometry, logical width bounds 440..560, anchored zero height and
12-unit margins. These callers require no signature or unit migration.

## Scope and verification

Add and demonstrate a failing real-backend regression before repair. Cover
initial/later requests, configure, zero axes, margins, zones, region boundaries,
repeated configure, Qt scales 1/1.25 and output scales 1/2 independently.
Run focused tests, provider suite/static checks, installed consumers and affected
shell/AI hosting regressions. Publish before umbrella pinning. Human S01 repair
acceptance is separate from local verification. F05 is umbrella-owned diagnosis.

## Probe teardown disposition

The existing context is a function-static object: with a stack QGuiApplication,
its Wayland proxies outlive the Qt display. The reduced fixture gives the
application static lifetime established before context construction, closes the
host and drains DeferredDelete before returning. This orders context destruction
before display destruction without changing production lifecycle in this repair.
The ordinary stack-application context teardown defect remains a separate issue;
a passing geometry probe must not be presented as its repair.
