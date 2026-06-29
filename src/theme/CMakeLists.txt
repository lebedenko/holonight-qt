add_library(holonight_theme STATIC
    themeloader.h
    themeloader.cpp
    themeresolver.h
    themeresolver.cpp
)

target_include_directories(holonight_theme PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(holonight_theme PUBLIC
    holonight_config
    holonight_palette
    Qt6::Gui
)
