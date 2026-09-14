# Separate processes keep Qt's process-wide style cache out of override acceptance.
string(JOIN "," composite_types ${HOLONIGHT_CONTROLS_PUBLIC_TYPES})
add_executable(holonight_runtime_composite_tests
    quick_palette_main.cpp test_shared_rendering.cpp test_runtime_composites.cpp test_form_focus.cpp test_dropdown_interaction.cpp test_input_interaction.cpp test_qml_smoke.cpp)
target_compile_definitions(holonight_runtime_composite_tests PRIVATE
    HOLONIGHT_QML_IMPORT_PATH="${CMAKE_BINARY_DIR}/qml"
    HOLONIGHT_COMPOSITE_TYPES="${composite_types}")
target_include_directories(holonight_runtime_composite_tests PRIVATE ${CMAKE_SOURCE_DIR}/qml ${CMAKE_SOURCE_DIR}/src/theme)
target_link_libraries(holonight_runtime_composite_tests PRIVATE
    holonight_theme holonight_theme_catalog holonight_palette holonight_metrics
    Qt6::Qml Qt6::QmlPrivate Qt6::Quick Qt6::QuickPrivate Qt6::QuickControls2 Qt6::QuickTemplates2Private
    Qt6::Widgets Qt6::Test GTest::gtest HoloNight::Config)
add_dependencies(holonight_runtime_composite_tests holonight_qml holonight_controls_qml)
set(composite_filter "SharedRendering.*:InputInteraction*.*:Controls/DropdownInteraction.*:Wrappers/FormFocus.*:RuntimeComposites.*:QmlSmoke.Controls_*:Variants/ComboBoxGeometry.*")
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

# Keyboard regressions also run at fractional scale in separate offscreen processes.
foreach(style IN ITEMS Holonight Fusion)
  foreach(scale IN ITEMS 1 1.25)
    add_test(NAME holonight_input_interaction_${style}_${scale}
        COMMAND holonight_runtime_composite_tests "--gtest_filter=InputInteraction*.*")
    set_property(TEST holonight_input_interaction_${style}_${scale} PROPERTY ENVIRONMENT
        "${HOLONIGHT_TEST_ENV};QT_QUICK_BACKEND=software;QT_QUICK_CONTROLS_STYLE=${style};QT_SCALE_FACTOR=${scale}")
    add_test(NAME holonight_dropdown_interaction_${style}_${scale}
        COMMAND holonight_runtime_composite_tests "--gtest_filter=Controls/DropdownInteraction.*")
    set_property(TEST holonight_dropdown_interaction_${style}_${scale} PROPERTY ENVIRONMENT
        "${HOLONIGHT_TEST_ENV};QT_QUICK_BACKEND=software;QT_QUICK_CONTROLS_STYLE=${style};QT_SCALE_FACTOR=${scale};QT_LOGGING_RULES=qt.quick.viewport.debug=true")
    add_test(NAME holonight_form_focus_${style}_${scale}
        COMMAND holonight_runtime_composite_tests "--gtest_filter=Wrappers/FormFocus.*")
    set_property(TEST holonight_form_focus_${style}_${scale} PROPERTY ENVIRONMENT
        "${HOLONIGHT_TEST_ENV};QT_QUICK_BACKEND=software;QT_QUICK_CONTROLS_STYLE=${style};QT_SCALE_FACTOR=${scale}")
  endforeach()
endforeach()

foreach(style IN ITEMS Holonight Fusion)
  foreach(scale IN ITEMS 1 1.25)
    add_test(NAME holonight_shared_rendering_${style}_${scale}
        COMMAND holonight_runtime_composite_tests "--gtest_filter=SharedRendering.*")
    set_property(TEST holonight_shared_rendering_${style}_${scale} PROPERTY ENVIRONMENT
        "${HOLONIGHT_TEST_ENV};QT_QUICK_BACKEND=software;QT_QUICK_CONTROLS_STYLE=${style};QT_SCALE_FACTOR=${scale}")
  endforeach()
endforeach()

