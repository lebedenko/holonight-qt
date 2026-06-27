# HoloNight Qt Theme Improvement Requirements

## Purpose

`holonight-qt` is the Qt theme layer for the broader HoloNight desktop system. It must provide a coherent visual foundation for Qt Widgets, Qt Quick Controls, platform theme integration, and HoloNight Shell usage.

This document captures the improvement requirements needed for `holonight-qt` to become the shared theme-token source consumed by `holonight-shell`, future `holonight-settings`, and optional app-theme exporters.

## Scope

In scope:

- palette token model
- theme configuration schema
- Qt `QPalette` generation
- Qt Widgets style plugin
- Qt Platform Theme plugin
- Qt Quick Controls module
- QML-accessible theme and palette singletons
- future compatibility with shell live theming
- future compatibility with terminal/editor theme exporters

Out of scope for this repository:

- shell runtime file watching
- shell IPC preview/apply/revert APIs
- direct editing of external app config files
- GTK, KDE, icon, and cursor theme implementation
- HoloNight Settings UI

## Product requirements

### R1. Own the canonical theme token model

`holonight-qt` must own the canonical theme token definitions used by Qt applications and the shell.

All visual colors must originate from the palette/theme token layer. QML controls, widget style painting, platform theme palette construction, and generated color-scheme files must not introduce unrelated hardcoded colors.

Current anchor:

- `palette/holonight/palette.h`
- `palette/palette.cpp`
- `data/holonight.colors`

Required outcome:

- one canonical token model with explicit semantic roles
- stable role names that can be consumed by `holonight-shell`
- no duplicate or conflicting token schemas across style, platform theme, and QML

### R2. Separate user intent from resolved tokens

Configuration must distinguish between user choices and resolved visual values.

User intent examples:

```json
{
  "appearance": {
    "mode": "dark",
    "colorScheme": "storm",
    "accent": "cyan"
  },
  "effects": {
    "transparency": 0.82,
    "blurStrength": 18
  },
  "fonts": {
    "ui": "Inter",
    "fixed": "JetBrains Mono",
    "baseSize": 10
  }
}
```

Resolved token examples:

```cpp
background = "#10131f";
surface = "#161925";
surfaceElevated = "#1a1b26";
textPrimary = "#c0caf5";
accentPrimary = "#7dcfff";
borderPassive = "#3b4261";
```

Qt/QML code must consume resolved tokens, not raw config decisions such as `mode == dark`.

### R3. Preserve compatibility with current config while preparing the shared schema

The repository currently loads:

```text
~/.config/holonight/theme.json
~/.config/holonight/theme.conf
```

The broader HoloNight system is expected to use:

```text
~/.config/holonight/config.toml
```

Requirements:

- keep existing `theme.json` and `theme.conf` support until a deliberate migration is implemented
- define the future shared schema in a way that can coexist with the current config loader
- avoid breaking existing environment overrides used for testing and development
- document precedence between shared config, legacy config, and environment variables before implementation

Recommended precedence for a future migration:

```text
defaults
legacy theme config, if present
shared holonight config, if present
environment overrides
explicit test override path
```

### R4. Expose live-capable QML theme properties

The current `HolonightTheme` singleton exposes config properties as constants. For future shell/settings integration, color and metric theme properties must be capable of emitting change notifications.

Requirements:

- expose resolved colors and metrics to QML through singleton APIs
- use `NOTIFY` signals for properties that may change at runtime
- keep truly static or startup-only values clearly marked
- ensure existing QQC2 controls can bind to theme tokens without manual refresh

Examples of live-capable values:

- accent color
- color scheme
- text colors
- border colors
- radius tokens
- opacity/transparency
- font sizes, if supported live
- control spacing metrics, if supported live

Examples of likely startup-only values:

- QML import paths
- platform theme selection
- icon theme inheritance
- font discovery paths

### R5. Refine surface token taxonomy

