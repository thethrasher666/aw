#
# Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
#

# Set default options on all target types.
function(aw_set_target_defaults TRGT)
    # Set properties.
    set_target_properties(${TRGT}
        PROPERTIES
            COMPILE_WARNING_AS_ERROR  ON
            C_VISIBILITY_PRESET       default
            CXX_VISIBILITY_PRESET     default
            POSITION_INDEPENDENT_CODE ON
            VISIBILITY_INLINES_HIDDEN OFF
    )

    # Set the target's compile definitions.
    target_compile_definitions(${TRGT}
        PRIVATE
            $<$<CXX_COMPILER_ID:MSVC>:_AMD64_>
            $<$<CXX_COMPILER_ID:MSVC>:NOMINMAX>
            $<$<CXX_COMPILER_ID:MSVC>:WIN32_LEAN_AND_MEAN>
    )

    # Set the C++ standard version.
    target_compile_features(${TRGT} PRIVATE cxx_std_23)

    # Set C++ flags.
    target_compile_options(${TRGT}
        PRIVATE
            # Set warnings as errors.
            "$<$<COMPILE_LANGUAGE:CXX>:$<$<CXX_COMPILER_ID:AppleClang,Clang,GNU>:-Wall>>"
            "$<$<COMPILE_LANGUAGE:CXX>:$<$<CXX_COMPILER_ID:MSVC>:/W4>>"
            "$<$<COMPILE_LANGUAGE:CXX>:$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Release>>:/Zi>>"
            "$<$<COMPILE_LANGUAGE:CXX>:$<$<AND:$<CXX_COMPILER_ID:AppleClang,GNU>,$<CONFIG:Release>>:-g -g2>>"
    )

    # Set the target's include directories.
    target_include_directories(${TRGT}
        PRIVATE
            "${CMAKE_SOURCE_DIR}/src"
            "${CMAKE_BINARY_DIR}/src" # For configured files.
    )
endfunction(aw_set_target_defaults)

# Defines a core shared library target.
function(aw_add_core_shared_library targetName)
    set(optionKeywords)

    set(oneValueKeywords
        DEFINE_SYMBOL               # The symbol to use for export declarations when building.
    )

    set(multiValueKeywords
        CXX_SOURCES                 # List of C/C++ source files.
        COMPILE_DEFINITIONS_PRIVATE # List of preprocessor definitions to use when compiling that upstream consumers don't require.
        INCLUDES_PUBLIC             # List of include directories to use for upstream consumers.
        INCLUDES_PRIVATE            # List of include directories to use for the target itself.
        LIBRARIES_PUBLIC            # List of dependent libraries to link that upstream consumers require.
        LIBRARIES_PRIVATE           # List of dependent libraries to link that upstream consumers don't require.
    )

    cmake_parse_arguments(args
        "${optionKeywords}"
        "${oneValueKeywords}"
        "${multiValueKeywords}"
        ${ARGN}
    )

    # Define the target.
    add_library(${targetName} SHARED ${args_CXX_SOURCES})

    _aw_add_core_library_common(${targetName}
        DEFINE_SYMBOL
            ${args_DEFINE_SYMBOL}
        LIBRARIES_PUBLIC
            ${args_LIBRARIES_PUBLIC}
        LIBRARIES_PRIVATE
            ${args_LIBRARIES_PRIVATE}
    )

    # Set RUNPATH/RPATH.
    set_target_properties(${targetName}
        PROPERTIES
            INSTALL_RPATH "${PROJECT_INSTALL_RPATH}"
    )

    # Set the target's compile definitions.
    target_compile_definitions(${targetName}
        PRIVATE
            ${args_COMPILE_DEFINITIONS_PRIVATE}
    )
endfunction(aw_add_core_shared_library)

# Defines a core static library target.
function(aw_add_core_static_library targetName)
    set(optionKeywords)

    set(multiValueKeywords
        CXX_SOURCES                 # List of C/C++ source files.
        COMPILE_DEFINITIONS_PRIVATE # List of preprocessor definitions to use when compiling that upstream consumers don't require.
        INCLUDES_PUBLIC             # List of include directories to use for upstream consumers.
        INCLUDES_PRIVATE            # List of include directories to use for the target itself.
        LIBRARIES_PUBLIC            # List of dependent libraries to link that upstream consumers require.
        LIBRARIES_PRIVATE           # List of dependent libraries to link that upstream consumers don't require.
    )

    cmake_parse_arguments(args
        "${optionKeywords}"
        "${oneValueKeywords}"
        "${multiValueKeywords}"
        ${ARGN}
    )

    # Define the target.
    add_library(${targetName} STATIC ${args_CXX_SOURCES})

    _aw_add_core_library_common(${targetName}
        INCLUDES_PUBLIC
            ${args_INCLUDES_PUBLIC}
        INCLUDES_PRIVATE
            ${args_INCLUDES_PRIVATE}
        LIBRARIES_PUBLIC
            ${args_LIBRARIES_PUBLIC}
        LIBRARIES_PRIVATE
            ${args_LIBRARIES_PRIVATE}
    )

    # Set the target's compile definitions.
    target_compile_definitions(${targetName}
        PRIVATE
            ${args_COMPILE_DEFINITIONS_PRIVATE}
    )
