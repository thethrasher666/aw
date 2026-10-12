#
# Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
#

# Define the target.
aw_add_core_static_library(gui.platform
    CXX_SOURCES
        "application.cxx"
        "window.cxx"

    INCLUDES_PUBLIC
        "${CMAKE_SOURCE_DIR}/src/aw/gui/win32"

    LIBRARIES_PUBLIC
        aw::core

    LIBRARIES_PRIVATE
        gdiplus
)
