add_executable(generate_holonight_colors generate_holonight_colors.cpp)

target_link_libraries(generate_holonight_colors PRIVATE holonight_palette)

set_target_properties(generate_holonight_colors PROPERTIES
    CXX_STANDARD 23
)

if(BUILD_TESTS)
    add_test(
        NAME generated_holonight_colors_current
        COMMAND generate_holonight_colors --check ${CMAKE_SOURCE_DIR}/data/holonight.colors
    )
    add_test(
        NAME generated_holonight_day_colors_current
        COMMAND generate_holonight_colors --mode light --check ${CMAKE_SOURCE_DIR}/data/holonight-day.colors
    )
endif()
