# HoloNight Frame and Shape Usage Guide

This guide defines the shared geometry and framing rules that make HoloNight applications feel related even when
their layouts differ. A frame is a semantic surface boundary: its role determines corner geometry, while palette
roles determine fill, border, and state.

QML applications should use `HnSurfaceFrame`. C++ and other toolkit integrations should reproduce the same
semantic role table through `Holonight::ShapeResolver` or an equivalent adapter.

## Design principles

1. Select shape by semantic surface role, not by a local radius literal.
2. Let the global appearance profile choose rounded, chamfered, or hybrid geometry.
3. Use hybrid corners for HoloNight's signature major and floating surfaces.
4. Keep controls and cards visually quieter than panels, HUDs, and popups.
5. Use pills only for compact labels, filters, status, and workspace indicators that genuinely need capsule
   geometry.
6. Clamp corner dimensions to half the shortest side so small frames remain valid.
7. The compositor owns the physical top-level window frame. Applications frame content, not the native window.

## Shape vocabulary

| Shape kind | Geometry | Intended character |
| --- | --- | --- |
| Rounded | All four corners use curves | Quiet, familiar content and controls |
| Chamfered | All four corners use straight diagonal cuts | Structural, technical, high-definition surfaces |
| Hybrid | Top-right and bottom-left are chamfered; the others are rounded | Signature HoloNight major/floating surface |
| Pill | Radius is half the rendered height | Compact capsule indicator or action |

Corner-mask values exposed to QML are:

| Token | Value |
| --- | ---: |
| `HnCornerMask.Inherit` | `-1` |
| `HnCornerMask.None` | `0` |
| `HnCornerMask.TopLeft` | `1` |
| `HnCornerMask.TopRight` | `2` |
| `HnCornerMask.BottomRight` | `4` |
| `HnCornerMask.BottomLeft` | `8` |
| `HnCornerMask.All` | `15` |

`Inherit` keeps the topology selected by the semantic role and effective corner style. Values `0`–`15` replace
that topology, and flags may be combined for any per-corner arrangement. Values outside `-1` and `0`–`15` are
invalid and fall back to the resolved topology. The standard hybrid mask is `TopRight | BottomLeft` (`10`).

## Primitive shape scale

These values are the default geometry primitives, in device-independent pixels:

| Primitive | Value | Primitive | Value |
| --- | ---: | --- | ---: |
| Radius XS | `4` | Chamfer XS | `4` |
| Radius S | `6` | Chamfer S | `6` |
| Radius M | `8` | Chamfer M | `10` |
| Radius L | `12` | Chamfer L | `16` |

Primitive names are an implementation scale, not component API. Applications should select a semantic role
instead of selecting XS/S/M/L.

## Semantic frame roles

The table describes the default `Inherit` profile at `shapeScale: 1`.

| Role | Default kind | Radius | Chamfer | Use for |
| --- | --- | ---: | ---: | --- |
| `HnSurfaceRole.Window` | Hybrid | `12` | `10` | Major in-app window-like canvas, embedded application shell |
| `HnSurfaceRole.Panel` | Chamfered | `12` | `16` | Structural sidebars, dashboards, prominent persistent panels |
| `HnSurfaceRole.Popup` | Hybrid | `12` | `10` | Popovers, combo popup containers, transient floating panels |
| `HnSurfaceRole.Card` | Rounded | `8` | `4` | Cards and ordinary elevated content groups |
| `HnSurfaceRole.Menu` | Hybrid | `6` | `4` | Context and application menus |
| `HnSurfaceRole.Tooltip` | Hybrid | `6` | `4` | Tooltips and brief contextual help |
| `HnSurfaceRole.Control` | Rounded | `6` | `4` | Buttons, fields, selectors, control backgrounds |
| `HnSurfaceRole.Pill` | Pill | half the height | `4` | Tags, filters, compact status and capsule actions |
| `HnSurfaceRole.Hud` | Hybrid | `12` | `10` | Heads-up displays and prominent transient overlays |
| `HnSurfaceRole.WorkspaceIndicator` | Chamfered | `6` | `6` | Workspace and compact navigation indicators |

For rounded roles, the chamfer value is retained only so a global or local chamfered override has appropriate
role sizing. The reverse is true for radius values on chamfered roles.

### Choosing a role

