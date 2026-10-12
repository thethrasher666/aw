//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "aw/vulkan-widgets/vulkan-context.hxx"
#include "aw/core/logging.hxx"
#include "aw/vulkan-widgets/bootstrap.hxx"

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

namespace aw::vulkan
{
    VulkanContext::VulkanContext()
    {
        VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);
    }

    VulkanContext::~VulkanContext()
    {
        if (_logicalDevice)
        {
            _logicalDevice.waitIdle();
            _logicalDevice.destroy();
        }

        if (!_instance)
        {
            return;
        }

        if (_surface)
        {
            _instance.destroySurfaceKHR(_surface);
        }

        if (_debugUtilsMessenger)
        {
            _instance.destroyDebugUtilsMessengerEXT(_debugUtilsMessenger, nullptr, vk::detail::DispatchLoaderDynamic(_instance, vkGetInstanceProcAddr));
        }

        _instance.destroy();
    }

    auto VulkanContext::create(gui::Window* window) -> bool
    {
        try
        {
            if (_instance = detail::initInstance(); !_instance)
            {
                awLogCritical("Failed to create Vulkan instance.");
                return false;
            }

            VULKAN_HPP_DEFAULT_DISPATCHER.init(_instance);
            _debugUtilsMessenger = detail::initDebugUtilsMessenger(_instance);

            if (_surface = detail::createSurface(_instance, window); !_surface)
            {
                awLogCritical("Failed to create Vulkan surface.");
                return false;
            }

            auto const devices = detail::createDevices(_instance, _surface);
            if (!devices)
            {
                awLogCritical("Failed to pick a suitable Vulkan device.");
                return false;
            }

            if (!devices->physical)
            {
                awLogCritical("Failed to pick a suitable physical Vulkan device.");
                return false;
            }

            if (!devices->logical)
            {
                awLogCritical("Failed to pick a suitable logical Vulkan device.");
                return false;
            }

            _physicalDevice = devices->physical.value();
            _logicalDevice = devices->logical.value();
            _graphicsQueueFamilyIndex = devices->graphicsQueueFamily;
            _graphicsQueue = _logicalDevice.getQueue(_graphicsQueueFamilyIndex, 0);
            _presentQueueFamilyIndex = devices->presentQueueFamily;
            _presentQueue = _logicalDevice.getQueue(_presentQueueFamilyIndex, 0);
        }
        catch (vk::SystemError const& error)
        {
            awLogCritical("Failed to initialise Vulkan: {}", error.what());
            return false;
        }

        return true;
    }

} // namespace aw::vulkan
