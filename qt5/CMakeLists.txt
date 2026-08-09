set(HOLONIGHT_QT5_PLUGIN_ROOT "${CMAKE_BINARY_DIR}/qt5/plugins")

add_library(holonight_qt5_palette STATIC ../palette/palette.cpp)
target_include_directories(holonight_qt5_palette PUBLIC ../palette)
target_link_libraries(holonight_qt5_palette PUBLIC Qt5::Gui)
set_target_properties(holonight_qt5_palette PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_library(holonight_qt5_theme STATIC
    ../config/theme_catalog.cpp
    ../src/theme/shaperesolver.cpp
    ../src/theme/themeresolver.cpp
    qt5probeappearance.cpp
)
target_include_directories(holonight_qt5_theme PUBLIC . ../config ../palette ../src/theme)
target_link_libraries(holonight_qt5_theme PUBLIC holonight_qt5_palette Qt5::Core Qt5::Gui)
set_target_properties(holonight_qt5_theme PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_library(holonight_qt5_style MODULE ../src/style/holonightstyle.cpp ../src/style/holonightstyleplugin.cpp)
target_compile_definitions(holonight_qt5_style PRIVATE HOLONIGHT_QT5_PROBE)
target_include_directories(holonight_qt5_style PRIVATE . ../config ../palette ../src/style ../src/theme)
target_link_libraries(holonight_qt5_style PRIVATE holonight_qt5_theme Qt5::Widgets)
set_target_properties(holonight_qt5_style PROPERTIES AUTOMOC ON
    LIBRARY_OUTPUT_DIRECTORY "${HOLONIGHT_QT5_PLUGIN_ROOT}/styles" OUTPUT_NAME holonight)

add_library(qholonight_qt5 MODULE qt5theme.cpp qt5theme.h qt5themeplugin.cpp qt5themeplugin.h)
target_include_directories(qholonight_qt5 PRIVATE . ../config ../palette ../src/theme)
target_link_libraries(qholonight_qt5 PRIVATE holonight_qt5_theme Qt5::Gui Qt5::GuiPrivate)
set_target_properties(qholonight_qt5 PROPERTIES AUTOMOC ON
    LIBRARY_OUTPUT_DIRECTORY "${HOLONIGHT_QT5_PLUGIN_ROOT}/platformthemes" OUTPUT_NAME qholonight)

add_executable(holonight_qt5_probe_tests probe_tests.cpp ../src/style/holonightstyle.cpp qt5theme.cpp)
target_compile_definitions(holonight_qt5_probe_tests PRIVATE HOLONIGHT_QT5_PROBE)
target_include_directories(holonight_qt5_probe_tests PRIVATE . ../config ../palette ../src/style ../src/theme)
target_link_libraries(holonight_qt5_probe_tests PRIVATE holonight_qt5_theme Qt5::Gui Qt5::GuiPrivate Qt5::Test Qt5::Widgets)
set_target_properties(holonight_qt5_probe_tests PROPERTIES AUTOMOC ON)
add_dependencies(holonight_qt5_probe_tests holonight_qt5_style qholonight_qt5)

add_test(NAME holonight_qt5_probe_tests COMMAND holonight_qt5_probe_tests)
set_tests_properties(holonight_qt5_probe_tests PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen;QT_QPA_PLATFORMTHEME=;QT_STYLE_OVERRIDE=;QT_PLUGIN_PATH=${HOLONIGHT_QT5_PLUGIN_ROOT}")

configure_file(check_linkage.cmake.in check_qt5_linkage.cmake @ONLY)
add_test(NAME holonight_qt5_linkage_test COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/check_qt5_linkage.cmake)
