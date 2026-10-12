//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <aw/gui/window.hxx>
#include <vulkan/vulkan.hpp>

#include <optional>

namespace aw::vulkan::detail
{
    /// A structure representing a Vulkan device, including both the physical and logical devices.
    struct VulkanDevice
    {
        std::optional<vk::PhysicalDevice> physical;                  ///< The physical device used for rendering.
        std::optional<vk::Device>         logical;                   ///< The logical device used for rendering.
        uint32_t                          graphicsQueueFamily = ~0u; ///< The index of the graphics queue family.
        uint32_t                          presentQueueFamily = ~0u;  ///< The index of the present queue family.
    };

    /// Initialize a Vulkan instance.
    /// \return A Vulkan instance.
    [[nodiscard]] auto initInstance() -> vk::Instance;

    /// Initialize the debug utils messenger for a Vulkan instance.
    /// \param instance The Vulkan instance.
    /// \return A Vulkan debug utils messenger.
    [[nodiscard]] auto initDebugUtilsMessenger(vk::Instance const& instance) -> vk::DebugUtilsMessengerEXT;

    /// Create a Vulkan surface for rendering.
    /// \param instance The Vulkan instance.
    /// \param window The window to create the surface for.
    /// \return A Vulkan surface.
    [[nodiscard]] auto createSurface(vk::Instance const& instance, gui::Window* window) -> vk::SurfaceKHR;

    /// Create a Vulkan device for rendering.
    /// \param instance The Vulkan instance.
    /// \param surface The Vulkan surface.
    /// \return A Vulkan device.
    [[nodiscard]] auto createDevices(vk::Instance const& instance, vk::SurfaceKHR const& surface) -> std::optional<VulkanDevice>;
} // namespace aw::vulkan::detail
