#
# Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
#

# Define the target.
aw_add_core_tool(rcc
    CXX_SOURCES
        "args.cxx"
        "generator.cxx"
        "main.cxx"
        "manifest.cxx"

    INCLUDES_PRIVATE
        "${CMAKE_CURRENT_BINARY_DIR}"

    LIBRARIES_PRIVATE
        aw::base
        tomlplusplus::tomlplusplus
)
