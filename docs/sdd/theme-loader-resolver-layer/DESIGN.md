# Theme Loader & Resolver Layer — Architecture Design

**Feature:** Theme Loader & Resolver Layer
**Status:** Design
**Date:** 2026-06-29

---

## 1. Component Overview

```
┌─────────────────────────────────────────────────────────────────────┐
│                         theme.conf (INI)                            │
│  [appearance]  mode=, scheme=, accent=                              │
│  [effects]     transparency=, blur_strength=                        │
└────────────────────────────┬────────────────────────────────────────┘
                             │ file I/O
                             ▼
                    ┌─────────────────┐
                    │  ThemeLoader    │  src/theme/
                    │  (static class) │◄─── HOLONIGHT_APPEARANCE_MODE env
                    └────────┬────────┘
                             │ ThemeConfig
                             ▼
                    ┌─────────────────┐
                    │  ThemeResolver  │  src/theme/
                    │  (static class) │◄─── QGuiApplication::styleHints()
                    └────────┬────────┘      (system mode only)
                             │ ColorTokens
                             ▼
              ┌──────────────────────────────┐
              │      HoloniightPalette       │  qml/
              │      (QML singleton)         │
              │  operator== suppression      │
              └──────────┬───────────────────┘
                         │ Q_PROPERTYs + paletteChanged()
                         ▼
                   QML bindings
              (color tokens in QML UI)

━━━━━━━━━━━━━━━━ existing static libs ━━━━━━━━━━━━━━━━

  ┌───────────────┐     ┌────────────────────┐
  │  palette/     │     │  config/            │
  │  ColorTokens  │◄────│  ThemeConfig        │
  │  darkTokens() │     │  ThemeConfig::load()│
  │  lightTokens()│     │  (now delegates to  │
  │  buildPalette()│    │   ThemeLoader)      │
  └───────────────┘     └────────────────────┘
        ▲                        ▲
        └────────────────────────┘
            both consumed by src/theme/

  ┌─────────────┐   ┌──────────────┐   ┌───────────┐
  │ src/style/  │   │src/platform- │   │   qml/    │
  │ QStyle      │   │theme/        │   │ QQC2      │
  │ plugin      │   │ QPlatform-   │   │ components│
  └─────────────┘   │ Theme plugin │   └───────────┘
                    └──────────────┘
```

---

## 2. Directory Layout

### Before

```
holonight-qt/
├── config/
├── palette/
├── platformtheme/          ← plugin sources at root
├── style/                  ← plugin sources at root
├── qml/
├── tools/
├── tests/
└── demo/
```

### After

```
holonight-qt/
├── config/
├── palette/
├── src/
│   ├── platformtheme/      ← moved from root
│   ├── style/              ← moved from root
│   └── theme/              ← NEW
│       ├── CMakeLists.txt
│       ├── themeloader.h
│       ├── themeloader.cpp
│       ├── themeresolver.h
│       └── themeresolver.cpp
├── qml/
├── tools/
├── tests/
│   └── test_theme_resolver.cpp   ← NEW
└── demo/
```

`config/`, `palette/`, `qml/`, `tools/`, `tests/`, `demo/` remain at root as specified.

---

## 3. Data Flow

