# HoloNight Color Usage Guide

This guide defines how applications should use HoloNight colors so that independently built interfaces share the
same visual hierarchy, interaction states, and semantic meaning. Consume colors by **role**, not by copying a
color because it looks suitable. A role remains meaningful when the active HoloNight scheme changes.

The canonical implementation is `Holonight::ColorTokens` in `palette/holonight/palette.h`. QML applications use
the live `HoloniightPalette` singleton. The values below are the resolved values of the default
`holonight-dark` scheme.

## Core rules

1. Use the most specific semantic token available.
2. Preserve the surface elevation order: `background` → `surface` → `surfaceElevated` → `surfaceRaised`.
3. Use text and foreground tokens for content, never surface colors.
4. Use `primary` for selection and links; use `borderFocus`/`focusRing` only for keyboard focus.
5. Use `error`, `warning`, and `success` only for their semantic meanings.
6. Apply translucent overlay tokens over the existing surface; do not replace the surface with their raw RGB.
7. Do not lower the opacity of disabled text manually. Use `textDisabled` and, where useful, `disabledOverlay`.
8. New integrations should use canonical roles and avoid the deprecated compatibility aliases.

Hex values use `#RRGGBB` for opaque colors and `#RRGGBBAA` for colors with alpha.

## Default scheme token reference

### Surfaces

| Token | Default | Use for | Avoid using for |
| --- | --- | --- | --- |
| `background` | `#0C1118` | Application canvas and main window background | Cards, fields, or buttons |
| `surface` | `#131A24` | Views, editors, input interiors, recessed content | Floating popups |
| `surfaceElevated` | `#18212D` | Cards, alternate rows, elevated content groups | Primary actions |
| `surfaceRaised` | `#202B39` | Buttons, toolbars, panels, menus, popovers | Page background |
| `surfaceHover` | `#253243` | Opaque hover fill where an overlay is unsuitable | Selected or focused state |
| `surfaceSelected` | `#5EA2FF24` | Quiet selected fill for list, navigation, and choice controls | Filled primary actions |
| `surfaceSelectedHover` | `#5EA2FF33` | Hovered state for a quiet selected surface | Unselected hover state |
| `surfaceInverse` | `#F3F6FA` | Tooltips and deliberately inverse surfaces | General light panels |

Use adjacent levels for most nesting. Skipping several levels makes a surface look detached from the rest of the
application. A typical page uses `background`, a content view uses `surface`, cards use `surfaceElevated`, and
controls or floating UI use `surfaceRaised`.

### Text and content

| Token | Default | Use for |
| --- | --- | --- |
| `textPrimary` | `#E7EDF5` | Titles, body text, primary labels, normal monochrome icons |
| `textSecondary` | `#C5D0DE` | Supporting labels, metadata, descriptions |
| `textMuted` | `#8D99AD` | Placeholder text and inactive but still meaningful content |
| `textDisabled` | `#5B6678` | Unavailable actions and disabled fields |
| `textInverse` | `#0F141C` | Text and icons on `surfaceInverse` |
| `textAccent` | `#5EA2FF` | Accent-colored headings, labels, and monochrome icons |

Keep body copy at `textPrimary` or `textSecondary`. `textMuted` is intentionally lower emphasis and should not be
used for essential instructions. Disabled content is exempt from normal contrast requirements only when it is
truly unavailable.

### Primary action and selection

| Token | Default | Use for |
| --- | --- | --- |
| `primary` | `#5EA2FF` | Selection fill, active selection border, links, primary action fill |
| `primaryHover` | `#77B2FF` | Hovered primary action |
| `primaryPressed` | `#438CEB` | Pressed primary action |
| `onPrimary` | `#081018` | Text and icons placed on a primary fill |

For filled primary controls, use the complete foreground/background pair. Do not place `textPrimary` on
`primary`; use `onPrimary`.

### Borders and focus

| Token | Default | Use for |
| --- | --- | --- |
| `borderSubtle` | `#263241` | Hairlines, decorative dividers, low-emphasis grouping |
| `borderPassive` | `#36465A` | Normal control and frame borders, separators |
| `borderStrong` | `#53677F` | High-emphasis neutral boundaries |
| `borderHover` | `#56D7FF55` | Hover border overlay |
| `borderFocus` | `#56D7FF` | Keyboard focus border |
| `borderActive` | `#5EA2FF` | Current, selected, or active border |
| `borderUrgent` | `#FF718C` | Invalid, destructive, critical, or urgent border |
| `selectionIndicator` | `#5EA2FF` | High-emphasis selected edge, outline, or check indicator |

The standard border is 1 px (`borderWidth`). Keyboard focus is a distinct 2 px ring
(`focusBorderWidth`). Focus must remain visible independently of hover and selection. If a component is both
invalid and focused, keep the urgent/error identity and add a separate visible focus indication rather than
silently changing the error border to blue.

