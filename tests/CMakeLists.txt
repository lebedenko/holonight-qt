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
