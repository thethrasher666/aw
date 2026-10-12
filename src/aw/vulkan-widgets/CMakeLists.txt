#
# Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
#

# Define the target.
aw_add_core_shared_library(vulkan-widgets
    CXX_SOURCES
        "bootstrap.cxx"
        "swap-chain.cxx"
        "vulkan-context.cxx"
        "vulkan-widget.cxx"

    DEFINE_SYMBOL
        AW_BUILD_VULKAN_WIDGETS

    COMPILE_DEFINITIONS_PRIVATE
        $<$<PLATFORM_ID:Darwin>:VK_USE_PLATFORM_METAL_EXT>
        $<$<PLATFORM_ID:Linux>:VK_USE_PLATFORM_XCB_KHR>
        $<$<PLATFORM_ID:Windows>:VK_USE_PLATFORM_WIN32_KHR>

    LIBRARIES_PUBLIC
        aw::widgets
        Vulkan::Vulkan
)

# The public headers include vulkan.hpp, so consumers must configure it identically.
target_compile_definitions(vulkan-widgets
    PUBLIC
        VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1
)
