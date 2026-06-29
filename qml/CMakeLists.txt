qt_add_qml_module(holonight_qml
    URI "Holonight"
    VERSION 1.0
    PLUGIN_TARGET holonight_qml
    OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Holonight
    SOURCES
        holoniightpalette.h
        holoniightpalette.cpp
        holonighttheme.h
        holonighttheme.cpp
    QML_FILES
        Button.qml
        CheckBox.qml
        ComboBox.qml
        Menu.qml
        MenuItem.qml
        ProgressBar.qml
        RadioButton.qml
        ScrollBar.qml
        Slider.qml
        Switch.qml
        TabBar.qml
        TabButton.qml
        TextField.qml
        ToolTip.qml
)

set_target_properties(holonight_qml PROPERTIES AUTOMOC ON)

set(HOLONIGHT_QML_FILES
    Button.qml CheckBox.qml ComboBox.qml Menu.qml MenuItem.qml
    ProgressBar.qml RadioButton.qml ScrollBar.qml Slider.qml Switch.qml
    TabBar.qml TabButton.qml TextField.qml ToolTip.qml
)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/holonight)
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/holonight-alias.qmldir
    ${CMAKE_CURRENT_BINARY_DIR}/holonight/qmldir
    COPYONLY
)
foreach(HOLONIGHT_QML_FILE IN LISTS HOLONIGHT_QML_FILES)
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/${HOLONIGHT_QML_FILE}
        ${CMAKE_CURRENT_BINARY_DIR}/holonight/${HOLONIGHT_QML_FILE}
        COPYONLY
    )
endforeach()

target_link_libraries(holonight_qml PRIVATE
    holonight_theme
    holonight_config
    holonight_palette
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Qml
)

install(TARGETS holonight_qml
    LIBRARY DESTINATION ${QT6_INSTALL_QML}/Holonight
)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/Holonight/qmldir
    DESTINATION ${QT6_INSTALL_QML}/Holonight
)
install(FILES ${HOLONIGHT_QML_FILES}
    DESTINATION ${QT6_INSTALL_QML}/Holonight
)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/holonight/qmldir
    DESTINATION ${QT6_INSTALL_QML}/holonight
)
install(FILES ${HOLONIGHT_QML_FILES}
    DESTINATION ${QT6_INSTALL_QML}/holonight
)
