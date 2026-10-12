//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <aw/gui/action.hxx>
#include <aw/widgets/widget.hxx>

namespace aw::widgets
{
    namespace platform
    {
        class PlatformMenu;    ///< Forward declaration.
        class PlatformMenuBar; ///< Forward declaration.

    } // namespace platform

    /// This class provides a menu widget for use in menu bars, context menus, and other popup menus.
    class Menu : public Widget
    {
    public:
        /// Constructor.
        /// \param text The text.
        /// \param parent The parent widget, if any.
        AW_WIDGETS_API explicit Menu(std::string const& text, Widget* parent = nullptr);

        /// Add an action to the menu.
        /// \param text The text.
        /// \return A non-owning pointer.
        [[nodiscard]] AW_WIDGETS_API auto addAction(std::string const& text) -> gui::Action*;

        /// Add an action to the menu.
        /// \param icon The icon.
        /// \param text The text.
        /// \return A non-owning pointer.
        [[nodiscard]] AW_WIDGETS_API auto addAction(gui::Icon const& icon, std::string const& text) -> gui::Action*;

        /// Get the native menu.
        /// \return A valid object.
        [[nodiscard]] auto nativeMenu()
        {
            return _platformMenu.get();
        }

        /// Get the text.
        /// \return A valid object.
        [[nodiscard]] auto text() -> std::string const&
        {
            return _text;
        }

    private:
        std::vector<gui::Action*>               _actions;      ///< The menu's actions.
        gui::Action*                            _menuAction{}; ///< The associated action.
        std::unique_ptr<platform::PlatformMenu> _platformMenu; ///< The platform-specific implementation of the menu.
        std::string const                       _text;         ///< The text.
    };

    /// This class provides a horizontal menu bar.
    class MenuBar : public Widget
    {
    public:
        /// Constructor.
        /// \param parent The parent widget, if any.
        AW_WIDGETS_API explicit MenuBar(Widget* parent = nullptr);

        /// Add a menu to the menu bar.
        /// \param menu The menu to add.
        [[nodiscard]] AW_WIDGETS_API auto addMenu(std::unique_ptr<Menu> menu) -> Menu*;

        /// Attach the menu bar to a window.
        /// \param window The window.
        AW_WIDGETS_API void attachToWindow(gui::Window* window);

    private:
        std::unique_ptr<platform::PlatformMenuBar> _platformMenuBar; ///< The platform-specific implementation of the menu bar.
    };
} // namespace aw::widgets
