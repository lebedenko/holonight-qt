set_source_files_properties(
    ${CMAKE_SOURCE_DIR}/style/holonightstyle.cpp
    ${CMAKE_SOURCE_DIR}/platformtheme/holonighttheme.cpp
    PROPERTIES SKIP_AUTOMOC OFF
)

add_executable(holonight_tests
    main.cpp
    test_palette_tokens.cpp
    test_palette_qpalette.cpp
    test_palette_contrast.cpp
    # Style smoke test: compile holonightstyle.cpp directly into the test binary
    ${CMAKE_SOURCE_DIR}/style/holonightstyle.cpp
    test_style_smoke.cpp
    # Platform theme smoke test: compile holonighttheme.cpp directly
    ${CMAKE_SOURCE_DIR}/platformtheme/holonighttheme.cpp
    test_platformtheme_smoke.cpp
)

set_target_properties(holonight_tests PROPERTIES AUTOMOC ON)

target_include_directories(holonight_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/style
    ${CMAKE_SOURCE_DIR}/platformtheme
)

target_link_libraries(holonight_tests PRIVATE
    holonight_palette
    Qt6::Gui
    Qt6::GuiPrivate
    Qt6::Test
    Qt6::Widgets
    GTest::gtest
)

add_test(NAME holonight_tests COMMAND holonight_tests
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

set_property(TEST holonight_tests PROPERTY
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen;QT_QPA_PLATFORMTHEME=;QT_STYLE_OVERRIDE="
)

# ── Style drawPrimitive tests ─────────────────────────────────────────────────

add_executable(holonight_style_primitive_tests
    main.cpp
    ${CMAKE_SOURCE_DIR}/style/holonightstyle.cpp
    test_style_drawprimitive.cpp
)

set_target_properties(holonight_style_primitive_tests PROPERTIES AUTOMOC ON)

target_include_directories(holonight_style_primitive_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/style
)

target_link_libraries(holonight_style_primitive_tests PRIVATE
    holonight_palette
    Qt6::Gui
    Qt6::Widgets
    GTest::gtest
)

add_test(NAME holonight_style_primitive_tests COMMAND holonight_style_primitive_tests
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

set_property(TEST holonight_style_primitive_tests PROPERTY
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen;QT_QPA_PLATFORMTHEME=;QT_STYLE_OVERRIDE="
)

# ── Style drawComplexControl tests ────────────────────────────────────────────

add_executable(holonight_style_complex_tests
    main.cpp
    ${CMAKE_SOURCE_DIR}/style/holonightstyle.cpp
    test_style_drawcomplexcontrol.cpp
)

set_target_properties(holonight_style_complex_tests PROPERTIES AUTOMOC ON)

target_include_directories(holonight_style_complex_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/style
)

target_link_libraries(holonight_style_complex_tests PRIVATE
    holonight_palette
    Qt6::Gui
    Qt6::Widgets
    GTest::gtest
)

add_test(NAME holonight_style_complex_tests COMMAND holonight_style_complex_tests
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

set_property(TEST holonight_style_complex_tests PROPERTY
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen;QT_QPA_PLATFORMTHEME=;QT_STYLE_OVERRIDE="
)

# ── QML smoke tests ───────────────────────────────────────────────────────────

add_executable(holonight_qml_smoke_tests
    main.cpp
    test_qml_smoke.cpp
)

set_target_properties(holonight_qml_smoke_tests PROPERTIES AUTOMOC ON)

target_compile_definitions(holonight_qml_smoke_tests PRIVATE
    HOLONIGHT_QML_IMPORT_PATH="${CMAKE_BINARY_DIR}/qml"
)

target_link_libraries(holonight_qml_smoke_tests PRIVATE
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Widgets
    GTest::gtest
)

add_test(NAME holonight_qml_smoke_tests COMMAND holonight_qml_smoke_tests
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

set_property(TEST holonight_qml_smoke_tests PROPERTY
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen;QT_QPA_PLATFORMTHEME=;QT_STYLE_OVERRIDE=;QML_IMPORT_PATH=${CMAKE_BINARY_DIR}/qml"
)