The current surface values are good, but role names should move toward clearer shell and app semantics.

Required target semantics:

```cpp
background       = #10131f
surface          = #161925
surfaceElevated  = #1a1b26
surfaceRaised    = #24283b
surfaceHover     = #1e2233
surfaceInverse   = #0d1117
```

Requirements:

- define whether existing names are retained, deprecated, or aliased
- preserve compatibility for existing tests and consumers during transition
- map each surface role to its intended Qt `QPalette` role
- document where each surface tier is used in QML controls and widget painting

Suggested mapping:

```text
background      -> QPalette::Window
surface         -> QPalette::Base
surfaceElevated -> cards, grouped containers, inline raised areas
surfaceRaised   -> menus, popups, detached panels, buttons where appropriate
surfaceHover    -> hover-state filled surfaces
surfaceInverse  -> tooltips and strong pop-out surfaces
```

### R6. Refine text token taxonomy

The current gap between secondary text and disabled/placeholder text is too large.

Required text roles:

```cpp
textPrimary   = #c0caf5
textSecondary = #a9b1d6
textMuted     = #737aa2
textDisabled  = #3b3f58
```

Requirements:

- reserve muted/disabled colors for non-essential or exempt text
- keep WCAG contrast tests for normal text pairs
- document any intentional WCAG exemptions
- update QML controls and widget style paths to use the correct text role

### R7. Refine border token taxonomy

Passive borders should be less visually dominant than focus or active borders.

Required border roles:

```cpp
borderSubtle  = #2f354d
borderPassive = #3b4261
borderStrong  = #565f89
borderHover   = #7dcfff with reduced alpha
borderActive  = #7aa2f7
borderFocus   = #00e0ff
borderUrgent  = #f7768e
```

Requirements:

- reserve `borderFocus` for real keyboard focus
- use `borderActive` for selected/current state
- use `borderSubtle` or `borderPassive` for idle frames and separators
- avoid passive cyan borders
- keep separator and frame rendering consistent across Qt Widgets
- update tests for contrast and expected drawing behavior

### R8. Align primary and accent semantics

The current `primary` value is more system-blue than HoloNight accent-blue/cyan.

Requirements:

- define whether primary represents selection fill, primary action, or selected control state
- align `primary` with the HoloNight accent family
- keep `#00e0ff` for focus-ring usage, not general active/selected state

Preferred calmer option:

```cpp
primary = #7aa2f7
primaryHover = #89b4fa
primaryPressed = #5f7fe7
onPrimary = #10131f
```

Alternative stronger cyber option:

```cpp
primary = #00c2e8
primaryHover = #4de9ff
primaryPressed = #009db2
onPrimary = #10131f
```

The selected option must pass existing contrast requirements.

### R9. Add explicit glow, glass, overlay, and scrim tokens

HoloNight Shell uses translucent and layered surfaces. The Qt theme should define these values intentionally instead of allowing every consumer to invent them.

Required token families:

```cpp
glowCyanSoft
glowBlueSoft
glowVioletSoft
scrim
glassTint
hoverOverlay
pressedOverlay
disabledOverlay
```

Requirements:

- alpha values must be part of the token contract where relevant
- tokens must document intended usage and non-usage
- Qt Widgets style must avoid excessive glow effects in dense app UIs
- QML controls may use glow/overlay tokens where appropriate

### R10. Add radius and metric tokens

The theme needs shared radius and spacing semantics instead of per-control magic numbers.

Required radius direction:

```text
small controls: 6-8 px
small panels/cards: 10-14 px
large shell panels: 16-24 px
```

Requirements:

- expose radius tokens to QML
- use consistent radius values across QQC2 controls
- keep Qt Widgets drawing compatible with native widget expectations
- avoid making dense desktop applications look like shell panels

### R11. Add canonical ANSI palette tokens

Terminal/exporter support must use intentional ANSI colors rather than deriving them later from UI tokens.

