//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <aw/core/event-dispatcher.hxx>
#include <aw/core/object.hxx>

#include <atomic>

namespace aw::core
{
    /// This provides a means of entering and leaving an event loop
    class EventLoop : public Object
    {
    public:
        /// Constructor.
        /// \param parent The parent object.
        AW_CORE_API explicit EventLoop(Object* parent = nullptr);

        /// Determines if the event loop is currently running.
        /// \return true if the event loop is running, false otherwise.
        [[nodiscard]] auto isRunning() const -> bool
        {
            return _running;
        }

        /// Processes events in the event loop.
        /// \param flags The flags that control the behaviour of the event loop.
        /// \return true if the event loop is still running, false otherwise.
        [[nodiscard]] AW_CORE_API auto processEvents(EventLoopFlags flags = EventLoopFlags::AllEvents) -> bool;

        /// Request the event loop to stop after the current iteration.
        AW_CORE_API void quit();

        /// Set the platform dispatcher used to process native OS events.
        AW_CORE_API void setDispatcher(EventDispatcher* dispatcher);

    private:
        EventDispatcher*  _dispatcher{};
        std::atomic<bool> _running{ true };
    };
} // namespace aw::core
