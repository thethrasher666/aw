#
# Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
#

# Define the target.
aw_add_core_static_library(core.platform
    CXX_SOURCES
        "application.mm"
        "locale.mm"
        "string.mm"

    INCLUDES_PUBLIC
        "${CMAKE_SOURCE_DIR}/src/aw/core/darwin"

    LIBRARIES_PUBLIC
        aw::base

        ${FRAMEWORK_COCOA}
)