Required standard ANSI colors:

```text
black   #1d202f
red     #f7768e
green   #9ece6a
yellow  #e0af68
blue    #7aa2f7
magenta #bb9af7
cyan    #7dcfff
white   #c0caf5
```

Required bright ANSI colors:

```text
brightBlack   #565f89
brightRed     #ff899d
brightGreen   #b9f27c
brightYellow  #ffd27d
brightBlue    #8cb4ff
brightMagenta #d0a8ff
brightCyan    #a4daff
brightWhite   #ffffff
```

Required terminal base colors:

```text
background    #10131f
foreground    #c0caf5
cursor        #7dcfff
cursorText    #10131f
selection     #33467C
selectionText #c0caf5
```

Requirements:

- expose ANSI colors in the token model
- keep names semantic, not only `ansi0` through `ansi15`
- avoid using `#00e0ff` as terminal cursor
- prepare the model for exporters such as Kitty, Ghostty, Alacritty, and WezTerm

### R12. Support curated HoloNight theme variants

The theme should eventually support a small curated set of HoloNight-native variants, not a large collection of unrelated direct ports.

Required variants:

```text
Storm    - Tokyo Night inspired default
Aurora   - Nord inspired
Mocha    - Catppuccin inspired
Ember    - Gruvbox inspired
Eclipse  - One Dark inspired
Neon     - Dracula inspired
```

Requirements:

- every variant must implement the same semantic token schema
- every variant must provide ANSI colors
- every variant must define dark/light support status explicitly
- direct third-party palette ports should be avoided unless license and cohesion are documented
- `Storm` must remain the default until another default is deliberately chosen

### R13. Provide robust Qt `QPalette` generation

`buildPalette()` must map semantic tokens to Qt roles consistently.

Requirements:

- active and inactive groups should remain equivalent unless a specific reason is documented
- disabled group colors should be derived predictably and tested
- highlight/highlighted-text pairs must meet contrast requirements
- tooltip, placeholder, link, visited-link, button, base, and window roles must be documented
- no `QPalette` role should depend on ad hoc colors outside the token model

### R14. Keep Qt Widgets behavior close to KDE/Fusion structure

The QStyle plugin should apply HoloNight colors while preserving normal Qt/KDE widget structure.

Requirements:

- do not invent extra generic frames or panel backgrounds
- avoid broad ancestor traversal and broad app-specific heuristics
- separators must use one consistent logical-pixel rule
- text edit, line edit, frame, splitter, and tool-button behavior must be tested
- flat idle tool buttons should not receive unnecessary borders
- focus indication must be deliberate and keyboard-focused

Existing reference:

- `docs/qt-widget-style-refactor-plan.md`

### R15. Keep QQC2 controls token-driven

The QML controls in `qml/` must consume the shared token model.

Requirements:

- Button, CheckBox, RadioButton, ComboBox, Slider, Switch, ProgressBar, TabBar, TabButton, Menu, MenuItem, ToolTip, ScrollBar, and TextField must use semantic tokens
- active, hover, pressed, disabled, checked, selected, error, and focus states must use explicit state tokens
- controls must avoid raw hex colors
- controls must expose consistent radius, spacing, border, and text behavior
- singleton changes must be bindable without recreating controls when values are marked live-capable

### R16. Keep platform theme integration focused

The platform theme should configure Qt application defaults without becoming the runtime owner of shell state.

Requirements:

- continue to provide icon theme, fallback icon theme, UI font, fixed font, and base font size
- apply the generated `QPalette`
- avoid shell-specific assumptions in the platform theme
- document which changes require application restart
- preserve environment override support for development and testing

### R17. Prepare for generated external app theme files

`holonight-qt` should own enough tokens for external exporters, but it should not directly edit user app configs.

Requirements:

