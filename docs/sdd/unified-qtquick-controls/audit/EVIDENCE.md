# Collection evidence — 2026-09-06 (Europe/Kyiv)

UTC timestamps below fall on 2026-09-05. These are loading observations, not visual or interaction passes.
Raw logs remain temporary; these selected excerpts and executable hashes are the durable record.
Paths rooted at `/tmp/uqc-20260906-prefix` identify the staged installation.

## haruna-ordinary

Run: `/tmp/uqc-audit-zvjb3vrb`. Date: `2026-09-05T22:17:24.164857+00:00`. Exit: `124` (bounded timeout).
Executable: `/usr/bin/haruna`; SHA-256: `5c0e21490d29d40adf96cbc24e7d22399fba3860f9f184ebec7bc75eb51a9dd3`.

Selected environment:

```json
{
  "QT_QPA_PLATFORM": "offscreen",
  "QT_QPA_PLATFORMTHEME": "holonight",
  "QT_QUICK_CONTROLS_STYLE": "Holonight",
  "QT_QUICK_CONTROLS_CONF": null,
  "QT_QUICK_CONTROLS_FALLBACK_STYLE": null,
  "QML_IMPORT_PATH": "/tmp/uqc-20260906-prefix/lib/qt6/qml",
  "QML2_IMPORT_PATH": null,
  "QT_PLUGIN_PATH": "/tmp/uqc-20260906-prefix/lib/qt6/plugins",
  "QT_QUICK_BACKEND": "software",
  "QML_IMPORT_TRACE": "1",
  "QT_DEBUG_PLUGINS": "1",
  "QT_LOGGING_RULES": "*.debug=true;*.info=true",
  "XDG_SESSION_ID": null
}
```

Qt output was forced to stderr; GDB symbol downloads were disabled. No input was sent.

