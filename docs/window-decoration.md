# Window decoration observation

Import Holonight.Core and bind `HnWindowDecoration.window` to a QQuickWindow. `mode` is Unknown, ServerSide, ToolkitClientSide or Undecorated; `externalDecorationPresent` is true for the two decorated modes. This is raw decoration state and does not establish whether an external title bar is rendered.

This observational helper is supported on native Wayland xdg toplevels using Qt 6.11 private APIs (verified against 6.11.2). Rebuild against the matching Qt version. It never changes flags, negotiates decoration or creates window controls. Unsupported shells/platforms and pending or suspended configuration report Unknown. BUILD_WAYLAND=OFF preserves the QML type and reports Unknown.

Frameless and fullscreen native xdg windows report Undecorated. An active Qt decoration reports ToolkitClientSide. A configured normal xdg window without a Qt decoration reports ServerSide unless the shell requires client decoration, in which case it reports Undecorated. Qt's state cannot reveal compositor rules that suppress title text within an SSD frame; that frame remains ServerSide. X11/XWayland and custom decoration plugins are outside this contract.

Surface lifecycle and GUI dispatcher boundaries refresh observations without a polling timer. Existing HnApplicationWindow header defaults are unchanged.

## External title presentation

Use `HnWindowPresentation { window: applicationWindow }` when deciding whether an application heading duplicates an external title bar. Its read-only `externalTitleBarState` is `Unknown`, `Present`, or `Absent`.

```qml
HnWindowPresentation { id: presentation; window: applicationWindow }
// On the application header:
titleVisible: applicationWindow.fullscreen || presentation.externalTitleBarState !== HnWindowPresentation.Present
```

Active supported Qt CSD reports Present. Explicit frameless/fullscreen windows report Absent. For raw SSD, the independently linked HoloNightSystem::Compositor provider identifies exactly one native PID/app ID match. Sway reports valid leaf decoration geometry, considering fullscreen ancestry; Hyprland, labwc and generic Wayland remain Unknown. SSD negotiation, borders and plugin names are not title-rendering evidence. Missing, ambiguous, unsupported, pending or disconnected observations stay Unknown. BUILD_WAYLAND=OFF preserves the presentation API and its Unknown fallback.

The helper requests snapshots on native lifecycle/exposure/state events and refreshes from backend events. Window replacement, surface destruction and stale replies clear observations. No periodic title-detection timer is added. Present denotes a reported title-bar area; themes that hide its text are outside detection. Native compositor checks remain pending until manually verified.
