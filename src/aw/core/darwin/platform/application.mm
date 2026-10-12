//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "platform/application.hxx"

#include <aw/coreFoundation/CoreFoundation.h>

namespace aw::core::platform
{
    PlatformApplication::PlatformApplication()
    {
    }

    auto PlatformApplication::processEvents(EventLoopFlags flags) -> bool
    {
        CFRunLoopRunResult result;

        if (flags == EventLoopFlags::WaitForMoreEvents)
        {
            // Blocking: wait for at least one source to fire.
            result = CFRunLoopRunInMode(kCFRunLoopDefaultMode, 1e308, true);
        }
        else
        {
            // Non-blocking: drain all pending sources without blocking.
            result = CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0, false);
        }

        return result != kCFRunLoopRunStopped;
    }
} // namespace aw::core::platform
