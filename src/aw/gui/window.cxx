//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "aw/gui/window.hxx"
#include "aw/gui/action.hxx"
#include "aw/gui/application.hxx"
#include "platform/window.hxx"

namespace aw::gui
{
    Window::Window(Window* parent, WindowFlags flags, WindowDelegate* delegate) : _delegate(delegate), _flags(flags), _parent(parent)
    {
        if (_platformWindow)
        {
            return;
        }

        setObjectName("Window");

        platform::PlatformWindow* parentPlatformWindow = nullptr;

        if (_parent)
        {
            parentPlatformWindow = _parent->_platformWindow.get();
        }

        _platformWindow = std::make_unique<platform::PlatformWindow>(this, parentPlatformWindow, _flags);

        if (_platformWindow)
        {
            _geometry = _platformWindow->geometry();
        }
    }

    Window::~Window() = default;

    void Window::hide()
    {
        if (!_platformWindow)
        {
            return;
        }

        _platformWindow->hide();
    }

    auto Window::nativeConnection() const -> void*
    {
        return _platformWindow ? _platformWindow->nativeConnection() : nullptr;
    }

    auto Window::nativeHandle() const -> void*
    {
        return _platformWindow ? _platformWindow->nativeHandle() : nullptr;
    }

    auto Window::nativeSurfaceHandle() -> void*
    {
        return _platformWindow ? _platformWindow->nativeSurfaceHandle() : nullptr;
    }

    void Window::onCommand(uint32_t const command)
    {
        if (auto* action = ActionRegistry::instance()->findActionByID(command); action)
        {
            action->activate();
        }
    }

    void Window::show()
    {
        if (!_parent && _flags != WindowFlags::Widget)
        {
            if (auto* app = GuiApplication::instance())
            {
                app->registerTopLevelWindow(this);
            }
        }

        _platformWindow->show();
    }

    void Window::setGeometry(core::Rect const& geometry)
    {
        if (!_platformWindow)
        {
            return;
        }

        _platformWindow->setGeometry(geometry);
    }

    void Window::setTitle(std::string const& title)
    {
        _platformWindow->setTitle(title);
    }

    void Window::setWindowIcon(Icon const& icon)
    {
        if (icon.isValid())
        {
            _platformWindow->setIcon(icon);
        }
    }

    void Window::setWindowState(WindowState state)
    {
        _windowState = state;
        _platformWindow->setWindowState(state);
    }

    void Window::closeEvent(core::CloseEvent& event)
    {
        if (_delegate)
        {
            _delegate->closeEvent(event);
            return;
        }

        event.accept();
    }

    auto Window::event(core::Event& event) -> bool
    {
        if (_delegate)
        {
            return _delegate->event(event);
        }

        if (event.type() == core::EventType::CloseWindow)
        {
            closeEvent(static_cast<core::CloseEvent&>(event));
            return true;
        }

        return false;
    }

    auto Window::onCloseRequested() -> bool
    {
        if (_delegate)
        {
            return _delegate->onCloseRequested();
        }

        core::CloseEvent eventClose;

        if (!event(eventClose))
        {
            closeEvent(eventClose);
        }

        return eventClose.isAccepted();
    }

    void Window::onGeometryChanged(core::Rect const& geometry)
    {
        _geometry = geometry;

        if (_delegate)
        {
            _delegate->onGeometryChanged(geometry);
        }
    }

    void Window::onNativeWindowDestroyed()
    {
        if (_delegate)
        {
            _delegate->onNativeWindowDestroyed();
        }

        if (!_parent && _flags != WindowFlags::Widget)
        {
            if (auto* app = GuiApplication::instance())
            {
                app->notifyTopLevelWindowClosed(this);
            }
        }
    }

} // namespace aw::gui
