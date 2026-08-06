# TASKS: System-wide Qt theme activation and consumer safety

**Spec:** `docs/sdd/system-wide-theme-activation/SPEC.md`
**Design:** `docs/sdd/system-wide-theme-activation/DESIGN.md`
**Status:** Accepted

- [x] **T-001:** Inventory platform-theme selection, Widgets style registration, Qt Quick style
  metadata, installed QML layout, delegate imports, README activation claims, and existing
  package/consumer tests.
- [x] **T-002:** Make the canonical `Holonight` Qt Quick Controls style self-contained, declare
  the explicit Basic fallback, and add direct `Holonight.Core` imports to every dependent style
  delegate.
- [x] **T-003:** Add exact-case metadata and installed-artifact assertions that reject lowercase
  modules, aliases, and symlinks without changing public types or CMake targets.
- [x] **T-004:** Add a DESTDIR-aware install-time diagnostic for a pre-existing lowercase
  `qml/holonight` tree; warn actionably and continue canonical installation without mutating the
  legacy tree.
- [x] **T-005:** Implement the reusable first-party QML import-policy checker with actionable,
  file-local diagnostics.
- [x] **T-006:** Add positive and negative policy fixtures covering runtime Controls imports,
  explicit HoloNight imports, missing file-local imports, parent-only imports, and intentional
  qualified Basic controls.
- [x] **T-007:** Add an installed-prefix foreign-consumer fixture that imports only
  `QtQuick.Controls`, selects `Holonight` before engine creation, instantiates every style-owned
  control, checks HoloNight-specific properties, and fails on QML warnings.
- [x] **T-008:** Add a hybrid `QApplication` plus `QQmlApplicationEngine` fixture that sanitizes
  style environment variables and verifies the supported two-variable contract and platform-theme
  Widgets style selection.
- [x] **T-009:** Extend staged package validation with a seeded lowercase legacy tree; assert the
  warning content, byte-for-byte preservation of legacy content, and successful canonical install.
- [x] **T-010:** Correct README activation examples and add the compatibility matrix for Widgets,
  runtime-style Quick Controls, hybrid applications, compile-time third-party styles,
  custom-painted controls, static applications, and non-Qt toolkits.
- [x] **T-011:** Run import-policy, style metadata, installed-prefix consumer, hybrid consumer,
  platform-theme, and package tests.
- [x] **T-012:** Run all QML lint targets, `task lint`, and the complete headless test suite.
- [x] **T-013:** Manually verify Dolphin or another QWidget application, a HoloNight Qt Quick
  application, and the Qt/QML hyprpolkitagent under the supported environment; record clean QML
  logs and any documented compatibility limitation.
- [x] **T-014:** Review the final implementation against every requirement, confirm no duplicate
  control aliases or lowercase compatibility module landed, and update the SDD status only after
  acceptance is complete.
