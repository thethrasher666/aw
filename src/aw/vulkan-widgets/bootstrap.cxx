//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "aw/vulkan-widgets/bootstrap.hxx"
#include "aw/base/version.hxx"
#include "aw/core/logging.hxx"

#include <algorithm>
#include <ranges>
#include <set>
#include <span>
#include <sstream>

namespace aw::vulkan::detail
{
    namespace
    {
        // Spelled out because the macro lives behind VK_ENABLE_BETA_EXTENSIONS.
        constexpr char const* kPortabilitySubsetExtensionName = "VK_KHR_portability_subset";

        struct VulkanDeviceCapabilities
        {
            uint32_t graphicsQueueFamily = ~0u;
            uint32_t presentQueueFamily = ~0u;

            bool apiVersion14 = false;      // apiVersion >= VK_API_VERSION_1_4
            bool localRead = false;         // VK_KHR_dynamic_rendering_local_read (1.4 core, partial)
            bool hostImageCopy = false;     // VK_EXT_host_image_copy (1.4 core, optional)
            bool unifiedLayouts = false;    // VK_KHR_unified_image_layouts (standalone extension)
            bool portabilitySubset = false; // VK_KHR_portability_subset (non-conformant implementations such as MoltenVK)

            std::optional<vk::PhysicalDevice> physicalDevice;

            bool valid() const
            {
                return graphicsQueueFamily != ~0u && presentQueueFamily != ~0u;
            }
        };

        auto createLogicalDevice(VulkanDeviceCapabilities const& caps, vk::PhysicalDevice const& physicalDevice)
        {
            const float                            priority = 1.0f;
            std::set<uint32_t>                     uniqueFamilies = { caps.graphicsQueueFamily, caps.presentQueueFamily };
            std::vector<vk::DeviceQueueCreateInfo> queueInfos;

            for (uint32_t family : uniqueFamilies)
            {
                vk::DeviceQueueCreateInfo qi{};
                qi.queueFamilyIndex = family;
                qi.queueCount = 1;
                qi.pQueuePriorities = &priority;
                queueInfos.push_back(qi);
            }

            std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
            if (caps.unifiedLayouts)
            {
                deviceExtensions.push_back(VK_KHR_UNIFIED_IMAGE_LAYOUTS_EXTENSION_NAME);
            }

            if (caps.portabilitySubset)
            {
                // Mandatory to enable whenever the device advertises it, as MoltenVK does.
                deviceExtensions.push_back(kPortabilitySubsetExtensionName);
            }

            // Built by hand rather than via vk::StructureChain: the unified
            // image layouts struct is spliced in conditionally at runtime,
            // which doesn't fit StructureChain's fixed compile-time shape.
            vk::PhysicalDeviceUnifiedImageLayoutsFeaturesKHR unifiedLayoutsEnable{};
            unifiedLayoutsEnable.unifiedImageLayouts = true;

            vk::PhysicalDeviceVulkan14Features vk14Enable{};
            vk14Enable.maintenance5 = true;
            vk14Enable.maintenance6 = true;
            vk14Enable.pushDescriptor = true;
            vk14Enable.hostImageCopy = caps.hostImageCopy;
            vk14Enable.dynamicRenderingLocalRead = caps.localRead;
            if (caps.unifiedLayouts)
            {
                vk14Enable.pNext = &unifiedLayoutsEnable;
            }

            vk::PhysicalDeviceVulkan13Features vk13Enable{};
            vk13Enable.dynamicRendering = true;
            vk13Enable.synchronization2 = true;
            vk13Enable.pNext = &vk14Enable;

            vk::PhysicalDeviceFeatures2 features2{};
            features2.pNext = &vk13Enable;

            vk::DeviceCreateInfo deviceInfo{};
            deviceInfo.pNext = &features2; // pEnabledFeatures must stay null when using this
            deviceInfo.queueCreateInfoCount = static_cast<uint32_t>(queueInfos.size());
            deviceInfo.pQueueCreateInfos = queueInfos.data();
            deviceInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
            deviceInfo.ppEnabledExtensionNames = deviceExtensions.data();

            return physicalDevice.createDevice(deviceInfo);
        }

        auto hasExtension(std::vector<vk::ExtensionProperties> const& props, char const* name) -> bool
        {
            return std::any_of(props.begin(), props.end(), [&](vk::ExtensionProperties const& e) { return std::strcmp(e.extensionName, name) == 0; });
        }

