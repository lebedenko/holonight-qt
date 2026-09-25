# Size-aware named icons and explicit QML rendering

Baseline: `863af4183bdf09ce05199b37e8f5dfb46a311ba1`.

The platform theme creates a `QIconEngine` that resolves installed icon-theme metadata and inheritance for requested size and scale. It renders supported SVG and bitmap files. KDE semantic SVG roles use the application palette for normal, selected, and disabled modes. Explicit `-symbolic` SVGs without roles use an alpha mask. Ordinary full-color artwork keeps its paint. Rendered output must track theme, source, size, and palette changes.

The platform theme supplies its icon theme through `SystemIconThemeName`. It must not call `QIcon::setThemeName()`: Qt treats that as a user override and bypasses `createIconEngine()` for `QIcon::fromTheme()`.

`HnIcon` exposes `name` or `source` and `Original` or `Semantic` rendering. Original provider URLs pass through. Semantic rendering of an unsupported provider URL reports `hasError`. The QML path uses component colors; the `QIconEngine` uses the application palette. Do not add a KDE Frameworks dependency.

Verify fixture themes, inheritance, mixed paint, bitmaps, palette modes, fractional scaling, QML asset and provider handling, and load failures. Run focused tests before clean acceptance.

Implementation: `src/icons/hniconengine.*`, `iconthemeresolver.*`, `iconrenderer.*`, `hniconimageprovider.cpp`, `hniconprovider.*`, `src/platformtheme/holonighttheme.*`, `qml/HnIcon.qml`, affected shared controls, and tests in `test_icon_renderer.cpp`, `test_platformtheme_bootstrap.cpp`, `test_platformtheme_icons.cpp`, and `test_qml_smoke.cpp`.

Local verification (2026-09-25): the Qt build, 96 unaffected non-accelerated CTests, platform bootstrap CTest, and 17 focused icon/platform tests passed. The initial 97-test non-accelerated run found an obsolete assertion that the platform theme set a user theme; that assertion was corrected and rerun. Five accelerated checks passed initially; the sixth, a separator rendering check unrelated to icons, passed on its focused rerun.
