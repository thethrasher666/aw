#
# Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
#

# Define the target.
aw_add_test(test_tool_linguist
    CXX_SOURCES
        "test-linguist.cxx"
        "${CMAKE_SOURCE_DIR}/src/aw/tools/linguist/generator.cxx"
)


# Define the target.
aw_add_test(test_tool_rcc
    CXX_SOURCES
        "test-rcc.cxx"
        "${CMAKE_SOURCE_DIR}/src/aw/tools/rcc/generator.cxx"
        "${CMAKE_SOURCE_DIR}/src/aw/tools/rcc/manifest.cxx"

    LIBRARIES_PRIVATE
        tomlplusplus::tomlplusplus

    COMPILE_DEFINITIONS_PRIVATE
        TEST_MANIFEST_FILE="${CMAKE_CURRENT_SOURCE_DIR}/rcc.toml"
)
