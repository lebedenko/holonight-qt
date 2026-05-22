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

target_link_libraries(qholonight PRIVATE
    holonight_config
    holonight_palette
    Qt6::Gui
    Qt6::GuiPrivate
)

install(TARGETS qholonight
    LIBRARY DESTINATION ${QT6_INSTALL_PLUGINS}/platformthemes
)
