# KDE-compatible semantic icon recoloring tasks

Status: Implemented and verified.
Date: 2026-09-30.
Owner: `holonight-qt` shared icon rendering and palette integration.

## Outcome

Render SVG icons with independently responsive paper and glyph colors through
both HoloNight's Qt icon engine and QML image provider. A paper sheet using
`ColorScheme-Text` and a glyph using `ColorScheme-Accent` must follow the current
palette, remain distinguishable when selected, and update when only Accent changes.
Preserve path geometry, layer order, gradients, opacity and fixed decorative paints.
Do not introduce a KDE runtime dependency.

Artwork, source-theme metadata and generated icon themes remain owned by
`holonight-icons`. This task changes shared rendering in `holonight-qt`.

## Evidence and current gaps

The sibling `holonight-icons` repository has an optional `task test:kde` that uses
an isolated theme and native `KIconLoader`. See its
[`docs/kde-native-recoloring.md`](../../../../holonight-icons/docs/kde-native-recoloring.md)
and [`tests/kde/application-xml.svg`](../../../../holonight-icons/tests/kde/application-xml.svg).
Those relative links assume sibling checkouts with their usual repository names.

On KDE IconThemes 6.30.0 / Qt 6.11.2, the prototype passed 24 native renders:
light/dark palettes, normal/selected/disabled states, 24/32 px and 1x/2x.
The 24 px cases scale the 32 px prototype; they do not prove a separate optical master.
Text, Highlight and Accent were deliberately different. Native selected Text and
Highlight both became highlighted text; Accent stayed distinct. Fixed translucent
black/white shading was preserved in normal rendering.

The native test also found a cache-refresh failure: changing only Accent with
`setCustomPalette()` returned the old Accent for an already loaded name, while an
uncached name rendered the new color. Do not reproduce this behavior. This result
concerns the isolated API path, not a verified Dolphin/Plasma notification failure.

Current HoloNight gaps:

- `IconSemanticColors` and `IconRenderer` support only Text, Highlight,
  PositiveText, NeutralText and NegativeText.
- QML provider URLs, palette hashes and rendered-image cache keys omit Accent.
- `HnIconEngine::colorsForMode()` changes selected Text but does not provide
  Accent; its positive/neutral/negative colors currently all use palette Text.
- `HnIcon.qml` exposes Normal, Muted, Disabled and Active, but no Selected state.
- The palette builder explicitly sets Highlight, but not `QPalette::Accent`.
- Background and HighlightedText stylesheet declarations currently retain defaults.

## Implementation checklist

### 1. Shared semantic roles and SVG handling

- [x] Extend `src/icons/iconrenderer.h` with an independent Accent color.
- [x] Complete the KDE role set with Background and HighlightedText as well.
- [x] Update `applySemanticColors()`, semantic-definition detection and assigned-role
      detection in `src/icons/iconrenderer.cpp` for all eight supported roles.
- [x] Restrict semantic stylesheet updates to `style#current-color-scheme`.
      Support valid attribute ordering and quoting; do not replace unrelated hex paints.
- [x] Ensure an Accent-only icon is recognized as semantic and cannot enter the
      legacy whole-image monochrome fallback.
- [x] Preserve literal black/white lighting, alpha, geometry and layer order.
      Original rendering mode must remain byte/paint faithful to authored SVGs.
- [x] Preserve old callers through an explicit compatibility policy for omitted
      new colors. Update aggregate initializers deliberately; never silently shift fields.

### 2. Palette construction and Qt icon engine

- [x] Set `QPalette::Accent` from the resolved primary token in `palette/palette.cpp`;
      define Active, Inactive and Disabled values from the shared palette tokens.
- [x] Audit palette producers/overrides in `src/style/`, `src/platformtheme/` and
      QML palette adapters so explicit Accent is not lost when palettes are rebuilt.
- [x] Keep Accent and Highlight distinct roles even where HoloNight defaults coincide.
- [x] Update `src/icons/hniconengine.cpp` to supply all semantic colors and state
      information. Use context/palette foreground semantics deliberately.
