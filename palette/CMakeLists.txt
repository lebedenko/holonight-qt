add_library(holonight_palette STATIC palette.cpp)

target_include_directories(holonight_palette
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(holonight_palette PUBLIC Qt6::Gui)

set_target_properties(holonight_palette PROPERTIES
    CXX_STANDARD 23
    POSITION_INDEPENDENT_CODE ON
    EXPORT_NAME Palette
)

install(TARGETS holonight_palette
    EXPORT HolonightQtTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(FILES holonight/palette.h DESTINATION include/holonight)
