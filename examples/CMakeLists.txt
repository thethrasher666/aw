#
# Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
#

if(NOT AW_BUILD_EXAMPLES)
    return()
endif()

# Include the dependent modules.
include("../share/cmake/aw")

# Add the command-line example.
add_subdirectory("cli")

# Add the graphics example.
add_subdirectory("graphics")

# Add the window example.
add_subdirectory("window")

# Function to deploy the example projects.
function(_deploy_examples)
    set(optionKeywords)

    set(oneValueKeywords)

    set(multiValueKeywords
        GUI_EXAMPLES        # List of example GUI projects to deploy.
        CLI_EXAMPLES        # List of example CLI projects to deploy.
    )

    cmake_parse_arguments(args
        "${optionKeywords}"
        "${oneValueKeywords}"
        "${multiValueKeywords}"
        ${ARGN}
    )

    # The framework libraries that every example needs at runtime.
    set(frameworkLibraries core gui widgets vulkan-widgets)

    foreach(example ${args_GUI_EXAMPLES})
        install(
            TARGETS
                ${example}
            BUNDLE
                DESTINATION ${PROJECT_INSTALL_BUNDLEDIR}
            RUNTIME
                DESTINATION ${PROJECT_INSTALL_BINDIR}
        )

        # A GUI example is self-contained, so its libraries ship next to its executable.
        if(APPLE)
            get_target_property(bundleName ${example} OUTPUT_NAME)
            set(exampleBundleDir "${bundleName}.app/Contents")
            set(exampleLibDir "${exampleBundleDir}/MacOS")
        else()
            set(exampleLibDir "${PROJECT_INSTALL_LIBDIR}")
        endif()

        install(
            TARGETS
                ${frameworkLibraries}
            LIBRARY
                DESTINATION ${exampleLibDir}
            RUNTIME
                DESTINATION ${exampleLibDir}
        )

        # On macOS, install the Vulkan runtime libraries into the app bundle.
        get_target_property(exampleLibraries ${example} LINK_LIBRARIES)

        if(APPLE AND "aw::vulkan-widgets" IN_LIST exampleLibraries)
            # Get the directory containing the Vulkan SDK libraries.
            get_target_property(targetLocation Vulkan::Vulkan LOCATION)
            cmake_path(GET targetLocation PARENT_PATH vulkanLibDir)

            # Install the Vulkan libraries alongside the executable.
            file(GLOB vulkanLibs "${vulkanLibDir}/libvulkan*.dylib")
            install(
                PROGRAMS
                    ${vulkanLibs}
                    "${vulkanLibDir}/libvulkan_kosmickrisp.dylib"
                DESTINATION
                    "${exampleLibDir}"
            )

            # Compute the paths to the JSON ICD files.
            cmake_path(GET vulkanLibDir PARENT_PATH baseVulkanDir)
            set(ICD_SOURCE "${baseVulkanDir}/share/vulkan/icd.d/libkosmickrisp_icd.json")
            set(ICD_DEST "${CMAKE_CURRENT_BINARY_DIR}/libkosmickrisp_icd.json")

            # Point the ICD at the driver, relative to "Contents/Resources/vulkan/icd.d".
            file(READ "${ICD_SOURCE}" ICD_CONTENT)
            string(REPLACE "../../../lib/" "../../../MacOS/" ICD_CONTENT "${ICD_CONTENT}")
            file(WRITE "${ICD_DEST}" "${ICD_CONTENT}")

            install(
                FILES
                    "${ICD_DEST}"
                DESTINATION
                    "${exampleBundleDir}/Resources/vulkan/icd.d/"
            )
        endif()
    endforeach()

    foreach(example ${args_CLI_EXAMPLES})
        install(
            TARGETS
                ${example}
                ${frameworkLibraries}
            RUNTIME
                DESTINATION ${PROJECT_INSTALL_BINDIR}
            LIBRARY
                DESTINATION ${PROJECT_INSTALL_LIBDIR}
        )
    endforeach()
endfunction(_deploy_examples)

# Deploy the examples.
_deploy_examples(
    GUI_EXAMPLES
        example.graphics
        example.window

    CLI_EXAMPLES
        example.cli
)
