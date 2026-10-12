//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <aw/core/event-loop.hxx>
#include <aw/core/object.hxx>

#include <thread>

namespace aw::core
{
    /// A Thread object represents a single OS thread and owns an EventLoop object for that thread.
    class Thread : public Object
    {
    public:
        /// Constructor.
        /// \param parent The parent object of this object.
        AW_CORE_API explicit Thread(Object* parent = nullptr);

        /// Destructor.
        AW_CORE_API virtual ~Thread() = default;

        /// Get the Thread object for the calling OS thread, or nullptr if unregistered.
        [[nodiscard]] AW_CORE_API static auto currentThread() -> Thread*;

        /// Get the event loop owned by this thread.
        [[nodiscard]] AW_CORE_API auto eventLoop() -> EventLoop*;

    private:
        EventLoop*  _eventLoop{}; ///< The event loop, owned as a child of this object.
        std::thread _thread;
    };
} // namespace aw::core
