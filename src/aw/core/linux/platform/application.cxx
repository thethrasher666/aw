//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "platform/application.hxx"

#include <cstdlib>

#include <xcb/xcb.h>

namespace aw::core::platform
{
    PlatformApplication::PlatformApplication() : _connection(xcb_connect(nullptr, nullptr))
    {
    }

    PlatformApplication::~PlatformApplication()
    {
        if (_connection)
        {
            xcb_disconnect(_connection);
            _connection = nullptr;
        }
    }

    auto PlatformApplication::processEvents(EventLoopFlags flags) -> bool
    {
        if (!_connection || xcb_connection_has_error(_connection) != 0)
        {
            return false;
        }

        if (flags == EventLoopFlags::WaitForMoreEvents)
        {
            // Blocking: wait for the next event from the X server.
            xcb_generic_event_t* event = xcb_wait_for_event(_connection);
            if (!event)
            {
                return false;
            }

            std::free(event);
            return true;
        }

        // Non-blocking: return immediately if there are no pending events.
        xcb_generic_event_t* event = xcb_poll_for_event(_connection);
        if (!event)
        {
            return true;
        }

        std::free(event);
        return true;
    }
} // namespace aw::core::platform