```
1. theme.conf read
   ThemeLoader::load() opens ThemeConfig::configFilePath()
   Parses [appearance]: mode, scheme, accent
   Parses [effects]: transparency, blur_strength
   Checks HOLONIGHT_APPEARANCE_MODE env var; overrides mode if set
   Falls back to AppearanceMode::Dark on parse error or missing file
   Returns ThemeConfig (all fields populated)

2. ThemeConfig::load() delegates
   ThemeConfig::load() { return ThemeLoader::load(); }
   All callers (existing + new) receive the extended ThemeConfig transparently

3. ThemeResolver resolves tokens
   ThemeResolver::resolve(ThemeConfig) is called
   Calls config.resolvedColorMode():
     - Dark  → darkTokens()
     - Light → lightTokens()
     - System → QGuiApplication::styleHints()->colorScheme()
                Dark/Unknown → darkTokens()
                Light        → lightTokens()
   If config.accent is non-empty and recognized:
     Applies accent overrides to the base ColorTokens (see §5)
   scheme and transparency stored in ThemeConfig but not yet applied
   Returns ColorTokens

4. HoloniightPalette construction
   Constructor calls ThemeLoader::load() → ThemeResolver::resolve()
   Stores result in tok_

5. HoloniightPalette::reload()
   Calls ThemeLoader::load() → ThemeResolver::resolve() → newTok
   If newTok == tok_: return (suppress emission)
   tok_ = newTok
   Q_EMIT paletteChanged()

6. QML bindings react
   paletteChanged() signal propagates to all bound Q_PROPERTYs
   QML items re-evaluate color bindings
```

---

## 4. ThemeLoader Design

### Responsibilities

- File I/O: read `theme.conf` using `QSettings` (INI format)
- Parse `[appearance]` group: `mode`, `scheme`, `accent`
- Parse `[effects]` group: `transparency`, `blur_strength`
- Apply `HOLONIGHT_APPEARANCE_MODE` environment override after parsing
- Validate mode string; fall back to `Dark` on invalid/missing values
- Clamp `transparency` to [0.0, 1.0]
- Return a fully-populated `ThemeConfig`

### Interface

```cpp
// src/theme/themeloader.h
#pragma once
namespace Holonight {
struct ThemeConfig;

class ThemeLoader {
public:
  ThemeLoader() = delete;

  [[nodiscard]] static ThemeConfig load();

private:
  [[nodiscard]] static AppearanceMode parseMode(const QString& value);
  [[nodiscard]] static AppearanceMode envOverride(AppearanceMode base);
};
} // namespace Holonight
```

### Relationship to `ThemeConfig::load()`

`ThemeConfig::load()` in `config/holonight/config.cpp` is updated to a one-liner:

```cpp
ThemeConfig ThemeConfig::load() {
  return ThemeLoader::load();
}
```

This preserves the existing public API. All callers that use `ThemeConfig::load()` automatically gain the new fields (`scheme`, `accent`, `transparency`) with zero changes on their side. The `config/` static lib gains a link dependency on `src/theme/` (see §8).

### Environment Override

```cpp
AppearanceMode ThemeLoader::envOverride(AppearanceMode base) {
  const QString env = qEnvironmentVariable("HOLONIGHT_APPEARANCE_MODE");
  if (env.isEmpty()) return base;
  return parseMode(env); // falls back to Dark on unrecognized value
}
```

---

## 5. ThemeResolver Design

### Interface

```cpp
// src/theme/themeresolver.h
#pragma once
namespace Holonight {
struct ThemeConfig;
struct ColorTokens;

class ThemeResolver {
public:
  ThemeResolver() = delete;

  [[nodiscard]] static ColorTokens resolve(const ThemeConfig& config);

private:
  [[nodiscard]] static ColorTokens resolveBase(ColorMode mode);
  static void applyAccent(ColorTokens& tok, const QString& accent);
};
} // namespace Holonight
```

### System Mode Resolution

```cpp
ColorTokens ThemeResolver::resolveBase(ColorMode mode) {
  if (mode == ColorMode::Dark) return darkTokens();
  if (mode == ColorMode::Light) return lightTokens();
  // ColorMode::System — requires QGuiApplication to be running
  const auto scheme = QGuiApplication::styleHints()->colorScheme();
  return (scheme == Qt::ColorScheme::Light) ? lightTokens() : darkTokens();
}
```

`resolvedColorMode()` on `ThemeConfig` still converts `AppearanceMode::System` → `ColorMode::System`. `ThemeResolver` is responsible for the actual Qt API call so the resolution point is explicit and testable.

### Accent Override Table

`applyAccent()` replaces the following slots based on the accent name. All other slots (surface, text, border-passive/subtle/strong/urgent, status colors) remain as returned by `darkTokens()`/`lightTokens()`.

