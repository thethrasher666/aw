//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "platform/application.hxx"

#include <Windows.h>

namespace aw::core::platform
{
    PlatformApplication::PlatformApplication()
    {
    }

    auto PlatformApplication::processEvents(EventLoopFlags flags) -> bool
    {
        MSG msg{};

        if (flags == EventLoopFlags::WaitForMoreEvents)
        {
            // Blocking: wait for the next message.
            if (::GetMessageW(&msg, nullptr, 0, 0) <= 0)
            {
                return false;
            }
        }
        else
        {
            // Non-blocking: return immediately if the queue is empty.
            if (!::PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
            {
                return true;
            }

            if (msg.message == WM_QUIT)
            {
                return false;
            }
        }

        ::TranslateMessage(&msg);
        ::DispatchMessageW(&msg);
        return true;
    }
} // namespace aw::core::platform
