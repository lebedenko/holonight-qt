cmake_minimum_required(VERSION 3.25)
project(holonight LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

find_package(Qt6 6.11 REQUIRED COMPONENTS Core Gui Widgets Quick QuickControls2)
find_package(Qt6 REQUIRED COMPONENTS GuiPrivate)

qt_policy(SET QTP0001 NEW)

option(BUILD_TESTS "Build GTest suite" OFF)
option(BUILD_DEMO "Build visual demo app" OFF)

if(BUILD_TESTS)
  find_package(Qt6 6.11 REQUIRED COMPONENTS Test)
  find_package(GTest REQUIRED)
  enable_testing()
endif()

add_subdirectory(palette)
add_subdirectory(config)
add_subdirectory(src)
add_subdirectory(qml)
add_subdirectory(tools)

if(BUILD_TESTS)
  add_subdirectory(tests)
endif()

if(BUILD_DEMO)
  add_subdirectory(demo)
endif()

install(FILES
    data/holonight-dark.colors
    data/holonight-light.colors
    data/holonight-mocha.colors
    data/holonight-latte.colors
    data/tokyonight-storm.colors
    data/tokyonight-day.colors
    data/holonight-ember.colors
    data/holonight-sol.colors
    data/holonight-cyber-d.colors
    data/holonight-cyber-l.colors
    DESTINATION share/color-schemes
)

install(EXPORT HolonightQtTargets
    NAMESPACE HolonightQt::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/HolonightQt
)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/HolonightQtConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/HolonightQtConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/HolonightQt
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/HolonightQtConfigVersion.cmake
    VERSION 0.1.0
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/HolonightQtConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/HolonightQtConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/HolonightQt
)

# --- Tidy target ---
find_program(CLANG_TIDY clang-tidy)

file(GLOB_RECURSE ALL_CXX_IMPL_FILES
    LIST_DIRECTORIES false
    CONFIGURE_DEPENDS
    src/*.cpp palette/*.cpp config/*.cpp tools/*.cpp tests/*.cpp
)

if(CLANG_TIDY)
  # Strip -mno-direct-extern-access (injected by Qt6) so clang-tidy doesn't choke on it.
  # The filtered DB lives in build/tidy/ so clang-tidy resolves it first.
  set(TIDY_DB_DIR ${CMAKE_BINARY_DIR}/tidy)
  set(STRIP_TIDY_FLAGS_SCRIPT ${CMAKE_BINARY_DIR}/strip_tidy_flags.cmake)
  file(MAKE_DIRECTORY ${TIDY_DB_DIR})
  file(WRITE ${STRIP_TIDY_FLAGS_SCRIPT}
        "file(READ \"${CMAKE_BINARY_DIR}/compile_commands.json\" db)\n"
        "string(REPLACE \"-mno-direct-extern-access\" \"\" db \"\${db}\")\n"
        "file(WRITE \"${TIDY_DB_DIR}/compile_commands.json\" \"\${db}\")\n"
    )
  add_custom_target(tidy
        COMMAND ${CMAKE_COMMAND} -P ${STRIP_TIDY_FLAGS_SCRIPT}
        COMMAND ${CLANG_TIDY} --quiet -p ${TIDY_DB_DIR} ${ALL_CXX_IMPL_FILES}
        COMMENT "Running clang-tidy"
    )
endif()
