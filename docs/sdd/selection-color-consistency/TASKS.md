# Selection color consistency

Status: Proposed.
Date: 2026-09-30.
Scope: future palette audit; no palette behavior changes in this task.

KDE maps Selection BackgroundNormal to QPalette Highlight and, by default,
Accent; Selection ForegroundNormal becomes HighlightedText. Accent and Highlight
are independent roles even when their defaults match. See the upstream
[KColorScheme palette construction](https://github.com/KDE/kcolorscheme/blob/master/src/kcolorscheme.cpp).

Selection indicates chosen content; keyboard focus indicates the input target;
hover indicates the pointer target; alternate surfaces separate adjacent content.
BackgroundAlternate is not inherently a hover color. Cyan focus decoration beside
blue selection is an intentional HoloNight choice, not automatically a defect.

File-level WCAG relative-luminance contrast measurements for Colors:Selection
ForegroundNormal against BackgroundNormal are approximately 7.37:1 for Dark
and 4.15:1 for Light. Negative and visited foregrounds are approximately 1.01:1
against their configured selection backgrounds; other status foregrounds also
need review. These measurements concern the checked-in RGB values, not effective
Plasma palettes after user Accent overrides, tinting, inactive/disabled effects,
or application palette changes.

Sources: [Dark scheme](../../../data/holonight-dark.colors),
[Light scheme](../../../data/holonight-light.colors),
[palette construction](../../../palette/palette.cpp),
[WCAG contrast method](https://www.w3.org/TR/WCAG22/#dfn-contrast-ratio).
Accent overrides change primary without resolving onPrimary again; audit the
[appearance resolver](../../../src/theme/themeresolver.cpp) when designing the remediation.

- [ ] Audit every supported scheme/accent combination, including custom Accent,
      normal, selected, inactive and disabled states.
- [ ] Define readable selection foregrounds, including negative, visited, neutral
      and positive status colors, with explicit contrast targets.
- [ ] Resolve selection foregrounds deliberately after Accent overrides; preserve
      independent Accent/Highlight and focus/hover roles.
- [ ] Test generated KDE schemes alongside Qt/QML palettes, checking both file
      values and effective Plasma palettes after user overrides.
- [ ] Document intentional contrast exceptions and verify status meaning without
      relying on color alone.

Related completed work: [semantic icon recoloring](../kde-semantic-icon-recoloring/TASKS.md).
