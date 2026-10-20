# Design: shared header close button

Two applications need the same compact dimensions, interaction colors, focus
outline and keyboard activation. HnHeaderCloseButton is a reusable composite in
Holonight.Controls, based on the runtime Button so application style overrides
remain supported. HnPopupCloseButton keeps its separate popup purpose and style.

The public API is the ordinary Button API: action or onClicked, enabled, visibility,
focus and accessibility. Defaults are a window-close theme icon at 24 px, compact
control height/width, 2 px padding, Compact internal-spacing corner radius,
transparent idle background, semantic error hover/pressed background, textPrimary
idle foreground, onError active foreground and textDisabled disabled foreground.
HnButtonSurface supplies the visual-focus outline. Return/Enter activate once;
Space retains native Button behavior. Applications own visibility and focus order.

HnIconButton forwards icon.name to HnIcon only when icon.source is empty. Explicit
sources preserve existing behavior and avoid HnIcon’s conflicting-input error.

Build Icons and Qt into an isolated prefix before Viewer and Files. Focused QML
regressions cover source precedence, accessibility, dimensions, action binding,
disabled behavior and keyboard activation. Consumer regressions cover hover colors,
focus order, visibility and close lifecycle. Complete provider acceptance and staged
installation checks before publishing; physical/visual native checks remain manual.
