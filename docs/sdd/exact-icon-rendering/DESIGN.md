# Exact Icon Rendering Design

## Architecture

The icon subsystem lives under `src/icons/` and builds as `holonight_icons`.

- `IconRenderer` loads SVG bytes, replaces supported KDE color-scheme stylesheet values, and renders with `QSvgRenderer`.
- `IconThemeResolver` resolves QRC URLs, file URLs, local paths, and theme icon names to SVG bytes.
- `HnIconImageProvider` exposes `image://hnicons` and caches rendered images.
- `Holonight.HnIcon` is a QML component that generates provider URLs for tinted symbolic SVGs and falls back to the original source when tinting is disabled.

## Qt Integration

`holonight_icons` links `Qt6::Svg`. The `Holonight` QML module plugin initializes the provider by calling a small registration helper. The provider is registered once per `QQmlEngine` under `hnicons`.

## SVG Color Handling

The renderer treats SVG as XML text and updates CSS custom property declarations for supported semantic classes. Replacement is constrained to known color-scheme property names and valid hex colors so arbitrary SVG content is not broadly rewritten.

The requested color is applied to text-like classes. The remaining semantic classes use the current Holonight palette:

- Highlight: `primary`
- PositiveText: `success`
- NeutralText: `warning`
- NegativeText: `error`

## Cache Keys

Cache keys include source identifier, target pixel size, device pixel ratio, requested color, and a palette hash derived from semantic palette role colors. This avoids stale renders when theme or accent colors change.

## QML Source URLs

`HnIcon` encodes the original source into the provider path and passes query parameters for `size`, `dpr`, `color`, and a palette hash. This keeps URLs deterministic and naturally invalidates Qt Quick image caching when relevant inputs change.