```text
unknown() : "/tmp/uqc-20260906-prefix/lib/qt6/plugins/styles/libholonight.so" loaded library
unknown() : "/tmp/uqc-20260906-prefix/lib/qt6/qml/Holonight/libholonight_qml.so" loaded library
unknown() : "/tmp/uqc-20260906-prefix/lib/qt6/qml/Holonight/Core/libholonight_core_qml.so" loaded library
qrc:/qt/qml/org/kde/haruna/qml/Main.qml | ApplicationWindow | qrc:/qt-project.org/imports/QtQuick/Controls/Fusion/ApplicationWindow.qml
qrc:/qt/qml/org/kde/haruna/qml/Header.qml | ToolBar | qrc:/qt-project.org/imports/QtQuick/Controls/Fusion/ToolBar.qml
qrc:/qt/qml/org/kde/haruna/qml/Haruna/Components/HamburgerMenu.qml | MenuItem | qrc:/qt/qml/Holonight/MenuItem.qml
qrc:/qt/qml/org/kde/haruna/qml/Haruna/Components/HamburgerMenu.qml | Menu | qrc:/qt/qml/Holonight/Menu.qml
qrc:/qt/qml/org/kde/haruna/qml/Haruna/Components/HamburgerMenu.qml | ToolButton | qrc:/qt-project.org/imports/QtQuick/Controls/Fusion/ToolButton.qml
qrc:/qt/qml/org/kde/haruna/qml/Haruna/Components/HamburgerMenu.qml | MenuSeparator | qrc:/qt-project.org/imports/QtQuick/Controls/Fusion/MenuSeparator.qml
qrc:/qt/qml/org/kde/haruna/qml/Haruna/Components/HamburgerMenu.qml | Popup | qrc:/qt-project.org/imports/QtQuick/Controls/Fusion/Popup.qml
qrc:/qt/qml/org/kde/haruna/qml/Header.qml | ToolSeparator | qrc:/qt-project.org/imports/QtQuick/Controls/Fusion/ToolSeparator.qml
qrc:/qt/qml/org/kde/haruna/qml/Header.qml | ToolButton | qrc:/qt-project.org/imports/QtQuick/Controls/Fusion/ToolButton.qml
qrc:/qt/qml/org/kde/haruna/qml/Footer.qml | ToolButton | qrc:/qt-project.org/imports/QtQuick/Controls/Fusion/ToolButton.qml
qrc:/qt/qml/org/kde/haruna/qml/Footer.qml | ToolBar | qrc:/qt-project.org/imports/QtQuick/Controls/Fusion/ToolBar.qml
qrc:/qt/qml/org/kde/haruna/qml/HProgressBar.qml | Popup | qrc:/qt-project.org/imports/QtQuick/Controls/Fusion/Popup.qml
qrc:/qt/qml/org/kde/haruna/qml/HProgressBar.qml | RadioDelegate | qrc:/qt-project.org/imports/QtQuick/Controls/Fusion/RadioDelegate.qml
qrc:/qt/qml/org/kde/haruna/qml/HProgressBar.qml | ToolTip | qrc:/qt/qml/Holonight/ToolTip.qml
qrc:/qt/qml/org/kde/haruna/qml/HProgressBar.qml | ToolBar | qrc:/qt-project.org/imports/QtQuick/Controls/Fusion/ToolBar.qml
qrc:/qt/qml/org/kde/haruna/qml/HProgressBar.qml | Slider | qrc:/qt/qml/Holonight/Slider.qml
qrc:/qt/qml/org/kde/haruna/qml/HProgressBar.qml | CheckBox | qrc:/qt/qml/Holonight/CheckBox.qml
qrc:/qt/qml/org/kde/haruna/qml/HProgressBar.qml | Label | qrc:/qt-project.org/imports/QtQuick/Controls/Fusion/Label.qml
qrc:/qt/qml/org/kde/haruna/qml/Footer.qml | ToolTip | qrc:/qt/qml/Holonight/ToolTip.qml
qrc:/qt/qml/org/kde/haruna/qml/VolumeSlider.qml | Slider | qrc:/qt/qml/Holonight/Slider.qml
qrc:/qt/qml/org/kde/haruna/qml/VolumeSlider.qml | Label | qrc:/qt-project.org/imports/QtQuick/Controls/Fusion/Label.qml
qrc:/qt/qml/org/kde/haruna/qml/Footer.qml | AbstractButton | qrc:/qt-project.org/imports/QtQuick/Controls/Basic/AbstractButton.qml
qrc:/qt/qml/org/kde/haruna/qml/Settings/SettingsWindow.qml | ItemDelegate | qrc:/qt/qml/Holonight/ItemDelegate.qml
qrc:/qt/qml/org/kde/haruna/qml/Settings/SettingsWindow.qml | Action | qrc:/qt-project.org/imports/QtQuick/Controls/Basic/Action.qml
qrc:/qt/qml/org/kde/haruna/qml/Header.qml | AbstractButton | qrc:/qt-project.org/imports/QtQuick/Controls/Basic/AbstractButton.qml
Thread 1 (Thread 0x7fffdb5fd740 (LWP 109618) "haruna"):
#0  0x00007ffff1ea0952 in ??? () at /usr/bin/../lib/libc.so.6
#1  0x00007ffff1f11a57 in ppoll () at /usr/bin/../lib/libc.so.6
#2  0x00007fffef500f59 in ??? () at /usr/lib/libglib-2.0.so.0
#3  0x00007fffef501055 in g_main_context_iteration () at /usr/lib/libglib-2.0.so.0
#4  0x00007ffff2a96f32 in QEventDispatcherGlib::processEvents(QFlags<QEventLoop::ProcessEventsFlag>) () at /usr/bin/../lib/libQt6Core.so.6
#5  0x00007ffff278f664 in QEventLoop::exec(QFlags<QEventLoop::ProcessEventsFlag>) () at /usr/bin/../lib/libQt6Core.so.6
#6  0x00007ffff278685a in QCoreApplication::exec() () at /usr/bin/../lib/libQt6Core.so.6
#7  0x000055555557fc85 in ??? ()
#8  0x00007ffff1e27781 in ??? () at /usr/bin/../lib/libc.so.6
#9  0x00007ffff1e278b9 in __libc_start_main () at /usr/bin/../lib/libc.so.6
#10 0x00005555555807a5 in ??? ()
```

## haruna-isolated

