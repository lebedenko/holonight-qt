# DESIGN: Icon Theme and Font Configuration

**Feature:** Hardcoded icon theme and font configuration in `HoloniightTheme`  
**Spec:** `docs/sdd/icon-font-config/SPEC.md`  
**Date:** 2026-05-20  
**Status:** Approved for implementation

---

## 1. Components

### 1.1 Files that change

| File | Change |
|---|---|
| `platformtheme/holonighttheme.h` | Add per-role `QFont` members; remove single `font_` field |
| `platformtheme/holonighttheme.cpp` | Constructor: call `QIcon::setThemeName` / `setFallbackThemeName`; rewrite `font()` switch |
| `tests/test_platformtheme_fonts.cpp` | New file — verifies all five font roles and fallback delegation |
| `tests/test_platformtheme_icons.cpp` | New file — verifies icon theme names post-construction |
| `tests/CMakeLists.txt` | Add both new `.cpp` files to the `holonight_tests` executable |

No other directories are touched (REQ-C-004).

### 1.2 What each change does

**`holonighttheme.h`**  
Replaces the single `QFont font_` private member with five `const QFont` members, one per
covered role. The `font(Font type)` virtual override signature is unchanged; the `[[nodiscard]]`
attribute is retained.

**`holonighttheme.cpp` — constructor**  
Adds two `QIcon` calls immediately after the member initialiser list.  Since `QIcon::setThemeName`
and `QIcon::setFallbackThemeName` are global (process-level) state, executing them in the
constructor is safe at the point `HoloniightThemePlugin::create()` fires — `QGuiApplication` is
already constructed by then (see §2 Data Flow).

**`holonighttheme.cpp` — `font()`**  
Switches on `type` to return a pointer to the matching pre-constructed `QFont` member.
Unrecognised roles fall through to `QPlatformTheme::font(type)`, which returns `nullptr` for all
roles not overridden by a base class.

---

## 2. Data Flow

### 2.1 Plugin construction sequence

```
QGuiApplication::QGuiApplication()
  └─ QPlatformIntegration::createPlatformTheme("holonight")
       └─ HoloniightThemePlugin::create("holonight", {})
            └─ new HoloniightTheme          ← constructor runs here
                 ├─ member initialisers: font_system_, font_title_, font_fixed_,
                 │                        font_small_, font_mini_ constructed
                 ├─ QIcon::setThemeName("Papirus-Dark")
                 └─ QIcon::setFallbackThemeName("breeze-dark")
```

`QGuiApplication` is fully constructed before any application code calls `QIcon::fromTheme()`, so
the theme names are in place before any icon lookup occurs.

### 2.2 Font query sequence

```
QApplication / Qt internals
  └─ QPlatformTheme::font(Font type)   ← virtual dispatch
       └─ HoloniightTheme::font(type)
            switch (type):
              SystemFont   → &font_system_   (Inter, Normal, 14pt)
              TitleBarFont → &font_title_    (Inter, Normal, 14pt)
              FixedFont    → &font_fixed_    (JetBrainsMono Nerd Font, Normal, 14pt)
              SmallFont    → &font_small_    (Inter, Normal, 12pt)
              MiniFont     → &font_mini_     (Inter, Normal, 10pt)
              default      → QPlatformTheme::font(type)  → nullptr
```

The returned pointer points into the `HoloniightTheme` object.  Qt caches the result per theme
instance; the pointer lifetime equals the platform theme lifetime (application lifetime).

### 2.3 `themeHint()` and icon themes

The current implementation already handles `SystemIconThemeName` / `SystemIconFallbackThemeName`
via `themeHint()`. The new feature replaces this with direct `QIcon` API calls in the constructor.
The `themeHint()` switch cases for those two hints are removed to avoid double-specification and
potential divergence between the two mechanisms (see §5 Alternatives Considered).

---

## 3. Interfaces / APIs

### 3.1 `HoloniightTheme` — updated header

