add_library(holonight_icons STATIC
    iconrenderer.cpp
    iconrenderer.h
    hniconengine.cpp
    hniconengine.h
    iconthemeresolver.cpp
    iconthemeresolver.h
    hniconimageprovider.cpp
    hniconimageprovider.h
)

set_target_properties(holonight_icons PROPERTIES
    AUTOMOC ON
    POSITION_INDEPENDENT_CODE ON
    EXPORT_NAME Icons
)

target_include_directories(holonight_icons PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include/holonight/icons>
)

target_link_libraries(holonight_icons PUBLIC
    holonight_theme
    holonight_appearance
    holonight_theme_catalog
    holonight_palette
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::Svg
)

install(TARGETS holonight_icons EXPORT HolonightQtTargets ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES hniconengine.h DESTINATION include/holonight/icons)
