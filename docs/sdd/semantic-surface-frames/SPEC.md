# SPEC: Semantic Surface Frames

**Feature:** Reusable semantic surface geometry for HoloNight QML applications
**Date:** 2026-07-22
**Status:** Implemented

---

## 1. Context

`holonight-shell` currently owns `apps/shell/qml/Controls/HudFrame.qml`, a shell-specific `Canvas`
component that combines reusable surface drawing with shell presentation defaults. The HoloNight AI
architecture requires the same visual language without making applications depend on the shell.

This cycle moves only the reusable frame contract into the `Holonight` QML module. Shell concerns such as
layer-surface behavior, blur, masks, outer margins, and HUD activation remain in `holonight-shell`.

## 2. Goals

- Give shell and application repositories one public surface-frame component.
- Give QML and Qt Widgets one shared C++ source of truth for shape geometry.
- Select geometry through semantic roles rather than repeated radius literals.
- Provide the recommended Hybrid profile while allowing Rounded and Chamfered profiles.
- Preserve local overrides as an escape hatch.
- Keep frame rendering declarative and suitable for static or dynamically resized surfaces.
- Establish a tested API that a later shared appearance singleton can drive.

## 3. Non-goals

- Refactoring `holonight-shell` in this repository.
- Adding `HnApplicationWindow` or client-side window decorations.
- Reading or watching `$XDG_CONFIG_HOME/holonight/appearance.json`.
- Defining compositor borders, shadows, input regions, blur, or layer-shell integration.
- Reproducing every gradient, highlight, glow, or offset from the current shell `HudFrame`.
- Migrating existing controls to semantic frames in this cycle.

## 4. Public API

### REQ-F-001: Expose semantic surface roles

The `Holonight` QML module shall expose these roles through `HnSurfaceRole`:

- `Window`
- `Panel`
- `Popup`
- `Card`
- `Menu`
- `Tooltip`
- `Control`
- `Pill`
- `Hud`
- `WorkspaceIndicator`

**Acceptance criteria:**

- Each role is accessible from QML as `HnSurfaceRole.<Role>`.
- `HnSurfaceFrame.surfaceRole` defaults to `HnSurfaceRole.Card`.
- Unsupported integer values resolve to Card geometry and do not produce invalid dimensions.

### REQ-F-002: Expose corner styles

The module shall expose `Inherit`, `Hybrid`, `Rounded`, and `Chamfered` through `HnCornerStyle`.

**Acceptance criteria:**

- `HnSurfaceFrame.cornerStyleOverride` defaults to `HnCornerStyle.Inherit`.
- `Inherit` uses the component's profile style.
- An explicit override affects that component instance only.

### REQ-F-003: Resolve role geometry from a profile

The shared theme library shall expose `ShapeTokens`, `ShapeKind`, `CornerMask`, `ResolvedShape`, and
`ShapeResolver`. The `Holonight` QML module shall expose that resolver through a thin, process-local
`HnShapeProfile` singleton rather than maintaining a second QML-specific token table.

Primitive tokens shall use this initial scale:

| Token | Value |
|---|---:|
| Radius XS / S / M / L | 4 / 6 / 8 / 12 |
| Chamfer XS / S / M / L | 4 / 6 / 10 / 16 |

The initial profile shall use these defaults:

| Role | Radius | Chamfer |
|---|---:|---:|
| Window | 12 | 10 |
| Panel | 12 | 16 |
| Popup | 12 | 10 |
| Card | 8 | 4 |
| Menu | 6 | 4 |
| Tooltip | 6 | 4 |
| Control | 6 | 4 |
| Pill | half of the rendered height | 4 |
| Hud | 12 | 10 |
| Workspace indicator | 6 | 6 |

**Acceptance criteria:**

- The defaults are available without configuration I/O.
- A resolved result includes its shape kind, radius, chamfer, and chamfered-corner mask.
- The inherited Hybrid profile keeps cards and controls rounded, workspace indicators and panels chamfered,
  and major/pop-up surfaces hybrid.
- Hybrid surfaces chamfer the top-right and bottom-left signature corners.
- Explicit `Rounded` and `Chamfered` styles override the semantic shape kind while retaining role sizing.
- All effective dimensions are finite and non-negative.

