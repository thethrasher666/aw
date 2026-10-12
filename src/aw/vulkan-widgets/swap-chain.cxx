//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "aw/vulkan-widgets/swap-chain.hxx"
#include "aw/vulkan-widgets/vulkan-context.hxx"

namespace aw::vulkan
{
    Swapchain::Swapchain(VulkanContext* context, vk::SurfaceKHR surface, vk::Extent2D windowExtent) : _context(context)
    {
        create(surface, windowExtent);
    }

    Swapchain::~Swapchain()
    {
        destroy();
    }

    void Swapchain::recreate(vk::SurfaceKHR surface, vk::Extent2D windowExtent)
    {
        _context->device().waitIdle();
        destroy();
        create(surface, windowExtent);
    }

    void Swapchain::destroy()
    {
        auto& device = _context->device();

        for (auto semaphore : _renderFinished)
        {
            device.destroySemaphore(semaphore);
        }
        _renderFinished.clear();

        for (auto view : _imageViews)
        {
            device.destroyImageView(view);
        }
        _imageViews.clear();

        _images.clear();
        _imagesInFlight.clear();

        if (_swapchain)
        {
            device.destroySwapchainKHR(_swapchain);
            _swapchain = nullptr;
        }
    }

    void Swapchain::create(vk::SurfaceKHR surface, vk::Extent2D windowExtent)
    {
        auto& gpu = _context->physicalDevice();
        auto  caps = gpu.getSurfaceCapabilitiesKHR(surface);
        auto  formats = gpu.getSurfaceFormatsKHR(surface);
        auto  modes = gpu.getSurfacePresentModesKHR(surface);

        vk::SurfaceFormatKHR chosenFormat = formats[0];
        for (auto& f : formats)
        {
            if (f.format == vk::Format::eB8G8R8A8Unorm && f.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
            {
                chosenFormat = f;
                break;
            }
        }

        vk::PresentModeKHR chosenMode = vk::PresentModeKHR::eFifo; // always available, vsynced
        for (auto& m : modes)
        {
            if (m == vk::PresentModeKHR::eMailbox)
            {
                chosenMode = m;
                break;
            }
        }

        _extent = caps.currentExtent.width != 0xFFFFFFFFu ? caps.currentExtent :
                                                            vk::Extent2D{ std::clamp(windowExtent.width, caps.minImageExtent.width, caps.maxImageExtent.width),
                                                                          std::clamp(windowExtent.height, caps.minImageExtent.height, caps.maxImageExtent.height) };

        uint32_t imageCount = caps.minImageCount + 1;
        if (caps.maxImageCount > 0)
            imageCount = std::min(imageCount, caps.maxImageCount);

        std::array<uint32_t, 2> families = {
            _context->graphicsQueueFamilyIndex(),
            _context->presentQueueFamilyIndex(),
        };

        vk::SwapchainCreateInfoKHR info{};
        info.surface = surface;
        info.minImageCount = imageCount;
        info.imageFormat = chosenFormat.format;
        info.imageColorSpace = chosenFormat.colorSpace;
        info.imageExtent = _extent;
        info.imageArrayLayers = 1;
        info.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
        info.preTransform = caps.currentTransform;
        info.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
        info.presentMode = chosenMode;
        info.clipped = true;

        if (families[0] != families[1])
        {
            info.imageSharingMode = vk::SharingMode::eConcurrent;
            info.queueFamilyIndexCount = 2;
            info.pQueueFamilyIndices = families.data();
        }
        else
        {
            info.imageSharingMode = vk::SharingMode::eExclusive;
        }

        _format = chosenFormat.format;
        _swapchain = _context->device().createSwapchainKHR(info);
        _images = _context->device().getSwapchainImagesKHR(_swapchain);

        _imagesInFlight.assign(_images.size(), vk::Fence{});

        _imageViews.clear();
        _imageViews.reserve(_images.size());
        _renderFinished.reserve(_images.size());
        for (auto image : _images)
        {
            vk::ImageViewCreateInfo viewInfo{};
            viewInfo.image = image;
            viewInfo.viewType = vk::ImageViewType::e2D;
            viewInfo.format = _format;
            viewInfo.subresourceRange = { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 };
            _imageViews.emplace_back(_context->device().createImageView(viewInfo));
            _renderFinished.emplace_back(_context->device().createSemaphore(vk::SemaphoreCreateInfo{}));
        }
    }

    FrameContext::FrameContext(VulkanContext* context) : _context(context)
    {
        vk::CommandPoolCreateInfo poolInfo{};
        poolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
        poolInfo.queueFamilyIndex = context->graphicsQueueFamilyIndex();
        _commandPool = context->device().createCommandPool(poolInfo);

        vk::CommandBufferAllocateInfo allocInfo{};
        allocInfo.commandPool = _commandPool;
        allocInfo.level = vk::CommandBufferLevel::ePrimary;
        allocInfo.commandBufferCount = kFramesInFlight;
        _commandBuffers = context->device().allocateCommandBuffers(allocInfo);

        for (uint32_t i = 0; i < kFramesInFlight; ++i)
        {
            _imageAvailable.emplace_back(context->device().createSemaphore(vk::SemaphoreCreateInfo{}));

            vk::FenceCreateInfo fenceInfo{};
            fenceInfo.flags = vk::FenceCreateFlagBits::eSignaled; // frame 0 shouldn't stall waiting
            _inFlight.emplace_back(context->device().createFence(fenceInfo));
        }
    }

    FrameContext::~FrameContext()
    {
        auto& device = _context->device();

        for (auto fence : _inFlight)
        {
            device.destroyFence(fence);
        }

        for (auto semaphore : _imageAvailable)
        {
            device.destroySemaphore(semaphore);
        }

        // Destroying the pool frees the command buffers allocated from it.
        device.destroyCommandPool(_commandPool);
    }
} // namespace aw::vulkan
