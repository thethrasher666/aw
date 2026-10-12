#
# Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
#

find_package(PkgConfig REQUIRED)
pkg_check_modules(XCB REQUIRED IMPORTED_TARGET xcb)
pkg_check_modules(XCB_RANDR QUIET IMPORTED_TARGET xcb-randr)

set(AW_GUI_PLATFORM_LINUX_PRIVATE_LIBRARIES
    PkgConfig::XCB
)

if(TARGET PkgConfig::XCB_RANDR)
    list(APPEND AW_GUI_PLATFORM_LINUX_PRIVATE_LIBRARIES PkgConfig::XCB_RANDR)
endif()

# Define the target.
aw_add_core_static_library(gui.platform
    CXX_SOURCES
        "application.cxx"
        "window.cxx"

    INCLUDES_PUBLIC
        "${CMAKE_SOURCE_DIR}/src/aw/gui/linux"

    LIBRARIES_PUBLIC
        aw::core

    LIBRARIES_PRIVATE
        ${AW_GUI_PLATFORM_LINUX_PRIVATE_LIBRARIES}
)
