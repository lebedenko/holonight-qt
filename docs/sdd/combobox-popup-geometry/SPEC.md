# ComboBox Popup Geometry Specification

## Requirements

- Parent the shared ComboBox `Popup.Item` to the containing window's `Overlay.overlay` so its existing scene-space
  coordinates are interpreted in the correct coordinate system.
- Refresh scene position and scale immediately before opening so layouts that translate or scale ancestors after
  component creation cannot leave the popup at its startup coordinates.
- Preserve uniform scaling and translation, above/below placement, bounded scrolling, delegates, keyboard behavior,
  margins, and the `effectiveScale` and transform-support diagnostics.
- Keep rotation, shear, and non-uniform scaling unsupported.
- Verify rendered popup and collapsed-control bounds at scales 0.78, 1.0, and 1.25 near both window edges.
- Verify delayed ancestor layout before opening produces the same rendered alignment and placement.
- Publish the corrected CMake package contract as version 0.1.1.
