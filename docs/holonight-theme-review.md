# HoloNight Qt/QML Theme Review

## Overall impression

The theme already looks promising, especially for an early work-in-progress stage. The strongest decisions so far are the separation between QStyle, QPlatformTheme, QQC2 module, and shared palette library. That gives the project a much better foundation than a purely visual QML-only theme.

## Main tips

### 1. Increase radius slightly

Current controls feel closer to a sharp KDE dark theme than a futuristic HoloNight sci-fi shell. Buttons and text fields would probably benefit from a slightly larger radius.

Suggested direction:

- Controls: 6–8 px
- Small panels/cards: 10–14 px
- Larger shell panels: 16–24 px

### 2. Use less cyan everywhere

Cyan is visually strong, so it should mostly communicate state: active, focused, selected, hovered, or primary action.

At the moment, cyan appears to be doing too much work: borders, headings, selections, progress, tabs, and outlines. This can make passive UI elements feel active.

Suggested hierarchy:

- Passive border: muted blue-gray
- Hover border: soft cyan
- Focus ring: bright cyan
- Active/selected: cyan + violet accent
- Critical/urgent: red/magenta

### 3. Make default borders darker

Passive frames and list views should probably use a darker outline. Keep bright cyan only for keyboard focus or important active states.

Suggested roles:

```text
Passive border: #565f89
Hover border:   #7dcfff with reduced opacity
Focus ring:     #00e0ff or #7dcfff
Active border:  cyan/violet gradient
```

### 4. Add more elevation

The screenshots are readable, but some surfaces visually blend together. Add a clearer distinction between:

- window background
- control background
- card background
- popup background
- tooltip background
- sidebar/panel background

You already have palette roles such as `surface`, `surfaceVariant`, `surfaceContainer`, `surfaceHover`, and `surfaceInverse`. Use them more aggressively to build visual hierarchy.

### 5. Architecture looks solid

Centralizing colors in the palette layer and using contrast tests is the right direction. This is especially important because the project will likely grow from a simple theme into a full shell visual system.

### 6. Consider normalizing the naming

Names like `HoloniightStyle`, `HoloniightTheme`, or `HoloniightPalette` may be funny or intentional, but the triple `i` can confuse contributors and users.

Unless this is deliberate branding, consider normalizing to:

```text
HoloNight
Holonight
```

### 7. Test with real applications soon

The demo is useful, but theme tuning should happen against real dense UIs too.

Good test apps:

- Dolphin
- Kate
- Qt Creator
- VLC or Haruna
- System Settings
- a random Qt configuration dialog

Themes often look good in component demos but reveal spacing, contrast, and density issues in real applications.

## Biggest correction

Reduce passive cyan borders.

That one change would probably make the theme feel more premium, less wireframe-like, and closer to the HoloNight design direction.

## Suggested next steps

1. Define strict token semantics: passive, hover, active, focus, urgent.
2. Reduce cyan usage in passive UI.
3. Increase radius slightly.
4. Add stronger surface elevation.
5. Test against real Qt/KDE applications.
6. Keep shell-specific icons separate from application icons.
7. Start documenting visual rules while the theme is still small.
