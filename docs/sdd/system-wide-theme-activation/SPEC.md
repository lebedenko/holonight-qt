# SPEC: System-wide Qt theme activation and consumer safety

**Feature:** Define and verify supported system-wide HoloNight theme activation
**Status:** Accepted
**Date:** 2026-08-06
**Accepted:** 2026-08-06

## Goal

Provide one supported session-wide activation contract for HoloNight across Qt Widgets and
runtime-style Qt Quick Controls applications, while preserving the canonical case-sensitive QML
module and preventing first-party consumers from relying on accidental or transitive imports.

## Activation requirements

- **REQ-A-001:** The supported session environment shall be:

  ```text
  QT_QPA_PLATFORMTHEME=holonight
  QT_QUICK_CONTROLS_STYLE=Holonight
  ```

- **REQ-A-002:** `QT_QPA_PLATFORMTHEME=holonight` shall select the HoloNight Qt platform theme and
  its Widgets style for applications that use the platform-theme mechanism.
- **REQ-A-003:** `QT_QUICK_CONTROLS_STYLE=Holonight` shall select the canonical HoloNight Qt Quick
  Controls style for applications that permit runtime style selection.
- **REQ-A-004:** A global `QT_STYLE_OVERRIDE=holonight` shall be documented as unsupported for
  hybrid `QApplication`/Qt Quick applications. It may be used as a per-application override for a
  Widgets-only application.
- **REQ-A-005:** Activation documentation shall not claim coverage for applications that select a
  different style at compile time, bypass Qt controls with custom painting, are statically linked
  without HoloNight, or do not use Qt.

## QML style and packaging requirements

- **REQ-QML-001:** `Holonight` shall remain the only public, case-sensitive URI for the Qt Quick
  Controls style. No lowercase module, compatibility export, or symlink shall be generated.
- **REQ-QML-002:** The installed `Holonight` style shall be a conventional, self-contained runtime
  Qt Quick Controls style with an explicit `Basic` fallback.
- **REQ-QML-003:** Every style delegate that consumes HoloNight Core types shall declare a direct
  `Holonight.Core` dependency; dependency resolution shall not rely on a parent, sibling, or
  transitive import.
- **REQ-QML-004:** Installation shall perform a DESTDIR-aware check for an existing lowercase
  `qml/holonight` tree at the effective installation prefix.
- **REQ-QML-005:** When the legacy lowercase tree exists, installation shall emit an actionable
  warning identifying it as stale and potentially shadowing canonical metadata. Installation
  shall neither delete nor modify that tree and shall still install the canonical module.
- **REQ-QML-006:** Existing public QML type names, CMake targets, module ownership, and control
  visuals shall remain unchanged. No duplicate aliases such as `HnButton` or `HnTextField` shall
  be introduced.

## First-party consumer import policy

- **REQ-C-001:** First-party QML shall prefer `import QtQuick.Controls` and application-level
  runtime style selection for standard controls.
- **REQ-C-002:** A file that uses controls supplied through the compile-time `Holonight` module
  shall declare `import Holonight` in that file.
- **REQ-C-003:** A parent component import, transitive module dependency, or sibling file import
  shall never satisfy REQ-C-002.
- **REQ-C-004:** Explicit, qualified use of `Basic` controls shall remain permitted when it is
  intentional and visible in the source.
- **REQ-C-005:** A reusable source-policy checker shall enforce these rules and support both
  repository QML and standalone fixture trees.

## Compatibility requirements

The README shall publish the following boundaries:

| Consumer | Expected result | Boundary |
| --- | --- | --- |
| Qt Widgets | HoloNight through the platform theme | Application must honor Qt platform-theme/style selection |
| Runtime-style Qt Quick Controls | HoloNight through `QT_QUICK_CONTROLS_STYLE` | Application must not lock another style before engine startup |
| Hybrid Widgets and Qt Quick | Both surfaces themed by the supported two-variable contract | Global `QT_STYLE_OVERRIDE` is unsupported |
| Compile-time third-party Qt Quick style | Third-party style remains authoritative | HoloNight does not force replacement |
| Custom-painted controls | Application rendering remains authoritative | Palette adoption, if any, is application-specific |
| Static application | Supported only when required HoloNight plugins/modules are linked and discoverable | Environment variables cannot add missing code |
| Non-Qt toolkit | Unchanged | Outside HoloNight's scope |

## Verification requirements

- **REQ-V-001:** An installed-prefix foreign-consumer fixture shall import only
  `QtQuick.Controls`, select `Holonight` at runtime, instantiate every style-owned control, verify
  HoloNight-specific observable properties, and fail on QML warnings.
- **REQ-V-002:** A hybrid fixture using `QApplication` and `QQmlApplicationEngine` shall verify the
  supported two-variable environment and Widgets style selection through the platform theme.
- **REQ-V-003:** Automated checks shall verify case-sensitive module metadata, the explicit Basic
  fallback, direct Core dependencies, and the absence of generated lowercase aliases.
- **REQ-V-004:** Package validation shall install over a pre-populated lowercase legacy tree and
  prove that an actionable warning is emitted, the tree is byte-for-byte untouched, and the
  canonical module is installed correctly.
- **REQ-V-005:** Import-policy tests shall include explicit HoloNight imports, missing file-local
  imports, parent-only imports, and intentional qualified Basic controls.
- **REQ-V-006:** QML lint targets, focused platform/style/package tests, `task lint`, and the full
  headless suite shall pass before implementation is accepted.
- **REQ-V-007:** Manual validation shall cover Dolphin or another QWidget application, a
  HoloNight Qt Quick application, and the Qt/QML hyprpolkitagent with clean QML logs.

## Constraints and non-goals

- Obsolete installations are diagnosed, not automatically repaired or removed.
- Applications that explicitly choose another compile-time style or custom rendering are
  documented limitations, not targets for forced restyling.
- PAM lockouts and hyprpolkitagent administrator-identity selection are outside this SDD.
- This work does not redesign controls or change existing public API names.

## Acceptance

The documented two-variable session contract themes compatible Widgets and Qt Quick consumers,
the canonical installed style works for a foreign runtime-style consumer without warnings, stale
lowercase installations are diagnosed without mutation, and automated policy checks prevent
first-party file-local import regressions. All automated and manual checks in REQ-V-001 through
REQ-V-007 pass.