Run: `/tmp/uqc-audit-5fwbtbxc`. Date: `2026-09-05T22:17:45.290347+00:00`. Exit: `124` (bounded timeout).
Executable: `/usr/bin/haruna`; SHA-256: `5c0e21490d29d40adf96cbc24e7d22399fba3860f9f184ebec7bc75eb51a9dd3`.

Selected environment:

```json
{
  "QT_QPA_PLATFORM": "offscreen",
  "QT_QPA_PLATFORMTHEME": "holonight",
  "QT_QUICK_CONTROLS_STYLE": "Holonight",
  "QT_QUICK_CONTROLS_CONF": null,
  "QT_QUICK_CONTROLS_FALLBACK_STYLE": null,
  "QML_IMPORT_PATH": "/tmp/uqc-20260906-prefix/lib/qt6/qml",
  "QML2_IMPORT_PATH": null,
  "QT_PLUGIN_PATH": "/tmp/uqc-20260906-prefix/lib/qt6/plugins",
  "QT_QUICK_BACKEND": "software",
  "QML_IMPORT_TRACE": "1",
  "QT_DEBUG_PLUGINS": "1",
  "QT_LOGGING_RULES": "*.debug=true;*.info=true",
  "XDG_SESSION_ID": null
}
```

Qt output was forced to stderr; GDB symbol downloads were disabled. No input was sent.