### REQ-F-004: Render a reusable surface frame

The module shall export `HnSurfaceFrame`, an item that draws a filled surface and border using the resolved
role geometry.

**Acceptance criteria:**

- The component exposes `surfaceRole`, `cornerStyleOverride`, `radiusOverride`, and `chamferOverride`.
- `radiusOverride` and `chamferOverride` default to `NaN`; finite non-negative values override resolution.
- The component exposes `fillColor`, `borderColor`, and `borderWidth` with defaults from
  `HoloniightPalette.surface`, `HoloniightPalette.borderPassive`, and `HoloniightPalette.borderWidth`.
- `effectiveRadius` and `effectiveChamfer` are read-only and observable in QML tests.
- `shapeKind` and `chamferedCorners` expose the resolved kind and corner mask read-only.
- Geometry is clamped to the available width and height so small surfaces remain valid.
- A border is drawn inside the item bounds and is not clipped at fractional display scale.
- Changes to size, role, style, overrides, colors, or border width update the rendered frame.

### REQ-F-005: Support all style/role combinations safely

Every role shall instantiate under every corner style without QML warnings, binding loops, negative geometry,
or malformed paths.

**Acceptance criteria:**

- Parameterized tests cover all roles with Hybrid, Rounded, and Chamfered styles.
- Tests cover zero size, dimensions smaller than the requested geometry, and Pill at multiple heights.
- Tests cover valid overrides and confirm that `NaN` restores profile resolution.

### REQ-F-006: Preserve module compatibility

The new types shall be available through both `import Holonight` and the existing lowercase compatibility
module `import holonight`.

**Acceptance criteria:**

- Both imports instantiate `HnSurfaceFrame`.
- Existing exported controls and singletons continue to load.
- Installation includes the new QML files and generated type information.

## 5. Quality constraints

### REQ-C-001: Keep ownership boundaries explicit

The implementation shall contain no references to shell types, layer-shell protocols, compositor APIs, chat
types, or repository-relative imports outside `holonight-qt`.

### REQ-C-002: Avoid animated Canvas rendering

The implementation shall use Qt Quick scene-graph primitives suitable for resizing and property updates. It
shall not use a JavaScript `Canvas` paint loop for the shared frame.

### REQ-C-003: Add no external dependency

No third-party package shall be added. The implementation may add Qt Quick Shapes as an explicit first-party
Qt component because `HnSurfaceFrame` imports `QtQuick.Shapes`.

### REQ-C-004: Follow existing QML conventions

Imports shall be unversioned, bindings shall be declarative, and the implementation shall not mix anchors and
layout sizing on the same item.

### REQ-C-005: Keep the first cycle reviewable

Changes shall be limited to the shared theme library, QML module, focused tests, CMake/export metadata, and
this SDD. Appearance-file persistence and consuming-repository migrations are deferred.

## 6. Verification

- Build `holonight_qml` and `holonight_qml_smoke_tests`.
- Run the focused `QmlSmoke.HnSurface*` and `QmlSmoke.HnShapeProfile*` tests offscreen.
- Run the complete QML smoke-test binary offscreen.
- Run `qmllint` through the project's available build/lint integration when it covers the new files.
- Perform a visual demo check before a downstream shell migration, but do not block this headless foundation
  cycle on changes to another repository.

## 7. Follow-up SDD cycles

1. Add `HnAppearance` with shared JSON parsing, directory watching, change notification, profile, scale, and
   advanced base-radius/base-chamfer overrides.
2. Add `HnApplicationWindow` for shared title-bar and client-content layout while leaving the compositor in
   control of ordinary window outer geometry.
3. Refactor `holonight-shell`'s `HudFrame` into a compatibility wrapper around `HnSurfaceFrame` and retain
   shell-only effects there.
4. Adopt `HnSurfaceFrame` in HoloNight AI for its panel and inner window surfaces.
5. Migrate `HoloniightStyle` radius constants and relevant popup/frame paths to the shared `ShapeResolver`,
   keeping ordinary controls rounded.

## 8. Acceptance summary

This cycle is complete when semantic roles and styles are public, `HnSurfaceFrame` renders every supported
combination safely, local overrides work, both module import spellings export the API, focused tests pass, and
no shell- or application-specific behavior has moved into this repository.
