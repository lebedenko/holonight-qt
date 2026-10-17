# Shared file UI controls

Five passive composites in `Holonight.Controls` share presentation without taking
ownership of file identities, decoding, navigation or application selection.

- `HnThumbnailDelegate`: `thumbnailContent` component slot, `caption`, `tooltip`,
  `showSelectionMarker`, `captionElide`; `checked`/`highlighted` indicate explicit selection.
  Current ListView position does not select it. `activated` handles click, Return,
  Enter and inherited Space activation; `contextRequested` handles the Menu key.
- `HnPreviewFrame`: `previewState` (Content/Loading/Empty/Error) selects
  `previewContent`, `loadingContent`, `emptyContent` or `errorContent`.
  Default implicit size is 240 × 180; caller dimensions remain stable across states.
- `HnMetadataList`: ordered `rows` of `{label, value}` or `{heading}`; caller formats
  values. `labelWidth` controls aligned labels; optional row `visible` hides a row without
  discarding its position. Values wrap and each row has a
  combined accessible identity.
- `HnLabeledKeyHint`: forwards `text`, `keyGroups`, `wrap`, `lowercaseLetters`;
  `description` contributes to combined `accessibleText`. `hintItem` and `descriptionItem` allow compatible
  caller typography and identification; the keycap retains a monospace font.
- `HnBreadcrumbBar`: `segments` contains display labels only; `segmentActivated(index)`
  preserves the original index even when collapsed. `locationEditRequested()` is
  emitted by Edit or Ctrl+L. `editable` controls the Edit affordance;
  `overflowMenu` exposes the standard menu for integration and keyboard checks. Intermediate locations appear in a standard keyboard
  accessible menu. The final label elides within the available width.

The existing selection foundation gains default-on `currentItemSelects`, disabled
only by thumbnails. Existing delegates retain their behavior. All colors and geometry
use semantic tokens and standard Controls, preserving runtime style overrides.

Gallery examples cover all five types. QML smoke tests exercise geometry, explicit
selection, semantic accessibility and compact breadcrumbs. The generated public-type
list feeds installed consumer checks. Application migration and native visual/theme
acceptance belong to the initiative ledger and are not implied by these provider tests.
