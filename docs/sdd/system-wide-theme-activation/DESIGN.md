# DESIGN: System-wide Qt theme activation and consumer safety

**Spec:** `docs/sdd/system-wide-theme-activation/SPEC.md`
**Status:** Accepted
**Date:** 2026-08-06
**Accepted:** 2026-08-06

## Activation model

The session contract deliberately uses the two Qt mechanisms that own the respective surfaces:

```text
QT_QPA_PLATFORMTHEME=holonight
QT_QUICK_CONTROLS_STYLE=Holonight
```

The lowercase platform-theme key is a plugin selection key; the case-sensitive `Holonight` value
is the canonical QML style URI. They are not aliases and their different casing is intentional.
The platform-theme plugin supplies Widgets integration. Qt Quick Controls performs its own style
selection before controls are loaded and therefore needs the second variable.

`QT_STYLE_OVERRIDE=holonight` bypasses the normal platform-theme choice for the Widgets style.
Because it can leak into hybrid processes and conflict with Qt Quick initialization or a
consumer's style policy, README examples shall not export it globally. Documentation may show it
only as a command-local Widgets-only troubleshooting or override mechanism.

## Runtime style packaging

The canonical install tree remains `qml/Holonight`. Its `qmldir`/generated module metadata shall
declare an unversioned `Basic` fallback using Qt's supported style dependency mechanism, so
versioned consumers do not incorrectly require a matching legacy Controls version. Every style
delegate imports `Holonight.Core` itself when it references palette, appearance, shape, icon, or
other Core types. This makes delegates independently loadable and prevents success from depending
on importer order.

There will be no lowercase `qml/holonight` module, filesystem symlink, generated alias type, or
second set of wrappers. A second URI would make behavior filesystem-dependent, conceal broken
consumer imports, and create ambiguous upgrade behavior.

## Legacy installation diagnostic

The install process shall inspect the effective destination assembled from `DESTDIR` and the
configured QML installation path. If `<effective-qml-root>/holonight` exists, it emits a CMake
warning that includes:

- the detected path;
- the canonical `Holonight` spelling and destination;
- the risk of stale metadata shadowing or confusing module discovery; and
- an instruction for the administrator to inspect and remove the legacy tree manually when safe.

The check is informational. It must not traverse, delete, rename, rewrite, or merge the legacy
tree. Normal installation continues, including canonical-module installation. Package tests use a
temporary staged prefix, seed a sentinel and representative legacy content, capture install
output, and compare the seeded tree before and after installation.

## First-party import-policy checker

The checker is a reusable repository script or test helper accepting one or more QML roots. It
evaluates each QML file independently. Its diagnostics identify the file, the relevant type use,
and the missing or disallowed import so failures are actionable.

The policy distinguishes three intentional patterns:

1. Standard runtime-styled controls use `import QtQuick.Controls`; style selection belongs to the
   application startup/session environment.
2. Types intentionally obtained from the compile-time style module require a file-local
   `import Holonight`.
3. Basic controls may be imported under an explicit qualifier and used through that qualifier.

An import in a parent component, another file, or a transitively imported module does not count.
Fixtures provide positive and negative cases for each boundary, including parent-only imports.
The checker shall not require or generate convenience aliases such as `HnButton` or
`HnTextField`.

## Integration fixtures

### Installed-prefix foreign consumer

The fixture is configured against a staged installed HoloNight prefix rather than the source or
build-tree import path. Its QML imports only `QtQuick.Controls`; process setup selects
`Holonight` before constructing the QML engine. It instantiates every style-owned control through
the public Controls API and checks stable, observable HoloNight-specific properties rather than
private object-tree details. A Qt message handler records warnings and critical messages, and any
QML diagnostic fails the test.

The fixture also proves that module metadata uses `Holonight`, that the Basic fallback is declared,
that all direct `Holonight.Core` dependencies resolve from the installed prefix, and that no
lowercase alias artifacts were installed.

### Hybrid consumer

The hybrid fixture creates `QApplication`, a representative Widget, and
`QQmlApplicationEngine` in one process. It runs with only the supported two activation variables,
checks that the Widget style was selected through the HoloNight platform theme, and constructs
runtime-styled Qt Quick Controls without warnings. The test must isolate the environment so a
developer session's `QT_STYLE_OVERRIDE` or other Quick Controls variables cannot influence the
result.

## Documentation and compatibility boundaries

The README activation section shall present the two-variable contract as the supported session
configuration and explain the distinct responsibility of each variable. It shall include the
compatibility matrix from the specification and a narrowly scoped Widgets-only example for
`QT_STYLE_OVERRIDE` if that override remains documented.

No claim is made that environment activation can replace a compile-time third-party style,
restyle custom-painted content, add plugins to a static binary, or affect non-Qt toolkits. These
are compatibility boundaries rather than implementation failures.

## Verification sequence

Verification proceeds from narrow to broad:

1. Import-policy checker unit/fixture tests.
2. Style metadata, delegate dependency, and lowercase-artifact checks.
3. Staged-package legacy-tree validation.
4. Installed-prefix foreign-consumer and hybrid-consumer tests.
5. QML lint targets and focused platform/style/package suites.
6. `task lint` and the complete headless test suite.
7. Manual launches of a QWidget application, a HoloNight Qt Quick application, and the Qt/QML
   hyprpolkitagent, with logs inspected for QML warnings.

## Risks and mitigations

- **Style selected too late:** fixtures set the environment before `QApplication` and QML engine
  construction.
- **Build-tree leakage:** foreign-consumer tests resolve exclusively from a staged installed
  prefix and sanitize import paths.
- **False-positive import diagnostics:** positive fixtures cover qualified Basic use and ordinary
  runtime-styled Controls imports.
- **Destructive migration:** the install hook only warns; sentinel-based tests prove the legacy
  tree remains untouched.
- **Case-insensitive development filesystem:** installed artifact assertions compare exact path
  and metadata spelling and explicitly reject generated lowercase entries.
- **Overstated compatibility:** the README matrix makes consumer-controlled and unsupported cases
  explicit.

## Traceability

- REQ-A-001 through REQ-A-005: activation model, hybrid fixture, and README matrix.
- REQ-QML-001 through REQ-QML-003 and REQ-QML-006: runtime packaging and installed-prefix
  foreign-consumer validation.
- REQ-QML-004 and REQ-QML-005: DESTDIR-aware diagnostic and staged legacy-tree package test.
- REQ-C-001 through REQ-C-005: import-policy checker and fixture corpus.
- REQ-V-001 through REQ-V-007: integration fixtures and verification sequence.