- Choose `Card` for most bounded content. It should be the default when no stronger structural meaning exists.
- Choose `Panel` for a persistent region that organizes a substantial part of the interface.
- Choose `Popup`, `Menu`, or `Tooltip` according to transient behavior rather than visual size alone.
- Choose `Control` for an interactive control's own boundary, not for a group containing many controls.
- Choose `Hud` for a prominent overlay that reports live state or presents immediate actions.
- Choose `Window` only for an embedded top-level-like surface. Do not use it to replace compositor decoration.
- Choose `Pill` sparingly. A page full of capsules weakens hierarchy and makes actions hard to scan.
- Choose `WorkspaceIndicator` for its specific navigation pattern; do not use it as a generic small card.

## Color and border pairing

Geometry and color roles are independent. Recommended defaults are:

| Frame role | Typical fill | Typical border |
| --- | --- | --- |
| Window | `background` or `surface` | `borderPassive` if embedded |
| Panel | `surfaceRaised` | `borderPassive` or `borderStrong` |
| Popup / Menu | `surfaceRaised` | `borderPassive` |
| Card | `surfaceElevated` | `borderPassive` |
| Tooltip | `surfaceInverse` | `borderStrong` or no border |
| Control | `surface` or `surfaceRaised` | State-dependent semantic border |
| Pill | `surfaceRaised` or a semantic fill | `borderPassive` / `borderActive` |
| Hud | `glassTint` over appropriate backing | `borderActive` or `borderStrong` |
| Workspace indicator | `workspaceOccupied` / `workspaceActive` | State-dependent semantic border |

See [Theme Colors Usage](theme-colors-usage.md) for state and foreground pairings. Normal frames use the 1 px
`borderWidth`; keyboard focus uses the 2 px `focusBorderWidth`.

## `HnSurfaceFrame` API

`HnSurfaceFrame` is exported by `import Holonight.Controls`. Its palette, shape, and appearance dependencies are
exported by `import Holonight.Core`.

### Inputs

| Property | Default | Meaning |
| --- | --- | --- |
| `surfaceRole` | `HnSurfaceRole.Card` | Semantic role used to resolve geometry |
| `cornerStyleOverride` | `HnCornerStyle.Inherit` | Per-instance shape-style override |
| `radiusOverride` | `NaN` | Finite non-negative per-instance radius override |
| `chamferOverride` | `NaN` | Finite non-negative per-instance chamfer override |
| `chamferedCornersOverride` | `HnCornerMask.Inherit` | Per-instance topology mask; combine corner flags as needed |
| `fillColor` | `HoloniightPalette.surface` | Frame fill |
| `borderColor` | `HoloniightPalette.borderPassive` | Frame stroke |
| `borderWidth` | `HoloniightPalette.borderWidth` | Frame stroke width; negative values normalize to zero |
| `contentData` | empty | Default child-content slot |

### Resolved read-only values

| Property | Meaning |
| --- | --- |
| `shapeKind` | `HnShapeKind.Rounded`, `Chamfered`, `Hybrid`, or `Pill` |
| `effectiveRadius` | Resolved, scaled, overridden, and geometry-clamped radius |
| `effectiveChamfer` | Resolved, scaled, overridden, and geometry-clamped chamfer |
| `chamferedCorners` | Resolved `HnCornerMask` flags |
| `normalizedBorderWidth` | `max(0, borderWidth)` |
| `frameInset` | Half the normalized border width |
| `drawableWidth`, `drawableHeight` | Size available after accounting for the inset stroke |

The stroke is centered on a path inset by half its width. This keeps the border inside the item bounds and avoids
clipping, including at fractional display scales.

## QML examples

### Standard card

```qml
import QtQuick
import QtQuick.Layouts
import Holonight.Core
import Holonight.Controls

HnSurfaceFrame {
    width: 360
    height: content.implicitHeight + 32
    surfaceRole: HnSurfaceRole.Card
    fillColor: HoloniightPalette.surfaceElevated
    borderColor: HoloniightPalette.borderPassive

    ColumnLayout {
        id: content
        anchors.fill: parent
        anchors.margins: 16
        spacing: 8

        Text {
            text: qsTr("Audio output")
            color: HoloniightPalette.textPrimary
        }

        Text {
            text: qsTr("Choose the device used for application sound.")
            color: HoloniightPalette.textSecondary
        }
    }
}
```

`HnSurfaceFrame` does not add content padding or clip children automatically. Set margins explicitly and enable
`clip` only when the product behavior requires it.

### Interactive control frame

```qml
HnSurfaceFrame {
    id: controlFrame

    surfaceRole: HnSurfaceRole.Control
    fillColor: press.pressed ? HoloniightPalette.surfaceHover
                             : HoloniightPalette.surfaceRaised
    borderColor: activeFocus ? HoloniightPalette.borderFocus
                             : hover.hovered ? HoloniightPalette.borderHover
                                             : HoloniightPalette.borderPassive
    borderWidth: activeFocus ? HoloniightPalette.focusBorderWidth
                             : HoloniightPalette.borderWidth

    HoverHandler {
        id: hover
    }

    TapHandler {
        id: press
    }
}
```

