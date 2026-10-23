# Checkbox consistency and Widgets demo

HoloNight checkboxes use a 3 logical pixel corner radius at 16×16, matching
Qt Quick. This applies to standalone Widgets controls, checkable group boxes,
and item-view check indicators. General control shape tokens stay unchanged.

## Rendering decisions

A private indicator painter handles unchecked, checked, and partial states.
It reads the option's active, inactive, or disabled palette group explicitly,
scales the radius and marks to the supplied rectangle, and paints a matching
keyboard focus outline. Standalone controls obtain indicator and label
rectangles from Qt. Group boxes copy their supplied palette and state without
requiring a widget pointer. Item views use Qt's check, decoration, and text
rectangles, preserving space and layout direction. Radio buttons retain their
circular painter. The Quick checkbox focus ring follows its indicator radius.

## Demo integration

`BUILD_DEMO` builds two installed executables: `holonight_demo` remains Qt
Quick, while `holonight_widgets_demo` uses ordinary Widgets in a `QMainWindow`.
Both share only demo startup and launch helpers; no library API is added.
The Widgets gallery uses the actual style and platform-theme plugins, with
build-tree discovery or paths relative to the installed executable. It sets
the HoloNight platform theme only when the environment variable is unset.
Qt retains control of explicit style and platform-theme overrides.

The Quick launch action resolves the sibling executable and starts a detached
process, passing the selected theme. When a theme was selected, the child does
not inherit the parent's temporary appearance path: each executable creates
and owns its own temporary configuration. Launch errors appear in the Quick
window. The existing standard Quick application window example remains.

## Acceptance

Rendering tests distinguish the checkbox radius by corner coverage with
antialiasing tolerance. They cover states, palette groups, primitive paths,
scaled rectangles, standalone RTL geometry, group boxes without widgets,
item-view decoration/text separation, and circular radio buttons. Demo tests
exercise the QML launch signal, inline error, sibling resolution, detached
launch arguments, and independent appearance ownership. Bounded startup tests
verify default HoloNight and explicit Fusion selection and check plugin origins
in build and staged installation trees. Visual review covers light and dark
schemes at 100% and 125% scaling, including checkbox focus and contexts.