- [x] Resolve positive/neutral/negative colors from existing semantic tokens rather
      than mapping all of them to Text when claiming full semantic compatibility.
- [x] Guard version-specific Qt APIs where older supported Qt or Qt 5 targets require
      it; do not break compatibility targets just to add Qt 6 Accent support.

### 3. One state-resolution policy for Qt and QML

- [x] Introduce shared state/color resolution used by both consumers.
- [x] Normal: resolve Text from the context foreground, Accent independently,
      and Highlight from selection background.
- [x] Selected: apply KDE-compatible role remapping. Text and Highlight become
      highlighted text; Background becomes selection background; HighlightedText
      becomes selection background. Positive/Neutral/Negative become highlighted text.
- [x] Preserve selected Accent as an accent-derived color. Verify the target KDE
      behavior before encoding its blend (current upstream uses 85% Accent plus
      15% highlighted text); do not collapse it into the paper color.
- [x] Establish the disabled policy before implementation: exact KDE pixel parity
      needs its image-effect behavior, while HoloNight may intentionally retain its
      token-based disabled treatment. Record the decision and test both consumers
      against it. Supplying only a disabled Text color is insufficient.
- [x] Append a Selected state/property to `qml/HnIcon.qml` without renumbering
      existing public enum values. Active and Selected remain separate concepts.
- [x] Ensure requesting state changes triggers rerendering rather than reusing pixels.

### 4. QML URL/provider pipeline and caches

- [x] Extend `src/icons/hniconprovider.h/.cpp` so `sourceUrl()` transports Accent,
      Background, HighlightedText and state/effect information where needed.
- [x] Preserve the existing QML-callable API through the separately named `sourceUrlWithOptions()` API;
      do not insert positional parameters that reinterpret existing arguments.
- [x] Update `qml/HnIcon.qml` bindings and relevant control call sites to supply
      resolved semantic colors and selection/disabled context.
- [x] Decode the new values in `src/icons/hniconimageprovider.cpp` with documented
      defaults for old URLs and validate supplied colors.
- [x] Include every resolved color (including alpha), applicable state/effects,
      source content and physical rendering size/DPR in cache identity.
- [x] Include the new colors in provider palette hashes and QML source URLs so
      Qt Quick's own image cache cannot hide an Accent-only update.
- [x] Retain content-sensitive source caching and existing in-flight request safety.
- [x] Verify normal-to-selected-to-normal changes and Accent-only changes without
      requiring a source rename, manual cache clear or application restart.

### 5. Regression and integration coverage

- [x] Extend `tests/test_icon_renderer.cpp` with all-role and Accent-only fixtures;
      use distinct Text, Highlight and Accent colors to detect accidental aliases.
- [x] Test the paper/glyph prototype with fixed translucent black/white shading.
      Keep copyright and GPL notices if importing the Papirus-derived fixture.
- [x] Test normal/selected/disabled states, light/dark palettes and 24/32 px at 1x/2x.
      Account for antialiased edges instead of requiring pure glyph pixels at 24 px.
- [x] Verify actual palette propagation through the Qt icon engine, not only direct
      calls to `IconRenderer::renderSvg()` with pre-resolved colors.
- [x] Add QML/provider tests for URL changes, cached pixels, selection and an
      Accent-only update with all other colors and source bytes unchanged.
- [x] Verify Original rendering, fixed-color artwork, mixed artwork, explicit
      symbolic fallback and existing five-role consumers remain correct.
- [ ] Compare against `holonight-icons`' native KDE fixtures as an optional developer
      check. Keep KDE out of the normal build/runtime dependency graph.
- [x] Run focused palette/icon/QML tests, then `task verify`; document environment
      restrictions and any intentionally different disabled-state appearance.
- [x] Update public icon documentation with roles, state behavior, defaults,
      backwards compatibility and cache-update guarantees.

## Acceptance criteria

1. The same Text/Accent prototype recolors through both Qt and QML paths.
2. Changing only Accent updates existing displayed icons without restarting.
3. Selecting the icon does not map both paper and glyph to one color.
4. Both consumers use the same documented disabled policy.
5. Fixed artwork, shading and geometry survive normal recoloring unchanged.
6. New role support does not break existing icon APIs, fallback paths or caches.
7. Existing verification passes, and test results include actual consumer paths.
8. Production HoloNight remains usable without KDE libraries installed.

