# SPEC: Settings and state controls

## Goal

Complete the third shared-controls cycle with reusable settings structure and caller-controlled empty and loading
presentations in `Holonight.Controls`.

## Requirements

`HnSettingsRow` provides semantic sizing, title and description text, optional leading content, one default control,
and explicit inline or stacked layout. It exposes loaded slot items, collapses absent content, inherits enabled
state, and forwards keyboard focus to the control.

`HnSectionHeader` provides title and description text, an optional divider, and optional leading and trailing
content. It is structural rather than interactive, collapses every unused region, and leaves focus behavior with
loaded trailing actions.

`HnEmptyState` provides an optional default icon, replacement graphic content, title and description text, and an
optional action. Missing or failed icons collapse. Content is centered and wraps, disabled state is inherited, and
the root exposes grouped accessibility metadata. Callers own visibility and empty-model decisions.

`HnLoadingState` provides title and description text, a running state, normalized progress, and an optional action.
Negative progress is indeterminate; non-negative progress is clamped to `[0, 1]`. Animation is gated by running and
effective visibility. Callers own lifecycle, cancellation, retries, and application state.

The module stays at version 1.0. Existing targets, aliases, dependencies, and imports remain compatible.
