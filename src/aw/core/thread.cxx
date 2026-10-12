//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "aw/core/thread.hxx"

namespace aw::core
{
    namespace
    {
        thread_local Thread* t_currentThread = nullptr;
    }

    Thread::Thread(Object* parent) : Object(parent)
    {
        t_currentThread = this;
        setObjectName("Thread");

        _eventLoop = createChild<EventLoop>(this);
    }

    auto Thread::eventLoop() -> EventLoop*
    {
        return _eventLoop;
    }

    auto Thread::currentThread() -> Thread*
    {
        return t_currentThread;
    }
} // namespace aw::core
