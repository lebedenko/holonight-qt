# Shared Wayland Surface Host — Design

The component separates protocol-global ownership from per-surface ownership. `LayerShellContext` is shared and has no
QML or window policy. Each `LayerSurfaceHost` is a small state machine (`Idle`, `WaitingForConfigure`, `Configured`,
`Closing`, `Closed`, `Failed`) driven by protocol callbacks and queued Qt disposal.

The host deliberately owns its view rather than accepting an arbitrary externally owned window. That makes the
destruction order structural: the layer-role wrapper is destroyed before the view member. A close callback first
invalidates guarded access, destroys the role, and queues final view disposal. This prevents the remap and protocol
errors caused by hiding a Qt Wayland window with a live custom role.

The installed package uses a separate Wayland export file. The base config retains its current dependencies and only
includes that export, plus Wayland-specific dependency discovery, from the `Wayland` component branch.

## Resource construction and teardown

The context uses Qt's public `QWaylandApplication` native interface to obtain the display, owns a registry listener,
and binds the layer-shell global with `min(advertisedVersion, 4)`. Per-host creation uses the public Wayland screen
native interface for `wl_output` and Qt's private Wayland window native interface for `wl_surface`; the latter is
confined to the implementation and is the reason the component links `Qt6::WaylandClient`.

The host stores only guarded public pointers. Teardown moves the owned view into a local guarded pointer, clears all
public access first, destroys the layer role synchronously, clears the raw surface, and schedules `deleteLater()`.
The window is never hidden. Output destruction is observed through `QScreen::destroyed`; compositor closure and
global loss enter the same teardown helper.

Input regions are copied into short-lived `wl_region` objects. `Default` passes null (the protocol's infinite default),
`Empty` passes an empty region, and `Region` adds every rectangle before setting and immediately destroying the
region. Region content is copied by the compositor on commit.

## Failure policy

Validation diagnostics identify the invalid field. Runtime diagnostics distinguish a non-Wayland Qt platform, an
unadvertised layer-shell global, missing native output/surface handles, role creation failure, and QML load errors.
Failures never leave a live role. Context unavailability is reported once per availability episode through its
diagnostic property; each host emits at most one terminal signal per open cycle.

## Test seams

The lifecycle controller talks to a private backend interface whose production implementation owns the actual view
and protocol objects. A deterministic fake records acknowledgements, commits, mapping, role destruction, guarded
pointer invalidation, and queued window deletion. Tests inject the fake and drive configure, close, compositor-close,
output-loss, and load-failure events without requiring a compositor. Package tests install into an isolated prefix:
one consumer masks Qt Wayland and Wayland discovery while requesting existing components, and another requests only
`Wayland`, compiles every public header, and checks its imported link interface.
