add_library(holonight_icons STATIC
    iconrenderer.cpp
    iconrenderer.h
    iconthemeresolver.cpp
    iconthemeresolver.h
    hniconimageprovider.cpp
    hniconimageprovider.h
)

set_target_properties(holonight_icons PROPERTIES
    AUTOMOC ON
    POSITION_INDEPENDENT_CODE ON
)

target_include_directories(holonight_icons PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(holonight_icons PUBLIC
    holonight_config
    holonight_palette
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::Svg
)
