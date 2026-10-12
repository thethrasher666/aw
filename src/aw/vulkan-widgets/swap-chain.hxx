//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <vulkan/vulkan.hpp>

namespace aw::vulkan
{
    class VulkanContext;

    /// A class that encapsulates a Vulkan swapchain.
    class Swapchain
    {
    public:
        /// Constructor.
        /// \param context The Vulkan context.
        /// \param surface The Vulkan surface.
        /// \param windowExtent The extent of the window.
        Swapchain(VulkanContext* context, vk::SurfaceKHR surface, vk::Extent2D windowExtent);

        /// Destructor.
        ~Swapchain();

        /// Disable copying.
        Swapchain(Swapchain const&) = delete;

        /// Disable assignment.
        auto operator=(Swapchain const&) -> Swapchain& = delete;

        /// Recreates the swapchain.
        /// \param surface The Vulkan surface.
        /// \param windowExtent The extent of the window.
        void recreate(vk::SurfaceKHR surface, vk::Extent2D windowExtent);

        /// Get the Vulkan swapchain handle.
        /// \return The Vulkan swapchain handle.
        auto handle() -> vk::SwapchainKHR&
        {
            return _swapchain;
        }

        /// Get the Vulkan swapchain format.
        /// \return The Vulkan swapchain format.
        auto format() const -> vk::Format
        {
            return _format;
        }

        /// Get the Vulkan swapchain extent.
        /// \return The Vulkan swapchain extent.
        auto extent() const -> vk::Extent2D
        {
            return _extent;
        }

        /// Get the Vulkan swapchain images.
        /// \return The Vulkan swapchain images.
        auto images() const -> const std::vector<vk::Image>&
        {
            return _images;
        }

        /// Get the Vulkan swapchain image views.
        /// \return The Vulkan swapchain image views.
        auto imageViews() const -> const std::vector<vk::ImageView>&
        {
            return _imageViews;
        }

        /// Get the semaphore signalled when rendering into the specified swapchain image has finished.
        /// These are owned per image rather than per frame-in-flight because a present operation keeps
        /// waiting on the semaphore until the image is released back to the application.
        /// \param index The index of the swapchain image.
        /// \return The semaphore signalled when rendering into the image has finished.
        auto renderFinished(uint32_t index) -> vk::Semaphore&
        {
            return _renderFinished[index];
        }

        /// Get the fence for the in-flight frame that currently owns the specified swapchain image.
        /// \param index The index of the swapchain image.
        /// \return The fence for the in-flight frame that currently owns the specified swapchain image.
        auto imageInFlightFence(uint32_t index) const -> vk::Fence
        {
            return _imagesInFlight[index];
        }

        /// Set the fence for the in-flight frame that currently owns the specified swapchain image.
        /// \param index The index of the swapchain image.
        /// \param fence The fence for the in-flight frame that currently owns the specified swapchain image.
        void setImageInFlightFence(uint32_t index, vk::Fence fence)
        {
            _imagesInFlight[index] = fence;
        }

    protected:
        /// Creates the Vulkan swapchain.
        /// \param surface The Vulkan surface.
        /// \param windowExtent The extent of the window.
        void create(vk::SurfaceKHR surface, vk::Extent2D windowExtent);

        /// Destroys every object owned by the current swapchain.
        void destroy();

    private:
        VulkanContext*             _context{};      ///< The Vulkan context.
        vk::SwapchainKHR           _swapchain;      ///< The Vulkan swapchain handle.
        vk::Format                 _format;         ///< The Vulkan swapchain format.
        vk::Extent2D               _extent;         ///< The Vulkan swapchain extent.
        std::vector<vk::Image>     _images;         ///< The Vulkan swapchain images.
        std::vector<vk::ImageView> _imageViews;     ///< The Vulkan swapchain image views.
        std::vector<vk::Semaphore> _renderFinished; ///< The semaphores signalled when rendering into each swapchain image has finished.
        std::vector<vk::Fence>     _imagesInFlight; ///< The fences for the in-flight frames that currently own the swapchain images. Null until a frame first claims that image.
    };

    /// A class that encapsulates a Vulkan frame context.
    class FrameContext
    {
    public:
        static constexpr uint32_t kFramesInFlight = 3; ///< The number of frames that can be in flight at once.

    public:
        /// Constructor.
        /// \param context The Vulkan context.
        explicit FrameContext(VulkanContext* context);

        /// Destructor.
        ~FrameContext();

        /// Disable copying.
        FrameContext(FrameContext const&) = delete;

        /// Disable assignment.
        auto operator=(FrameContext const&) -> FrameContext& = delete;

        /// Get the Vulkan command buffer for the specified frame.
        /// \param i The index of the frame.
        /// \return The Vulkan command buffer for the specified frame.
        auto commandBuffer(uint32_t i) -> vk::CommandBuffer&
        {
            return _commandBuffers[i];
        }

        /// Get the Vulkan semaphore that signals when an image is available for rendering for the specified frame.
        /// \param i The index of the frame.
        /// \return The Vulkan semaphore that signals when an image is available for rendering for the specified frame.
        auto imageAvailable(uint32_t i) -> vk::Semaphore&
        {
            return _imageAvailable[i];
        }

        /// Get the Vulkan fence that signals when a frame is in flight for the specified frame.
        /// \param i The index of the frame.
        /// \return The Vulkan fence that signals when a frame is in flight for the specified frame
        auto inFlightFence(uint32_t i) -> vk::Fence&
        {
            return _inFlight[i];
        }

    private:
        VulkanContext*                 _context{};      ///< The Vulkan context.
        vk::CommandPool                _commandPool;    ///< The Vulkan command pool for the frame context.
        std::vector<vk::CommandBuffer> _commandBuffers; ///< The Vulkan command buffers for the frame context.
        std::vector<vk::Semaphore>     _imageAvailable; ///< The Vulkan semaphores for the frame context that signal when an image is available for rendering.
        std::vector<vk::Fence>         _inFlight;       ///< The Vulkan fences for the frame context that signal when a frame is in flight.
    };
} // namespace aw::vulkan
