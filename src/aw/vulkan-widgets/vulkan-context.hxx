//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <aw/gui/window.hxx>
#include <aw/vulkan-widgets/api.hxx>
#include <vulkan/vulkan.hpp>

namespace aw::vulkan
{
    /// A class that encapsulates the Vulkan context.
    class VulkanContext
    {
    public:
        /// Constructor.
        AW_VULKAN_WIDGETS_API explicit VulkanContext();

        /// Destructor.
        AW_VULKAN_WIDGETS_API ~VulkanContext();

        /// Creates the Vulkan context.
        /// \param window The window to create the Vulkan context for.
        /// \return true if the context was created successfully, false otherwise.
        [[nodiscard]] AW_VULKAN_WIDGETS_API auto create(gui::Window* window) -> bool;

        /// Get the Vulkan device.
        /// \return The Vulkan device.
        [[nodiscard]] AW_VULKAN_WIDGETS_API auto device() -> vk::Device&
        {
            return _logicalDevice;
        }

        [[nodiscard]] AW_VULKAN_WIDGETS_API auto graphicsQueue() -> vk::Queue&
        {
            return _graphicsQueue;
        }

        [[nodiscard]] AW_VULKAN_WIDGETS_API auto graphicsQueueFamilyIndex() const -> uint32_t
        {
            return _graphicsQueueFamilyIndex;
        }

        [[nodiscard]] AW_VULKAN_WIDGETS_API auto presentQueue() -> vk::Queue&
        {
            return _presentQueue;
        }

        [[nodiscard]] AW_VULKAN_WIDGETS_API auto presentQueueFamilyIndex() const -> uint32_t
        {
            return _presentQueueFamilyIndex;
        }

        /// Get the Vulkan physical device.
        /// \return The Vulkan physical device.
        [[nodiscard]] AW_VULKAN_WIDGETS_API auto physicalDevice() -> vk::PhysicalDevice&
        {
            return _physicalDevice;
        }

        /// Get the Vulkan surface this context presents to.
        /// \return The Vulkan surface.
        [[nodiscard]] AW_VULKAN_WIDGETS_API auto surface() -> vk::SurfaceKHR&
        {
            return _surface;
        }

    private:
        vk::Instance               _instance;                   ///< The Vulkan instance.
        vk::DebugUtilsMessengerEXT _debugUtilsMessenger;        ///< The Vulkan debug utils messenger.
        vk::SurfaceKHR             _surface;                    ///< The Vulkan surface.
        vk::PhysicalDevice         _physicalDevice;             ///< The Vulkan physical device.
        vk::Device                 _logicalDevice;              ///< The Vulkan logical device.
        vk::Queue                  _graphicsQueue;              ///< The Vulkan graphics queue.
        uint32_t                   _graphicsQueueFamilyIndex{}; ///< The Vulkan graphics queue family index.
        vk::Queue                  _presentQueue;               ///< The Vulkan present queue.
        uint32_t                   _presentQueueFamilyIndex{};  ///< The Vulkan present queue family index
    };
} // namespace aw::vulkan
