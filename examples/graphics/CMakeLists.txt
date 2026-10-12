#
# Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
#

# Define the target.
aw_add_example_program(example.graphics
    GUI

    DISPLAY_NAME
        "Graphics Example"

    CXX_SOURCES
        "app.cxx"
        "entry-point.cxx"
        "main-window.cxx"

        $<$<PLATFORM_ID:Darwin>:main-darwin.mm>
        $<$<PLATFORM_ID:Linux>:main-linux.cxx>
        $<$<PLATFORM_ID:Windows>:main-win32.cxx>

    LIBRARIES_PRIVATE
        aw::vulkan-widgets
)

# Add the resources.
aw_add_resources(example.graphics
    RESOURCE_FILES
        "resources/icons.toml"

    TRANSLATION_FILES
        "resources/translations.toml"
)
