#
# Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
#

# Set the minimum CMake version.
cmake_minimum_required(VERSION 3.27.0 FATAL_ERROR)

# Include pre-project definitions.
list(PREPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include("toolchain-defaults")

# Define the project.
project(aw
    VERSION         0.0.1.0
    DESCRIPTION     "A cross-platform application development framework for creating applications on Linux, macOS and Windows."
    HOMEPAGE_URL    "https://www.jamiekenyon.com/"
    LANGUAGES       CXX
)

# Include the dependent modules.
include("install-dirs")
include("options")
include("packages")
include("targets")

# Add the documentation.
add_subdirectory("docs")

# Add the sources.
add_subdirectory("src")

# Enable testing.
if(AW_BUILD_TESTING)
    enable_testing()
    include("Catch")
    add_subdirectory("tests")
endif()

# Add the examples.
add_subdirectory("examples")
