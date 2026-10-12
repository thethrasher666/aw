//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "aw/widgets/main-window.hxx"

namespace aw::widgets
{
    MainWindow::MainWindow(Widget* parent, gui::WindowFlags flags) : Widget(parent, flags)
    {
        setObjectName("MainWindow");
    }

    auto MainWindow::setMenuBar(std::unique_ptr<MenuBar> menuBar) -> MenuBar*
    {
        _menuBar = addChild(std::make_unique<MenuBar>(this));
        _menuBar->attachToWindow(window());
        return _menuBar;
    }
} // namespace aw::widgets
