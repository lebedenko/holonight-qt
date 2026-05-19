qt_add_plugin(holonight
    CLASS_NAME HoloniightStylePlugin
    PLUGIN_TYPE styles
)

target_sources(holonight PRIVATE
    holonightstyle.h
    holonightstyle.cpp
    holonightstyleplugin.h
    holonightstyleplugin.cpp
)

target_link_libraries(holonight PRIVATE
    holonight_palette
    Qt6::Widgets
)

install(TARGETS holonight
    LIBRARY DESTINATION ${QT6_INSTALL_PLUGINS}/styles
)
