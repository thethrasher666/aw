#
# Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
#

# Define the target.
aw_add_core_static_library(widgets.platform
    CXX_SOURCES
        "menu.cxx"

    INCLUDES_PUBLIC
        "${CMAKE_SOURCE_DIR}/src/aw/widgets/win32"

    LIBRARIES_PUBLIC
        aw::core
)