```text
unknown() : "/tmp/uqc-20260906-prefix/lib/qt6/plugins/styles/libholonight.so" loaded library
unknown() : "/tmp/uqc-20260906-prefix/lib/qt6/qml/Holonight/libholonight_qml.so" loaded library
unknown() : "/tmp/uqc-20260906-prefix/lib/qt6/qml/Holonight/Core/libholonight_core_qml.so" loaded library
qrc:/qt/qml/org/kde/haruna/qml/Footer.qml | ToolTip | qrc:/qt/qml/Holonight/ToolTip.qml
qrc:/qt/qml/org/kde/haruna/qml/Footer.qml | ToolBar | qrc:/qt-project.org/imports/QtQuick/Controls/Fusion/ToolBar.qml
qrc:/qt/qml/org/kde/haruna/qml/Haruna/Components/HamburgerMenu.qml | MenuItem | qrc:/qt/qml/Holonight/MenuItem.qml
qrc:/qt/qml/org/kde/haruna/qml/Haruna/Components/HamburgerMenu.qml | ToolButton | qrc:/qt-project.org/imports/QtQuick/Controls/Fusion/ToolButton.qml
qrc:/qt/qml/org/kde/haruna/qml/Haruna/Components/HamburgerMenu.qml | MenuSeparator | qrc:/qt-project.org/imports/QtQuick/Controls/Fusion/MenuSeparator.qml
qrc:/qt/qml/org/kde/haruna/qml/Haruna/Components/HamburgerMenu.qml | Menu | qrc:/qt/qml/Holonight/Menu.qml
qrc:/qt/qml/org/kde/haruna/qml/Haruna/Components/HamburgerMenu.qml | Popup | qrc:/qt-project.org/imports/QtQuick/Controls/Fusion/Popup.qml
qrc:/qt/qml/org/kde/haruna/qml/Footer.qml | ToolButton | qrc:/qt-project.org/imports/QtQuick/Controls/Fusion/ToolButton.qml
qrc:/qt/qml/org/kde/haruna/qml/VolumeSlider.qml | Label | qrc:/qt-project.org/imports/QtQuick/Controls/Fusion/Label.qml
qrc:/qt/qml/org/kde/haruna/qml/VolumeSlider.qml | Slider | qrc:/qt/qml/Holonight/Slider.qml
qrc:/qt/qml/org/kde/haruna/qml/HProgressBar.qml | Label | qrc:/qt-project.org/imports/QtQuick/Controls/Fusion/Label.qml
qrc:/qt/qml/org/kde/haruna/qml/HProgressBar.qml | Slider | qrc:/qt/qml/Holonight/Slider.qml
qrc:/qt/qml/org/kde/haruna/qml/HProgressBar.qml | CheckBox | qrc:/qt/qml/Holonight/CheckBox.qml
qrc:/qt/qml/org/kde/haruna/qml/HProgressBar.qml | RadioDelegate | qrc:/qt-project.org/imports/QtQuick/Controls/Fusion/RadioDelegate.qml
qrc:/qt/qml/org/kde/haruna/qml/HProgressBar.qml | Popup | qrc:/qt-project.org/imports/QtQuick/Controls/Fusion/Popup.qml
qrc:/qt/qml/org/kde/haruna/qml/HProgressBar.qml | ToolBar | qrc:/qt-project.org/imports/QtQuick/Controls/Fusion/ToolBar.qml
qrc:/qt/qml/org/kde/haruna/qml/HProgressBar.qml | ToolTip | qrc:/qt/qml/Holonight/ToolTip.qml
qrc:/qt/qml/org/kde/haruna/qml/Footer.qml | AbstractButton | qrc:/qt-project.org/imports/QtQuick/Controls/Basic/AbstractButton.qml
qrc:/qt/qml/org/kde/haruna/qml/Settings/SettingsWindow.qml | ItemDelegate | qrc:/qt/qml/Holonight/ItemDelegate.qml
qrc:/qt/qml/org/kde/haruna/qml/Settings/SettingsWindow.qml | Action | qrc:/qt-project.org/imports/QtQuick/Controls/Basic/Action.qml
qrc:/qt/qml/org/kde/haruna/qml/Main.qml | ApplicationWindow | qrc:/qt-project.org/imports/QtQuick/Controls/Fusion/ApplicationWindow.qml
qrc:/qt/qml/org/kde/haruna/qml/Header.qml | ToolBar | qrc:/qt-project.org/imports/QtQuick/Controls/Fusion/ToolBar.qml
qrc:/qt/qml/org/kde/haruna/qml/Header.qml | ToolSeparator | qrc:/qt-project.org/imports/QtQuick/Controls/Fusion/ToolSeparator.qml
qrc:/qt/qml/org/kde/haruna/qml/Header.qml | ToolButton | qrc:/qt-project.org/imports/QtQuick/Controls/Fusion/ToolButton.qml
qrc:/qt/qml/org/kde/haruna/qml/Header.qml | AbstractButton | qrc:/qt-project.org/imports/QtQuick/Controls/Basic/AbstractButton.qml
Thread 1 (Thread 0x7fffdb5fd740 (LWP 109722) "haruna"):
#0  0x00007ffff1ea0952 in ??? () at /usr/bin/../lib/libc.so.6
#1  0x00007ffff1f11a57 in ppoll () at /usr/bin/../lib/libc.so.6
#2  0x00007fffef500f59 in ??? () at /usr/lib/libglib-2.0.so.0
#3  0x00007fffef501055 in g_main_context_iteration () at /usr/lib/libglib-2.0.so.0
#4  0x00007ffff2a96f32 in QEventDispatcherGlib::processEvents(QFlags<QEventLoop::ProcessEventsFlag>) () at /usr/bin/../lib/libQt6Core.so.6
#5  0x00007ffff278f664 in QEventLoop::exec(QFlags<QEventLoop::ProcessEventsFlag>) () at /usr/bin/../lib/libQt6Core.so.6
#6  0x00007ffff278685a in QCoreApplication::exec() () at /usr/bin/../lib/libQt6Core.so.6
#7  0x000055555557fc85 in ??? ()
#8  0x00007ffff1e27781 in ??? () at /usr/bin/../lib/libc.so.6
#9  0x00007ffff1e278b9 in __libc_start_main () at /usr/bin/../lib/libc.so.6
#10 0x00005555555807a5 in ??? ()
```

## neochat

Run: `/tmp/uqc-audit-3c3uewqe`. Date: `2026-09-05T22:18:56.832994+00:00`. Exit: `124` (bounded timeout).
Executable: `/usr/bin/neochat`; SHA-256: `1b012ee5b6a8e3659ce098e93916ac910bcdc52e92234199de64c664e370ea17`.

Selected environment:

