//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <aw/core/rect.hxx>
#include <aw/gui/api.hxx>
#include <aw/gui/icon.hxx>
#include <aw/gui/window.hxx>

#include <cstdint>
#include <Windows.h>

namespace aw::gui::platform
{
    /// Win32 platform window implementation.
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
        [[nodiscard]] auto geometry() const -> core::Rect;

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
        [[nodiscard]] auto nativeHandle() const -> void*
        {
            return _nativeHandle;
        }

        /// Get the native handle used to create a rendering surface.
        /// \return The native render-surface handle of this window.
        [[nodiscard]] auto nativeSurfaceHandle() -> void*
        {
            return _nativeHandle;
        }

        /// Set the geometry of this window.
        /// \param geometry The new geometry, in parent-relative coordinates for child windows.
        void setGeometry(core::Rect const& geometry);

        /// Set the icon of this window.
        /// \param icon The new icon to apply.
        void setIcon(Icon const& icon);

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

    private:
        static LRESULT windowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

    private:
        HWND    _nativeHandle{}; ///< The native window handle.
        Window* _owner{};        ///< The owning platform-agnostic window.
    };
} // namespace aw::gui::platform
