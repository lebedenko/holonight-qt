configure_file(config.h.in "${CMAKE_CURRENT_BINARY_DIR}/config.h")

qt_add_executable(holonight_controls_gallery
    main.cpp
)

qt_add_qml_module(holonight_controls_gallery
    URI "HolonightControlsGallery"
    VERSION 1.0
    IMPORT_PATH
        ${CMAKE_BINARY_DIR}/qml
    IMPORTS
        Holonight
        Holonight.Core
        Holonight.Controls
    QML_FILES
        Main.qml
        SurfaceHierarchyExample.qml
        pages/SurfacesPage.qml
        pages/TypographyPage.qml
        pages/InputsPage.qml
        pages/ButtonsPage.qml
        pages/DelegatesPage.qml
        pages/SettingsPage.qml
    NO_PLUGIN
)

target_include_directories(holonight_controls_gallery PRIVATE
    "${CMAKE_CURRENT_BINARY_DIR}"
)

target_link_libraries(holonight_controls_gallery PRIVATE
    Qt6::Quick
    Qt6::QuickControls2
)

add_dependencies(holonight_controls_gallery
    holonight_qml
    holonight_core_qml
    holonight_controls_qml
)

install(TARGETS holonight_controls_gallery RUNTIME DESTINATION bin)
