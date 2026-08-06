# DESIGN: Shared Appearance Configuration

`AppearanceConfig` lives in `holonight_theme` beside `ShapeResolver`. It owns parsing and derives a
`ShapeTokens` instance, keeping JSON and policy out of QML. `HnAppearance` is the process-local QML adapter: it
stores the current value, watches the resolved path, emits one `appearanceChanged` notification, and exposes a
shape-resolution method.

`HnSurfaceFrame` calls `HnAppearance.resolve(...)`. Its binding includes the singleton revision, so existing
instances are re-evaluated after a meaningful reload. Explicit component style and numeric overrides retain
highest precedence. The global style applies only when the component requests `Inherit`.

Watching the directory as well as the file handles editors that save by rename and configuration created after
application startup. Reload emits no change when normalized configuration is unchanged.
