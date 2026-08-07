# DESIGN: Appearance Configuration Foundation Adoption

## Target layers

The adopted dependency flow is:

```text
HoloNight::Config value/load result
              |
              v
Qt appearance reader + theme catalog validation
              |
      resolved Qt appearance state
       /          |           \
palette       typography      shapes
colors        icons/layout    ShapeResolver
       \          |           /
        engine-local QML projections
```

The Qt reader converts standard strings and numbers only after the provider has accepted the entire document. It
adds catalog diagnostics for unknown schemes or unsupported accent combinations. It owns watcher mechanics but no
schema defaults or serialization.

The QML module creates one engine-local state object and injects or shares it with the three public projections. This
avoids three watchers and prevents palette, fonts, and shapes from observing different revisions. Plugin targets use
the same non-QML reader class because they have separate lifecycles and may load without a QML engine.

## State publication

A candidate follows a two-phase path: load/validate into an isolated value, then resolve the complete Qt projection.
Only a successful candidate replaces active state. Publication compares semantic sub-values and emits narrowly:

- one global appearance revision for any accepted semantic change;
- palette notification only when resolved `ColorTokens` change;
- theme notification only when catalog, typography, icon, cursor, or layout projection changes;
- shape notification only when resolved shape selection changes.

Watcher events are coalesced before reload and both directory and file watches are rearmed after every event. The
watcher never interprets a temporarily missing rename target as a request to reset active state.

## Theme and typography projection

The selected scheme is catalog-validated and determines `ColorMode`; there is no separate mode member. Accent is
resolved by the existing catalog. Catalog failures reject the candidate rather than silently selecting a different
user preference.

Canonical configured font fields map directly to Qt families and sizes. Derived semantic roles are computed from the
UI size with the existing clamped relationships unless a role has an explicit canonical size: title-family/title-size
and display-family/display-size are direct configured roles. Public naming should make configured values distinct
from derived role sizes to avoid recreating the old `baseFontSize` ambiguity.

## Metric implementation

`MetricTokens` belongs beside the design-system token models but outside `ColorTokens`. A small `ControlSize` enum and
indexed role records replace switch statements duplicated in QML. `HnMetrics` is a C++-backed QML singleton or an
immutable generated projection; either implementation must read `MetricTokens` rather than restating numeric values
in QML.

The initial values intentionally preserve current rendering. Moving them is an ownership refactor, not a density
redesign. Tests compare all roles exactly and ensure scheme/accent resolution cannot modify metrics.

## Target and package cleanup

The current local target named `holonight_config` no longer owns configuration persistence. Rename or split it so its
remaining catalog/projection responsibility is explicit, and link the canonical imported `HoloNight::Config` target
privately where possible. Installed `HolonightQt` package metadata must declare any dependency required by exported
targets.

The legacy `AppearanceConfig` serializer and `ThemeConfig` loader are deleted once all internal targets use the new
reader. Shape calculation methods move to a renderer-domain projection or remain on `ShapeResolver`; generic
configuration values must not acquire Qt rendering methods.

## Trade-offs

- One QML state object is more coordination than independent singletons, but guarantees atomic cross-domain updates.
- Strict catalog rejection may leave last-known-good state active after a structurally valid but unsupported scheme;
  this is preferable to presenting Settings' saved selection while rendering a silent fallback.
- Clean removal intentionally breaks pre-stable metric and legacy configuration APIs. Forwarding aliases would retain
  the responsibility defects this initiative exists to remove.
