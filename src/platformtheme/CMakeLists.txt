qt_add_plugin(qholonight
    CLASS_NAME HoloniightThemePlugin
    PLUGIN_TYPE platformthemes
)

target_sources(qholonight PRIVATE
    holonighttheme.h
    holonighttheme.cpp
    holonightthemeplugin.h
    holonightthemeplugin.cpp
)

set_target_properties(qholonight PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/qt6/plugins/platformthemes"
)

target_link_libraries(qholonight PRIVATE
    holonight_theme
    holonight_config
    holonight_palette
    Qt6::Gui
    Qt6::GuiPrivate
)

install(TARGETS qholonight
    LIBRARY DESTINATION ${QT6_INSTALL_PLUGINS}/platformthemes
)