### Overlays and effects

| Token | Default | Alpha | Use for |
| --- | --- | ---: | --- |
| `glowCyanSoft` | `#56D7FF33` | 20% | Subtle cyan glow |
| `glowBlueSoft` | `#5EA2FF30` | 19% | Subtle primary-blue glow |
| `glowVioletSoft` | `#9A8CFF30` | 19% | Subtle special/tertiary glow |
| `scrim` | `#00000088` | 53% | Blocking modal backdrop |
| `glassTint` | `#18212DD8` | 85% | Translucent glass surface tint |
| `hoverOverlay` | `#56D7FF12` | 7% | Hover layer over the current surface |
| `pressedOverlay` | `#56D7FF22` | 13% | Pressed layer over the current surface |
| `disabledOverlay` | `#0C111880` | 50% | Optional disabled-state wash |
| `focusRing` | `#56D7FF` | 100% | Keyboard focus ring; intentionally matches `borderFocus` |
| `shadow` | `#000000` | 100% | Shadow base; set opacity and blur in the renderer |

Alpha tokens are compositing layers. For example, a hovered card retains `surfaceElevated` and draws
`hoverOverlay` above it. Do not convert `hoverOverlay` to an opaque fill because its result is designed to depend
on the underlying surface.

### Status and accents

| Token | Default | Use for |
| --- | --- | --- |
| `error` | `#FF718C` | Error messages, destructive actions, failed status |
| `warning` | `#F2C46B` | Caution and warning status |
| `success` | `#79D97F` | Completion, confirmation, healthy status |
| `onError` | `#081018` | Text/icons on a filled error surface |
| `accentCyan` | `#56D7FF` | General neon/cyan accent |
| `accentBlue` | `#5EA2FF` | Secondary blue accent; equal to `primary` in the default scheme |
| `accentViolet` | `#9A8CFF` | Special, tertiary, or workspace distinction |
| `accentYellow` | `#F2C46B` | Warm attention accent; equal to `warning` in the default scheme |

Token equality in one scheme does not make roles interchangeable. For example, `accentYellow` may decorate a
category, while `warning` communicates risk. Keep the semantic role in application code so another scheme can
change the values independently.

### Workspace fills

| Token | Default | Use for |
| --- | --- | --- |
| `workspaceOccupied` | `#18212D` | Occupied or urgent workspace pill background |
| `workspaceActive` | `#202B39` | Active workspace pill background |

Pair the fills with explicit active/urgent borders or content colors; the fill alone should not be the only state
signal.

### ANSI terminal palette

| Token | Default | Token | Default |
| --- | --- | --- | --- |
| `ansiBlack` | `#1A222D` | `ansiBrightBlack` | `#4B5B70` |
| `ansiRed` | `#F06A84` | `ansiBrightRed` | `#FF8CA0` |
| `ansiGreen` | `#74D88B` | `ansiBrightGreen` | `#99F2A9` |
| `ansiYellow` | `#EFC36B` | `ansiBrightYellow` | `#FFD98A` |
| `ansiBlue` | `#5EA2FF` | `ansiBrightBlue` | `#82BAFF` |
| `ansiMagenta` | `#A88CFF` | `ansiBrightMagenta` | `#C3AEFF` |
| `ansiCyan` | `#56D7FF` | `ansiBrightCyan` | `#8EE8FF` |
| `ansiWhite` | `#DCE6F2` | `ansiBrightWhite` | `#FFFFFF` |

Terminal integrations should preserve the normal/bright distinction. `ansi0` through `ansi15` are deprecated
aliases for these named tokens.

## Shared non-color metrics

The palette also exposes a small set of cross-control metrics:

| Token | Default | Use |
| --- | ---: | --- |
| `borderWidth` | `1` | Normal border |
| `focusBorderWidth` | `2` | Keyboard focus ring |
| `separatorWidth` | `1` | Divider or separator |
| `controlHeight` | `32` | Standard compact control height |
| `controlPadding` | `6` | Base internal control padding |

## Component recipes

