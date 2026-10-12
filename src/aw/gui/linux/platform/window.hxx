//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <aw/core/rect.hxx>
#include <aw/gui/api.hxx>
#include <aw/gui/icon.hxx>
#include <aw/gui/window.hxx>

#include <xcb/xcb.h>

#include <cstdint>

namespace aw::gui::platform
{
    /// Linux/XCB platform window implementation.
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

        /// Get the native handle of this window.
        /// \return The native handle of this window.
        [[nodiscard]] auto nativeConnection() const -> void*
        {
            return _connection;
        }

        /// Get the native handle of this window.
        /// \return The native handle of this window.
        [[nodiscard]] auto nativeHandle() const -> void*
        {
            return reinterpret_cast<void*>(static_cast<uintptr_t>(_nativeHandle));
        }

        /// Get the native handle used to create a rendering surface.
        /// \return The native render-surface handle of this window.
        [[nodiscard]] auto nativeSurfaceHandle() -> void*
        {
            return nativeHandle();
        }

        /// Set the geometry of this window.
        /// \param geometry The new geometry, in parent-relative coordinates for child windows.
        void setGeometry(Rect const& geometry);

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

        /// Dispatch a native XCB event to the owning platform window.
        /// \return true if the event was handled by a platform window.
        [[nodiscard]] static auto dispatchEvent(xcb_connection_t* connection, xcb_generic_event_t const* event) -> bool;

    private:
        xcb_atom_t        _wmProtocols{};              ///< The atom for the WM_PROTOCOLS property.
        xcb_atom_t        _wmDeleteWindow{};           ///< The atom for the WM_DELETE_WINDOW protocol.
        xcb_atom_t        _netWmState{};               ///< The atom for the _NET_WM_STATE property.
        xcb_atom_t        _netWmStateMaximizedHoriz{}; ///< The atom for the _NET_WM_STATE_MAXIMIZED_HORZ property.
        xcb_atom_t        _netWmStateMaximizedVert{};  ///< The atom for the _NET_WM_STATE_MAXIMIZED_VERT property.
        xcb_atom_t        _netWmStateFullscreen{};     ///< The atom for the _NET_WM_STATE_FULLSCREEN property.
        xcb_connection_t* _connection{};               ///< The XCB connection to the X server.
        xcb_window_t      _nativeHandle{};             ///< The native XCB window handle.
        Window*           _owner{};                    ///< The owning platform-agnostic window.
    };
} // namespace aw::gui::platform
