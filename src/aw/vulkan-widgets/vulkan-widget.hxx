//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <aw/vulkan-widgets/swap-chain.hxx>
#include <aw/vulkan-widgets/vulkan-context.hxx>
#include <aw/widgets/widget.hxx>

#include <array>
#include <memory>

namespace aw::vulkan
{
    /// A widget that uses Vulkan for rendering.
    class VulkanWidget : public widgets::Widget
    {
    public:
        /// Constructor.
        /// \param context A unique pointer to a VulkanContext object that will be used for rendering.
        /// \param parent The parent object of this widget.
        /// \param flags The flags for this widget window.
        AW_VULKAN_WIDGETS_API explicit VulkanWidget(std::unique_ptr<VulkanContext> context, widgets::Widget* parent = nullptr, gui::WindowFlags flags = gui::WindowFlags::Widget);

        /// Destructor.
        AW_VULKAN_WIDGETS_API virtual ~VulkanWidget();

        /// Get the Vulkan context used for rendering.
        /// \return The Vulkan context, or nullptr if this widget has none.
        [[nodiscard]] AW_VULKAN_WIDGETS_API auto context() -> VulkanContext*
        {
            return _context.get();
        }

        /// Set the colour used to clear the swapchain image at the start of each frame.
        /// \param color The clear colour as RGBA components in the [0, 1] range.
        AW_VULKAN_WIDGETS_API void setClearColor(std::array<float, 4> const& color)
        {
            _clearColor = color;
        }

        /// Render a single frame, recreating the swapchain when it no longer matches the window.
        /// This is what an application drives once per iteration of its render loop.
        AW_VULKAN_WIDGETS_API void update() override;

    protected:
        /// Records the draw commands for a single frame, inside an already-begun dynamic rendering pass.
        /// The base implementation records nothing, so the frame is just a clear.
        /// \param commandBuffer The command buffer being recorded.
        AW_VULKAN_WIDGETS_API virtual void render([[maybe_unused]] vk::CommandBuffer& commandBuffer)
        {
        }

        /// Invoked when the native geometry of this widget changes.
        AW_VULKAN_WIDGETS_API void onGeometryChanged(core::Rect const& geometry) override;

    private:
        /// Creates the Vulkan context, swapchain and per-frame resources on first use.
        /// \return true once the widget is ready to render.
        [[nodiscard]] auto initialise() -> bool;

        /// Draws a frame using Vulkan.
        /// \return true if the frame was presented, false if the swapchain needs recreating.
        [[nodiscard]] auto drawFrame() -> bool;

        /// Get the current window extent in pixels.
        /// \return The current window extent.
        [[nodiscard]] auto windowExtent() const -> vk::Extent2D;

        std::unique_ptr<vulkan::VulkanContext> _context;                              ///< The Vulkan context used for rendering.
        std::unique_ptr<vulkan::Swapchain>     _swapchain;                            ///< The Vulkan swapchain used for rendering.
        std::unique_ptr<vulkan::FrameContext>  _frames;                               ///< The per-frame-in-flight command buffers and synchronisation primitives.
        std::array<float, 4>                   _clearColor{ 1.0f, 0.0f, 0.0f, 1.0f }; ///< The clear colour, cornflower blue by default.
        uint32_t                               _frameIndex{};                         ///< The frame-in-flight slot used by the next frame.
        bool                                   _initialised{};                        ///< Whether the Vulkan objects have been created.
        bool                                   _initialisationFailed{};               ///< Set once creation fails so that it is not retried every frame.
        bool                                   _swapchainOutOfDate{};                 ///< Set when the swapchain no longer matches the window.
    };
} // namespace aw::vulkan
