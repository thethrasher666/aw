#
# Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
#

# Configure files.
configure_file("version.hxx.in" "version.hxx")

# Define the target.
aw_add_core_static_library(base
    CXX_SOURCES
        "qoi.cxx"

    LIBRARIES_PRIVATE
        qoi
)
