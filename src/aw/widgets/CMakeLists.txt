#
# Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
#

# Define the target.
aw_add_core_shared_library(widgets
    CXX_SOURCES
        "application.cxx"
        "layout.cxx"
        "main-window.cxx"
        "menu.cxx"
        "widget.cxx"

    DEFINE_SYMBOL
        AW_BUILD_WIDGETS

    LIBRARIES_PUBLIC
        aw::gui

    LIBRARIES_PRIVATE
        aw::widgets.platform
)

# Define the target.
aw_add_python_module(awwidgets
    CXX_SOURCES
        "module.cxx"
        "wrap-application.cxx"
        "wrap-layouts.cxx"
        "wrap-widget.cxx"

    LIBRARIES_PRIVATE
        aw::widgets
)

# Add the platform-specific subdirectories.
add_subdirectory("darwin")
add_subdirectory("linux")
add_subdirectory("win32")
