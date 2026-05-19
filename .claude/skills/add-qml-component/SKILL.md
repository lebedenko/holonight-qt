---
name: add-qml-component
description: Scaffold a new QQC2 component for the holonight theme. Follows the existing Button/ScrollBar/TextField pattern — creates the QML file and registers it in CMakeLists. Use when adding a new Qt Quick Controls 2 component.
disable-model-invocation: true
---

The user will provide a component name via $ARGUMENTS (e.g., `Slider`, `CheckBox`, `ComboBox`).

## Steps

1. **Read the existing components** for reference:
   - `qml/Button.qml`
   - `qml/ScrollBar.qml`
   - `qml/TextField.qml`
   - `qml/CMakeLists.txt`
   Read `palette/holonight/palette.h` to understand available color tokens.

2. **Create `qml/<ComponentName>.qml`**
   - Import `QtQuick` and `QtQuick.Controls` at the right versions (match existing QML files)
   - Use colors from the holonight palette (reference `Palette.<token>` or whatever pattern the existing components use)
   - Follow the same visual structure as the existing components: hover state, pressed state, focus ring, disabled state
   - Semi-transparent overlay tokens (`hover`, `pressed`, `focusRing`) are intentionally alpha < 255 — preserve that

3. **Register the component in `qml/CMakeLists.txt`**
   - Add `<ComponentName>.qml` to the `qt_add_qml_module` sources list, in alphabetical order

4. **Report** what was created and suggest running `/verify` to confirm it builds cleanly.
