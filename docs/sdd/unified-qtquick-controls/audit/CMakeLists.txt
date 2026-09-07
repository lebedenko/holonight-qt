cmake_minimum_required(VERSION 3.25)
project(uqc_discovery LANGUAGES CXX)
find_package(Qt6 REQUIRED COMPONENTS Quick QuickControls2)
qt_add_executable(uqc-discovery main.cpp)
qt_add_resources(uqc-discovery config PREFIX "/" FILES qtquickcontrols2.conf)
target_link_libraries(uqc-discovery PRIVATE Qt6::Quick Qt6::QuickControls2)

qt_add_executable(uqc-layout-probe layout-probe.cpp)
target_link_libraries(uqc-layout-probe PRIVATE Qt6::Quick Qt6::QuickControls2)