Affected slots: `primary`, `primaryHover`, `primaryPressed`, `borderFocus`, `borderActive`, `focusRing`, `glowCyanSoft`, `glowBlueSoft`, `glowVioletSoft`, `accentCyan`, `accentBlue`, `accentViolet`, `accentYellow`.

| Slot            | `cyan`      | `blue`      | `violet`    | `yellow`    |
|-----------------|-------------|-------------|-------------|-------------|
| `primary`       | `#7dcfff`   | `#7aa2f7`   | `#bb9af7`   | `#e0af68`   |
| `primaryHover`  | `#a3d8ff`   | `#9ab5ff`   | `#cdb4ff`   | `#f0c98a`   |
| `primaryPressed`| `#5cb8f5`   | `#5f88e8`   | `#a07fdf`   | `#c99050`   |
| `borderFocus`   | `#7dcfff`   | `#7aa2f7`   | `#bb9af7`   | `#e0af68`   |
| `borderActive`  | `#7dcfff`   | `#7aa2f7`   | `#bb9af7`   | `#e0af68`   |
| `focusRing`     | `#7dcfff55` | `#7aa2f755` | `#bb9af755` | `#e0af6855` |
| `glowCyanSoft`  | `#7dcfff22` | `#7aa2f722` | `#bb9af722` | `#e0af6822` |
| `glowBlueSoft`  | `#7dcfff18` | `#7aa2f718` | `#bb9af718` | `#e0af6818` |
| `glowVioletSoft`| `#7dcfff12` | `#7aa2f712` | `#bb9af712` | `#e0af6812` |
| `accentCyan`    | `#7dcfff`   | `#7dcfff`   | `#7dcfff`   | `#7dcfff`   |
| `accentBlue`    | `#7aa2f7`   | `#7aa2f7`   | `#7aa2f7`   | `#7aa2f7`   |
| `accentViolet`  | `#bb9af7`   | `#bb9af7`   | `#bb9af7`   | `#bb9af7`   |
| `accentYellow`  | `#e0af68`   | `#e0af68`   | `#e0af68`   | `#e0af68`   |

Note: `accentCyan/Blue/Violet/Yellow` slots are the canonical named-color references used by semantic aliases elsewhere in the palette; they do not change across accents. Only the `primary*`, `border*`, `focusRing`, and `glow*` slots change to reflect the active accent.

An unrecognized or empty `accent` string leaves the base tokens unmodified. `scheme` and `transparency` are parsed and stored in `ThemeConfig` but `applyAccent` does not act on them; reserved for a future pass.

---

## 6. ColorTokens Equality

### Approach

Add `operator==` to `ColorTokens` as a member-by-member comparison over all fields:

```cpp
// palette/holonight/palette.h
struct ColorTokens {
  // ... existing fields ...
  [[nodiscard]] bool operator==(const ColorTokens&) const noexcept = default;
};
```

C++23 defaulted `operator==` compares all non-static data members in declaration order. This is correct for `ColorTokens` because all fields are `QColor` or integral types, both of which define `operator==`.

`QColor::operator==` compares color spec and all channel values (RGBA). Alpha-bearing tokens (`focusRing`, `hover`, `pressed`, overlay tokens) compare correctly because `QColor` includes the alpha channel.

### Deprecated alias fields