endfunction(aw_add_core_static_library)

# Helper function to define a core library target.
function(_aw_add_core_library_common targetName)
    set(optionKeywords)

    set(oneValueKeywords
        DEFINE_SYMBOL               # The symbol to use for export declarations when building.
    )

    set(multiValueKeywords
        INCLUDES_PUBLIC             # List of include directories to use for upstream consumers.
        INCLUDES_PRIVATE            # List of include directories to use for the target itself.
        LIBRARIES_PUBLIC            # List of dependent libraries to link that upstream consumers require.
        LIBRARIES_PRIVATE           # List of dependent libraries to link that upstream consumers don't require.
    )

    cmake_parse_arguments(args
        "${optionKeywords}"
        "${oneValueKeywords}"
        "${multiValueKeywords}"
        ${ARGN}
    )

    # Define an alias for downstream consumption.
    add_library(aw::${targetName} ALIAS ${targetName})

    # Set the defaults.
    aw_set_target_defaults(${targetName})

    # Set the target's export symbol for building.
    if (args_DEFINE_SYMBOL)
        set_target_properties(${targetName} PROPERTIES DEFINE_SYMBOL ${args_DEFINE_SYMBOL})
    endif()

    # Set the target's include directories.
    target_include_directories(${targetName}
        PUBLIC
            ${args_INCLUDES_PUBLIC}
        PRIVATE
            ${args_INCLUDES_PRIVATE}
    )

    # Set the target's dependencies.
    target_link_libraries(${targetName}
        PUBLIC
            ${args_LIBRARIES_PUBLIC}
        PRIVATE
            ${args_LIBRARIES_PRIVATE}
    )
endfunction(_aw_add_core_library_common)

# Defines a core tool target.
function(aw_add_core_tool targetName)
    set(optionKeywords)

    set(multiValueKeywords
        CXX_SOURCES                 # List of C/C++ source files.
        INCLUDES_PRIVATE            # List of include directories to use for the target itself.
        LIBRARIES_PRIVATE           # List of dependent libraries to link that upstream consumers don't require.
    )

    cmake_parse_arguments(args
        "${optionKeywords}"
        "${oneValueKeywords}"
        "${multiValueKeywords}"
        ${ARGN}
    )

    configure_file("${CMAKE_SOURCE_DIR}/resources/resources.rc.in"  "resources.rc")
    configure_file("${CMAKE_SOURCE_DIR}/resources/project.icns"     "project.icns" COPYONLY)
    configure_file("${CMAKE_SOURCE_DIR}/resources/project.ico"      "project.ico" COPYONLY)

    # Define the target.
    add_executable(${targetName} ${args_CXX_SOURCES} $<$<PLATFORM_ID:Windows>:resources.rc>)

    # Set the defaults.
    aw_set_target_defaults(${targetName})

    # Set RUNPATH/RPATH.
    set_target_properties(${targetName}
        PROPERTIES
            INSTALL_RPATH "${PROJECT_INSTALL_RPATH}"
    )

    # Set the target's include directories.
    target_include_directories(${targetName}
        PRIVATE
            ${args_INCLUDES_PRIVATE}
    )

    # Set the target's dependencies.
    target_link_libraries(${targetName}
        PRIVATE
            ${args_LIBRARIES_PRIVATE}
    )
endfunction(aw_add_core_tool)

