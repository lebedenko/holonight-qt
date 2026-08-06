find_package(KF6SyntaxHighlighting QUIET)
if(KF6SyntaxHighlighting_FOUND)
    add_subdirectory(theme-generator)
else()
    message(STATUS "KF6SyntaxHighlighting not found - skipping theme-generator tool")
endif()

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
        NAME generated_holonight_mocha_colors_current
        COMMAND generate_holonight_colors --scheme holonight-mocha --check ${CMAKE_SOURCE_DIR}/data/holonight-mocha.colors
    )
    add_test(
        NAME generated_holonight_latte_colors_current
        COMMAND generate_holonight_colors --scheme holonight-latte --check ${CMAKE_SOURCE_DIR}/data/holonight-latte.colors
    )
    add_test(
        NAME generated_holonight_storm_colors_current
        COMMAND generate_holonight_colors --scheme holonight-storm --check ${CMAKE_SOURCE_DIR}/data/holonight-storm.colors
    )
    add_test(
        NAME generated_holonight_day_colors_current
        COMMAND generate_holonight_colors --scheme holonight-day --check ${CMAKE_SOURCE_DIR}/data/holonight-day.colors
    )
    add_test(
        NAME generated_holonight_ember_colors_current
        COMMAND generate_holonight_colors --scheme holonight-ember --check ${CMAKE_SOURCE_DIR}/data/holonight-ember.colors
    )
    add_test(
        NAME generated_holonight_sol_colors_current
        COMMAND generate_holonight_colors --scheme holonight-sol --check ${CMAKE_SOURCE_DIR}/data/holonight-sol.colors
    )
    add_test(
        NAME generated_holonight_cyber_d_colors_current
        COMMAND generate_holonight_colors --scheme holonight-cyber-d --check ${CMAKE_SOURCE_DIR}/data/holonight-cyber-d.colors
    )
    add_test(
        NAME generated_holonight_cyber_l_colors_current
        COMMAND generate_holonight_colors --scheme holonight-cyber-l --check ${CMAKE_SOURCE_DIR}/data/holonight-cyber-l.colors
    )
    add_test(
        NAME generated_holonight_dracula_colors_current
        COMMAND generate_holonight_colors --scheme holonight-dracula --check ${CMAKE_SOURCE_DIR}/data/holonight-dracula.colors
    )
    add_test(
        NAME generated_holonight_alucard_colors_current
        COMMAND generate_holonight_colors --scheme holonight-alucard --check ${CMAKE_SOURCE_DIR}/data/holonight-alucard.colors
    )
endif()
