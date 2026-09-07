add_executable(holonight_control_palette_tests quick_palette_main.cpp test_control_palette.cpp)
target_compile_definitions(holonight_control_palette_tests PRIVATE
    HOLONIGHT_QML_IMPORT_PATH="${CMAKE_BINARY_DIR}/qml")
target_link_libraries(holonight_control_palette_tests PRIVATE
    Qt6::Qml Qt6::Quick Qt6::Widgets GTest::gtest HoloNight::Config)
add_dependencies(holonight_control_palette_tests holonight_qml)
add_test(NAME holonight_control_palette_tests COMMAND holonight_control_palette_tests)
set_property(TEST holonight_control_palette_tests PROPERTY ENVIRONMENT
    "${HOLONIGHT_TEST_ENV};QT_QUICK_CONTROLS_STYLE=Holonight")
