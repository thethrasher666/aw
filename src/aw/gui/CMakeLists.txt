#
# Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
#

# Define the target.
aw_add_core_shared_library(gui
    CXX_SOURCES
        "action.cxx"
        "application.cxx"
        "icon.cxx"
        "window.cxx"

    DEFINE_SYMBOL
        AW_BUILD_GUI

    LIBRARIES_PUBLIC
        aw::core

    LIBRARIES_PRIVATE
        aw::gui.platform
)

# Define the target.
aw_add_python_module(awgui
    CXX_SOURCES
        "module.cxx"
        "wrap-application.cxx"
        "wrap-window.cxx"

    LIBRARIES_PRIVATE
        aw::gui
)

# Add the platform-specific subdirectories.
add_subdirectory("darwin")
add_subdirectory("linux")
add_subdirectory("win32")