- provide token data suitable for exporters
- avoid direct writes to Kitty, Ghostty, Alacritty, WezTerm, Neovim, VS Code, GTK, or KDE configs from this core theme layer
- if exporter code is added later, generate files under `~/.config/holonight/generated/`
- require per-app opt-in outside this repository's core theme path

## Compatibility requirements

### C1. Preserve existing activation paths

Existing activation must continue to work:

```bash
QT_QPA_PLATFORMTHEME=holonight your-qt-app
QT_STYLE_OVERRIDE=holonight your-qt-app
```

The `Holonight` QML module must continue to import as:

```qml
import Holonight
```

The lowercase compatibility alias should remain unless a separate deprecation plan exists.

### C2. Preserve intentional naming unless a migration is planned

The repository currently uses intentional class names with three `i` characters, such as `HoloniightStyle` and `HoloniightPalette`.

Requirements:

- do not rename public classes casually
- if names are normalized later, provide compatibility aliases or a migration plan
- avoid mixing new public naming conventions without documentation

### C3. Preserve testability without installed plugins

Tests must continue to compile plugin sources directly and run headlessly.

Requirements:

- no test should require a system install
- no test should require a live compositor
- `QT_QPA_PLATFORM=offscreen` should remain sufficient for automated tests

## Documentation requirements

The following must be documented before or alongside implementation:

- canonical token schema
- config schema and precedence
- legacy config migration behavior
- surface, text, border, accent, status, overlay, radius, and ANSI role meanings
- Qt `QPalette` role mapping
- live-capable vs startup-only settings
- intentional contrast deviations
- per-theme variant status

Existing docs that may need updates:

- `README.md`
- `docs/holonight-design-deviations.md`
- `docs/holonight-theme-review.md`
- `docs/holonight-effects.md`
- `docs/qt-widget-style-refactor-plan.md`

## Testing requirements

Palette tests must cover:

- token presence and expected defaults
- text/background contrast
- selection contrast
- border/focus contrast where applicable
- ANSI palette presence
- light/dark or variant-specific invariants once variants exist

QPalette tests must cover:

- role mapping for active, inactive, and disabled groups
- highlight/highlighted-text contrast
- tooltip colors
- placeholder colors
- link colors

QML tests must cover:

- singleton property exposure
- QQC2 component smoke tests
- no raw color regressions where practical
- live-capable property notification behavior once implemented

Style tests must cover:

- separators
- splitter rendering
- text-edit and line-edit frames
- flat tool-button idle behavior
- focus ring behavior
- scale-safe frame geometry

Manual verification should include real applications:

- Dolphin
- Kate
- Qt Creator
- System Settings
- VLC or Haruna
- dense Qt configuration dialogs

## Acceptance criteria

The improvement effort is acceptable when:

- all theme colors used by Qt style, platform theme, and QML originate from semantic tokens
- `holonight-qt` exposes a canonical token model usable by `holonight-shell`
- current config loading remains compatible or has a documented migration
- QML has bindable theme properties for values intended to change live
- the palette taxonomy includes refined surface, text, border, accent, overlay, radius, and ANSI roles
- `Storm` remains a complete default theme
- new variants can be added without changing consumer code
- tests validate contrast, QPalette mapping, QML exposure, and widget rendering behavior
- external app theme sync remains optional and generated-file based
- README and design docs describe the new token/config contract clearly

## Recommended delivery phases

1. Document the canonical token schema and config precedence.
2. Refine the existing `Storm` token taxonomy while preserving compatibility aliases.
3. Update `QPalette` mapping and contrast tests.
4. Update QML singletons and QQC2 controls to consume the refined tokens.
5. Update QStyle rendering paths to use refined border/surface semantics.
6. Add ANSI palette tokens for `Storm`.
7. Add live-capable QML properties with change notifications where supported.
8. Add theme variant infrastructure.
9. Add additional curated variants.
10. Prepare exporter-facing token serialization without writing external app configs.

