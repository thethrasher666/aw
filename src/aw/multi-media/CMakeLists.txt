#
# Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
#

# Define the target.
aw_add_core_shared_library(multi-media
    CXX_SOURCES
        "audio.cxx"
        "player.cxx"

    DEFINE_SYMBOL
        AW_BUILD_MULTIMEDIA

    LIBRARIES_PUBLIC
        aw::core
        aw::multi-media.platform
)

# Add the platform-specific subdirectories.
add_subdirectory("darwin")
add_subdirectory("linux")
add_subdirectory("win32")
