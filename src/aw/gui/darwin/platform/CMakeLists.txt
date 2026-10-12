#
# Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
#

# Define the target.
aw_add_core_static_library(gui.platform
    CXX_SOURCES
        "application.mm"
        "window.mm"

    INCLUDES_PUBLIC
        "${CMAKE_SOURCE_DIR}/src/aw/gui/darwin"

    LIBRARIES_PUBLIC
        aw::core

        ${FRAMEWORK_APPKIT}
        ${FRAMEWORK_QUARTZ}
)
