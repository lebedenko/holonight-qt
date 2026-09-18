cmake_minimum_required(VERSION 3.25)
project(uqc_discovery LANGUAGES CXX)
find_package(Qt6 REQUIRED COMPONENTS Quick QuickControls2)
qt_add_executable(uqc-discovery main.cpp)
qt_add_resources(uqc-discovery config PREFIX "/" FILES qtquickcontrols2.conf)
target_link_libraries(uqc-discovery PRIVATE Qt6::Quick Qt6::QuickControls2)

qt_add_executable(uqc-layout-probe layout-probe.cpp)
target_link_libraries(uqc-layout-probe PRIVATE Qt6::Quick Qt6::QuickControls2)

# Opt-in audit project only; deliberately not registered with default CTest.
find_package(Qt6 REQUIRED COMPONENTS Test QuickPrivate)
qt_add_executable(uqc-palette-transition palette-transition.cpp)
qt_add_resources(uqc-palette-transition palette_config PREFIX "/" FILES qtquickcontrols2.conf)
target_link_libraries(uqc-palette-transition PRIVATE Qt6::Quick Qt6::QuickPrivate Qt6::QuickControls2 Qt6::Test)
