configure_file(config.h.in "${CMAKE_CURRENT_BINARY_DIR}/config.h")

add_library(holonight_demo_startup STATIC startup.cpp startup.h)
target_include_directories(holonight_demo_startup PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}" PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries(holonight_demo_startup PUBLIC Qt6::Core PRIVATE holonight_theme_catalog HoloNight::Config)

qt_add_executable(holonight_widgets_demo widgets.cpp)
target_link_libraries(holonight_widgets_demo PRIVATE holonight_demo_startup Qt6::Widgets)
add_dependencies(holonight_widgets_demo holonight qholonight)
install(TARGETS holonight_widgets_demo RUNTIME DESTINATION bin)

qt_add_executable(holonight_demo
    main.cpp
)

qt_add_qml_module(holonight_demo
    URI "HoloniightDemo"
    VERSION 1.0
    IMPORT_PATH
        ${CMAKE_BINARY_DIR}/qml
    IMPORTS
        QtQuick.Controls
        Holonight.Core
        Holonight.Controls
    QML_FILES
        Main.qml
    NO_PLUGIN
)

target_include_directories(holonight_demo PRIVATE
    "${CMAKE_CURRENT_BINARY_DIR}"
)

target_link_libraries(holonight_demo PRIVATE
    holonight_demo_startup
    Qt6::Quick
    Qt6::QuickControls2
)

add_dependencies(holonight_demo holonight_qml holonight_core_qml holonight_controls_qml holonight_widgets_demo)

install(TARGETS holonight_demo RUNTIME DESTINATION bin)

qt_add_resources(holonight_demo controls_config
    PREFIX "/"
    FILES qtquickcontrols2.conf
)

if(BUILD_TESTS)
  find_package(Python3 REQUIRED COMPONENTS Interpreter)
  add_executable(holonight_demo_startup_tests ../tests/main.cpp ../tests/test_demo_startup.cpp)
  target_link_libraries(holonight_demo_startup_tests PRIVATE holonight_demo_startup Qt6::Widgets Qt6::Test GTest::gtest)
  add_test(NAME holonight_demo_startup_tests COMMAND holonight_demo_startup_tests)
  set_property(TEST holonight_demo_startup_tests PROPERTY
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen;QT_QPA_PLATFORMTHEME=;QT_STYLE_OVERRIDE=")
  foreach(mode IN ITEMS default environment command-line platform-environment)
    add_test(NAME holonight_widgets_demo_startup_${mode}
      COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tests/check_widgets_startup.py
        $<TARGET_FILE:holonight_widgets_demo> ${CMAKE_BINARY_DIR}/lib/qt6/plugins ${mode})
  endforeach()
endif()
