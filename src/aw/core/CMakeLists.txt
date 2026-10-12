#
# Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
#

# Define the target.
aw_add_core_shared_library(core
    CXX_SOURCES
        "application.cxx"
        "event-loop.cxx"
        "locale.cxx"
        "logging.cxx"
        "object.cxx"
        "resource.cxx"
        "string.cxx"
        "translation.cxx"
        "thread.cxx"

    DEFINE_SYMBOL
        AW_BUILD_CORE

    LIBRARIES_PUBLIC
        aw::base

    LIBRARIES_PRIVATE
        aw::core.platform
)

# Define the target.
aw_add_python_module(awcore
    CXX_SOURCES
        "module.cxx"
        "wrap-application.cxx"
        "wrap-object.cxx"

    LIBRARIES_PRIVATE
        aw::core
)

# Add the platform-specific subdirectories.
add_subdirectory("darwin")
add_subdirectory("linux")
add_subdirectory("win32")
