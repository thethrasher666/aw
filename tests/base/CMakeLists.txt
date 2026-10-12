#
# Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
#

# Define the target.
aw_add_test(test_base
    CXX_SOURCES
        "test-qoi.cxx"
        "test-version.cxx"

    COMPILE_DEFINITIONS_PRIVATE
        TEST_ASSET_PATH=\"${CMAKE_CURRENT_SOURCE_DIR}/data/square.qoi\"

    LIBRARIES_PRIVATE
        aw::base
)
