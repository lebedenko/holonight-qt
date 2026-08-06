# SPEC: Shared controls foundation

**Status:** Implemented
**Date:** 2026-07-27

## Scope

Establish canonical `Holonight.Core` and `Holonight.Controls` QML modules, semantic control sizes, the first rich
input control, install-tree CMake consumption, and an optional visual gallery. Existing `Holonight` and lowercase
`holonight` APIs remain compatible.

## Core contract

- `HnControlSize` exports `Compact`, `Normal`, `Large`, and `Hero`.
- `HnControlMetrics` provides immutable lookups for height, horizontal padding, icon size, and internal spacing.
- Invalid size values normalize to `Normal`.
- Metrics are respectively:

  | Role | Height | Padding | Icon | Spacing |
  |---|---:|---:|---:|---:|
  | Compact | 28 | 8 | 16 | 4 |
  | Normal | 32 | 8 | 16 | 6 |
  | Large | 40 | 12 | 20 | 8 |
  | Hero | 52 | 16 | 24 | 10 |

## Search-field contract

- `HnSearchField` derives from the HoloNight `TextField` and preserves its text-input, validation, accessibility,
  focus, and keyboard contracts.
- `sizeRole` defaults to `Normal` and controls the implicit height and internal metrics without changing font size.
- Leading and trailing component slots are independently composable.
- A packaged semantic search icon appears when the leading slot is empty.
- An accessible clear button appears only while the control is enabled, editable, and non-empty.
- Custom trailing content remains present beside the clear button.
- Clearing restores input focus.
- Escape clears non-empty input; on empty input it remains unaccepted for ancestor handling.
- Launcher modes, models, history, suggestions, commands, and application-domain behavior are out of scope.

## Packaging contract

- `find_package(HolonightQt REQUIRED COMPONENTS Core Controls)` succeeds from an install tree.
- It provides `HolonightQt::Core` and `HolonightQt::Controls`.
- Canonical QML imports are installed without lowercase aliases for the new modules.
- QML module version is `1.0`; source imports remain unversioned.

## Acceptance

Contract tests cover enum and metric values, fallback, search-field sizing and behavior, slots, inherited states,
keyboard handling, accessibility, and install-tree package/import consumption. The optional controls gallery
covers all sizes and principal visual states.
