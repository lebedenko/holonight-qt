cmake_minimum_required(VERSION 3.25)
project(holonight LANGUAGES C CXX)

# Installed QML providers follow the standard CMake prefix search path.
set(_qml_prefixes ${CMAKE_PREFIX_PATH})
if(DEFINED ENV{CMAKE_PREFIX_PATH})
  string(REPLACE ":" ";" _prefix_environment "$ENV{CMAKE_PREFIX_PATH}")
  list(APPEND _qml_prefixes ${_prefix_environment})
endif()
foreach(_prefix IN LISTS _qml_prefixes)
  if(IS_DIRECTORY "${_prefix}/lib/qt6/qml")
    list(APPEND QML_IMPORT_PATH "${_prefix}/lib/qt6/qml")
  endif()
endforeach()
if(DEFINED ENV{QML_IMPORT_PATH})
  string(REPLACE ":" ";" _qml_environment "$ENV{QML_IMPORT_PATH}")
  list(APPEND QML_IMPORT_PATH ${_qml_environment})
endif()
list(REMOVE_DUPLICATES QML_IMPORT_PATH)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(GNUInstallDirs)
install(FILES LICENSE THIRD_PARTY_NOTICES.md LICENSES/GPL-3.0-or-later.txt LICENSES/MIT.txt
  DESTINATION "${CMAKE_INSTALL_DATADIR}/licenses/${PROJECT_NAME}")
include(CMakePackageConfigHelpers)

find_package(Qt6 6.11 REQUIRED COMPONENTS Core Gui Widgets Quick QuickControls2 QuickShapes Svg Qml)
find_package(Qt6 ${Qt6_VERSION} EXACT REQUIRED COMPONENTS GuiPrivate)
find_package(HoloNightConfig CONFIG REQUIRED)

get_target_property(HOLONIGHT_CONFIG_RUNTIME_LIBRARY HoloNight::Config IMPORTED_LOCATION_DEBUG)
if(NOT HOLONIGHT_CONFIG_RUNTIME_LIBRARY)
  get_target_property(HOLONIGHT_CONFIG_RUNTIME_LIBRARY HoloNight::Config IMPORTED_LOCATION_RELEASE)
endif()
if(NOT HOLONIGHT_CONFIG_RUNTIME_LIBRARY)
  get_target_property(HOLONIGHT_CONFIG_RUNTIME_LIBRARY HoloNight::Config IMPORTED_LOCATION_NOCONFIG)
endif()
get_filename_component(HOLONIGHT_CONFIG_RUNTIME_DIR "${HOLONIGHT_CONFIG_RUNTIME_LIBRARY}" DIRECTORY)

qt_policy(SET QTP0001 NEW)

option(BUILD_TESTS "Build GTest suite" OFF)
option(BUILD_DEMO "Build visual demo app" OFF)
option(BUILD_CONTROLS_GALLERY "Build the shared controls gallery" OFF)
option(BUILD_QT5_PROBES "Build non-installed Qt 5 compatibility discovery targets" OFF)
option(BUILD_SYNTAX_THEMES "Generate and install syntax highlighting themes" ON)
option(BUILD_WAYLAND "Build the optional Wayland layer-surface component" ON)

if(BUILD_WAYLAND)
  add_subdirectory(wayland)
endif()

if(BUILD_QT5_PROBES)
  find_package(Qt5 5.15 REQUIRED COMPONENTS Core Gui Test Widgets)
endif()

if(BUILD_TESTS)
  find_package(Qt6 6.11 REQUIRED COMPONENTS Test)
  find_package(Qt6 ${Qt6_VERSION} EXACT REQUIRED COMPONENTS QmlPrivate)
  find_package(GTest 1.14 CONFIG REQUIRED)
  if(NOT TARGET GTest::gtest)
    message(FATAL_ERROR "Installed GTest must provide GTest::gtest")
  endif()
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

if(BUILD_CONTROLS_GALLERY)
  add_subdirectory(examples/controls-gallery)
endif()

if(BUILD_QT5_PROBES)
  add_subdirectory(qt5)
endif()

install(FILES
    data/holonight-dark.colors
    data/holonight-light.colors
    data/holonight-mocha.colors
    data/holonight-latte.colors
    data/holonight-storm.colors
    data/holonight-day.colors
    data/holonight-ember.colors
    data/holonight-sol.colors
    data/holonight-cyber-d.colors
    data/holonight-cyber-l.colors
    data/holonight-dracula.colors
    data/holonight-alucard.colors
    data/holonight-frost.colors
    data/holonight-snow.colors
    data/holonight-canopy.colors
    data/holonight-glade.colors
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
    VERSION 0.1.1
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/HolonightQtConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/HolonightQtConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/HolonightQt
)

# --- Format / tidy targets ---
find_program(CLANG_FORMAT clang-format)

file(GLOB_RECURSE ALL_SOURCE_FILES
    LIST_DIRECTORIES false
    CONFIGURE_DEPENDS
    src/*.cpp src/*.h palette/*.cpp palette/*.h config/*.cpp config/*.h
    qml/*.cpp qml/*.h tools/*.cpp tools/*.h tests/*.cpp tests/*.h demo/*.cpp demo/*.h qt5/*.cpp qt5/*.h
    wayland/*.cpp wayland/*.h
)

if(CLANG_FORMAT)
  add_custom_target(format
      COMMAND ${CLANG_FORMAT} -i ${ALL_SOURCE_FILES}
      COMMENT "Running clang-format"
      VERBATIM
  )
  add_custom_target(format-check
      COMMAND ${CLANG_FORMAT} --dry-run --Werror ${ALL_SOURCE_FILES}
      COMMENT "Checking clang-format"
      VERBATIM
  )
endif()

# Use the same direct compilation-database check as task tidy.
add_custom_target(tidy
  COMMAND python3 "${CMAKE_CURRENT_SOURCE_DIR}/scripts/quality.py" tidy --build "${CMAKE_BINARY_DIR}"
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
  VERBATIM
)
