configure_file(config.h.in "${CMAKE_CURRENT_BINARY_DIR}/config.h")

qt_add_executable(holonight_demo
    main.cpp
)

qt_add_qml_module(holonight_demo
    URI "HoloniightDemo"
    VERSION 1.0
    QML_FILES
        Main.qml
    NO_PLUGIN
)

target_include_directories(holonight_demo PRIVATE
    "${CMAKE_CURRENT_BINARY_DIR}"
)

target_link_libraries(holonight_demo PRIVATE
    Qt6::Quick
    Qt6::QuickControls2
)

add_dependencies(holonight_demo holonight_qml)
