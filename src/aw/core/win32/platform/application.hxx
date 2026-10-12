//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <aw/core/event-dispatcher.hxx>

namespace aw::core::platform
{
    /// The platform-specific implementation of the CoreApplication class.
    class PlatformApplication : public EventDispatcher
    {
    public:
        /// Constructor.
        explicit PlatformApplication();

        /// Processes Win32 messages for the current thread.
        auto processEvents(EventLoopFlags flags) -> bool override;
    };
} // namespace aw::core::platform
