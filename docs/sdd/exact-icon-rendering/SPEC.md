# Exact Icon Rendering Specification

## Goal

Provide exact-color symbolic SVG rendering for Holonight QML through a shared `HnIcon` component and `image://hnicons` image provider.

## Requirements

- `Holonight.HnIcon` exposes the shell-compatible API: `source`, `size`, `iconState`, `tinted`, `normalColor`, `mutedColor`, `disabledColor`, `activeColor`, and `resolvedColor`.
- Tinted SVG icons render with exact requested colors through `QSvgRenderer`, not shader colorization.
- `image://hnicons/...` supports QRC, local file, and theme icon names when an SVG source can be resolved.
- Supported semantic SVG classes are:
  - `ColorScheme-Text`
  - `ColorScheme-Highlight`
  - `ColorScheme-PositiveText`
  - `ColorScheme-NeutralText`
  - `ColorScheme-NegativeText`
- The requested icon color maps to text-like semantic classes. Highlight, positive, neutral, and negative classes map to palette roles.
- Empty or invalid sources return a null image without crashing.
- Cache entries are keyed by source/name, logical size, device pixel ratio, requested color, and palette generation/hash.

## Verification

- Unit tests validate exact rendered pixels for supported semantic classes.
- Cache tests validate source, size, DPR, and color-sensitive keys.
- QML smoke tests validate `HnIcon` API compatibility and provider URL updates.
- `task test` passes.
