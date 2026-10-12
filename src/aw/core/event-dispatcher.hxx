//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <aw/core/api.hxx>

namespace aw::core
{
    /// Flags that can be used to control the behaviour of an event loop.
    enum class EventLoopFlags
    {
        AllEvents,                  ///< Process all events in the queue.
        WaitForMoreEvents = 1 << 0, ///< Wait for more events if the queue is empty.
    };

    /// Abstract interface for processing native OS events on behalf of an EventLoop.
    class EventDispatcher
    {
    public:
        AW_CORE_API virtual ~EventDispatcher() = default;

        /// Process pending events for the owning event loop.
        /// \param flags Controls whether to block waiting for events.
        /// \return true if the loop should continue running, false to stop.
        virtual auto processEvents(EventLoopFlags flags) -> bool = 0;
    };
} // namespace aw::core
