#
# Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
#

include_guard(GLOBAL)
include(GNUInstallDirs)

# ---- Per-platform layout -----------------------------------------------------

# Application bundles are installed at the root of the prefix so that each one is a self-contained,
# Finder-launchable ".app"; the loader reaches the shared libraries via the bundle's install RPATH.
if(APPLE)
    set(PROJECT_INSTALL_BUNDLEDIR       ".")
    set(PROJECT_INSTALL_BINDIR          "bin")
    set(PROJECT_INSTALL_LIBDIR          "lib")
    set(PROJECT_INSTALL_FRAMEWORKDIR    "Frameworks")
    set(PROJECT_INSTALL_RESOURCEDIR     "Resources")
elseif(LINUX)
    set(PROJECT_INSTALL_BUNDLEDIR       ".")
    set(PROJECT_INSTALL_BINDIR          "bin")
    set(PROJECT_INSTALL_LIBDIR          "lib")
    set(PROJECT_INSTALL_FRAMEWORKDIR    "lib")
    set(PROJECT_INSTALL_RESOURCEDIR     ".")
elseif(WIN32)
    set(PROJECT_INSTALL_BUNDLEDIR       ".")
    set(PROJECT_INSTALL_BINDIR          "bin")
    set(PROJECT_INSTALL_LIBDIR          "bin")
    set(PROJECT_INSTALL_FRAMEWORKDIR    "bin")
    set(PROJECT_INSTALL_RESOURCEDIR     "resources")
endif()

# ---- Install RPATHs ----------------------------------------------------------

if(APPLE)
    set(PROJECT_INSTALL_RPATH
        "@executable_path"
        "@executable_path/../${PROJECT_INSTALL_LIBDIR}"
        "@executable_path/../${PROJECT_INSTALL_FRAMEWORKDIR}"
        "@loader_path"
    )

    # Relative to "<prefix>/<name>.app/Contents/MacOS".
    set(PROJECT_BUNDLE_INSTALL_RPATH
        "@executable_path/../Frameworks"
        "@executable_path/../../../${PROJECT_INSTALL_FRAMEWORKDIR}"
        "@loader_path"
    )
elseif(LINUX)
    set(PROJECT_INSTALL_RPATH
        "$ORIGIN"
        "$ORIGIN/../${PROJECT_INSTALL_FRAMEWORKDIR}"
    )

    set(PROJECT_BUNDLE_INSTALL_RPATH ${PROJECT_INSTALL_RPATH})
else()
    set(PROJECT_INSTALL_RPATH)
    set(PROJECT_BUNDLE_INSTALL_RPATH)
endif()

# ---- Absolute "FULL" variants -----------------------------------------------

set(_install_dir_names
    BUNDLEDIR
    BINDIR
    LIBDIR
    FRAMEWORKDIR
    RESOURCEDIR
)

foreach(_name IN LISTS _install_dir_names)
    set(PROJECT_INSTALL_FULL_${_name} "${CMAKE_INSTALL_PREFIX}/${PROJECT_INSTALL_${_name}}")
endforeach()

unset(_install_dir_names)
