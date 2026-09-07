# Separate processes keep Qt's process-wide style cache out of override acceptance.
string(JOIN "," composite_types ${HOLONIGHT_CONTROLS_PUBLIC_TYPES})
add_executable(holonight_runtime_composite_tests
    quick_palette_main.cpp test_runtime_composites.cpp test_qml_smoke.cpp)
target_compile_definitions(holonight_runtime_composite_tests PRIVATE
    HOLONIGHT_QML_IMPORT_PATH="${CMAKE_BINARY_DIR}/qml"
    HOLONIGHT_COMPOSITE_TYPES="${composite_types}")
target_include_directories(holonight_runtime_composite_tests PRIVATE ${CMAKE_SOURCE_DIR}/qml ${CMAKE_SOURCE_DIR}/src/theme)
target_link_libraries(holonight_runtime_composite_tests PRIVATE
    holonight_theme holonight_theme_catalog holonight_palette holonight_metrics
    Qt6::Qml Qt6::QmlPrivate Qt6::Quick Qt6::QuickPrivate Qt6::QuickControls2 Qt6::QuickTemplates2Private
    Qt6::Widgets Qt6::Test GTest::gtest HoloNight::Config)
add_dependencies(holonight_runtime_composite_tests holonight_qml holonight_controls_qml)
set(composite_filter "RuntimeComposites.*:QmlSmoke.Controls_*:Variants/ComboBoxGeometry.*")
foreach(style IN ITEMS Holonight Fusion)
  add_test(NAME holonight_runtime_composites_${style}
      COMMAND holonight_runtime_composite_tests "--gtest_filter=${composite_filter}")
  set_property(TEST holonight_runtime_composites_${style} PROPERTY ENVIRONMENT
      "${HOLONIGHT_TEST_ENV};QT_QUICK_BACKEND=software;QT_QUICK_CONTROLS_STYLE=${style}")
endforeach()
add_test(NAME holonight_core_isolation
    COMMAND holonight_runtime_composite_tests --gtest_filter=CoreIsolation.*)
set_property(TEST holonight_core_isolation PROPERTY ENVIRONMENT
    "${HOLONIGHT_TEST_ENV};QT_QUICK_BACKEND=software;QT_QUICK_CONTROLS_STYLE=Fusion")
