//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <aw/core/rect.hxx>
#include <aw/gui/api.hxx>
#include <aw/gui/icon.hxx>
#include <aw/gui/window.hxx>

#include <cstdint>
#include <string>

namespace aw::gui::platform
{
    /// Darwin platform window implementation.
    class PlatformWindow
    {
    public:
        /// Constructor.
        /// \param owner The owning platform-agnostic window.
        /// \param parent The optional parent platform window.
        /// \param flags The flags used to configure the native window style.
        PlatformWindow(Window* owner, PlatformWindow* parent, WindowFlags flags);

        /// Destructor.
        ~PlatformWindow();

        /// Get the geometry of this window.
        /// \return The geometry of this window in pixels.
        [[nodiscard]] auto geometry() const -> Rect;

        /// Get the frame height of this window.
        /// \return The frame height of this window in pixels.
        [[nodiscard]] auto height() const -> int32_t;

        /// Hide the window.
        void hide();

        /// Get the native connection of this window.
        /// \return The native connection of this window.
        [[nodiscard]] auto nativeConnection() const -> void*
        {
            return nullptr;
        }

        /// Get the native handle of this window.
        /// \return The native handle of this window.
        [[nodiscard]] auto nativeHandle() const -> void*;

        /// Get the CAMetalLayer used to create a rendering surface, installing a layer-backed
        /// content view on first use.
        /// \return The native render-surface handle of this window.
        [[nodiscard]] auto nativeSurfaceHandle() -> void*;

        /// Set the geometry of this window.
        /// \param geometry The new geometry, in parent-relative coordinates for child windows.
        void setGeometry(Rect const& geometry);

        /// Set the icon of this window.
        /// \param icon The new icon to apply.
        void setIcon([[maybe_unused]] Icon const& icon)
        {
        }

        /// Set the title of this window.
        /// \param title The new title of this window.
        void setTitle(std::string const& title);

        /// Set the presentation state of this window.
        /// \param state The new state to apply.
        void setWindowState(WindowState state);

        /// Show the window.
        void show();

        /// Get the frame width of this window.
        /// \return The frame width of this window in pixels.
        [[nodiscard]] auto width() const -> int32_t;

        /// Notify the owner that this window's geometry changed.
        void notifyGeometryChanged();

        /// Notify the owner that this window is being destroyed.
        void notifyNativeWindowDestroyed();

        /// Request close approval from the owner.
        /// \return true if the close should proceed.
        [[nodiscard]] auto notifyCloseRequested() -> bool;

        /// Clear the stored native window handle.
        void clearNativeWindow();

    private:
        void*           _nativeWindow{};   ///< The native NSWindow instance.
        void*           _nativeDelegate{}; ///< The native delegate instance.
        void*           _metalLayer{};     ///< The CAMetalLayer backing the render surface, if requested.
        Window*         _owner{};          ///< The owning platform-agnostic window.
        PlatformWindow* _parent{};         ///< The parent platform window, if any.
    };
} // namespace aw::gui::platform
