find_package(KF6SyntaxHighlighting REQUIRED)

add_executable(holonight-theme-generator
    main.cpp
    text_style_mapping.cpp
    kate_theme_writer.cpp
)

target_link_libraries(holonight-theme-generator PRIVATE
    holonight_palette
    holonight_theme_catalog
    Qt6::Gui
    KF6::SyntaxHighlighting
)

set_target_properties(holonight-theme-generator PROPERTIES
    CXX_STANDARD 23
)

set(_THEME_OUTPUT_DIR "${CMAKE_BINARY_DIR}/generated-themes")
add_custom_command(
    OUTPUT "${_THEME_OUTPUT_DIR}/.stamp"
    COMMAND holonight-theme-generator "${_THEME_OUTPUT_DIR}"
    COMMAND ${CMAKE_COMMAND} -E touch "${_THEME_OUTPUT_DIR}/.stamp"
    DEPENDS holonight-theme-generator
    COMMENT "Generating KSyntaxHighlighting theme JSON files from ColorTokens"
    VERBATIM
)
add_custom_target(generate-syntax-themes ALL DEPENDS "${_THEME_OUTPUT_DIR}/.stamp")

install(DIRECTORY "${_THEME_OUTPUT_DIR}/" DESTINATION "${CMAKE_INSTALL_DATADIR}/holonight/themes"
        FILES_MATCHING PATTERN "*.theme")
