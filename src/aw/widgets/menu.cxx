//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "aw/widgets/menu.hxx"
#include "platform/menu.hxx"

namespace aw::widgets
{
    Menu::Menu(std::string const& text, Widget* parent) : Widget(parent), _text(text)
    {
        _menuAction = addChild(std::make_unique<gui::Action>(text, this));
        _platformMenu = std::make_unique<platform::PlatformMenu>(text);
    }

    auto Menu::addAction(std::string const& text) -> gui::Action*
    {
        _actions.emplace_back(createChild<gui::Action>(text));

        _platformMenu->addAction(_actions.back()->text(), _actions.back()->ID());

        return _actions.back();
    }

    auto Menu::addAction(gui::Icon const& icon, std::string const& text) -> gui::Action*
    {
        _actions.emplace_back(createChild<gui::Action>(icon, text));

        _platformMenu->addAction(_actions.back()->text(), _actions.back()->ID());

        return _actions.back();
    }

    MenuBar::MenuBar(Widget* parent) : Widget(parent)
    {
        _platformMenuBar = std::make_unique<platform::PlatformMenuBar>();
    }

    auto MenuBar::addMenu(std::unique_ptr<Menu> menu) -> Menu*
    {
        auto* menuPtr = addChild(std::move(menu));

        _platformMenuBar->addMenu(menuPtr->text(), menuPtr->nativeMenu());

         return menuPtr;
    }

    void MenuBar::attachToWindow(gui::Window* window)
    {
        _platformMenuBar->attachToWindow(window->nativeHandle());
    }

} // namespace aw::widgets