If `ColorTokens` carries any deprecated alias fields (fields that mirror another field's value), they are included in the comparison automatically. This is conservative — a theoretical change that only touches the canonical field and not the alias would still trigger `!=`. This is acceptable: the suppression mechanism is an optimization, not a correctness requirement. False positives (unnecessary emissions) are safe; false negatives (missed emissions) are not.

### Maintenance burden

See §11 (Known Risks).

---

## 7. HoloniightPalette Wiring

### Constructor (pseudocode)

```cpp
HoloniightPalette::HoloniightPalette(QObject* parent)
    : QObject{parent}
{
    // New path: ThemeLoader → ThemeResolver
    const ThemeConfig cfg = ThemeLoader::load();
    tok_ = ThemeResolver::resolve(cfg);
}
```

### reload() (pseudocode)

```cpp
void HoloniightPalette::reload() {
    const ThemeConfig cfg = ThemeLoader::load();
    const ColorTokens newTok = ThemeResolver::resolve(cfg);
    if (newTok == tok_) {
        return;  // suppress: nothing changed, avoid QML re-layout churn
    }
    tok_ = newTok;
    Q_EMIT paletteChanged();
}
```

The suppression check uses the defaulted `operator==` from §6. The check happens before assignment so `tok_` is never dirtied on a no-op reload.

---

## 8. CMake Build Graph

### Existing graph (simplified)

```
palette  ←─── config
palette  ←─── style/
palette  ←─── platformtheme/
palette  ←─── qml/
config   ←─── platformtheme/
config   ←─── qml/
```

### New graph

```
palette  ←─── config
palette  ←─── src/theme/   (ThemeLoader + ThemeResolver, new static lib)
config   ←─── src/theme/
src/theme/ ←─ config       (ThemeLoader calls ThemeConfig::configFilePath())
src/theme/ ←─ palette      (ThemeResolver calls darkTokens/lightTokens)
```

Cycle check: `config` depends on `palette`; `src/theme/` depends on `config` and `palette`. `config` does NOT depend on `src/theme/` at the static-lib level. The delegation is achieved by updating `config/holonight/config.cpp` to `#include "src/theme/themeloader.h"` and calling `ThemeLoader::load()`. The `config` CMake target therefore gains `src/theme/holonight_theme` as a link dependency.

```
src/platformtheme/ ←─ src/theme/
src/style/         ←─ palette, config  (unchanged)
qml/               ←─ src/theme/       (HoloniightPalette uses ThemeLoader + ThemeResolver)
tests/             ←─ src/theme/, palette, config
```

### New CMakeLists.txt target (src/theme/)

```cmake
add_library(holonight_theme STATIC
    themeloader.cpp
    themeresolver.cpp
)
target_link_libraries(holonight_theme PUBLIC
    holonight_palette
    holonight_config
    Qt6::Gui
)
target_include_directories(holonight_theme PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
```

---

## 9. Key Decisions with Rationale

### ThemeConfig::load() delegates to ThemeLoader::load()

All existing callers (`platformtheme/`, `style/`, `HoloniightPalette`, tests) call `ThemeConfig::load()`. Changing the implementation body while keeping the signature preserves binary and source compatibility with zero changes at call sites. Tests in `test_config.cpp` continue to pass unmodified because the public API is unchanged and `ThemeLoader::load()` reads the same file with the same fallback behavior.

### ThemeResolver is a static (stateless) class

`ThemeResolver` has no member state between calls. The input `ThemeConfig` fully determines the output `ColorTokens`. A stateless static class avoids lifetime, ownership, and thread-safety questions. If future needs require caching, a singleton can be introduced without changing callers.

### ThemeLoader is a static (stateless) class

Same reasoning. File I/O is cheap relative to QML re-layout; caching would introduce invalidation complexity without measurable benefit. `QSettings` does its own buffering at the OS level.

### Suppress paletteChanged when tokens are equal

`paletteChanged()` causes every bound QML property to re-evaluate, which can trigger layout recalculation across the entire UI. On a hot path (e.g., `reload()` called on a timer or IPC signal), emitting unchanged signals generates noise. The `operator==` check is O(N) over ~60 fields, which is negligible compared to QML layout cost. This is a pure optimization; omitting it would not cause incorrect behavior.

### Accent override replaces slots rather than blending

Accent colors are discrete design choices (fixed hex values), not mathematical transforms of existing colors. Blending would produce unpredictable results across dark/light bases. A hardcoded table ensures predictable, design-reviewed output and makes it trivial to audit which slots are affected.

### scheme and transparency parsed but not yet applied

Parsing them into `ThemeConfig` now means existing consumers (config tests, future components) see the fields populated from day one. Deferring the rendering effect avoids scope creep in this cycle while establishing the data pipeline.

---

## 10. Alternatives Considered

### Alt A: Put ThemeLoader logic inside config/

The `config/` static lib already owns `ThemeConfig` and `ThemeConfig::load()`. The new parsing logic could be added directly to `config.cpp`.

**Rejected because:** `config/` would then need to link against `Qt6::Gui` for `QGuiApplication::styleHints()` (needed by `ThemeResolver` — though `ThemeResolver` is separate). More importantly, mixing file-parsing logic with struct definition makes `config/` harder to test in isolation. A dedicated `src/theme/` lib has a single responsibility (loading + resolving) and can be mocked or replaced independently. The delegation pattern keeps `config/` as a pure data-definition lib.

### Alt B: Manual INI parsing instead of QSettings

`QSettings` with `QSettings::IniFormat` handles comments, quoting, multi-value keys, and platform-specific path encoding. A hand-rolled parser would need to replicate this and introduce new edge cases.

**Rejected because:** `QSettings` is already used in the project for this file. The only downside — `QSettings` constructs a `QCoreApplication`-aware path — is irrelevant here because `ThemeConfig::configFilePath()` already returns the absolute path and `ThemeLoader` passes it directly to `QSettings(path, IniFormat)`.

### Alt C: QML-side resolution (resolve mode in HoloniightPalette, not in C++)

Accent and mode resolution could live entirely in QML, keeping C++ a thin data pipe.

**Rejected because:** QML lacks strong typing for color token structs, making accent table lookup error-prone. C++ resolution also means the `QPlatformTheme` and `QStyle` plugins (which have no QML dependency) can reuse the same resolver. Testing is simpler in GTest than in QML test harnesses.

---

## 11. Known Risks

### operator== maintenance burden

`ColorTokens` currently has ~60 fields. The defaulted `operator==` covers all of them automatically. However, if a future developer adds a field to `ColorTokens` but forgets that the defaulted comparison now includes it, a field that is intentionally always identical across modes could cause false positives (unnecessary emissions). More critically, if a developer adds a field and explicitly writes a manual `operator==` that omits the new field, the suppression logic will silently fail to emit on changes to that field. **Mitigation:** a static_assert on `sizeof(ColorTokens)` in `test_theme_resolver.cpp` that documents the expected size; any field addition breaks the assert and forces a deliberate update.

### System mode resolution requires QGuiApplication

`ThemeResolver::resolveBase(ColorMode::System)` calls `QGuiApplication::styleHints()`. If called before `QGuiApplication` is constructed (e.g., in a unit test that only creates `QCoreApplication`), this will crash or return a null pointer. **Mitigation:** guard with `qApp` null check and fall back to dark; document this precondition in the header. Tests that exercise system mode resolution must construct `QGuiApplication` (or use `offscreen` platform).

### Accent palette values are hardcoded

The accent color table in `ThemeResolver` is not derivable from the base palette — it is a designer decision. If the base dark/light palette evolves (e.g., `darkTokens()` changes `primary` to a new hue), the hardcoded accent table will not track the change, and the "no accent" vs "cyan accent" paths will diverge silently. **Mitigation:** comment in `themeresolver.cpp` cross-references `palette/holonight/palette.h` and notes that accent values must be reviewed when base tokens change. Long-term, a future cycle can move accent definitions into a structured config or derive them algorithmically.

### Directory rename breaks include paths and CMake targets

Moving `platformtheme/` → `src/platformtheme/` and `style/` → `src/style/` changes `#include` paths and `target_include_directories` in `CMakeLists.txt`. Any out-of-tree consumers or documentation referencing the old paths will break. **Mitigation:** CMake `ALIAS` targets (`holonight_style`, `holonight_platformtheme`) keep the logical names stable; physical paths are an implementation detail.