# Qt viewport logging evaluates geometry during deferred popup construction.
# Preserve coverage under the diagnostic mode used by actual-app acceptance.
foreach(style IN ITEMS Holonight Fusion)
  add_test(NAME holonight_popup_background_diagnostics_${style}
      COMMAND holonight_runtime_composite_tests
      "--gtest_filter=SharedRendering.InstalledFormComboPopupBackground:SharedRendering.ComboPopupBackgroundTracksLateLayout:SharedRendering.Scroll*Teardown")
  set_property(TEST holonight_popup_background_diagnostics_${style} PROPERTY ENVIRONMENT
      "${HOLONIGHT_TEST_ENV};QT_QUICK_BACKEND=software;QT_QUICK_CONTROLS_STYLE=${style};QT_SCALE_FACTOR=1;QT_LOGGING_RULES=qt.quick.viewport.debug=true")
endforeach()

set(render_audit "${CMAKE_SOURCE_DIR}/docs/sdd/unified-qtquick-controls/audit")
add_library(holonight_palette_diagnostics MODULE "${render_audit}/palette-diagnostics.cpp")
set_target_properties(holonight_palette_diagnostics PROPERTIES PREFIX "" OUTPUT_NAME "palette-diagnostics")
target_link_libraries(holonight_palette_diagnostics PRIVATE Qt6::QuickPrivate Qt6::Qml Qt6::Widgets)
add_executable(holonight_palette_diagnostics_check
    "${render_audit}/check-palette-diagnostics.cpp" "${render_audit}/palette-diagnostics.cpp")
target_link_libraries(holonight_palette_diagnostics_check PRIVATE Qt6::QuickPrivate Qt6::Qml Qt6::Widgets Qt6::Test)
add_dependencies(holonight_palette_diagnostics_check holonight_qml)
foreach(style IN ITEMS Holonight Fusion)
  foreach(observer IN ITEMS on off)
    add_test(NAME holonight_palette_diagnostics_${style}_${observer} COMMAND holonight_palette_diagnostics_check)
    set_property(TEST holonight_palette_diagnostics_${style}_${observer} PROPERTY ENVIRONMENT
        "${HOLONIGHT_TEST_ENV};QT_QUICK_BACKEND=software;QT_QUICK_CONTROLS_STYLE=${style};QT_SCALE_FACTOR=1;UQC_IMPORT_PATH=${CMAKE_BINARY_DIR}/qml")
    if(observer STREQUAL "on")
      set_property(TEST holonight_palette_diagnostics_${style}_${observer} APPEND PROPERTY ENVIRONMENT
          "HOLONIGHT_PALETTE_DIAGNOSTICS=1")
    endif()
  endforeach()
endforeach()
add_executable(holonight_render_diagnostics_check
    "${render_audit}/check-render-diagnostics.cpp" "${render_audit}/render-diagnostics.cpp")
target_link_libraries(holonight_render_diagnostics_check PRIVATE Qt6::Quick Qt6::Qml Qt6::Test)
add_dependencies(holonight_render_diagnostics_check holonight_qml)
foreach(style IN ITEMS Holonight Fusion)
  foreach(observer IN ITEMS on off)
    add_test(NAME holonight_render_diagnostics_${style}_${observer} COMMAND holonight_render_diagnostics_check)
    set_property(TEST holonight_render_diagnostics_${style}_${observer} PROPERTY ENVIRONMENT
        "${HOLONIGHT_TEST_ENV};QT_QUICK_BACKEND=software;QT_QUICK_CONTROLS_STYLE=${style};QT_SCALE_FACTOR=1;QT_LOGGING_RULES=qt.quick.viewport.debug=true;UQC_IMPORT_PATH=${CMAKE_BINARY_DIR}/qml")
    if(observer STREQUAL "on")
      set_property(TEST holonight_render_diagnostics_${style}_${observer} APPEND PROPERTY ENVIRONMENT
          "HOLONIGHT_RENDER_DIAGNOSTICS=1")
    endif()
  endforeach()
endforeach()
