add_library(holonight_theme STATIC
    themeloader.h
    themeloader.cpp
    themeresolver.h
    themeresolver.cpp
)

target_include_directories(holonight_theme
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include/holonight/theme>
)

target_link_libraries(holonight_theme PUBLIC
    holonight_config
    holonight_palette
    Qt6::Gui
)

set_target_properties(holonight_theme PROPERTIES
    EXPORT_NAME Theme
)

install(TARGETS holonight_theme
    EXPORT HolonightQtTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(FILES themeloader.h themeresolver.h DESTINATION include/holonight/theme)
