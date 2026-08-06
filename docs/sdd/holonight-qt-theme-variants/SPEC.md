# HoloNight Qt Theme Variants Specification

## Overview

`holonight-qt` shall treat `appearance/scheme` as the canonical user-selected theme variant. `appearance/mode` remains only derived compatibility metadata for older configs and older consumers.

The shell already writes:

```ini
[appearance]
scheme=holonight-dark
accent=cyan
mode=dark
```

`holonight-qt` shall resolve appearance from that file without asking the system color-scheme setting for normal theme selection.

## Scheme IDs

Supported scheme IDs:

- `holonight-dark`: default HoloNight dark variant.
- `holonight-light`: default HoloNight light variant.
- `tokyonight-storm`: TokyoNight-compatible dark variant name.
- `tokyonight-day`: TokyoNight-compatible light variant name.

Unknown, empty, or malformed `scheme` values shall be ignored.

## Resolution Rules

Theme resolution order:

1. If `appearance/scheme` is valid, use it.
2. If `scheme` is missing or invalid and legacy `appearance/mode=light`, resolve to `holonight-light`.
3. If `scheme` is missing or invalid and legacy `appearance/mode=dark` or `system`, resolve to `holonight-dark`.
4. If both values are missing or invalid, resolve to `holonight-dark`.

`system` mode no longer acts as an active theme selection mode. It exists only as legacy input and resolves to the dark default.

## Color Scheme

`HoloniightTheme::colorScheme()` shall return the Qt color scheme derived from the resolved scheme:

- `holonight-dark` and `tokyonight-storm` return `Qt::ColorScheme::Dark`.
- `holonight-light` and `tokyonight-day` return `Qt::ColorScheme::Light`.

It shall not return light or dark based directly on raw `appearance/mode`.

## Accent

`appearance/accent` selects the active accent family:

- `cyan`
- `blue`
- `violet`
- `yellow`

Invalid or missing accent values resolve to `cyan`.

The selected accent shall be applied by `ThemeResolver` to the resolved token set. The affected slots are the existing primary, focus, border-active, and glow slots already controlled by accent handling. Base surface, typography, radius, and metric tokens remain scheme-defined and are not accent-specific.

## Token Compatibility

For this pass:

- `holonight-dark` and `tokyonight-storm` may resolve to the current dark token values.
- `holonight-light` and `tokyonight-day` may resolve to the current light token values.
- Radius, metrics, and typography/font defaults stay identical across all four schemes.

The explicit IDs are still required so future variants can diverge without another config migration.

## Non-Goals

- Do not add a new config file path.
- Do not remove legacy JSON or legacy INI parsing unless already planned elsewhere.
- Do not add new palette token categories.
- Do not make `holonight-qt` own or publish the XDG portal backend.