For standard Qt Quick Controls, prefer the controls exported by the HoloNight module. Use a custom
`HnSurfaceFrame` when the application owns the component behavior.

### Translucent HUD

```qml
HnSurfaceFrame {
    surfaceRole: HnSurfaceRole.Hud
    fillColor: HoloniightPalette.glassTint
    borderColor: HoloniightPalette.borderActive
}
```

The glass tint supplies translucency but not compositor blur. Do not assume blur or glow support in the shared
frame API.

### Explicit local override

```qml
HnSurfaceFrame {
    surfaceRole: HnSurfaceRole.Panel
    cornerStyleOverride: HnCornerStyle.Rounded
    radiusOverride: 10
}
```

Use local overrides only when the surface has a real product-specific geometry requirement. `Inherit` is the
preferred value because it responds to the user's global appearance profile.

### Directional and adjacent panels

For docked layouts, keep exterior corners rounded and point chamfers toward internal seams:

```qml
HnSurfaceFrame {
    surfaceRole: HnSurfaceRole.Panel
    chamferedCornersOverride: HnCornerMask.TopRight | HnCornerMask.BottomRight
}
```

Use `TopRight | BottomRight` for a left panel and `TopLeft | BottomLeft` for a right panel. Any combination is
valid, including one chamfer with three rounded corners or three chamfers with one rounded corner. Keep central
content quieter with `Card` or inherited `Window` geometry. Use passive borders for normal adjacency and reserve
bright borders for active or focused states.

The existing `Panel` role intentionally retains its 16 px chamfer scale. Per-corner masks solve adjacency without
adding directional `DockLeft` or `DockRight` roles.

## Resolution and override precedence

Geometry resolves in this order:

1. Select the primitive radius/chamfer sizes for the semantic role.
2. Apply `shapeScale` from the global appearance configuration.
3. If configured, replace all scaled radii with `baseRadius` and all scaled chamfers with `baseChamfer`.
4. Resolve `cornerStyleOverride`: an explicit component style wins; `Inherit` uses the configured global style.
5. Apply finite, non-negative component `radiusOverride` and `chamferOverride`.
6. Clamp both dimensions to half the shortest drawable side.
7. Apply `chamferedCornersOverride`: `Inherit` or an invalid value keeps the resolved mask; `0`–`15` replaces it.
8. Classify the final topology: zero chamfers is `Rounded`, all four is `Chamfered`, and a mixed mask is `Hybrid`.
   An all-rounded `Pill` remains `Pill`.

Unknown role integers fall back to `Card`; unknown style integers fall back to `Inherit`. A width or height of
zero resolves both dimensions to zero. Negative, infinite, and `NaN` component overrides are ignored.

## Global appearance configuration

The default configuration path is:

```text
~/.config/holonight/appearance.json
```

Set `HOLONIGHT_APPEARANCE_FILE` to use another file. The implemented JSON format is:

```json
{
  "version": 1,
  "cornerStyle": "inherit",
  "shapeScale": 1.0
}
```

Optional advanced values:

```json
{
  "version": 1,
  "cornerStyle": "hybrid",
  "shapeScale": 1.0,
  "baseRadius": 8,
  "baseChamfer": 10
}
```

| Field | Accepted values | Default |
| --- | --- | --- |
| `version` | `1`; another explicit version rejects the file | No version requirement when omitted |
| `cornerStyle` | `inherit`, `hybrid`, `rounded`, `chamfered` | `inherit` |
| `shapeScale` | `0.25`–`4.0` | `1.0` |
| `baseRadius` | `0`–`128` | Unset |
| `baseChamfer` | `0`–`128` | Unset |

`HnAppearance` watches the file and its directories and emits live changes. `HnSurfaceFrame` includes the
appearance revision in its binding, so existing instances re-resolve automatically. Applications may call
`HnAppearance.reload()` after changing the file themselves.

`baseRadius` and `baseChamfer` intentionally flatten the corresponding primitive scale. They are advanced
accessibility or product-profile controls, not a substitute for semantic role selection.

## Using the resolver outside `HnSurfaceFrame`

### QML

Use `HnAppearance.roundedRadius()` for Qt Quick items that only support a uniform rounded `radius`:

```qml
Rectangle {
    readonly property int appearanceRevision: HnAppearance.revision
    radius: HnAppearance.roundedRadius(HnSurfaceRole.Control, width, height,
                                       appearanceRevision)
}
```