```json
{
  "QT_QPA_PLATFORM": "offscreen",
  "QT_QPA_PLATFORMTHEME": "holonight",
  "QT_QUICK_CONTROLS_STYLE": "Holonight",
  "QT_QUICK_CONTROLS_CONF": null,
  "QT_QUICK_CONTROLS_FALLBACK_STYLE": null,
  "QML_IMPORT_PATH": "/tmp/uqc-20260906-prefix/lib/qt6/qml",
  "QML2_IMPORT_PATH": null,
  "QT_PLUGIN_PATH": "/tmp/uqc-20260906-prefix/lib/qt6/plugins",
  "QT_QUICK_BACKEND": "software",
  "QML_IMPORT_TRACE": "1",
  "QT_DEBUG_PLUGINS": "1",
  "QT_LOGGING_RULES": "*.debug=true;*.info=true",
  "XDG_SESSION_ID": null
}
```

Qt output was forced to stderr; GDB symbol downloads were disabled. No input was sent.

```text
qt.core.library: "/tmp/uqc-20260906-prefix/lib/qt6/plugins/styles/libholonight.so" loaded library
qt.core.library: "/tmp/uqc-20260906-prefix/lib/qt6/qml/Holonight/libholonight_qml.so" loaded library
qt.core.library: "/tmp/uqc-20260906-prefix/lib/qt6/qml/Holonight/Core/libholonight_core_qml.so" loaded library
qrc:/qt/qml/org/kde/neochat/login/WelcomePage.qml | QQC2.ToolButton | qrc:/qt-project.org/imports/QtQuick/Controls/Basic/ToolButton.qml
qrc:/qt/qml/org/kde/neochat/login/WelcomePage.qml | QQC2.ToolTip | qrc:/qt/qml/Holonight/ToolTip.qml
qrc:/qt/qml/org/kde/neochat/login/WelcomePage.qml | QQC2.Label | qrc:/qt-project.org/imports/QtQuick/Controls/Basic/Label.qml
qrc:/qt/qml/org/kde/neochat/login/WelcomePage.qml | QQC2.Control | qrc:/qt-project.org/imports/QtQuick/Controls/Basic/Control.qml
```

## tokodon

Run: `/tmp/uqc-audit-1znb89j6`. Date: `2026-09-05T22:18:55.042770+00:00`. Exit: `124` (bounded timeout).
Executable: `/usr/bin/tokodon`; SHA-256: `adf147e9e8b123821fc406c985929f8c84cf0436550312c4e3b861e35b492f23`.

Selected environment:

```json
{
  "QT_QPA_PLATFORM": "offscreen",
  "QT_QPA_PLATFORMTHEME": "holonight",
  "QT_QUICK_CONTROLS_STYLE": "Holonight",
  "QT_QUICK_CONTROLS_CONF": null,
  "QT_QUICK_CONTROLS_FALLBACK_STYLE": null,
  "QML_IMPORT_PATH": "/tmp/uqc-20260906-prefix/lib/qt6/qml",
  "QML2_IMPORT_PATH": null,
  "QT_PLUGIN_PATH": "/tmp/uqc-20260906-prefix/lib/qt6/plugins",
  "QT_QUICK_BACKEND": "software",
  "QML_IMPORT_TRACE": "1",
  "QT_DEBUG_PLUGINS": "1",
  "QT_LOGGING_RULES": "*.debug=true;*.info=true",
  "XDG_SESSION_ID": null
}
```

Qt output was forced to stderr; GDB symbol downloads were disabled. No input was sent.

```text
qt.core.library: "/tmp/uqc-20260906-prefix/lib/qt6/plugins/styles/libholonight.so" loaded library
qt.core.library: "/tmp/uqc-20260906-prefix/lib/qt6/qml/Holonight/libholonight_qml.so" loaded library
qt.core.library: "/tmp/uqc-20260906-prefix/lib/qt6/qml/Holonight/Core/libholonight_core_qml.so" loaded library
qrc:/qt/qml/org/kde/tokodon/qml/LoginFlow/WelcomePage.qml | QQC2.AbstractButton | qrc:/qt-project.org/imports/QtQuick/Controls/Basic/AbstractButton.qml
qrc:/qt/qml/org/kde/tokodon/qml/LoginFlow/WelcomePage.qml | QQC2.Label | qrc:/qt-project.org/imports/QtQuick/Controls/Basic/Label.qml
```

