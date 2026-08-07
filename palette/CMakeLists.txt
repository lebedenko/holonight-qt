add_library(holonight_palette STATIC palette.cpp)
add_library(holonight_metrics STATIC metrics.cpp)

target_include_directories(holonight_palette
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(holonight_palette PUBLIC Qt6::Gui)
target_include_directories(holonight_metrics
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

set_target_properties(holonight_palette PROPERTIES
    CXX_STANDARD 23
    POSITION_INDEPENDENT_CODE ON
    EXPORT_NAME Palette
)
set_target_properties(holonight_metrics PROPERTIES
    CXX_STANDARD 23
    POSITION_INDEPENDENT_CODE ON
    EXPORT_NAME MetricTokens
)

install(TARGETS holonight_palette
    EXPORT HolonightQtTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(TARGETS holonight_metrics
    EXPORT HolonightQtTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(FILES holonight/palette.h DESTINATION include/holonight)
install(FILES holonight/metrics.h DESTINATION include/holonight)