## Companion work in holonight-icons

After the shared renderer is ready, extend its palette/validator role definitions,
replace whole-asset import exemptions with exact decorative-layer exemptions,
and promote reviewed prototypes into the canonical MIME-type masters. Run that
repository's `task verify` separately. Do not edit its generated themes by hand.

## References

- [KDE native role/state implementation](https://github.com/KDE/kiconthemes/blob/master/src/kiconcolors.cpp)
- [KDE native SVG loader](https://github.com/KDE/kiconthemes/blob/master/src/kiconloader.cpp)
- [KIconLoader API](https://api.kde.org/kiconloader.html)
- [Existing exact icon rendering tasks](../exact-icon-rendering/TASKS.md)

## Implementation progress (2026-09-30)

Implemented eight roles with original aggregate field ordering retained. XML parsing
limits declaration edits to the decoded `current-color-scheme` style ID; comments,
CDATA, unrelated styles, geometry and authored paints are preserved. A QtSvg-only
rendering adaptation uses `color-opacity` for semantic alpha, since QtSvg rejects
CSS RGBA colors; the public byte rewrite changes only color values.

The shared resolver implements KDE Selected and the chosen HoloNight disabled
policy: disabled palette roles and 50% background-blended default Accent/status
colors, retaining source alpha and applying no whole-image disabled effect.
Active and On remain independent of selection. Accent is explicitly populated in
Qt >= 6.6 palettes; Qt 5/older Qt fallbacks are guarded. Existing style rebuilding
uses `current.resolve(palette_)` and preserves its original resolve mask; the
platform palette uses the shared builder. Window and icon palette adapters resolve
application/control overrides against current appearance defaults without aliasing
Accent to Highlight.

QML Core now observes palette context, application events and appearance updates.
Control composites pass their owning palettes; Item composites expose an optional
forwarded context. Popup icon delegates select when highlighted and prioritize
disabled; pressed buttons remain Active. Legacy APIs and URLs retain five-role
compatibility. New options serialize state-resolved RGBA values, state, content
revision and DPR; rendered caches additionally track physical size and symbolic
mode, preserving content checks, bounded costs and in-flight coordination.

Validation so far:

- 19 focused renderer/provider/engine tests passed, including the 24-case all-role
  palette/state/size/DPR matrix and the 24-case Papirus-derived paper/glyph prototype.
- Live QML normal/selected/disabled and Accent-only/alpha-only updates passed at
  1x and 2x, including application and control overrides and 24/32 px rendering.
- Existing QML smoke, package-install, provider/demo/gallery import-policy checks,
  palette variants and both examples' startup tests passed in the focused run.
- `task verify HOLONIGHT_CONFIG_BUILD_DIR=/tmp/hn-config-build` passed: build,
  clang-tidy (with configured nonfatal warnings), and all 95 CTest entries. The
  dependency build stayed in `/tmp`; `patchelf` was installed there for package tests.
- Qt 5 probes are unavailable: Qt 5 development CMake packages are not installed.
- Native KDE comparison was not run. Normal build/test paths require no KDE libraries.

The imported prototype carries Papirus Development Team and Andrii L attribution
and GPL-3.0-only notices in `tests/fixtures/icons/application-xml.svg`. Its 24 px
render scales the 32 px prototype; it does not assert a separate optical master.

Final validation also passed `reuse lint` (435/435 files with licensing metadata)
and `git diff --check`. One earlier concurrent verification attempt collided in
shared package/startup scratch directories; the final isolated `task verify` run
passed every check. Legacy URLs without DPR preserve their original asset lookup
behavior; options URLs use logical size and DPR for lookup and requested physical
size directly for rendering. No companion repository files were changed.

## Deferred palette audit

[Selection color consistency](../selection-color-consistency/TASKS.md) records
the proposed contrast and scheme/accent audit. It does not change palette behavior.
