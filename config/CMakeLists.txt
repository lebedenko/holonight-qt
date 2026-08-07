add_library(holonight_theme_catalog STATIC
    theme_catalog.cpp
)

add_library(holonight_appearance STATIC
    holonight/appearance.h
    holonight/appearance_reader.h
    appearance.cpp
    appearance_reader.cpp
)

target_include_directories(holonight_theme_catalog
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(holonight_theme_catalog
    PUBLIC holonight_palette Qt6::Core Qt6::Gui
)

target_include_directories(holonight_appearance
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)
target_link_libraries(holonight_appearance
    PUBLIC holonight_palette Qt6::Core
    PRIVATE holonight_theme_catalog HoloNight::Config
)
set_target_properties(holonight_appearance PROPERTIES
    AUTOMOC ON
    CXX_STANDARD 23
    POSITION_INDEPENDENT_CODE ON
    EXPORT_NAME Appearance
)

set_target_properties(holonight_theme_catalog PROPERTIES
    CXX_STANDARD 23
    POSITION_INDEPENDENT_CODE ON
    EXPORT_NAME ThemeCatalog
)

install(TARGETS holonight_theme_catalog
    EXPORT HolonightQtTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(TARGETS holonight_appearance
    EXPORT HolonightQtTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(FILES
    holonight/appearance.h
    holonight/appearance_reader.h
    holonight/theme_catalog.h
    DESTINATION include/holonight
)
