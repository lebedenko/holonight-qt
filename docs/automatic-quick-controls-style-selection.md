# Automatic HoloNight Quick Controls Style Selection

## Summary

The HoloNight Qt Quick Controls style is implemented and installed correctly, but downstream applications do not
select it automatically from the platform-theme plugin alone. Importing `QtQuick.Controls` chooses controls from the
active style; it does not activate the `Holonight` style.

The reliable application-level mechanism that avoids an explicit `QQuickStyle::setStyle()` call is an embedded
`qtquickcontrols2.conf` resource:

```ini
[Controls]
Style=Holonight
```

The file must be available as `:/qtquickcontrols2.conf` before the first Qt Quick Controls type is loaded.

## Verified existing pieces

The shared library already provides the pieces needed after `Holonight` has been selected:

- The `Holonight` QML module exports styled implementations such as `Slider`, `Switch`, and `RadioButton`.
- Its generated `qmldir` declares the module as a Qt Quick Controls style and imports `QtQuick.Controls.Basic` as
  the fallback style.
- The style module and its QML files are installed under `lib/qt6/qml/Holonight`.
- The QPA platform-theme plugin is installed under `lib/qt6/plugins/platformthemes` and exposes the `holonight` key.
- `HoloniightTheme::themeHint(QPlatformTheme::StyleNames)` returns `Holonight` followed by `Fusion`.
- The inspected system installation matches the current source for the affected control files.

When `QT_QUICK_CONTROLS_STYLE=Holonight` is present, controls imported through
`import QtQuick.Controls as Controls` resolve to the HoloNight implementations. The observed audio-page controls
therefore demonstrate that the style is loading; their appearance is produced by the current HoloNight control and
palette definitions.

## Missing automatic-selection contract

No inspected downstream application resource provides `qtquickcontrols2.conf`. The settings application also does
not call `QQuickStyle::setStyle()`, so its Quick Controls style currently depends on process environment such as:

```text
QT_QUICK_CONTROLS_STYLE=Holonight
```

That environment variable was present in the validated desktop session, but it is an external deployment property,
not a self-contained application contract. A different launcher, service manager, test environment, or desktop
session may omit it and silently select Qt's platform default style.

The platform-theme plugin does not close this gap. `QPlatformTheme::StyleNames` is principally a Qt Widgets style
hint and is not a reliable selector for a custom Qt Quick Controls style. Installing or activating the HoloNight
platform theme is therefore insufficient to guarantee that `QtQuick.Controls` resolves through `Holonight`.

## Recommended downstream integration

Applications that should always use HoloNight controls without imperative style selection should:

1. Add a `qtquickcontrols2.conf` file containing the `[Controls]` configuration above.
2. Embed it at resource path `:/qtquickcontrols2.conf` in the application target.
3. Import public Qt controls through a namespace, for example:

   ```qml
   import QtQuick.Controls as Controls
   ```

4. Keep the `Holonight` QML style module discoverable through a standard Qt QML import path or application
   deployment bundle.
5. Keep `QtQuick.Controls.Basic` imports internal to the HoloNight style implementation; downstream application QML
   should not bypass style selection by importing Basic directly.

This configuration is application-specific by design. It makes the style deterministic without relying on a shell
environment and without coupling application startup code to `QQuickStyle`.

## Deployment considerations

Selecting the style and discovering the style module are separate requirements:

- `qtquickcontrols2.conf` selects the name `Holonight`.
- Qt must still find `Holonight/qmldir`, its QML files, and its plugin through the effective QML import paths.
- A non-system prefix must be exposed through deployment layout, an application-relative import path, or an
  appropriate runtime environment.
- The QPA platform-theme plugin has its own plugin-search path and activation requirements. It supplies palette,
  font, icon, and color-scheme integration, but should not be treated as the Quick Controls style selector.

Downstream build and packaging tests should cover both resource selection and installed-prefix discovery rather than
assuming the developer session's environment.

## Recommended verification

Add integration coverage that starts a minimal downstream QML application with style-related environment variables
cleared and verifies:

- the configured Quick Controls style name is `Holonight`;
- representative `QtQuick.Controls` types instantiate successfully;
- a representative control resolves to HoloNight-specific geometry or properties;
- the installed-prefix case works without source-tree QML import paths;
- direct `QtQuick.Controls.Basic` imports are absent from downstream application QML.

Also retain a negative test with no configuration to document that installing the platform-theme plugin by itself
does not guarantee HoloNight Quick Controls selection.

## Separate visual-design finding

Style activation should not be confused with visual correctness. In the inspected audio-page screenshot, the
controls were already rendered by the HoloNight style. The visible results follow the current shared definitions:

- `Slider.qml` colors its handle and filled groove with `HoloniightPalette.primary` and its border with
  `borderPassive`.
- `Switch.qml` uses HoloNight track and thumb metrics and palette roles.
- `RadioButton.qml` uses the HoloNight indicator geometry and selection colors.

If those controls should look different, the required work is a shared-control visual-design change in
`holonight-qt`, not another style-loading change in the downstream application.

## Acceptance criteria for automatic selection

- A downstream application with embedded `:/qtquickcontrols2.conf` uses `Holonight` when
  `QT_QUICK_CONTROLS_STYLE` is unset.
- The application does not call `QQuickStyle::setStyle()`.
- The application imports `QtQuick.Controls`, not `QtQuick.Controls.Basic`.
- Installed and development-prefix deployments both locate the HoloNight style module.
- Platform-theme activation remains independently configurable and is not required merely to select the QML style.