        auto probeDevice(vk::PhysicalDevice const& device, vk::SurfaceKHR surface) -> VulkanDeviceCapabilities
        {
            VulkanDeviceCapabilities caps{};

            auto const extProps = device.enumerateDeviceExtensionProperties();
            if (!hasExtension(extProps, VK_KHR_SWAPCHAIN_EXTENSION_NAME))
            {
                return caps; // no swapchain support -> not usable for a window surface
            }

            // Core version-gated features.
            auto        coreChain = device.getFeatures2<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan13Features, vk::PhysicalDeviceVulkan14Features>();
            const auto& vk13 = coreChain.get<vk::PhysicalDeviceVulkan13Features>();
            const auto& vk14 = coreChain.get<vk::PhysicalDeviceVulkan14Features>();

            if (!vk13.dynamicRendering || !vk13.synchronization2)
            {
                return caps; // hard floor unmet
            }

            caps.apiVersion14 = device.getProperties().apiVersion >= VK_API_VERSION_1_4;
            caps.localRead = static_cast<bool>(vk14.dynamicRenderingLocalRead);
            caps.hostImageCopy = static_cast<bool>(vk14.hostImageCopy);
            caps.portabilitySubset = hasExtension(extProps, kPortabilitySubsetExtensionName);

            // Standalone extension: only query its feature struct if the extension is actually advertised, rather than chaining it unconditionally.
            if (hasExtension(extProps, VK_KHR_UNIFIED_IMAGE_LAYOUTS_EXTENSION_NAME))
            {
                auto extChain = device.getFeatures2<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceUnifiedImageLayoutsFeaturesKHR>();
                caps.unifiedLayouts = static_cast<bool>(extChain.get<vk::PhysicalDeviceUnifiedImageLayoutsFeaturesKHR>().unifiedImageLayouts);
            }

            // Prefer a single queue family that does both graphics and present -- fewer ownership-transfer/sync points in the frame loop later.
            auto const              queueFamilies = device.getQueueFamilyProperties();
            std::optional<uint32_t> graphics, present;
            for (uint32_t i = 0; i < queueFamilies.size(); ++i)
            {
                if (queueFamilies[i].queueFlags & vk::QueueFlagBits::eGraphics)
                {
                    graphics = i;
                }

                if (device.getSurfaceSupportKHR(i, surface))
                {
                    present = i;
                }

                if (graphics && present && *graphics == *present)
                {
                    break;
                }
            }

            if (graphics)
            {
                caps.graphicsQueueFamily = *graphics;
            }

            if (present)
            {
                caps.presentQueueFamily = *present;
            }
            return caps;
        }

        auto pickPhysicalDevice(vk::Instance const& instance, vk::SurfaceKHR const& surface) -> std::optional<VulkanDeviceCapabilities>
        {
            auto const devices = instance.enumeratePhysicalDevices();

            if (devices.empty())
            {
                return std::nullopt;
            }

            std::optional<VulkanDeviceCapabilities> fallback;

            for (auto const& device : devices)
            {
                auto caps = detail::probeDevice(device, surface);
                if (!caps.valid())
                {
                    continue;
                }

                caps.physicalDevice = device;

                if (device.getProperties().deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
                {
                    return caps;
                }

                // Keep the first usable non-discrete GPU in case there is no discrete one at all.
                if (!fallback)
                {
                    fallback = caps;
                }
            }

            return fallback;
        }

        [[nodiscard]] vk::Bool32 validationCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT      severity,
                                                    vk::DebugUtilsMessageTypeFlagsEXT             types,
                                                    vk::DebugUtilsMessengerCallbackDataEXT const* data,
                                                    void* /*userData*/)
        {
            std::stringstream ss;

            ss << vk::to_string(severity) << ": " << vk::to_string(types) << ":\n";

            ss << "\tmessageIDName   = <" << data->pMessageIdName << ">\n";
            ss << "\tmessageIdNumber = " << data->messageIdNumber << "\n";
            ss << "\tmessage         = <" << data->pMessage << ">\n";

            if (0 < data->queueLabelCount)
            {
                ss << "\tQueue Labels:\n";

                std::span arr{ data->pQueueLabels, data->queueLabelCount };
                for (auto const& label : arr)
                {
                    ss << "\t\tlabelName = <" << label.pLabelName << ">\n";
                }
            }

            if (0 < data->cmdBufLabelCount)
            {
                ss << "\tCommandBuffer Labels:\n";

                std::span arr{ data->pCmdBufLabels, data->cmdBufLabelCount };
                for (auto const& label : arr)
                {
                    ss << "\t\tlabelName = <" << label.pLabelName << ">\n";
                }
            }

            if (0 < data->objectCount)
            {
                ss << "\tObjects:\n";

                std::span arr{ data->pObjects, data->objectCount };

                for (auto const& object : arr)
                {
                    ss << "\t\tobjectType   = " << vk::to_string(object.objectType) << "\n";
                    ss << "\t\tobjectHandle = " << object.objectHandle << "\n";

                    if (object.pObjectName)
                    {
                        ss << "\t\tobjectName   = <" << object.pObjectName << ">\n";
                    }
                }
            }

            auto const output = ss.str();

            if (severity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eError)
            {
                awLogCritical("{}", output);
            }

            if (severity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning)
            {
                awLogWarning("{}", output);
            }

            if (severity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo)
            {
                awLogInfo("{}", output);
            }

            if (severity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose)
            {
                awLogDebug("{}", output);
            }

            return VK_TRUE;
        }
    } // namespace

