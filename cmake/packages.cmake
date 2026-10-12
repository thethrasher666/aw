#
# Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
#

include("FetchContent")

# Apple dependencies.
if(APPLE)
    find_library(FRAMEWORK_APPKIT           AppKit          REQUIRED)
    find_library(FRAMEWORK_AUDIO_TOOLBOX    AudioToolbox    REQUIRED)
    find_library(FRAMEWORK_CORE_AUDIO       CoreAudio       REQUIRED)
    find_library(FRAMEWORK_COCOA            Cocoa           REQUIRED)
    find_library(FRAMEWORK_QUARTZ           QuartzCore      REQUIRED)
endif(APPLE)

# Catch2.
FetchContent_Declare(Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.15.2
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(Catch2)

# Doxygen.
find_package(Doxygen REQUIRED)

# Linux packages.
if(LINUX)
    find_package(ALSA REQUIRED)
endif()

# PyBind.
FetchContent_Declare(pybind11
    GIT_REPOSITORY https://github.com/pybind/pybind11
    GIT_TAG        v3.1.0
)
FetchContent_MakeAvailable(pybind11)

# Quite OK Image (QOI).
FetchContent_Declare(QOI
    GIT_REPOSITORY https://github.com/phoboslab/qoi.git
    GIT_TAG        master
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(QOI)

# QOI ships a single header with no build system of its own.
if(NOT TARGET qoi)
    add_library(qoi INTERFACE)
    target_include_directories(qoi SYSTEM INTERFACE "${qoi_SOURCE_DIR}")
endif()

# TOML++.
FetchContent_Declare(TomlPlusPlus
    GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
    GIT_TAG        v3.4.0
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(TomlPlusPlus)

# Vulkan SDK.
find_package(Vulkan REQUIRED)
