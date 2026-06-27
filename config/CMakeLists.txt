add_library(holonight_config STATIC
    config.cpp
)

target_include_directories(holonight_config
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(holonight_config PUBLIC holonight_palette Qt6::Core Qt6::Gui)

set_target_properties(holonight_config PROPERTIES
    CXX_STANDARD 23
    POSITION_INDEPENDENT_CODE ON
)

install(TARGETS holonight_config ARCHIVE DESTINATION lib)
install(FILES holonight/config.h DESTINATION include/holonight)
