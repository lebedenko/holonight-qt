qt_add_qml_module(holonight_qml
    URI "Holonight"
    VERSION 1.0
    PLUGIN_TARGET holonight_qml
    OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Holonight
    IMPORTS
        QtQuick.Controls.Basic
    QML_FILES
        Button.qml
        CheckBox.qml
        ComboBox.qml
        ItemDelegate.qml
        Menu.qml
        MenuItem.qml
        ProgressBar.qml
        RadioButton.qml
        ScrollBar.qml
        ScrollView.qml
        Slider.qml
        SpinBox.qml
        Switch.qml
        TabBar.qml
        TabButton.qml
        TextArea.qml
        TextField.qml
        ToolTip.qml
)

set_target_properties(holonight_qml PROPERTIES AUTOMOC ON)

set_source_files_properties(
    core/HnControlSize.qml
    core/HnControlMetrics.qml
    core/HnTypographyRole.qml
    PROPERTIES
        QT_QML_SINGLETON_TYPE TRUE
)
set_source_files_properties(core/HnControlSize.qml PROPERTIES QT_RESOURCE_ALIAS HnControlSize.qml)
set_source_files_properties(core/HnControlMetrics.qml PROPERTIES QT_RESOURCE_ALIAS HnControlMetrics.qml)
set_source_files_properties(core/HnTypographyRole.qml PROPERTIES QT_RESOURCE_ALIAS HnTypographyRole.qml)
set_source_files_properties(core/HnLabel.qml PROPERTIES QT_RESOURCE_ALIAS HnLabel.qml)

qt_add_qml_module(holonight_core_qml
    URI "Holonight.Core"
    VERSION 1.0
    PLUGIN_TARGET holonight_core_qml
    OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Holonight/Core
    SOURCES
        hnappearance.h
        hnappearance.cpp
        hnshapeprofile.h
        hnshapeprofile.cpp
        hnshapetypes.h
        holoniightpalette.h
        holoniightpalette.cpp
        holonighttheme.h
        holonighttheme.cpp
        ${CMAKE_SOURCE_DIR}/src/icons/hniconprovider.h
        ${CMAKE_SOURCE_DIR}/src/icons/hniconprovider.cpp
    QML_FILES
        core/HnControlSize.qml
        core/HnControlMetrics.qml
        core/HnTypographyRole.qml
        core/HnLabel.qml
        HnIcon.qml
)

set_target_properties(holonight_core_qml PROPERTIES
    AUTOMOC ON
)

target_link_libraries(holonight_core_qml PRIVATE
    holonight_theme
    holonight_config
    holonight_icons
    holonight_palette
    Qt6::Qml
    Qt6::Quick
)

set(HOLONIGHT_CONTROLS_PUBLIC_QML_FILES
    controls/HnAppTitle.qml
    controls/HnHeaderBar.qml
    controls/HnSearchField.qml
    controls/HnIconComboBox.qml
    controls/HnTextArea.qml
    controls/HnFormField.qml
    controls/HnSettingsRow.qml
    controls/HnSectionHeader.qml
    controls/HnEmptyState.qml
    controls/HnLoadingState.qml
    controls/HnNavigationDelegate.qml
    controls/HnListDelegate.qml
    controls/HnCardDelegate.qml
    controls/HnActionDelegate.qml
    controls/HnStatusIndicator.qml
    controls/HnKeyHint.qml
    controls/HnPanelHeader.qml
    controls/HnSegmentedControl.qml
    controls/HnColorPicker.qml
    controls/HnChoiceCard.qml
    controls/HnActionBar.qml
    controls/HnIconButton.qml
    controls/HnSeparator.qml
    HnApplicationWindow.qml
    HnSurfaceFrame.qml
)

set(HOLONIGHT_CONTROLS_INTERNAL_QML_FILES
    controls/HnSelectableDelegate.qml
)

set(HOLONIGHT_CONTROLS_PUBLIC_TYPES)
foreach(qml_file IN LISTS HOLONIGHT_CONTROLS_PUBLIC_QML_FILES)
    get_filename_component(qml_type "${qml_file}" NAME_WE)
    list(APPEND HOLONIGHT_CONTROLS_PUBLIC_TYPES "${qml_type}")
    set_source_files_properties("${qml_file}" PROPERTIES QT_RESOURCE_ALIAS "${qml_type}.qml")
endforeach()
set_property(GLOBAL PROPERTY HOLONIGHT_CONTROLS_PUBLIC_TYPES "${HOLONIGHT_CONTROLS_PUBLIC_TYPES}")