```cpp
class HoloniightTheme : public QPlatformTheme {
 public:
  HoloniightTheme();

  [[nodiscard]] const QPalette* palette(Palette type = SystemPalette) const override;
  [[nodiscard]] QVariant        themeHint(ThemeHint hint) const override;
  [[nodiscard]] Qt::ColorScheme colorScheme() const override;
  [[nodiscard]] const QFont*    font(Font type = SystemFont) const override;

 private:
  QFont font_system_;  // Inter, Normal, 14pt
  QFont font_title_;   // Inter, Normal, 14pt
  QFont font_fixed_;   // JetBrainsMono Nerd Font, Normal, 14pt
  QFont font_small_;   // Inter, Normal, 12pt
  QFont font_mini_;    // Inter, Normal, 10pt
};
```

No new public API is added; the `font()` override already exists in the base class.

### 3.2 `HoloniightTheme` — constructor signature

```cpp
HoloniightTheme::HoloniightTheme()
    : font_system_{QStringLiteral("Inter"), 14},
      font_title_{QStringLiteral("Inter"), 14},
      font_fixed_{QStringLiteral("JetBrainsMono Nerd Font"), 14},
      font_small_{QStringLiteral("Inter"), 12},
      font_mini_{QStringLiteral("Inter"), 10}
{
  QIcon::setThemeName(QStringLiteral("Papirus-Dark"));
  QIcon::setFallbackThemeName(QStringLiteral("breeze-dark"));
}
```

`QFont(family, pointSize)` initialises `weight` to `QFont::Normal` by default; no explicit weight
call is needed.

### 3.3 `HoloniightTheme::font()` signature (unchanged)

```cpp
const QFont* HoloniightTheme::font(Font type) const;
```

Return type is `const QFont*` because `QPlatformTheme::font()` is declared with that return type.
Qt documents that a `nullptr` return for unrecognised roles is the correct base-class behaviour.

### 3.4 `themeHint()` — removed cases

```cpp
// REMOVED from the switch:
case SystemIconThemeName:         // was: return QStringLiteral("papirus");
case SystemIconFallbackThemeName: // was: return QStringLiteral("hicolor");
```

Both fall through to the `default` branch, which delegates to `QPlatformTheme::themeHint(hint)`.

---

## 4. Key Decisions with Rationale

### 4.1 Constructor vs. `themeHint()` for icon themes

The spec (REQ-F-001, REQ-F-002) explicitly requires `QIcon::setThemeName()` /
`QIcon::setFallbackThemeName()` to be called from the constructor.

Beyond spec compliance, the `QIcon` API is the authoritative mechanism for setting the icon theme
at the Qt platform level.  Using `QIcon::setThemeName()` is the call that actually configures icon
lookup in `QIconLoader`; the `themeHint(SystemIconThemeName)` path is a hint to the *platform
integration*, which may or may not call `QIcon::setThemeName()` in response, depending on the
backend.  On Wayland/X11 with the native integration, these `themeHint` values are typically
read once during integration initialisation and may arrive too early or be ignored after
`QGuiApplication` construction.  The direct `QIcon` call guarantees the name is set at the exact
moment the theme object is created, which is after the platform integration is already running.

The `themeHint()` cases for `SystemIconThemeName` and `SystemIconFallbackThemeName` are removed to
eliminate dual specification.

### 4.2 Five separate `QFont` members vs. a lookup table

A `std::array` or `QHash<Font, QFont>` would be slightly more compact but adds indirection and
makes the mapping harder to read at a glance.  Five named members (`font_system_`, `font_title_`,
etc.) are self-documenting, have zero runtime cost beyond the array alternative, and fit within
the 120-character column limit without contortion.

### 4.3 `const QFont*` return type (pointer-to-member)

`QPlatformTheme::font()` is declared as `virtual const QFont* font(Font type) const` and
returning `nullptr` is the documented signal for "not overridden".  Returning a pointer directly
into the `HoloniightTheme` object (rather than a heap allocation or a static local) is safe
because the theme object lives for the entire application lifetime.  A static-local
`QFont` inside the function body would also work but would add one branch per call to guard
the first-time initialisation; member construction in the initialiser list is cleaner.

### 4.4 Hardcoded values (REQ-C-007)

The spec explicitly prohibits runtime configuration.  All family names and point sizes are
`QStringLiteral` / integer literals in the source.  No environment variables, INI files, or
compile-time options control them.

