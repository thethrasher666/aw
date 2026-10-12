#
# Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
#

if(NOT LINUX)
    return()
endif()

# Add the platform-specific subdirectories.
add_subdirectory("platform")
