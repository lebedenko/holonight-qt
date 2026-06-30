add_executable(generate_holonight_colors generate_holonight_colors.cpp)

target_link_libraries(generate_holonight_colors PRIVATE holonight_palette)

set_target_properties(generate_holonight_colors PROPERTIES
    CXX_STANDARD 23
)

if(BUILD_TESTS)
    add_test(
        NAME generated_holonight_dark_colors_current
        COMMAND generate_holonight_colors --scheme holonight-dark --check ${CMAKE_SOURCE_DIR}/data/holonight-dark.colors
    )
    add_test(
        NAME generated_holonight_light_colors_current
        COMMAND generate_holonight_colors --scheme holonight-light --check ${CMAKE_SOURCE_DIR}/data/holonight-light.colors
    )
    add_test(
        NAME generated_tokyonight_storm_colors_current
        COMMAND generate_holonight_colors --scheme tokyonight-storm --check ${CMAKE_SOURCE_DIR}/data/tokyonight-storm.colors
    )
    add_test(
        NAME generated_tokyonight_day_colors_current
        COMMAND generate_holonight_colors --scheme tokyonight-day --check ${CMAKE_SOURCE_DIR}/data/tokyonight-day.colors
    )
endif()