---

## 5. Alternatives Considered

### 5.1 Config-file approach

A `~/.config/holonight/fonts.conf` or XDG settings approach would allow per-user customisation
without recompilation.  Rejected: REQ-C-007 forbids runtime configuration; config parsing would
add an I/O dependency and error-handling surface area that is out of scope for this feature.

### 5.2 `themeHint()` for icon themes (keeping status quo)

Retaining `themeHint(SystemIconThemeName)` instead of calling `QIcon::setThemeName()` directly
is the simpler change but does not satisfy REQ-F-001 / REQ-F-002, which require the `QIcon`
calls.  Additionally, the `themeHint` path is less reliable across Qt platform integrations (see
§4.1).  Rejected: does not meet spec acceptance criteria.

### 5.3 `QStyle::polish()` for fonts

`QStyle::polish(QApplication*)` can push custom fonts into a `QApplication`.  Rejected: fonts
belong at the platform-theme layer (they affect all Qt applications using the theme, not only
styled widgets); `QStyle::polish` would require including `<QApplication>` in the style plugin
and creates a dependency between two otherwise independent plugins.  REQ-C-004 also restricts
changes to `platformtheme/` only.

### 5.4 Single `QFont` member with late switch (current state)

The existing code returns the same `font_` for every role.  Extending it via a `switch` that
mutates a local copy would require heap-allocating the copy to return a stable pointer — that
leaks memory.  Pre-constructing per-role members and returning `&member` is the correct approach.

---

## 6. Known Risks

### 6.1 Early constructor call timing

`HoloniightThemePlugin::create()` is called from inside `QGuiApplication`'s constructor via the
platform integration.  At that point `QGuiApplication` is not yet fully constructed; however,
`QIcon::setThemeName()` only writes a `QString` into a global `QIconLoader` structure and does
not trigger any GUI operations.  This is safe.

Risk level: **low** — Qt itself calls platform theme constructors at this stage for all bundled
themes (e.g., `QGnomePlatformTheme`).

Mitigation: the existing smoke test `InstantiatesWithoutCrash` (in `test_platformtheme_smoke.cpp`)
already validates construction under `QApplication`; the new icon-theme test exercises
construction and queries `QIcon::themeName()` post-construction.

### 6.2 Font unavailability on minimal systems

If `Inter` or `JetBrainsMono Nerd Font` is not installed, `QFont` will silently fall back to the
closest available family.  Qt does not raise an error; the application will render with a
different font but will not crash.

Risk level: **medium** for end-user experience, **zero** for correctness.

Mitigation: REQ-F-011 requires a manual visual check.  The unit tests assert `fnt->family()`
against the configured string; on a CI machine without these fonts installed the assertion may
fail.  To avoid CI fragility, the font tests should assert only on `fnt != nullptr` and the
requested family/size from `HoloniightTheme`'s perspective (i.e., the value stored in the
`QFont`, not what Qt resolves to at render time).  `QFont::family()` returns the *requested*
family name, not the resolved one — this distinction means the test is stable even when the font
is absent.

### 6.3 Icon theme name case sensitivity

`QIcon::setThemeName("Papirus-Dark")` is case-sensitive on Linux (XDG icon directories use
the theme name verbatim).  If a distribution packages the theme under a different casing
(e.g., `papirus-dark`), icons will not resolve.  This is a deployment concern, not a code
correctness issue, and is explicitly accepted under REQ-C-007 (no runtime override allowed).

---

## 7. Test Approach

### 7.1 Pattern

All platformtheme tests compile `holonighttheme.cpp` directly into the test binary (no plugin
loading).  `QApplication` is constructed in `main.cpp` (shared entry point).  Environment is
forced to `QT_QPA_PLATFORM=offscreen;QT_QPA_PLATFORMTHEME=;QT_STYLE_OVERRIDE=` via CMake
`set_property(TEST ... PROPERTY ENVIRONMENT ...)` to prevent the installed theme from
interfering with the default `QPalette`.

### 7.2 `test_platformtheme_fonts.cpp` (new)

