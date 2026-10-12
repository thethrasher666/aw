//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <aw/widgets/menu.hxx>
#include <aw/widgets/widget.hxx>

namespace aw::widgets
{
    /// The main window of the application.
    class MainWindow : public Widget
    {
    public:
        /// Constructor.
        /// \param parent The parent object of this widget.
        /// \param flags The window flags for this widget.
        AW_WIDGETS_API explicit MainWindow(Widget* parent = nullptr, gui::WindowFlags flags = gui::WindowFlags::Window);

        /// Get the menu bar.
        /// \return The window's menu bar.
        [[nodiscard]] auto menuBar() -> MenuBar*
        {
            return _menuBar;
        }

        /// Set the menu bar.
        /// \param menuBar The menu bar.
        /// \return A non-owning pointer.
        [[nodiscard]] AW_WIDGETS_API auto setMenuBar(std::unique_ptr<MenuBar> menuBar) -> MenuBar*;

    private:
        MenuBar* _menuBar{}; ///< The window's menu bar.
    };
} // namespace aw::widgets