Passing the revision creates an explicit binding dependency. For full hybrid or chamfered geometry, use
`HnSurfaceFrame`; a `Rectangle` cannot reproduce those paths.

`HnAppearance.resolve()` returns a map containing `kind`, `radius`, `chamfer`, and `corners` when custom drawing
is unavoidable. `HnShapeProfile.resolve()` exposes the unconfigured default resolver; application-facing code
should normally use `HnAppearance` so global preferences are honored.

### C++

```cpp
const Holonight::ResolvedShape shape = Holonight::ShapeResolver::resolve(
    Holonight::SurfaceRole::Popup,
    Holonight::CornerStyle::Inherit,
    QSizeF{width, height},
    qQNaN(),
    qQNaN(),
    static_cast<int>(Holonight::Corner::TopRight | Holonight::Corner::BottomRight));
```

Build paths from `shape.kind` and `shape.chamfered_corners`: rounded corners use curves, and chamfered corners use
straight diagonal segments. Do not infer topology from the radius or chamfer value alone.

### Other UI toolkits

Create a central shape resolver with the same role table and override order. At render time:

1. Inset the path by half the border width.
2. Compute the drawable width and height after the full border width.
3. Clamp radius and chamfer to half the shorter drawable dimension.
4. Start at the top-left edge and trace clockwise.
5. Draw a straight diagonal at corners selected by the chamfer mask; draw a quadratic or equivalent curve at
   the other corners.
6. Close and fill the path, then stroke it using round joins.

Keep geometry in logical/device-independent pixels and let the toolkit perform device scaling.

## Top-level application windows

Use `HnApplicationWindow` for a consistent application canvas and optional in-app header:

```qml
import QtQuick
import Holonight
import Holonight.Core
import Holonight.Controls

HnApplicationWindow {
    width: 960
    height: 640
    visible: true
    title: qsTr("Mixer")

    applicationHeaderVisible: true
    applicationHeaderHeight: 40
    contentPadding: 16

    applicationHeaderTrailingData: Button {
        text: qsTr("Settings")
    }

    HnSurfaceFrame {
        anchors.fill: parent
        surfaceRole: HnSurfaceRole.Panel
        fillColor: HoloniightPalette.surfaceElevated
    }
}
```

`HnApplicationWindow` deliberately remains a normally decorated Qt `Window`. It does not set
`Qt.FramelessWindowHint`, draw an inner outer-window border, provide native window buttons, change the native
input region, or request compositor blur/shadows. The window manager or compositor owns the physical title bar,
outer rounding, resizing, movement, shadow, and window controls. Drawing a second application frame around the
whole client area creates a doubled border and should be avoided.

Its shared layout API is:

| Property | Default | Meaning |
| --- | --- | --- |
| `applicationHeaderVisible` | `false` | Enables the in-app header |
| `applicationHeaderHeight` | `40` | Requested header height; effective value is non-negative |
| `contentPadding` | `0` | Padding around client content; effective value is non-negative |
| `applicationHeaderColor` | `HoloniightPalette.surface` | Header background |
| `applicationHeaderLeadingData` | empty | Leading header slot |
| `applicationHeaderCenterData` | window title | Center header slot |
| `applicationHeaderTrailingData` | empty | Trailing header slot |
| `contentData` | empty | Default client-content slot |

## Common mistakes

- **Drawing a frame around the native window:** duplicates compositor decoration. Frame internal surfaces only.
- **Using `radius: 6` throughout QML:** prevents global appearance changes and loses semantic hierarchy.
- **Forcing every surface to chamfered or hybrid locally:** overwhelms the signature geometry and ignores user
  preference.
- **Using `Rectangle` for a hybrid frame:** it cannot draw the required alternating corner topology.
- **Using `radiusPill: 999`:** the legacy sentinel is deprecated; the semantic pill radius is half the height.
- **Applying an unclamped corner size:** can create overlapping or malformed paths on small items.
- **Clipping all frame children:** can cut focus rings, shadows, menus, and intentional overflow.
- **Using color to imply elevation without a role:** pair semantic frame geometry with semantic palette surfaces.

## Review checklist

- Every bounded surface has the narrowest appropriate semantic role.
- Cards and controls are rounded by default; major/floating surfaces use their inherited profile.
- Local style and size overrides have a documented product reason.
- Content padding is explicit and children are clipped only when required.
- Border paths are inset and corner dimensions are clamped.
- Focus uses the dedicated focus border and remains visible.
- Appearance configuration changes propagate to existing components.
- Top-level native decoration remains the compositor's responsibility.
- Portable integrations reproduce both the role table and the override precedence.
