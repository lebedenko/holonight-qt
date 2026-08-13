# Shared Wayland Surface Host — Specification

Status: Accepted

## Outcome

Applications can request an optional installed `HolonightQt::Wayland` component that owns the complete
`wlr-layer-shell` binding, Qt window, Wayland surface, and layer-role lifecycle. Existing HoloNight theme, style,
Core, and Controls consumers configure without discovering or linking Wayland-specific dependencies.

## Public API

All public types use the `Holonight::Wayland` namespace.

- `LayerShellContext` binds one `zwlr_layer_shell_v1` global per process and reports availability, negotiated version,
  and one actionable failure diagnostic.
- `LayerSurfaceSpec` describes output, namespace, layer, anchors, requested dimensions, margins, exclusive zone,
  keyboard interactivity, input region, QML source, and initial properties.
- `LayerSurfaceHost` owns one `QQuickView`, its `wl_surface`, and its layer role. It provides guarded view/root access,
  `configured`, `closed`, and `failed` signals, and an idempotent queued close operation safe inside QML callbacks.

`LayerSurfaceSpec` contains a non-null `QScreen*`, a non-empty namespace, one of the four protocol layers, anchor
flags, non-negative width and height, four signed margins, an exclusive zone, protocol-version-compatible keyboard
interactivity, an input policy (`Default`, `Empty`, or a supplied `QRegion`), a valid non-empty QML URL, initial QML
properties, window flags, and a background color. A caller may supply a `QQmlEngine`; otherwise the view owns its
engine. `beforeLoad` runs after engine creation and before `setSource()`, allowing image providers to be registered
without exposing Wayland handles.

`LayerSurfaceHost::open()` is valid only from `Idle` or `Closed`. It validates the complete spec before allocating a
window or protocol object and returns whether startup reached `WaitingForConfigure`. Accessors for the view, root
object, and engine are guarded and return null from `Closing`, `Closed`, or `Failed`. After open, size, margins,
anchors, exclusive zone, keyboard interactivity, and input region can be changed; each accepted mutation applies the
pending protocol state and commits the `wl_surface`.

The public enums intentionally mirror layer-shell v4 values but do not expose generated protocol declarations.
Keyboard `OnDemand` is rejected when the negotiated global version is below 4. Anchor bits outside top, bottom,
left, and right are rejected. Width and height must be non-negative and must be nonzero on an axis that is not
anchored to both opposing edges.

## Lifecycle invariants

1. The host creates the Qt window without mapping it as an xdg-toplevel, obtains its `wl_surface`, assigns the layer
   role, applies the full spec, loads QML, and commits the surface.
2. A configure is acknowledged before applying compositor-provided dimensions. The view maps only after the first
   configure.
3. Close requests are idempotent. The layer role is destroyed synchronously; Qt window disposal is deferred until the
   active callback returns.
4. `QQuickView::hide()` is never called while the layer role is live.
5. Output loss, compositor close, QML load failure, and owner destruction follow the same role-before-window order.
6. Public view and root pointers become null once closure begins and never outlive their owner.

The concrete state transitions are:

- `Idle|Closed -> WaitingForConfigure` after complete resource creation and the initial commit.
- `WaitingForConfigure -> Configured` on the first configure; later configures remain `Configured`.
- `WaitingForConfigure|Configured -> Closing -> Closed` for explicit close, compositor close, output loss, or owner
  destruction. Role destruction and pointer invalidation happen entering `Closing`; view deletion is queued; the
  `closed` signal is emitted once.
- `Idle|Closed|WaitingForConfigure|Configured -> Failed` for validation, protocol/window creation, or QML loading
  failure. Any allocated role is destroyed before queued view deletion and `failed(diagnostic)` is emitted once.

The context binds at most version 4. It changes availability when the advertised global appears or is removed and
retains one actionable diagnostic: the process must run on Qt's Wayland platform and the compositor must advertise
`zwlr_layer_shell_v1`. Loss of the global prevents new hosts; existing roles close through their normal teardown.

## Packaging

The main package config loads Wayland dependencies and the Wayland target export only when `Wayland` is requested.
The component requires Qt Core, Gui, Qml, Quick, WaylandClient, the Qt Wayland scanner at build time, and
`wayland-client`. Protocol generation is owned here; consumers do not generate or directly own the layer-shell role.

## Verification

- Deterministic lifecycle-state tests cover configure, close before configure, repeated open/close, output loss,
  QML-callback closure, and observable role-before-window destruction.
- Installed-package consumers prove existing components configure without Wayland discovery and that a Wayland
  consumer receives `HolonightQt::Wayland` and its public headers.
- `task verify` passes.
- `format-check` passes.