# Defines an example target.
function(aw_add_example_program targetName)
    set(optionKeywords
        GUI                         # Build a windowed application: an ".app" bundle on macOS, a WinMain executable on Windows.
    )

    set(oneValueKeywords
        DISPLAY_NAME                # Name presented to the user by the platform's shell.
    )

    set(multiValueKeywords
        CXX_SOURCES                 # List of C/C++ source files.
        LIBRARIES_PRIVATE           # List of dependent libraries to link that upstream consumers don't require.
    )

    cmake_parse_arguments(args
        "${optionKeywords}"
        "${oneValueKeywords}"
        "${multiValueKeywords}"
        ${ARGN}
    )

    if(NOT args_DISPLAY_NAME)
        set(args_DISPLAY_NAME ${targetName})
    endif()

    configure_file("${CMAKE_SOURCE_DIR}/resources/resources.rc.in"  "resources.rc")
    configure_file("${CMAKE_SOURCE_DIR}/resources/project.icns"     "project.icns" COPYONLY)
    configure_file("${CMAKE_SOURCE_DIR}/resources/project.ico"      "project.ico" COPYONLY)

    # Define the target.
    if(args_GUI)
        add_executable(${targetName} WIN32 MACOSX_BUNDLE
            ${args_CXX_SOURCES}
            $<$<PLATFORM_ID:Darwin>:project.icns>
            $<$<PLATFORM_ID:Windows>:resources.rc>
        )

        set_source_files_properties("project.icns" PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    else()
        add_executable(${targetName}
            ${args_CXX_SOURCES}
            $<$<PLATFORM_ID:Windows>:resources.rc>
        )
    endif()

    # Ensure examples can use the tools.
    add_dependencies(${targetName} linguist rcc)

    # Set the defaults.
    aw_set_target_defaults(${targetName})

    if(args_GUI AND APPLE)
        set_target_properties(${targetName}
            PROPERTIES
                INSTALL_RPATH                       "${PROJECT_BUNDLE_INSTALL_RPATH}"
                MACOSX_BUNDLE_BUNDLE_NAME           "${args_DISPLAY_NAME}"
                MACOSX_BUNDLE_BUNDLE_VERSION        "${PROJECT_VERSION}"
                MACOSX_BUNDLE_COPYRIGHT             "Copyright (c) ${BUILD_YEAR} Jamie Kenyon. All Rights Reserved."
                MACOSX_BUNDLE_GUI_IDENTIFIER        "com.jamiekenyon.${targetName}"
                MACOSX_BUNDLE_INFO_PLIST            "${CMAKE_SOURCE_DIR}/resources/MacOSXBundleInfo.plist.in"
                MACOSX_BUNDLE_SHORT_VERSION_STRING  "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}"
                OUTPUT_NAME                         "${args_DISPLAY_NAME}"
        )
    else()
        set_target_properties(${targetName}
            PROPERTIES
                INSTALL_RPATH "${PROJECT_INSTALL_RPATH}"
        )
    endif()

    # Set the target's dependencies.
    target_link_libraries(${targetName}
        PRIVATE
            ${args_LIBRARIES_PRIVATE}
    )
endfunction(aw_add_example_program)

# Defines a Python bindings module target.
function(aw_add_python_module targetName)
    set(optionKeywords)

    set(oneValueKeywords)

    set(multiValueKeywords
        CXX_SOURCES                 # List of C/C++ source files.
        LIBRARIES_PRIVATE           # List of dependent libraries to link that upstream consumers don't require.
    )

    cmake_parse_arguments(args
        "${optionKeywords}"
        "${oneValueKeywords}"
        "${multiValueKeywords}"
        ${ARGN}
    )

    # Define the target.
    pybind11_add_module(${targetName} ${args_CXX_SOURCES})

    # Set the defaults.
    aw_set_target_defaults(${targetName})

    # Set the target's dependencies.
    target_link_libraries(${targetName}
        PRIVATE
            ${args_LIBRARIES_PRIVATE}
    )
endfunction(aw_add_python_module)

# Defines a test target.
function(aw_add_test targetName)
    set(optionKeywords)

    set(oneValueKeywords)

    set(multiValueKeywords
        CXX_SOURCES                 # List of C/C++ source files.
        COMPILE_DEFINITIONS_PRIVATE # List of preprocessor definitions to use when compiling that upstream consumers don't require.
        LIBRARIES_PRIVATE           # List of dependent libraries to link that upstream consumers don't require.
    )

    cmake_parse_arguments(args
        "${optionKeywords}"
        "${oneValueKeywords}"
        "${multiValueKeywords}"
        ${ARGN}
    )

    # Define the target.
    add_executable(${targetName} ${args_CXX_SOURCES})

    # Ensure tests can use the tools.
    add_dependencies(${targetName} linguist rcc)

    # Set the defaults.
    aw_set_target_defaults(${targetName})

    # Set RUNPATH/RPATH.
    set_target_properties(${targetName}
        PROPERTIES
            INSTALL_RPATH "${PROJECT_INSTALL_RPATH}"
    )

    # Set the target's compile definitions.
    target_compile_definitions(${targetName}
        PRIVATE
            ${args_COMPILE_DEFINITIONS_PRIVATE}
    )

    # Set the target's dependencies.
    target_link_libraries(${targetName}
        PRIVATE
            ${args_LIBRARIES_PRIVATE}
            Catch2::Catch2WithMain
    )

    # On Windows there is no RPATH equivalent, so a test executable can't find the shared
    # library DLLs it links against unless they live next to it or on PATH. ctest works around
    # this via catch_discover_tests's DL_PATHS, but that doesn't help when the executable is
    # launched directly (e.g. from a debugger or an IDE's test explorer), so copy its runtime
    # DLL dependencies alongside it too.
    if(WIN32)
        # copy_if_different requires at least one source argument, so fall back to a no-op
        # when the target has no runtime DLL dependencies (e.g. a purely statically-linked test).
        add_custom_command(TARGET ${targetName} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E $<IF:$<BOOL:$<TARGET_RUNTIME_DLLS:${targetName}>>,copy_if_different,true>
                $<TARGET_RUNTIME_DLLS:${targetName}>
                $<$<BOOL:$<TARGET_RUNTIME_DLLS:${targetName}>>:$<TARGET_FILE_DIR:${targetName}>>
            COMMAND_EXPAND_LISTS
        )
    endif()

    # Discover the tests.
    # Tests are not installed, so discovery must run against the build tree.
    catch_discover_tests(${targetName}
        DISCOVERY_MODE
            PRE_TEST
        DL_PATHS
            $<TARGET_RUNTIME_DLL_DIRS:${targetName}>
        WORKING_DIRECTORY
            $<TARGET_FILE_DIR:${targetName}>
    )
endfunction(aw_add_test)
