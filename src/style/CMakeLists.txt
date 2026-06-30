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

set_target_properties(holonight PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/qt6/plugins/styles"
)

target_link_libraries(holonight PRIVATE
    holonight_theme
    holonight_config
    holonight_palette
    Qt6::Widgets
)

install(TARGETS holonight
    LIBRARY DESTINATION ${QT6_INSTALL_PLUGINS}/styles
)
