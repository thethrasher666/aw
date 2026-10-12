#
# Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
#

# Define the target.
aw_add_core_static_library(multi-media.platform
    CXX_SOURCES
        "audio.cxx"

    INCLUDES_PUBLIC
        "${CMAKE_SOURCE_DIR}/src/aw/multi-media/linux"

    LIBRARIES_PUBLIC
        aw::core
        ALSA::ALSA
)
