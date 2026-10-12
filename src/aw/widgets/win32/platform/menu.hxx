//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <string>
#include <Windows.h>

namespace aw::widgets::platform
{
    /// Win32 platform menu implementation.
    class PlatformMenu
    {
    public:
        /// Constructor.
        /// \param text The text.
        PlatformMenu(std::string const& text);

        /// Add an action to the menu.
        /// \param text The text.
        /// \param ID The action ID.
        void addAction(std::string const& text, uint32_t const ID);

        /// Get the native handle.
        /// \return A valid object.
        auto nativeHandle() const
        {
            return _menu;
        }

        /// Get the text.
        /// \return A valid object.
        [[nodiscard]] auto text() -> std::string const&
        {
            return _text;
        }

    private:
        HMENU             _menu{}; ///< Native handle.
        std::string const _text;   ///< The text.
    };

    /// Win32 platform menu bar implementation.
    class PlatformMenuBar
    {
    public:
        /// Constructor.
        PlatformMenuBar();

        /// Add a menu to the menu bar.
        /// \param text The text.
        /// \param menu The menu to add.
        void addMenu(std::string const& text, PlatformMenu const* menu);

        /// Attach the menu bar to a window.
        /// \param window The window.
        void attachToWindow(void* window);

    private:
        HMENU _menuBar{}; ///< Native handle.
        HWND  _window{};  ///< Window the menu bar is attached to, if any.
    };
} // namespace aw::widgets::platform
