//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "aw/core/event-loop.hxx"

namespace aw::core
{
    EventLoop::EventLoop(Object* parent) : Object(parent)
    {
        setObjectName("EventLoop");
    }

    auto EventLoop::processEvents(EventLoopFlags flags) -> bool
    {
        if (_running.load(std::memory_order_relaxed) && _dispatcher)
        {
            if (!_dispatcher->processEvents(flags))
            {
                _running.store(false, std::memory_order_relaxed);
            }
        }

        return _running.load(std::memory_order_relaxed);
    }

    void EventLoop::quit()
    {
        _running.store(false, std::memory_order_relaxed);
    }

    void EventLoop::setDispatcher(EventDispatcher* dispatcher)
    {
        _dispatcher = dispatcher;
    }
} // namespace aw::core
