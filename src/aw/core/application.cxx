//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "aw/core/application.hxx"
#include "platform/application.hxx"

#include <atomic>
#include <csignal>
#include <stdexcept>

namespace aw::core
{
    namespace
    {
        std::atomic<aw::core::CoreApplication*> g_coreApplicationInstance = nullptr;
        std::atomic<bool>                       g_signalReceived{ false };

        void handleSignal(int) noexcept
        {
            g_signalReceived.store(true, std::memory_order_relaxed);
        }
    } // namespace

    CoreApplication::CoreApplication(std::vector<std::string> const& arguments) : _arguments(arguments), _platform(std::make_unique<platform::PlatformApplication>())
    {
        CoreApplication* expected = nullptr;

        setObjectName("CoreApplication");

        // Enforce single-instance semantics and publish the instance safely.
        if (!g_coreApplicationInstance.compare_exchange_strong(expected, this, std::memory_order_release, std::memory_order_relaxed))
        {
            throw std::logic_error("CoreApplication instance already exists");
        }

        std::signal(SIGINT, handleSignal);
        std::signal(SIGTERM, handleSignal);

        _thread = createChild<Thread>(this);
        _thread->eventLoop()->setDispatcher(_platform.get());
    }

    CoreApplication::~CoreApplication()
    {
        std::signal(SIGTERM, SIG_DFL);
        std::signal(SIGINT, SIG_DFL);

        CoreApplication* expected = this;
        g_coreApplicationInstance.compare_exchange_strong(expected, nullptr, std::memory_order_release, std::memory_order_relaxed);
    }

    auto CoreApplication::instance() -> CoreApplication*
    {
        return g_coreApplicationInstance.load(std::memory_order_acquire);
    }

    auto CoreApplication::run() -> bool
    {
        onBeforeRun();

        auto* loop = _thread->eventLoop();

        while (loop->processEvents(EventLoopFlags::AllEvents))
        {
            if (g_signalReceived.load(std::memory_order_relaxed))
            {
                loop->quit();
                continue;
            }

            if (_continuousUpdate)
            {
                // Never block: the frame itself paces the loop through the swapchain's present mode.
                onFrame();
                continue;
            }

            // Avoid busy-spinning when idle by blocking until the next native event arrives.
            if (!loop->processEvents(EventLoopFlags::WaitForMoreEvents))
            {
                break;
            }

            if (g_signalReceived.load(std::memory_order_relaxed))
            {
                loop->quit();
            }
        }

        onAfterRun();
        return true;
    }

    void CoreApplication::quit()
    {
        _thread->eventLoop()->quit();
    }
} // namespace aw::core
