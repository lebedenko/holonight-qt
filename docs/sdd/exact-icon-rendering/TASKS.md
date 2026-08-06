# Exact Icon Rendering Tasks

1. Add the `Qt6::Svg` dependency and `src/icons/` build target.
2. Implement `IconRenderer`.
3. Implement `IconThemeResolver`.
4. Implement `HnIconImageProvider` and QML engine registration.
5. Add the shared `Holonight.HnIcon` QML component.
6. Add renderer, cache, and QML smoke tests.
7. Update docs/install packaging for the new QML file and CMake dependency.
