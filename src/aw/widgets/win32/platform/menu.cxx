//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "platform/menu.hxx"

namespace aw::widgets::platform
{
    PlatformMenu::PlatformMenu(std::string const& text) : _text(text)
    {
        _menu = CreateMenu();
    }

    void PlatformMenu::addAction(std::string const& text, uint32_t const ID)
    {
        AppendMenu(_menu, MF_STRING, ID, text.data());
    }

    PlatformMenuBar::PlatformMenuBar()
    {
        _menuBar = CreateMenu();
    }

    void PlatformMenuBar::addMenu(std::string const& text, PlatformMenu const* menu)
    {
        AppendMenu(_menuBar, MF_POPUP, reinterpret_cast<UINT_PTR>(menu->nativeHandle()), text.data());

        if (_window)
        {
            DrawMenuBar(_window);
        }
    }

    void PlatformMenuBar::attachToWindow(void* window)
    {
        _window = static_cast<HWND>(window);
        SetMenu(_window, _menuBar);
        SetWindowPos(_window, nullptr, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
    }
} // namespace aw::widgets::platform
