//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "aw/vulkan-widgets/vulkan-widget.hxx"

#include "aw/core/logging.hxx"

#include <algorithm>

namespace aw::vulkan
{
    VulkanWidget::VulkanWidget(std::unique_ptr<VulkanContext> context, Widget* parent, gui::WindowFlags flags) : Widget(parent, flags), _context(std::move(context))
    {
        setObjectName("VulkanWidget");
    }

    VulkanWidget::~VulkanWidget()
    {
        if (_initialised)
        {
            // Nothing may be destroyed while the GPU still has work referencing it.
            _context->device().waitIdle();
        }
    }

    void VulkanWidget::update()
    {
        if (!initialise())
        {
            return;
        }

        auto const extent = windowExtent();
        if (extent.width == 0 || extent.height == 0)
        {
            return; // Minimised or not yet mapped, so there is nothing to present to.
        }

        if (_swapchainOutOfDate)
        {
            _swapchain->recreate(_context->surface(), extent);
            _swapchainOutOfDate = false;
        }

        if (!drawFrame())
        {
            _swapchainOutOfDate = true;
            return;
        }

        _frameIndex = (_frameIndex + 1) % FrameContext::kFramesInFlight;
    }

    void VulkanWidget::onGeometryChanged(core::Rect const& geometry)
    {
        Widget::onGeometryChanged(geometry);
        _swapchainOutOfDate = true;
    }

    auto VulkanWidget::initialise() -> bool
    {
        if (_initialised)
        {
            return true;
        }

        if (_initialisationFailed || !_context || !window())
        {
            return false;
        }

        auto const extent = windowExtent();
        if (extent.width == 0 || extent.height == 0)
        {
            return false; // Wait until the native window has a usable size.
        }

        if (!_context->create(window()))
        {
            awLogCritical("Failed to create the Vulkan context for the widget.");
            _initialisationFailed = true;
            return false;
        }

        _swapchain = std::make_unique<Swapchain>(_context.get(), _context->surface(), extent);
        _frames = std::make_unique<FrameContext>(_context.get());
        _initialised = true;

        return true;
    }

    auto VulkanWidget::windowExtent() const -> vk::Extent2D
    {
        return vk::Extent2D{ static_cast<uint32_t>(std::max(width(), 0)), static_cast<uint32_t>(std::max(height(), 0)) };
    }

