#
# Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
#

# Define the target.
aw_add_example_program(example.cli
    CXX_SOURCES
        "main.cxx"

    LIBRARIES_PRIVATE
        aw::core
)
