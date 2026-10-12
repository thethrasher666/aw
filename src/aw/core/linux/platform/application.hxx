//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <aw/core/event-dispatcher.hxx>

#include <xcb/xcb.h>

namespace aw::core::platform
{
    /// The platform-specific implementation of the CoreApplication class.
    class PlatformApplication : public EventDispatcher
    {
    public:
        /// Constructor.
        explicit PlatformApplication();

        /// Destructor.
        ~PlatformApplication() override;

        /// Processes XCB events for the current thread.
        auto processEvents(EventLoopFlags flags) -> bool override;

    private:
        xcb_connection_t* _connection{};
    };
} // namespace aw::core::platform