    auto VulkanWidget::drawFrame() -> bool
    {
        auto& device = _context->device();
        auto  inFlightFence = _frames->inFlightFence(_frameIndex);
        (void) device.waitForFences(inFlightFence, VK_TRUE, UINT64_MAX);

        uint32_t imageIndex = 0;

        try
        {
            // eSuboptimalKHR is reported as a success and the image stays presentable this frame.
            auto const acquired = device.acquireNextImageKHR(_swapchain->handle(), UINT64_MAX, _frames->imageAvailable(_frameIndex), nullptr);
            imageIndex = acquired.value;
        }
        catch (vk::OutOfDateKHRError const&)
        {
            return false;
        }

        // If a different frame-in-flight slot is still rendering into (or
        // presenting) this exact image, wait for it before we touch it too.
        // Harmless no-op if it's already signaled, or if it's the same fence
        // we're about to wait on/reset below.
        if (vk::Fence claimedBy = _swapchain->imageInFlightFence(imageIndex))
        {
            (void) device.waitForFences(claimedBy, VK_TRUE, UINT64_MAX);
        }
        _swapchain->setImageInFlightFence(imageIndex, inFlightFence);

        device.resetFences(inFlightFence);

        auto& cmd = _frames->commandBuffer(_frameIndex);
        cmd.reset();
        cmd.begin(vk::CommandBufferBeginInfo{});

        vk::Image image = _swapchain->images()[imageIndex];

        // UNDEFINED -> COLOR_ATTACHMENT_OPTIMAL.
        vk::ImageMemoryBarrier2 toAttachment{};
        toAttachment.srcStageMask = vk::PipelineStageFlagBits2::eTopOfPipe;
        toAttachment.srcAccessMask = vk::AccessFlagBits2::eNone;
        toAttachment.dstStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput;
        toAttachment.dstAccessMask = vk::AccessFlagBits2::eColorAttachmentWrite;
        toAttachment.oldLayout = vk::ImageLayout::eUndefined;
        toAttachment.newLayout = vk::ImageLayout::eColorAttachmentOptimal;
        toAttachment.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        toAttachment.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        toAttachment.image = image;
        toAttachment.subresourceRange = { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 };

        vk::DependencyInfo dep1{};
        dep1.imageMemoryBarrierCount = 1;
        dep1.pImageMemoryBarriers = &toAttachment;
        cmd.pipelineBarrier2(dep1);

        vk::RenderingAttachmentInfo colorAttachment{};
        colorAttachment.imageView = _swapchain->imageViews()[imageIndex];
        colorAttachment.imageLayout = vk::ImageLayout::eColorAttachmentOptimal;
        colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
        colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
        colorAttachment.clearValue.color.float32 = _clearColor;

        vk::RenderingInfo renderingInfo{};
        renderingInfo.renderArea = vk::Rect2D{ vk::Offset2D{ 0, 0 }, _swapchain->extent() };
        renderingInfo.layerCount = 1;
        renderingInfo.colorAttachmentCount = 1;
        renderingInfo.pColorAttachments = &colorAttachment;

        cmd.beginRendering(renderingInfo);
        render(cmd);
        cmd.endRendering();

        // COLOR_ATTACHMENT_OPTIMAL -> PRESENT_SRC_KHR.
        vk::ImageMemoryBarrier2 toPresent{};
        toPresent.srcStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput;
        toPresent.srcAccessMask = vk::AccessFlagBits2::eColorAttachmentWrite;
        toPresent.dstStageMask = vk::PipelineStageFlagBits2::eBottomOfPipe;
        toPresent.dstAccessMask = vk::AccessFlagBits2::eNone;
        toPresent.oldLayout = vk::ImageLayout::eColorAttachmentOptimal;
        toPresent.newLayout = vk::ImageLayout::ePresentSrcKHR;
        toPresent.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        toPresent.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        toPresent.image = image;
        toPresent.subresourceRange = { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 };

        vk::DependencyInfo dep2{};
        dep2.imageMemoryBarrierCount = 1;
        dep2.pImageMemoryBarriers = &toPresent;
        cmd.pipelineBarrier2(dep2);

        cmd.end();

        vk::SemaphoreSubmitInfo waitInfo{};
        waitInfo.semaphore = _frames->imageAvailable(_frameIndex);
        waitInfo.stageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput;

        vk::SemaphoreSubmitInfo signalInfo{};
        signalInfo.semaphore = _swapchain->renderFinished(imageIndex);
        signalInfo.stageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput;

        vk::CommandBufferSubmitInfo cmdInfo{};
        cmdInfo.commandBuffer = cmd;

        vk::SubmitInfo2 submitInfo{};
        submitInfo.waitSemaphoreInfoCount = 1;
        submitInfo.pWaitSemaphoreInfos = &waitInfo;
        submitInfo.commandBufferInfoCount = 1;
        submitInfo.pCommandBufferInfos = &cmdInfo;
        submitInfo.signalSemaphoreInfoCount = 1;
        submitInfo.pSignalSemaphoreInfos = &signalInfo;

        _context->graphicsQueue().submit2(submitInfo, inFlightFence);

        vk::PresentInfoKHR presentInfo{};
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &_swapchain->renderFinished(imageIndex);
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &_swapchain->handle();
        presentInfo.pImageIndices = &imageIndex;

        try
        {
            return _context->presentQueue().presentKHR(presentInfo) != vk::Result::eSuboptimalKHR;
        }
        catch (vk::OutOfDateKHRError const&)
        {
            return false;
        }
    }
} // namespace aw::vulkan
