# CTV-004 — Qt 5 Widgets feasibility

Status: Complete

## Objective and baseline

Evaluate Qt 5 palette/style and platform-theme support independently from the published `holonight-qt@fb0acc6`
baseline. The reference systems are Ubuntu 24.04's Qt 5 development baseline and Arch Linux Qt 5.15.19 with
KeePassXC 2.7.12. These discovery artifacts are non-installed prototypes, not a Qt 5 product contract.

## Scope and boundaries

- `BUILD_QT5_PROBES` is `OFF` by default and is the only entry point for Qt 5 discovery targets.
- Qt 5 recompiles the existing token catalog, theme catalog, accent resolver, palette builder, shape resolver, and
  compatible Widgets renderer from source. It never links a Qt 6 target and does not duplicate the token catalog.
- `qt5/qt5probeappearance.*` is the private compatibility boundary for the discovery configuration and Qt-major
  runtime defaults. It intentionally does not change the canonical configuration schema or Qt 6 watcher contract.
- The Qt 5 platform theme is a separate source prototype because `QPlatformTheme` differs across majors. In
  particular, Qt 5 has no `Qt::ColorScheme`/`QPlatformTheme::colorScheme()` contract.
- Qt 5 plugins are emitted only to `build/qt5/plugins/styles` and `build/qt5/plugins/platformthemes`. They have no
  install rule, package export, or QML target. Qt 6 output and MOC directories remain unchanged.

## API and packaging findings

Qt 5 lacks `QLatin1StringView`, `QFlags::fromInt`, and `Q_UNREACHABLE_RETURN`; small source-compatible forms are used
in shared files. Palette roles used here exist in both 5.15 and 6.x. The private QPA plugin headers are available via
`Qt5::GuiPrivate`, but that interface is not ABI stable. Any future platform-theme package would therefore have to be
built against, depend on, and be rebuilt for the exact Qt 5 minor/package revision. Loading a plugin built for a
different private-QPA revision is unsupported.

The optional style and platform theme must be packaged separately from Qt 6 artifacts and from each other. Removing
either build-tree directory leaves Qt's Fusion/platform defaults available; no global `QT_PLUGIN_PATH`,
`QT_QPA_PLATFORMTHEME`, `QT_STYLE_OVERRIDE`, or session setting is changed by this study.

## Automated evidence

On Arch Qt 5.15.19 the combined Qt 6 + Qt 5 build succeeds. The Qt 5 probe covers all 12 built-in schemes and the
`default`, `cyan`, `blue`, `violet`, and `yellow` accent inputs; exact Window, Base, Highlight, HighlightedText, and
disabled Text roles; deterministic scheme resolution; primary-text contrast; focused/hovered button rendering;
style discovery; and platform palette, style, icon, and font hints. ELF checks reject Qt 6 dependencies in Qt 5
plugins and Qt 5 dependencies in existing Qt 6 plugins.

The matrix identifies three existing semantic combinations below the 3:1 selection foreground gate:
`holonight-light/cyan` (2.766:1), `holonight-latte/cyan` (2.793:1), and `holonight-latte/yellow` (2.617:1). Tests
record those exact failures so they cannot disappear from the decision record while the prototype remains.

## KeePassXC runtime evidence — 2026-08-09

The following isolated launches used temporary HOME/config/data directories and the build-tree plugin root:

```sh
env HOME=/tmp/ctv004-keepassxc-native XDG_CONFIG_HOME=/tmp/ctv004-keepassxc-native/config \
  XDG_DATA_HOME=/tmp/ctv004-keepassxc-native/data QT_PLUGIN_PATH=$PWD/build-ctv004/qt5/plugins \
  QT_QPA_PLATFORM=wayland QT_QPA_PLATFORMTHEME=holonight QT_STYLE_OVERRIDE=Holonight keepassxc

env HOME=/tmp/ctv004-keepassxc-xwayland XDG_CONFIG_HOME=/tmp/ctv004-keepassxc-xwayland/config \
  XDG_DATA_HOME=/tmp/ctv004-keepassxc-xwayland/data QT_PLUGIN_PATH=$PWD/build-ctv004/qt5/plugins \
  QT_QPA_PLATFORM=xcb QT_QPA_PLATFORMTHEME=holonight QT_STYLE_OVERRIDE=Holonight keepassxc

timeout 5s env HOME=/tmp/ctv004-keepassxc-native \
  XDG_CONFIG_HOME=/tmp/ctv004-keepassxc-native/config XDG_DATA_HOME=/tmp/ctv004-keepassxc-native/data \
  QT_PLUGIN_PATH=/tmp/ctv004-no-plugins QT_QPA_PLATFORM=wayland QT_QPA_PLATFORMTHEME= QT_STYLE_OVERRIDE= keepassxc
```

Both plugin-enabled launches reached and retained the KeePassXC welcome window until manually terminated. Native
Wayland logged only the expected unsupported activation request; XWayland emitted no diagnostic. The isolated
native fallback also remained running until the timeout with both prototypes absent. Automated offscreen probes,
including a `QT_SCALE_FACTOR=2` run, cover palette, selection, disabled roles, focus rendering, fonts, icons, and
high-DPI startup. No screenshot was retained. Native-dialog integration was not implemented or claimed, so that gate
fails rather than being inferred from application startup.

## Decisions

### Qt 5 semantic palette and Widgets style — reject for CTV-201

Recommend CTV-201 become `Superseded` with native Qt/Fusion fallback. Compilation, single-major linkage, loading,
rendering, and KeePassXC startup pass, but the required every-scheme/every-accent contrast gate fails. A future
proposal may reconsider the package after the canonical accent foreground mapping is fixed without regressing Qt 6.

### Qt 5 platform theme — reject for CTV-202

Recommend CTV-202 become `Superseded` with the native Qt platform theme. Palette, fonts, icons, and style hints work,
and the exact-minor rebuild policy is viable, but this prototype deliberately provides no native-dialog backend.
Consequently the dialogs gate is not satisfied, and the private-QPA maintenance cost is not justified for hints that
can be supplied through stable/native mechanisms. The platform prototype remains useful only as non-installed
evidence.

## Limitations

Ubuntu 24.04 execution is delegated to CI and requires `qtbase5-dev` plus `qtbase5-private-dev`. This local run does
not establish compatibility with every Qt 5 patch set, application-owned rendering, screen-reader behavior, or
fractional compositor scaling. No Qt 5 QML modules, schemas, installers, or runtime activation are delivered.
