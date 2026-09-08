set_source_files_properties(
    ${CMAKE_SOURCE_DIR}/src/style/holonightstyle.cpp
    ${CMAKE_SOURCE_DIR}/src/platformtheme/holonighttheme.cpp
    PROPERTIES SKIP_AUTOMOC OFF
)

set(HOLONIGHT_TEST_ENV
    "QT_QPA_PLATFORM=offscreen"
    "QT_QPA_PLATFORMTHEME="
    "QT_STYLE_OVERRIDE="
    "HOLONIGHT_APPEARANCE_FILE=/tmp/holonight-tests-missing-appearance.toml"
)

add_executable(holonight_tests
    main.cpp
    test_resolved_appearance.cpp
    test_palette_tokens.cpp
    test_metric_tokens.cpp
    test_palette_qpalette.cpp
    test_palette_contrast.cpp
    test_shape_resolver.cpp
    test_icon_renderer.cpp
    # Style smoke test: compile holonightstyle.cpp directly into the test binary
    ${CMAKE_SOURCE_DIR}/src/style/holonightstyle.cpp
    test_style_smoke.cpp
    # Platform theme smoke test: compile holonighttheme.cpp directly
    ${CMAKE_SOURCE_DIR}/src/platformtheme/holonighttheme.cpp
    test_platformtheme_smoke.cpp
    test_platformtheme_fonts.cpp
    test_platformtheme_icons.cpp
)

set_target_properties(holonight_tests PROPERTIES AUTOMOC ON)

target_include_directories(holonight_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/src/style
    ${CMAKE_SOURCE_DIR}/src/platformtheme
)

target_link_libraries(holonight_tests PRIVATE
    holonight_theme
    holonight_appearance
    holonight_theme_catalog
    holonight_icons
    holonight_palette
    holonight_metrics
    Qt6::Gui
    Qt6::GuiPrivate
    Qt6::Test
    Qt6::Widgets
    GTest::gtest
    HoloNight::Config
)

