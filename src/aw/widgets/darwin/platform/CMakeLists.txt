#
# Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
#

# Define the target.
aw_add_core_static_library(widgets.platform
    CXX_SOURCES
        "menu.mm"

    INCLUDES_PUBLIC
        "${CMAKE_SOURCE_DIR}/src/aw/widgets/darwin"

    LIBRARIES_PUBLIC
        aw::core

        ${FRAMEWORK_APPKIT}
        ${FRAMEWORK_QUARTZ}
)
