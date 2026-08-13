find_package(Qt6 6.11 REQUIRED COMPONENTS WaylandClient)
find_package(Qt6WaylandScannerTools 6.11 REQUIRED)
find_package(WaylandScanner REQUIRED)
find_package(Wayland REQUIRED COMPONENTS Client)
find_package(PkgConfig REQUIRED)
pkg_get_variable(WAYLAND_PROTOCOLS_DIR wayland-protocols pkgdatadir)
set(XDG_SHELL_PROTOCOL "${WAYLAND_PROTOCOLS_DIR}/stable/xdg-shell/xdg-shell.xml")
if(NOT EXISTS "${XDG_SHELL_PROTOCOL}")
  message(FATAL_ERROR "xdg-shell protocol XML not found: ${XDG_SHELL_PROTOCOL}")
endif()

add_library(holonight_wayland STATIC
    layershellcontext.cpp
    layersurfacebackend.cpp
    layersurfacehost.cpp
    include/holonight/wayland/layershellcontext.h
    include/holonight/wayland/layersurfacehost.h
    include/holonight/wayland/layersurfacespec.h
    include/holonight/wayland/wayland.h
)
add_library(HolonightQt::Wayland ALIAS holonight_wayland)

qt6_generate_wayland_protocol_client_sources(holonight_wayland
    FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/wlr-layer-shell-unstable-v1.xml
        ${XDG_SHELL_PROTOCOL}
)

set_target_properties(holonight_wayland PROPERTIES
    EXPORT_NAME Wayland
    POSITION_INDEPENDENT_CODE ON
)
target_compile_features(holonight_wayland PUBLIC cxx_std_23)
target_include_directories(holonight_wayland
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}
        $<TARGET_PROPERTY:Qt6::GuiPrivate,INTERFACE_INCLUDE_DIRECTORIES>
)
target_compile_options(holonight_wayland PRIVATE "-isystem${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries(holonight_wayland
    PUBLIC
        Qt6::Core
        Qt6::Gui
        Qt6::Qml
        Qt6::Quick
        Qt6::WaylandClient
        Wayland::Client
)

install(TARGETS holonight_wayland
    EXPORT HolonightQtWaylandTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(EXPORT HolonightQtWaylandTargets
    FILE HolonightQtWaylandTargets.cmake
    NAMESPACE HolonightQt::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/HolonightQt
)