| Component/state | Fill | Content | Border/layer |
| --- | --- | --- | --- |
| Application page | `background` | `textPrimary` | None or `borderSubtle` dividers |
| Content view/editor | `surface` | `textPrimary` | `borderPassive` |
| Card | `surfaceElevated` | `textPrimary` / `textSecondary` | `borderPassive` |
| Neutral button | `surfaceRaised` | `textPrimary` | `borderPassive` |
| Neutral button hover | Existing fill | `textPrimary` | `hoverOverlay` and/or `borderHover` |
| Neutral button pressed | Existing fill | `textPrimary` | `pressedOverlay` |
| Icon-only action, idle | Transparent | `textSecondary` | None |
| Icon-only action, hover / pressed | `surfaceHover` / `surface` | `textSecondary` / `primary` | Focus border when focused |
| Primary button | `primary` | `onPrimary` | `borderActive` if a border is needed |
| Quiet selected item | `surfaceSelected` | `textPrimary` | Optional `selectionIndicator` |
| Quiet selected item hover | `surfaceSelectedHover` | `textPrimary` | Optional `selectionIndicator` |
| Prominent filled selection | `primary` | `onPrimary` | `borderActive` |
| Text field | `surface` | `textPrimary`; placeholder `textMuted` | `borderPassive` |
| Focused field | `surface` | `textPrimary` | `borderFocus`, `focusBorderWidth` |
| Invalid field | `surface` | `textPrimary` | `borderUrgent`; message `error` |
| Menu/popover | `surfaceRaised` | `textPrimary` | `borderPassive` |
| Menu item, highlighted | `primary` | `onPrimary` for text and icons | None |
| Tooltip | `surfaceInverse` | `textInverse` | Usually none or `borderStrong` |
| Disabled control | Existing fill | `textDisabled` | Optional `disabledOverlay` |
| Modal backdrop | Existing application | Existing content | `scrim` above the application |

Treat icons as foreground content. Quiet icon-only actions use `textSecondary` at rest and `primary` while
pressed; ordinary menu icons follow `textPrimary`. Disabled icons use `textDisabled`, and icons on a filled
primary/highlighted state use `onPrimary`. Let the shared `HnIconButton` and HoloNight `MenuItem` style apply
these state colors; application code should use `icon.source` and override `icon.color` only when the icon
carries an intentional semantic status.

## Using the palette

### QML with the canonical HoloNight modules

Prefer live bindings so scheme reloads propagate:

```qml
import QtQuick
import Holonight.Core
import Holonight.Controls

HnSurfaceFrame {
    surfaceRole: HnSurfaceRole.Card
    fillColor: HoloniightPalette.surfaceElevated
    borderColor: activeFocus ? HoloniightPalette.borderFocus
                             : HoloniightPalette.borderPassive
    borderWidth: activeFocus ? HoloniightPalette.focusBorderWidth
                             : HoloniightPalette.borderWidth

    Text {
        text: qsTr("Connection settings")
        color: HoloniightPalette.textPrimary
    }
}
```

`HoloniightPalette` watches the resolved theme configuration and can also be refreshed explicitly with
`HoloniightPalette.reload()`. Bind to its properties instead of assigning resolved colors once in JavaScript.

### Qt/C++

Use the selected scheme's `ColorTokens` when code needs a semantic color directly:

```cpp
const Holonight::ColorTokens colors =
    Holonight::tokensForScheme(Holonight::ThemeSchemeKind::HoloNightDark);

painter.fillRect(contentRect, colors.surface);
painter.setPen(QPen{colors.borderPassive, colors.borderWidth});
```

For ordinary widgets, prefer the installed HoloNight style and its `QPalette`; this avoids duplicating control
state logic. The main `QPalette` mappings are:

| Qt role | HoloNight token |
| --- | --- |
| `Window` | `background` |
| `Base` | `surface` |
| `AlternateBase` | `surfaceElevated` |
| `Button` | `surfaceRaised` |
| `ToolTipBase` | `surfaceInverse` |
| `WindowText`, `Text`, `ButtonText` | `textPrimary` |
| `PlaceholderText` | `textMuted` |
| `ToolTipText` | `textInverse` |
| `Highlight`, `Link` | `primary` |
| `HighlightedText` | `onPrimary` |

### Other UI toolkits and web applications

Create one adapter at the application theme boundary. Keep HoloNight role names in the adapter, then map local
component variables to them:

```css
:root[data-theme="holonight-dark"] {
  --hn-background: #0c1118;
  --hn-surface: #131a24;
  --hn-surface-elevated: #18212d;
  --hn-surface-raised: #202b39;
  --hn-text-primary: #e7edf5;
  --hn-text-secondary: #c5d0de;
  --hn-border-passive: #36465a;
  --hn-border-focus: #56d7ff;
  --hn-primary: #5ea2ff;
  --hn-on-primary: #081018;
}
```

Hard-coded values are acceptable only at that adapter boundary. Components should reference the adapter's
semantic variables so changing schemes does not require rewriting component CSS or code.

## Review checklist

- Every color is referenced through a semantic role.
- Nested surfaces preserve a clear and restrained elevation sequence.
- Foregrounds use the matching `on…` or text token.
- Hover, press, focus, selection, disabled, and error states remain distinguishable.
- Keyboard focus uses the dedicated focus color and 2 px metric.
- Translucent tokens are composited instead of treated as opaque colors.
- Status colors communicate status rather than decoration alone.
- The interface remains understandable without relying only on color.
- No deprecated token is introduced in new code.