add_test(NAME holonight_tests COMMAND holonight_tests
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

set_property(TEST holonight_tests PROPERTY
    ENVIRONMENT "${HOLONIGHT_TEST_ENV}"
)

# ── Style drawPrimitive tests ─────────────────────────────────────────────────

# Platform themes are created while QGuiApplication is still installing its event dispatcher.
# Keep this in a separate process so fatal Qt warnings report bootstrap regressions cleanly.
add_executable(holonight_platformtheme_bootstrap_test
    test_platformtheme_bootstrap.cpp
)

target_link_libraries(holonight_platformtheme_bootstrap_test PRIVATE
    Qt6::Gui
)

add_dependencies(holonight_platformtheme_bootstrap_test qholonight)

add_test(NAME holonight_platformtheme_bootstrap_test
    COMMAND holonight_platformtheme_bootstrap_test
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

set_property(TEST holonight_platformtheme_bootstrap_test PROPERTY
    ENVIRONMENT
        "QT_QPA_PLATFORM=offscreen"
        "QT_QPA_PLATFORMTHEME=holonight"
        "QT_PLUGIN_PATH=${CMAKE_BINARY_DIR}/lib/qt6/plugins"
        "QT_FATAL_WARNINGS=1"
        "HOLONIGHT_APPEARANCE_FILE=/tmp/holonight-tests-missing-appearance.toml"
)

add_executable(holonight_style_primitive_tests
    main.cpp
    ${CMAKE_SOURCE_DIR}/src/style/holonightstyle.cpp
    test_style_drawprimitive.cpp
)

set_target_properties(holonight_style_primitive_tests PROPERTIES AUTOMOC ON)

target_include_directories(holonight_style_primitive_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/src/style
)

target_link_libraries(holonight_style_primitive_tests PRIVATE
    holonight_theme
    holonight_theme_catalog
    holonight_palette
    Qt6::Gui
    Qt6::Widgets
    GTest::gtest
)

add_test(NAME holonight_style_primitive_tests COMMAND holonight_style_primitive_tests
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

set_property(TEST holonight_style_primitive_tests PROPERTY
    ENVIRONMENT "${HOLONIGHT_TEST_ENV}"
)

# ── Style drawComplexControl tests ────────────────────────────────────────────

add_executable(holonight_style_complex_tests
    main.cpp
    ${CMAKE_SOURCE_DIR}/src/style/holonightstyle.cpp
    test_style_drawcomplexcontrol.cpp
)

set_target_properties(holonight_style_complex_tests PROPERTIES AUTOMOC ON)

target_include_directories(holonight_style_complex_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/src/style
)

target_link_libraries(holonight_style_complex_tests PRIVATE
    holonight_theme
    holonight_theme_catalog
    holonight_palette
    Qt6::Gui
    Qt6::Widgets
    GTest::gtest
)

add_test(NAME holonight_style_complex_tests COMMAND holonight_style_complex_tests
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

set_property(TEST holonight_style_complex_tests PROPERTY
    ENVIRONMENT "${HOLONIGHT_TEST_ENV}"
)

# ── Theme resolver tests ──────────────────────────────────────────────────────

add_executable(holonight_theme_resolver_tests
    main.cpp
    test_semantic_appearance.cpp
    test_theme_resolver.cpp
)

set_target_properties(holonight_theme_resolver_tests PROPERTIES AUTOMOC ON)

target_include_directories(holonight_theme_resolver_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/src/theme
)

target_link_libraries(holonight_theme_resolver_tests PRIVATE
    holonight_theme
    holonight_theme_catalog
    holonight_palette
    Qt6::Gui
    Qt6::Widgets
    GTest::gtest
)

add_test(NAME holonight_theme_resolver_tests COMMAND holonight_theme_resolver_tests
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

set_property(TEST holonight_theme_resolver_tests PROPERTY
    ENVIRONMENT "${HOLONIGHT_TEST_ENV}"
)

# ── QML smoke tests ───────────────────────────────────────────────────────────

add_executable(holonight_qml_smoke_tests
    main.cpp
    test_qml_smoke.cpp
    test_qml_avatar.cpp
)

set_target_properties(holonight_qml_smoke_tests PROPERTIES AUTOMOC ON)

target_compile_definitions(holonight_qml_smoke_tests PRIVATE
    HOLONIGHT_QML_IMPORT_PATH="${CMAKE_BINARY_DIR}/qml"
)

target_include_directories(holonight_qml_smoke_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/qml
    ${CMAKE_SOURCE_DIR}/src/theme
)

target_link_libraries(holonight_qml_smoke_tests PRIVATE
    holonight_theme
    holonight_theme_catalog
    holonight_palette
    holonight_metrics
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Test
    Qt6::Widgets
    GTest::gtest
    HoloNight::Config
)

add_dependencies(holonight_qml_smoke_tests
    holonight_qml
    holonight_core_qml
    holonight_controls_qml
)

add_test(NAME holonight_qml_smoke_tests COMMAND holonight_qml_smoke_tests
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

set_property(TEST holonight_qml_smoke_tests PROPERTY
    ENVIRONMENT "${HOLONIGHT_TEST_ENV};QML_IMPORT_PATH=${CMAKE_BINARY_DIR}/qml"
)

if(BUILD_WAYLAND)
  add_executable(holonight_wayland_tests
      main.cpp
      test_wayland_surface_host.cpp
  )
  target_link_libraries(holonight_wayland_tests PRIVATE
      HolonightQt::Wayland
      Qt6::Gui
      Qt6::Test
      Qt6::Widgets
      GTest::gtest
  )
  target_include_directories(holonight_wayland_tests PRIVATE ${CMAKE_SOURCE_DIR}/wayland)
  add_test(NAME holonight_wayland_tests COMMAND holonight_wayland_tests)
  set_property(TEST holonight_wayland_tests PROPERTY ENVIRONMENT "${HOLONIGHT_TEST_ENV}")
endif()

# Copied Qt QML plugins retain relative RPATHs; provide the configured Qt libraries
# explicitly when the isolated runner clears ambient loader paths.
get_target_property(HOLONIGHT_QT_CORE_LIBRARY Qt6::Core IMPORTED_LOCATION_RELEASE)
if(NOT HOLONIGHT_QT_CORE_LIBRARY)
  get_target_property(HOLONIGHT_QT_CORE_LIBRARY Qt6::Core IMPORTED_LOCATION)
endif()
get_filename_component(HOLONIGHT_QT_RUNTIME_DIR "${HOLONIGHT_QT_CORE_LIBRARY}" DIRECTORY)

find_package(Python3 REQUIRED COMPONENTS Interpreter)
find_program(HOLONIGHT_PATCHELF_EXECUTABLE NAMES patchelf REQUIRED)

find_program(HOLONIGHT_QML_EXECUTABLE NAMES qml6 qml REQUIRED)
find_program(HOLONIGHT_QMLLINT_EXECUTABLE NAMES qmllint6 qmllint REQUIRED)

get_property(HOLONIGHT_CONTROLS_PUBLIC_TYPES GLOBAL PROPERTY HOLONIGHT_CONTROLS_PUBLIC_TYPES)
if(NOT HOLONIGHT_CONTROLS_PUBLIC_TYPES)
  message(FATAL_ERROR "The public Holonight.Controls type inventory is empty")
endif()

if(IS_ABSOLUTE "${QT6_INSTALL_QML}")
  set(HOLONIGHT_SYSTEM_QT_QML_DIR "${QT6_INSTALL_QML}")
else()
  set(HOLONIGHT_SYSTEM_QT_QML_DIR "${QT6_INSTALL_PREFIX}/${QT6_INSTALL_QML}")
endif()

set(HOLONIGHT_BUILD_SYNTAX_THEMES OFF)
if(TARGET generate-syntax-themes)
  set(HOLONIGHT_BUILD_SYNTAX_THEMES ON)
endif()

configure_file(
    test_package_install.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/test_package_install.cmake
    @ONLY
)
configure_file(
    test_qml_import_policy.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/test_qml_import_policy.cmake
    @ONLY
)

add_test(
    NAME holonight_package_install_test
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/test_package_install.cmake
)
add_test(
    NAME holonight_qml_import_policy_test
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/test_qml_import_policy.cmake
)
add_test(
    NAME holonight_qml_source_policy_test
    COMMAND bash ${CMAKE_SOURCE_DIR}/scripts/check-qml-import-policy.sh ${CMAKE_SOURCE_DIR}/qml
)

foreach(scope IN ITEMS demo examples/controls-gallery)
  string(REPLACE "/" "_" name "${scope}")
  add_test(NAME holonight_qml_policy_${name}
      COMMAND bash ${CMAKE_SOURCE_DIR}/scripts/check-qml-import-policy.sh ${CMAKE_SOURCE_DIR}/${scope})
endforeach()

find_package(Qt6 ${Qt6_VERSION} EXACT REQUIRED COMPONENTS QuickPrivate QuickTemplates2Private)

# Each process initializes the selected style and appearance before loading QML.
add_executable(holonight_quick_palette_tests quick_palette_main.cpp test_quick_palette.cpp)
target_compile_definitions(holonight_quick_palette_tests PRIVATE
    HOLONIGHT_QML_IMPORT_PATH="${CMAKE_BINARY_DIR}/qml")
target_link_libraries(holonight_quick_palette_tests PRIVATE
    holonight_theme holonight_theme_catalog holonight_palette
    Qt6::Qml Qt6::Quick Qt6::QuickPrivate Qt6::QuickTemplates2Private Qt6::QuickControls2 Qt6::Widgets Qt6::Test GTest::gtest HoloNight::Config)
add_dependencies(holonight_quick_palette_tests holonight_qml holonight_controls_qml)
foreach(scheme IN ITEMS dark light mocha latte storm day ember sol cyber-d cyber-l dracula alucard frost snow canopy glade)
  add_test(NAME holonight_quick_palette_${scheme} COMMAND holonight_quick_palette_tests)
  set_property(TEST holonight_quick_palette_${scheme} PROPERTY ENVIRONMENT
      "${HOLONIGHT_TEST_ENV};QT_QUICK_BACKEND=software;QT_QUICK_CONTROLS_STYLE=Holonight;UQC_SCHEME=holonight-${scheme}")
endforeach()
add_test(NAME holonight_quick_palette_hybrid COMMAND holonight_quick_palette_tests)
set_property(TEST holonight_quick_palette_hybrid PROPERTY ENVIRONMENT
    "${HOLONIGHT_TEST_ENV};QT_QUICK_BACKEND=software;QT_QUICK_CONTROLS_STYLE=Holonight;UQC_HYBRID=1;QT_PLUGIN_PATH=${CMAKE_BINARY_DIR}/lib/qt6/plugins")

add_executable(holonight_quick_style_selection quick_style_selection.cpp)
target_link_libraries(holonight_quick_style_selection PRIVATE
    holonight_theme holonight_theme_catalog holonight_palette
    Qt6::Qml Qt6::QmlPrivate Qt6::Quick Qt6::QuickControls2 HoloNight::Config)
add_dependencies(holonight_quick_style_selection holonight_qml holonight_controls_qml)

include(QuickPaletteInfrastructure.cmake)

include(RuntimeComposites.cmake)

if(BUILD_DEMO OR BUILD_CONTROLS_GALLERY)
  find_package(Python3 REQUIRED COMPONENTS Interpreter)
  set(HOLONIGHT_EXAMPLE_TARGETS)
  if(BUILD_DEMO)
    list(APPEND HOLONIGHT_EXAMPLE_TARGETS holonight_demo)
  endif()
  if(BUILD_CONTROLS_GALLERY)
    list(APPEND HOLONIGHT_EXAMPLE_TARGETS holonight_controls_gallery)
  endif()
  foreach(example IN LISTS HOLONIGHT_EXAMPLE_TARGETS)
    foreach(mode IN ITEMS default environment command-line configuration)
      add_test(NAME ${example}_startup_${mode}
          COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/check_example_startup.py
              $<TARGET_FILE:${example}> ${CMAKE_BINARY_DIR}/qml ${mode})
    endforeach()
  endforeach()
endif()

# Deliberately links only Qt: no provider libraries/resources may pre-register modules.
add_executable(holonight_isolated_style_probe isolated_style_probe.cpp)
target_link_libraries(holonight_isolated_style_probe PRIVATE
    Qt6::Gui Qt6::Qml Qt6::QmlPrivate Qt6::QuickControls2)
qt_add_resources(holonight_isolated_style_probe isolated_controls_config
    PREFIX "/" BASE "${CMAKE_SOURCE_DIR}/demo" FILES "${CMAKE_SOURCE_DIR}/demo/qtquickcontrols2.conf")

foreach(ratio IN ITEMS 1.0 1.25)
  add_test(NAME holonight_quick_rendering_dpr_${ratio} COMMAND holonight_quick_palette_tests
      "--gtest_filter=QuickPalette.OffscreenRendering*:QuickPalette.NewButtonStatesRender*")
  set_property(TEST holonight_quick_rendering_dpr_${ratio} PROPERTY ENVIRONMENT
      "${HOLONIGHT_TEST_ENV};QT_QUICK_BACKEND=software;QT_QUICK_CONTROLS_STYLE=Holonight;QT_SCALE_FACTOR=${ratio}")
endforeach()
