#
# Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
#

# Define the target.
aw_add_core_static_library(core.platform
    CXX_SOURCES
        "application.cxx"
        "locale.cxx"
        "string.cxx"

    INCLUDES_PUBLIC
        "${CMAKE_SOURCE_DIR}/src/aw/core/win32"

    LIBRARIES_PUBLIC
        aw::base
)