set_source_files_properties(${HOLONIGHT_CONTROLS_INTERNAL_QML_FILES} PROPERTIES
    QT_RESOURCE_ALIAS HnSelectableDelegate.qml
    QT_QML_INTERNAL_TYPE TRUE
)
set_source_files_properties(controls/assets/search.svg PROPERTIES QT_RESOURCE_ALIAS assets/search.svg)
set_source_files_properties(controls/assets/clear.svg PROPERTIES QT_RESOURCE_ALIAS assets/clear.svg)
set_source_files_properties(controls/assets/more-vertical.svg PROPERTIES QT_RESOURCE_ALIAS assets/more-vertical.svg)
set_source_files_properties(controls/assets/edit.svg PROPERTIES QT_RESOURCE_ALIAS assets/edit.svg)
set_source_files_properties(controls/assets/delete.svg PROPERTIES QT_RESOURCE_ALIAS assets/delete.svg)
set_source_files_properties(controls/assets/folder.svg PROPERTIES QT_RESOURCE_ALIAS assets/folder.svg)
set_source_files_properties(controls/assets/paperclip.svg PROPERTIES QT_RESOURCE_ALIAS assets/paperclip.svg)
set_source_files_properties(controls/assets/wrench.svg PROPERTIES QT_RESOURCE_ALIAS assets/wrench.svg)
set_source_files_properties(controls/assets/send.svg PROPERTIES QT_RESOURCE_ALIAS assets/send.svg)

qt_add_qml_module(holonight_controls_qml
    URI "Holonight.Controls"
    VERSION 1.0
    PLUGIN_TARGET holonight_controls_qml
    OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Holonight/Controls
    SOURCES
        hnseparatoralignment.h
        hnseparatorgeometry.h
        hnseparatorgeometry.cpp
    IMPORTS
        Holonight
        Holonight.Core
    QML_FILES
        ${HOLONIGHT_CONTROLS_PUBLIC_QML_FILES}
        ${HOLONIGHT_CONTROLS_INTERNAL_QML_FILES}
    RESOURCES
        controls/assets/search.svg
        controls/assets/clear.svg
        controls/assets/more-vertical.svg
        controls/assets/edit.svg
        controls/assets/delete.svg
        controls/assets/folder.svg
        controls/assets/paperclip.svg
        controls/assets/wrench.svg
        controls/assets/send.svg
)

set_target_properties(holonight_controls_qml PROPERTIES
    AUTOMOC ON
)

target_link_libraries(holonight_controls_qml PRIVATE
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
)

add_dependencies(holonight_controls_qml
    holonight_core_qml
    holonight_qml
)

add_library(holonight_core INTERFACE)
set_target_properties(holonight_core PROPERTIES EXPORT_NAME Core)
target_link_libraries(holonight_core INTERFACE
    Qt6::Core
    Qt6::Qml
)

add_library(holonight_controls INTERFACE)
set_target_properties(holonight_controls PROPERTIES EXPORT_NAME Controls)
target_link_libraries(holonight_controls INTERFACE
    holonight_core
    Qt6::Quick
    Qt6::QuickControls2
)

set(HOLONIGHT_QML_FILES
    Button.qml CheckBox.qml ComboBox.qml ItemDelegate.qml Menu.qml MenuItem.qml ProgressBar.qml RadioButton.qml
    ScrollBar.qml ScrollView.qml Slider.qml SpinBox.qml Switch.qml TabBar.qml TabButton.qml TextArea.qml TextField.qml
    ToolTip.qml
)

target_link_libraries(holonight_qml PRIVATE
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::QuickShapes
    Qt6::Qml
)

configure_file(
    ${CMAKE_SOURCE_DIR}/cmake/CheckLegacyQmlInstall.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/CheckLegacyQmlInstall.cmake
    @ONLY
)
install(SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/CheckLegacyQmlInstall.cmake)

install(TARGETS holonight_qml
    LIBRARY DESTINATION ${QT6_INSTALL_QML}/Holonight
)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/Holonight/qmldir
    DESTINATION ${QT6_INSTALL_QML}/Holonight
)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/Holonight/holonight_qml.qmltypes
    DESTINATION ${QT6_INSTALL_QML}/Holonight
)
install(FILES ${HOLONIGHT_QML_FILES}
    DESTINATION ${QT6_INSTALL_QML}/Holonight
)
install(TARGETS holonight_core_qml
    LIBRARY DESTINATION ${QT6_INSTALL_QML}/Holonight/Core
)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/Holonight/Core/qmldir
    ${CMAKE_CURRENT_BINARY_DIR}/Holonight/Core/holonight_core_qml.qmltypes
    core/HnControlSize.qml
    core/HnControlMetrics.qml
    core/HnTypographyRole.qml
    core/HnLabel.qml
    HnIcon.qml
    DESTINATION ${QT6_INSTALL_QML}/Holonight/Core
)

install(TARGETS holonight_controls_qml
    LIBRARY DESTINATION ${QT6_INSTALL_QML}/Holonight/Controls
)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/Holonight/Controls/qmldir
    ${CMAKE_CURRENT_BINARY_DIR}/Holonight/Controls/holonight_controls_qml.qmltypes
    ${HOLONIGHT_CONTROLS_PUBLIC_QML_FILES}
    ${HOLONIGHT_CONTROLS_INTERNAL_QML_FILES}
    DESTINATION ${QT6_INSTALL_QML}/Holonight/Controls
)
install(FILES
    controls/assets/search.svg
    controls/assets/clear.svg
    controls/assets/more-vertical.svg
    controls/assets/edit.svg
    controls/assets/delete.svg
    controls/assets/folder.svg
    controls/assets/paperclip.svg
    controls/assets/wrench.svg
    controls/assets/send.svg
    DESTINATION ${QT6_INSTALL_QML}/Holonight/Controls/assets
)

install(TARGETS
    holonight_core
    holonight_controls
    EXPORT HolonightQtTargets
)