Tests are added to the existing `holonight_tests` executable (same source-list pattern as
`test_platformtheme_smoke.cpp`).

Covered assertions per font role:

| Role | family | weight | pointSize |
|---|---|---|---|
| `SystemFont` | `"Inter"` | `QFont::Normal` | 14 |
| `TitleBarFont` | `"Inter"` | `QFont::Normal` | 14 |
| `FixedFont` | `"JetBrainsMono Nerd Font"` | `QFont::Normal` | 14 |
| `SmallFont` | `"Inter"` | `QFont::Normal` | 12 |
| `MiniFont` | `"Inter"` | `QFont::Normal` | 10 |

Additional tests:
- `font()` returns non-null for each of the five roles.
- `font(MenuBarFont)`, `font(PushButtonFont)`, `font(LabelFont)` each return `nullptr`
  (delegation to base class, REQ-F-008).

Implementation note: `QFont::weight()` returns `QFont::Weight` (an enum).
`QFont::Normal` == `QFont::Weight::Normal` == 400. Use `EXPECT_EQ(fnt->weight(), QFont::Normal)`.

### 7.3 `test_platformtheme_icons.cpp` (new)

Also compiled into `holonight_tests`.

```cpp
TEST(PlatformThemeIcons, ThemeNameIsPapirus) {
  HoloniightTheme theme;
  EXPECT_EQ(QIcon::themeName(), QStringLiteral("Papirus-Dark"));
}

TEST(PlatformThemeIcons, FallbackThemeNameIsBreezeDark) {
  HoloniightTheme theme;
  EXPECT_EQ(QIcon::fallbackThemeName(), QStringLiteral("breeze-dark"));
}
```

No mock or fixture needed — `QIcon::setThemeName` writes global state that is immediately
readable via `QIcon::themeName()`.

### 7.4 Existing smoke test update

`test_platformtheme_smoke.cpp::FontIsInterTenPt` currently expects point size 10.  After the
change, `SystemFont` returns 14pt.  This test must be updated:

```cpp
TEST(PlatformThemeSmoke, FontIsInterFourteenPt) {
  HoloniightTheme theme;
  const QFont* fnt = theme.font(QPlatformTheme::SystemFont);
  ASSERT_NE(fnt, nullptr);
  EXPECT_EQ(fnt->family(), QStringLiteral("Inter"));
  EXPECT_EQ(fnt->pointSize(), 14);
}
```

The `IconThemeIsPapirus` smoke test currently checks `themeHint(SystemIconThemeName)`.  After the
`themeHint` cases are removed, that test will fail.  It should be deleted or replaced by the
dedicated icon test in `test_platformtheme_icons.cpp`.

### 7.5 CMakeLists.txt addition

```cmake
add_executable(holonight_tests
    main.cpp
    ...
    ${CMAKE_SOURCE_DIR}/platformtheme/holonighttheme.cpp
    test_platformtheme_smoke.cpp
    test_platformtheme_fonts.cpp   # new
    test_platformtheme_icons.cpp   # new
)
```

No new `target_link_libraries` or `target_include_directories` entries are needed; both files
use the same headers and Qt modules already present.

---

## 8. Implementation Checklist

1. Update `holonighttheme.h`: replace `QFont font_` with five named members.
2. Update `holonighttheme.cpp`:
   a. Add `#include <QIcon>` (local block, after existing Qt includes — actually Qt header, placed in Qt block).
   b. Rewrite constructor initialiser list with five fonts; add `QIcon` calls in body.
   c. Rewrite `font()` to switch on role and return matching member pointer or `QPlatformTheme::font(type)`.
   d. Remove `SystemIconThemeName` and `SystemIconFallbackThemeName` cases from `themeHint()`.
3. Update `test_platformtheme_smoke.cpp`: fix `FontIsInterTenPt` → 14pt; remove/replace `IconThemeIsPapirus`.
4. Add `tests/test_platformtheme_fonts.cpp`.
5. Add `tests/test_platformtheme_icons.cpp`.
6. Update `tests/CMakeLists.txt`: add the two new source files.
7. Run `/verify` (cmake build + clang-tidy + ctest).
