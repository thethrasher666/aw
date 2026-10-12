//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <aw/core/event.hxx>
#include <aw/core/object.hxx>
#include <aw/core/rect.hxx>
#include <aw/gui/api.hxx>
#include <aw/gui/icon.hxx>

#include <memory>
#include <string>

namespace aw::gui
{
    namespace platform
    {
        class PlatformWindow; ///< Forward declaration.
    }

    /// Delegate interface used by composed windows to receive native-window events.
    class WindowDelegate
    {
    public:
        /// Destructor.
        virtual ~WindowDelegate() = default;

        /// Invoked when the native platform requests the window to close.
        /// \param event The close event.
        virtual void closeEvent(core::CloseEvent& event)
        {
            event.accept();
        }

        /// Generic event entry point.
        /// \param event The event to handle.
        /// \return true if the event was handled.
        [[nodiscard]] virtual auto event(core::Event& event) -> bool
        {
            if (event.type() == core::EventType::CloseWindow)
            {
                closeEvent(static_cast<core::CloseEvent&>(event));
                return true;
            }

            return false;
        }

        /// Invoked when the native platform requests the window to close.
        /// \return true to accept the close and destroy the native window; false to ignore it.
        [[nodiscard]] virtual auto onCloseRequested() -> bool
        {
            core::CloseEvent close;

            if (!event(close))
            {
                closeEvent(close);
            }

            return close.isAccepted();
        }

        /// Invoked when the native window geometry changes.
        /// \param rect The new geometry of the window in screen coordinates.
        virtual void onGeometryChanged([[maybe_unused]] core::Rect const& rect)
        {
        }

        /// Invoked when the native platform window has been destroyed.
        virtual void onNativeWindowDestroyed()
        {
        }
    };

    /// Flags for generic windows.
    enum class WindowFlags
    {
        Widget,          ///< Child window embedded in a parent.
        Window = 1 << 0, ///< Top-level resizable window.
        Dialog = 1 << 1, ///< Top-level dialog-style window.
    };

    /// Represents the presentation state of a window.
    enum class WindowState
    {
        Normal,     ///< The window is in its normal state.
        Minimized,  ///< The window is minimized to the taskbar or dock.
        Maximized,  ///< The window is maximized to fill the available screen space.
        FullScreen, ///< The window is in full-screen mode, covering the entire screen.
    };

    /// A window is a rectangular area on the screen that can be used to display content and receive user input.
    class Window : public core::Object
    {
    public:
        /// Constructor.
        /// \param parent The parent window, if any.
        /// \param flags The behavior flags for this window.
        /// \param delegate Optional delegate that receives window events.
        AW_GUI_API explicit Window(Window* parent = nullptr, WindowFlags flags = WindowFlags::Window, WindowDelegate* delegate = nullptr);

        /// Destructor.
        AW_GUI_API virtual ~Window();

        /// Get the frame height of this window.
        /// \return The frame height of this window in pixels.
        [[nodiscard]] AW_GUI_API auto height() const -> int32_t
        {
            return _geometry.height();
        }

        /// Hide the window.
        AW_GUI_API void hide();

        /// Get the native connection of this window.
        /// \return The native connection of this window.
        [[nodiscard]] AW_GUI_API auto nativeConnection() const -> void*;

        /// Get the native handle of this window.
        /// \return The native handle of this window.
        [[nodiscard]] AW_GUI_API auto nativeHandle() const -> void*;

        /// Get the native handle to pass to a graphics API when creating a rendering surface.
        /// The backing object is created on first use and is platform specific (a CAMetalLayer on macOS,
        /// the window handle elsewhere); callers should treat it as opaque.
        /// \return The native render-surface handle of this window.
        [[nodiscard]] AW_GUI_API auto nativeSurfaceHandle() -> void*;

        /// Handle a command.
        /// \param command The command.
        void onCommand(uint32_t const command);

        /// Get the parent window, if any.
        /// \return The parent window, or nullptr if there is no parent.
        [[nodiscard]] auto parent() const -> Window*
        {
            return _parent;
        }

        /// Show the window.
        AW_GUI_API void show();

        /// Set the geometry of this window.
        /// \param geometry The new geometry, in parent-relative coordinates for child windows.
        AW_GUI_API void setGeometry(core::Rect const& geometry);

        /// Set the title of this window.
        /// \param title The new title of this window.
        AW_GUI_API void setTitle(std::string const& title);

        /// Set the icon of this window.
        /// \param icon The new icon of this window.
        AW_GUI_API void setWindowIcon(Icon const& icon);

        /// Set the presentation state of this window.
        /// \param state The new window state.
        AW_GUI_API void setWindowState(WindowState state);

        /// Get the current presentation state of this window.
        /// \return The current window state.
        [[nodiscard]] AW_GUI_API auto windowState() const -> WindowState
        {
            return _windowState;
        }

        /// Get the frame width of this window.
        /// \return The frame width of this window in pixels.
        [[nodiscard]] AW_GUI_API auto width() const -> int32_t
        {
            return _geometry.width();
        }

    protected:
        /// Handles close requests from the native platform.
        AW_GUI_API virtual void closeEvent(core::CloseEvent& event);

        /// Generic event entry point.
        /// \return true if the event was handled.
        [[nodiscard]] AW_GUI_API virtual auto event(core::Event& event) -> bool;

        /// Invoked when the native platform requests the window to close.
        /// \return true to accept the close and destroy the native window; false to ignore it.
        [[nodiscard]] AW_GUI_API virtual auto onCloseRequested() -> bool;

        /// Invoked when the native window geometry changes.
        AW_GUI_API virtual void onGeometryChanged(core::Rect const& geometry);

        /// Invoked when the native platform window has been destroyed.
        AW_GUI_API virtual void onNativeWindowDestroyed();

    private:
        friend class platform::PlatformWindow;

        core::Rect                                _geometry;                           ///< The geometry of this window.
        WindowDelegate*                           _delegate{};                         ///< Optional delegate for window events.
        WindowFlags                               _flags{ WindowFlags::Window };       ///< The behavior flags for this window.
        Window*                                   _parent{};                           ///< The parent window, if any.
        std::unique_ptr<platform::PlatformWindow> _platformWindow;                     ///< The platform-specific implementation of the window.
        WindowState                               _windowState{ WindowState::Normal }; ///< The current presentation state of this window.
    };
} // namespace aw::gui