## Fixture

Reproduction: [checklist](CHECKLIST.md). All three fresh processes exited 0.
The checker asserted each top-level type URL and loaded provider library path; no system HoloNight library
was loaded. QRC URLs alone cannot distinguish installations. The system Qt import path remains available.

| Mode | Button / TextField | ApplicationWindow / Label / ToolButton / Dialog |
|---|---|---|
| Embedded default | Staged HoloNight | Basic |
| Explicit Fusion | Fusion; no HoloNight library loaded | Fusion |
| Embedded default + setFallbackStyle(Fusion) | Staged HoloNight | Fusion |

Raw fixture logs: `/tmp/uqc-fixture-t52beawq/{default,fusion,haruna-fallback}.log`.

## Matching upstream sources

Retrieved from canonical KDE GitHub mirrors at release tags; hashes identify retrieved content.
Sources describe possible surfaces, including lazy pages. They do not establish that those pages were shown.

| Source | SHA-256 |
|---|---|
| [neochat src/app/main.cpp](https://github.com/KDE/neochat/blob/v26.08.0/src/app/main.cpp) | `8a8af655439caf298e7c72656a20928b2f58f8b069ee3b92996b1ebdb1d7b434` |
| [neochat src/login/WelcomePage.qml](https://github.com/KDE/neochat/blob/v26.08.0/src/login/WelcomePage.qml) | `f3091fc84b7487e74018a8ddb722540f9db20fde18d1a28f200de683db8c07a1` |
| [neochat src/login/Homeserver.qml](https://github.com/KDE/neochat/blob/v26.08.0/src/login/Homeserver.qml) | `e146e00720fa631ec85c0a8c3856379e7cd7cd8de30b2a013e8b21b7b27004ed` |
| [tokodon src/main.cpp](https://github.com/KDE/tokodon/blob/v26.08.0/src/main.cpp) | `16e439f40c2a4fd7b30abc89aa448e752d0a44be7df49870fc00eeb4ae83583c` |
| [tokodon src/qml/LoginFlow/WelcomePage.qml](https://github.com/KDE/tokodon/blob/v26.08.0/src/qml/LoginFlow/WelcomePage.qml) | `fec216cb83091673245949d86fb62e9832f38d8799314a08f3cb990afad11a93` |
| [tokodon src/qml/LoginFlow/ServersPage.qml](https://github.com/KDE/tokodon/blob/v26.08.0/src/qml/LoginFlow/ServersPage.qml) | `5ee74b259cf6d755ab070f5ef4f4e184c04ea4978884fd8a7a5b9e7f151894f2` |
| [haruna src/qml/HProgressBar.qml](https://github.com/KDE/haruna/blob/v1.8.1/src/qml/HProgressBar.qml) | `e07f6588a440d134101087ea5b917bee2a2f47fee114f7b6c95461b858bfa131` |
| [haruna src/qml/VolumeSlider.qml](https://github.com/KDE/haruna/blob/v1.8.1/src/qml/VolumeSlider.qml) | `70eea4dd0f7bebe57f74069b34bfe0a07e8f0ed74beef6b97b781c12316e2b40` |

Release tree revisions: NeoChat `e3da5107a962af6401a88d0283a9377a4864917b`;
Tokodon `206e3ea8177460e0339fcedf8e6628ec9d963ada`.

## Haruna menu bar dependent origin

```text
unknown() : resolveType: qrc:/qt/qml/org/kde/haruna/qml/MenuBarLoader.qml "QQC2.MenuBar"  =>  ""   QUrl("qrc:/qt-project.org/imports/QtQuick/Controls/Fusion/MenuBar.qml")  TYPE/URL
unknown() : resolveType: qrc:/qt-project.org/imports/QtQuick/Controls/Fusion/MenuBar.qml "MenuBarItem"  =>  ""   QUrl("qrc:/qt-project.org/imports/QtQuick/Controls/Fusion/MenuBarItem.qml")  TYPE/URL
```
