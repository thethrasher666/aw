//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "aw/gui/application.hxx"
#include "platform/application.hxx"

#include <aw/gui/window.hxx>

#include <aw/core/thread.hxx>

namespace aw::gui
{
    GuiApplication::GuiApplication(std::vector<std::string> const& arguments) : CoreApplication(arguments)
    {
        _platform = std::make_unique<platform::GuiApplicationPlatform>();
        core::Thread::currentThread()->eventLoop()->setDispatcher(_platform.get());
    }

    GuiApplication::~GuiApplication() = default;

    auto GuiApplication::instance() -> GuiApplication*
    {
        return dynamic_cast<GuiApplication*>(CoreApplication::instance());
    }

    void GuiApplication::notifyTopLevelWindowClosed(Window* window)
    {
        if (!window)
        {
            return;
        }

        _topLevelWindows.erase(window);

        if (!_quitOnLastWindowClosed || !_topLevelWindows.empty())
        {
            return;
        }

        if (auto* currentThread = core::Thread::currentThread())
        {
            if (auto* loop = currentThread->eventLoop())
            {
                loop->quit();
            }
        }
    }

    void GuiApplication::registerTopLevelWindow(Window* window)
    {
        if (!window)
        {
            return;
        }

        _topLevelWindows.insert(window);
    }

    auto GuiApplication::screens() -> std::vector<Screen>
    {
        return platform::GuiApplicationPlatform::screens();
    }
} // namespace aw::gui
