# Window decoration observation

Import Holonight.Core and bind `HnWindowDecoration.window` to a QQuickWindow. `mode` is Unknown, ServerSide, ToolkitClientSide or Undecorated; `externalDecorationPresent` is true for the two decorated modes. For example, a toolbar title can use `fullscreen || !decoration.externalDecorationPresent`.

This observational helper is supported on native Wayland xdg toplevels using Qt 6.11 private APIs (verified against 6.11.2). Rebuild against the matching Qt version. It never changes flags, negotiates decoration or creates window controls. Unsupported shells/platforms and pending or suspended configuration report Unknown. BUILD_WAYLAND=OFF preserves the QML type and reports Unknown.

Frameless and fullscreen native xdg windows report Undecorated. An active Qt decoration reports ToolkitClientSide. A configured normal xdg window without a Qt decoration reports ServerSide unless the shell requires client decoration, in which case it reports Undecorated. Qt's state cannot reveal compositor rules that suppress title text within an SSD frame; that frame remains ServerSide. X11/XWayland and custom decoration plugins are outside this contract.

Surface lifecycle and GUI dispatcher boundaries refresh observations without a polling timer. Existing HnApplicationWindow header defaults are unchanged.
