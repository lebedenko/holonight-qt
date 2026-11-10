# Consistent browsing contract

HnModeBadge reserves space for all supported mode labels. HnFormat formats byte sizes. HnMetadataList provides aligned label/value rows; browser consumers bound label widths to fit narrow panels. Viewer retains its compact popup, selectable path and image-specific metadata. Browser services and selection policy remain outside the toolkit.

See the umbrella docs/initiatives/consistent-file-browsing.md for interaction and dependency contracts. Historical designs remain historical.

## Verification

Installed browser, Files and chooser consumers exercised the new controls. Files mode-badge and footer regressions and the user-operated native checks passed.
