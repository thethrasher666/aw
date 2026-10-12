#
# Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
#

find_package(PkgConfig REQUIRED)
pkg_check_modules(XCB REQUIRED IMPORTED_TARGET xcb)

# Define the target.
aw_add_core_static_library(core.platform
    CXX_SOURCES
        "application.cxx"
        "locale.cxx"
        "string.cxx"

    INCLUDES_PUBLIC
        "${CMAKE_SOURCE_DIR}/src/aw/core/linux"

    LIBRARIES_PUBLIC
        aw::base
        PkgConfig::XCB
)
