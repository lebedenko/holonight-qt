cmake_minimum_required(VERSION 3.25)
project(holonight LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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
add_subdirectory(style)
add_subdirectory(qml)
add_subdirectory(platformtheme)
add_subdirectory(tools)

if(BUILD_TESTS)
  add_subdirectory(tests)
endif()

if(BUILD_DEMO)
  add_subdirectory(demo)
endif()

install(FILES data/holonight.colors DESTINATION share/color-schemes)
