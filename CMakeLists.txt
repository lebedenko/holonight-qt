cmake_minimum_required(VERSION 3.25)
project(holonight LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

find_package(Qt6 6.11 REQUIRED COMPONENTS Core Gui Widgets Quick QuickControls2 QuickShapes Svg)
find_package(Qt6 REQUIRED COMPONENTS GuiPrivate)

qt_policy(SET QTP0001 NEW)

option(BUILD_TESTS "Build GTest suite" OFF)
option(BUILD_DEMO "Build visual demo app" OFF)
option(BUILD_CONTROLS_GALLERY "Build the shared controls gallery" OFF)

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

if(BUILD_CONTROLS_GALLERY)
  add_subdirectory(examples/controls-gallery)
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

# --- Format / tidy targets ---
find_program(CLANG_FORMAT clang-format)
find_program(CLANG_TIDY clang-tidy)
find_program(RUN_CLANG_TIDY run-clang-tidy)

file(GLOB_RECURSE ALL_SOURCE_FILES
    LIST_DIRECTORIES false
    CONFIGURE_DEPENDS
    src/*.cpp src/*.h palette/*.cpp palette/*.h config/*.cpp config/*.h
    qml/*.cpp qml/*.h tools/*.cpp tools/*.h tests/*.cpp tests/*.h demo/*.cpp demo/*.h
)

file(GLOB_RECURSE ALL_CXX_SRC_IMPL_FILES
    LIST_DIRECTORIES false
    CONFIGURE_DEPENDS
    src/*.cpp palette/*.cpp config/*.cpp qml/*.cpp tools/*.cpp demo/*.cpp
)

file(GLOB_RECURSE ALL_CXX_TEST_IMPL_FILES
    LIST_DIRECTORIES false
    CONFIGURE_DEPENDS
    tests/*.cpp
)
list(REMOVE_ITEM ALL_CXX_TEST_IMPL_FILES "${CMAKE_CURRENT_SOURCE_DIR}/tests/main.cpp")
set(ALL_CXX_IMPL_FILES ${ALL_CXX_SRC_IMPL_FILES} ${ALL_CXX_TEST_IMPL_FILES})

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

if(CLANG_TIDY)
  include(ProcessorCount)
  ProcessorCount(TIDY_JOBS)
  if(TIDY_JOBS EQUAL 0)
    set(TIDY_JOBS 1)
  endif()

  set(TIDY_HIDE_PROGRESS_FLAG "")
  if(RUN_CLANG_TIDY)
    execute_process(
        COMMAND ${RUN_CLANG_TIDY} --help
        OUTPUT_VARIABLE RUN_CLANG_TIDY_HELP
        ERROR_VARIABLE RUN_CLANG_TIDY_HELP
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if(RUN_CLANG_TIDY_HELP MATCHES "-hide-progress")
      set(TIDY_HIDE_PROGRESS_FLAG "-hide-progress")
    endif()
  endif()

  set(TIDY_DB_DIR ${CMAKE_BINARY_DIR}/tidy)
  set(STRIP_TIDY_FLAGS_SCRIPT ${CMAKE_BINARY_DIR}/strip_tidy_flags.cmake)
  file(MAKE_DIRECTORY ${TIDY_DB_DIR})
  file(WRITE ${STRIP_TIDY_FLAGS_SCRIPT}
        "file(READ \"${CMAKE_BINARY_DIR}/compile_commands.json\" db)\n"
        "string(REPLACE \"-mno-direct-extern-access\" \"\" db \"\${db}\")\n"
        "file(WRITE \"${TIDY_DB_DIR}/compile_commands.json\" \"\${db}\")\n"
    )

  function(add_holonight_tidy_target target_name target_sources)
    if(RUN_CLANG_TIDY)
      add_custom_target(${target_name}
          COMMAND ${CMAKE_COMMAND} -P ${STRIP_TIDY_FLAGS_SCRIPT}
          COMMAND ${RUN_CLANG_TIDY}
              -quiet
              ${TIDY_HIDE_PROGRESS_FLAG}
              -j ${TIDY_JOBS}
              -p ${TIDY_DB_DIR}
              ${${target_sources}}
          COMMENT "Running ${target_name}"
          VERBATIM
      )
    else()
      add_custom_target(${target_name}
          COMMAND ${CMAKE_COMMAND} -P ${STRIP_TIDY_FLAGS_SCRIPT}
          COMMAND ${CLANG_TIDY} --quiet -p ${TIDY_DB_DIR} ${${target_sources}}
          COMMENT "Running ${target_name}"
          VERBATIM
      )
    endif()
  endfunction()

  add_holonight_tidy_target(tidy-src ALL_CXX_SRC_IMPL_FILES)
  add_holonight_tidy_target(tidy-tests ALL_CXX_TEST_IMPL_FILES)
  add_holonight_tidy_target(tidy ALL_CXX_IMPL_FILES)
endif()
