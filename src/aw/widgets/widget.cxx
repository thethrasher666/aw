//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "aw/widgets/widget.hxx"

#include "aw/widgets/layout.hxx"

namespace aw::widgets
{
    Widget::Widget(Widget* parent, gui::WindowFlags flags) : core::Object(parent), _flags(flags), _parent(parent)
    {
        gui::Window* parentWindow = nullptr;

        setObjectName("Widget");

        if (_parent)
        {
            parentWindow = _parent->_window.get();
        }

        WindowDelegate* delegate = this;
        _window = std::make_unique<gui::Window>(parentWindow, _flags, delegate);

        if (_window)
        {
            _geometry = core::Rect{ 0, 0, _window->width(), _window->height() };
        }
    }

    Widget::~Widget()
    {
        // Children must go before _window, whose native window is the parent of theirs.
        destroyChildren();
    }

    void Widget::hide()
    {
        if (_window)
        {
            _window->hide();
        }

        if (_layout)
        {
            for (Widget* widget : _layout->widgets())
            {
                widget->hide();
            }
        }
    }

    void Widget::setGeometry(core::Rect const& geometry)
    {
        if (_window)
        {
            _window->setGeometry(geometry);
        }
    }

    void Widget::setLayout(std::unique_ptr<Layout> layout)
    {
        std::unique_ptr<Object> const previousLayout = _layout ? removeChild(_layout) : nullptr;

        _layout = addChild(std::move(layout));

        if (_layout)
        {
            _layout->_parent = this;
            _layout->update();
        }
    }

    void Widget::setTitle(std::string const& title)
    {
        if (!_window)
        {
            return;
        }

        _window->setTitle(title);
    }

    void Widget::setWindowIcon(gui::Icon const& icon)
    {
        if (!_window)
        {
            return;
        }

        _window->setWindowIcon(icon);
    }

    void Widget::setWindowState(gui::WindowState state)
    {
        if (!_window)
        {
            return;
        }

        _window->setWindowState(state);
    }

    void Widget::show()
    {
        if (_window)
        {
            _window->show();
        }

        if (_layout)
        {
            for (Widget* widget : _layout->widgets())
            {
                widget->show();
            }
        }
    }

    void Widget::update()
    {
        if (_layout)
        {
            for (Widget* widget : _layout->widgets())
            {
                widget->update();
            }
        }
    }

    void Widget::closeEvent(core::CloseEvent& event)
    {
        event.accept();
    }

    auto Widget::event(core::Event& event) -> bool
    {
        if (event.type() == core::EventType::CloseWindow)
        {
            closeEvent(static_cast<core::CloseEvent&>(event));
            return true;
        }

        return false;
    }

    auto Widget::onCloseRequested() -> bool
    {
        core::CloseEvent close;

        if (!event(close))
        {
            closeEvent(close);
        }

        return close.isAccepted();
    }

    void Widget::onGeometryChanged(core::Rect const& geometry)
    {
        _geometry = geometry;

        if (_layout)
        {
            _layout->update();
        }
    }

    void Widget::onNativeWindowDestroyed()
    {
    }
} // namespace aw::widgets
