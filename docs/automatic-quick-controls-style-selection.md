# Automatic HoloNight Quick Controls Style Selection

## Application default

Embed this file as `:/qtquickcontrols2.conf` in every graphical executable:

```ini
[Controls]
Style=Holonight
```

```cmake
qt_add_resources(my_app controls_config
    PREFIX "/"
    FILES qtquickcontrols2.conf
)
```

Application QML uses the runtime namespace for standard controls, enums and attached properties:

```qml
import QtQuick.Controls as Controls
import Holonight.Core
import Holonight.Controls

Controls.Button {
    text: qsTr("Apply")
    display: Controls.Button.TextOnly
    Controls.ToolTip.text: qsTr("Apply changes")
}
```

Do not import `Holonight` or `QtQuick.Controls.Basic` directly in application QML, declare a fixed style in the
application module's CMake `IMPORTS`, or call `QQuickStyle::setStyle()` in normal startup. Shared composites use
runtime Controls internally; Core stays independent of the selected style. `Holonight.Core` and `Holonight.Controls`
retain their intentional HoloNight appearance APIs under explicit style overrides.

## Overrides and coverage

The embedded configuration is an overridable default. Qt gives an imperative `QQuickStyle::setStyle()` call priority,
followed by command-line `-style`, `QT_QUICK_CONTROLS_STYLE`, and configuration. `QT_QUICK_CONTROLS_CONF` can select
another configuration file. Explicit competing style imports bypass runtime selection. See
[Qt style selection](https://doc.qt.io/qt-6/qtquickcontrols-styles.html).

The provider implements 27 standard controls. Other types use its declared Basic fallback; an application's explicit
fallback choice, such as Haruna's Fusion fallback, remains effective. Application-painted controls retain their own
visuals. A selected style name alone does not prove that every visible control uses HoloNight painting.

Standard controls honor effective application/control Qt palettes. Session appearance supplies defaults; choosing
an application palette does not write desktop appearance configuration. Composites retain their explicit appearance
boundary. Use `HnSearchField` or `HnTextArea` when an override-safe error API is needed, and `HnIconComboBox` for the
shared popup sizing API. HoloNight-only standard-control extensions are unavailable in other styles: guard optional
assignments or use public composite APIs.

## Deployment

Style selection and module discovery are separate requirements. Qt must find `Holonight/qmldir`, its QML files,
and ABI-compatible plugins through the effective QML import paths. A non-system prefix requires an application-relative
path or an explicit process import path. See [Qt deployment](https://doc.qt.io/qt-6/qtquickcontrols-deployment.html).

The platform-theme plugin has its own plugin search path and activation. `QT_QPA_PLATFORMTHEME=holonight` supplies
platform integration but does not by itself select the Quick Controls style. HoloNight sessions export both that
variable and `QT_QUICK_CONTROLS_STYLE=Holonight`, preserving explicit user overrides. Native dynamically linked Qt 6
applications are the supported third-party scope; Qt 5 Quick, static binaries, Flatpak and AppImage are outside this
initiative.

## Provider examples and verification

The demo and controls gallery embed the configuration above. Their build executables use the build QML directory;
installed executables discover the QML directory relative to their installation prefix without adding a build-tree
fallback. Build both examples to enable their startup acceptance:

```sh
cmake -S . -B build -DBUILD_TESTS=ON -DBUILD_DEMO=ON -DBUILD_CONTROLS_GALLERY=ON
cmake --build build -j 6
ctest --test-dir build -R 'startup_|qml_.*policy|package_install' --output-on-failure
```

Fresh offscreen processes check the embedded default, environment Fusion, command-line Fusion overriding environment
HoloNight, and an external Fusion configuration. The checks inspect runtime type resolution, loaded plugin paths and
QML diagnostics in build and staged installations. All gallery pages instantiate eagerly. These bounded startup
checks do not certify desktop interaction or visual appearance. Separate provider palette/rendering and composite
fixtures cover those contracts within their documented scope.

Consumer adoption, isolated missing-module/competing-style acceptance and final real-application checks are tracked
in the [UQC provider implementation record](sdd/unified-qtquick-controls/IMPLEMENTATION.md) and umbrella initiative.
Historical audio-page screenshots and discovery observations are not final ecosystem acceptance. Hyprland/Sway
activation and human-operated visual/input checks remain required before integration.