    auto initInstance() -> vk::Instance
    {
        std::vector<char const*> layers
        {
#if !defined(NDEBUG)
            "VK_LAYER_KHRONOS_validation"
#endif // #if !defined(NDEBUG)
        };

        std::vector<char const*> extensions
        {
#if !defined(NDEBUG)
            VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
#endif // #if !defined(NDEBUG)

            VK_KHR_SURFACE_EXTENSION_NAME,
#if defined(VK_USE_PLATFORM_METAL_EXT)
            VK_EXT_METAL_SURFACE_EXTENSION_NAME, VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,
#elif defined(VK_USE_PLATFORM_WIN32_KHR)
            VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#elif defined(VK_USE_PLATFORM_XCB_KHR)
            VK_KHR_XCB_SURFACE_EXTENSION_NAME,
#endif // #if defined(VK_USE_PLATFORM_XCB_KHR)
        };

        vk::InstanceCreateFlags flags{};
#if defined(VK_USE_PLATFORM_METAL_EXT)
        // MoltenVK is a non-conformant implementation and is only enumerated when this is set.
        flags |= vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR;
#endif // #if defined(VK_USE_PLATFORM_METAL_EXT)

        vk::ApplicationInfo    appCreateInfo(base::version::name().data(), base::version::ordinal(), base::version::name().data(), base::version::ordinal(), VK_API_VERSION_1_4);
        vk::InstanceCreateInfo instanceCreateInfo(flags, &appCreateInfo, layers, extensions);
        return vk::createInstance(instanceCreateInfo);
    }

    auto initDebugUtilsMessenger(vk::Instance const& instance) -> vk::DebugUtilsMessengerEXT
    {
        vk::DebugUtilsMessengerCreateInfoEXT const info({},
                                                        vk::DebugUtilsMessageSeverityFlagBitsEXT::eError | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning,
                                                        vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
                                                        validationCallback);
        return instance.createDebugUtilsMessengerEXT(info, nullptr, vk::detail::DispatchLoaderDynamic(instance, vkGetInstanceProcAddr));
    }

    auto createSurface(vk::Instance const& instance, gui::Window* window) -> vk::SurfaceKHR
    {
        // The window hands back whatever the platform needs for a presentable surface; on macOS that
        // is a CAMetalLayer rather than the NSWindow returned by nativeHandle().
        void* const surfaceHandle = window->nativeSurfaceHandle();

        if (!surfaceHandle)
        {
            return {};
        }

#if defined(VK_USE_PLATFORM_METAL_EXT)
        // Metal extension.
        vk::MetalSurfaceCreateInfoEXT surfaceCreateInfo({}, static_cast<CAMetalLayer const*>(surfaceHandle));
        return instance.createMetalSurfaceEXT(surfaceCreateInfo);

#elif defined(VK_USE_PLATFORM_WIN32_KHR)
        // Win32.
        vk::Win32SurfaceCreateInfoKHR surfaceCreateInfo({}, ::GetModuleHandleA(nullptr), static_cast<HWND>(surfaceHandle));
        return instance.createWin32SurfaceKHR(surfaceCreateInfo);

#elif defined(VK_USE_PLATFORM_XCB_KHR)
        // XCB (Linux).
        xcb_connection_t*           xcb_connection = static_cast<xcb_connection_t*>(window->nativeConnection());
        xcb_window_t                xcb_window = static_cast<xcb_window_t>(reinterpret_cast<uintptr_t>(surfaceHandle));
        vk::XcbSurfaceCreateInfoKHR surfaceCreateInfo({}, xcb_connection, xcb_window);
        return instance.createXcbSurfaceKHR(surfaceCreateInfo);
#endif
    }

    auto createDevices(vk::Instance const& instance, vk::SurfaceKHR const& surface) -> std::optional<VulkanDevice>
    {
        auto caps = pickPhysicalDevice(instance, surface);
        if (caps)
        {
            VulkanDevice result;

            result.physical = caps->physicalDevice;
            result.logical = createLogicalDevice(caps.value(), caps->physicalDevice.value());
            result.graphicsQueueFamily = caps->graphicsQueueFamily;
            result.presentQueueFamily = caps->presentQueueFamily;

            return result;
        }

        return {};
    }
} // namespace aw::vulkan::detail
