//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "application.hxx"
#include "window.hxx"

#include <cstdlib>

#include <xcb/xcb.h>

namespace aw::gui::platform
{
    namespace
    {
        xcb_connection_t* g_connection = nullptr;
    }

    GuiApplicationPlatform::GuiApplicationPlatform() : _connection(xcb_connect(nullptr, nullptr))
    {
        g_connection = _connection;
    }

    GuiApplicationPlatform::~GuiApplicationPlatform()
    {
        xcb_connection_t* oldConnection = _connection;

        if (_connection)
        {
            xcb_disconnect(_connection);
            _connection = nullptr;
        }

        if (g_connection == oldConnection)
        {
            g_connection = nullptr;
        }
    }

    auto GuiApplicationPlatform::connection() -> xcb_connection_t*
    {
        return g_connection;
    }

    auto GuiApplicationPlatform::processEvents(EventLoopFlags flags) -> bool
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

            [[maybe_unused]] bool const handled = PlatformWindow::dispatchEvent(_connection, event);
            std::free(event);
            return true;
        }

        // Non-blocking: return immediately if there are no pending events.
        xcb_generic_event_t* event = xcb_poll_for_event(_connection);
        if (!event)
        {
            return true;
        }

        [[maybe_unused]] bool const handled = PlatformWindow::dispatchEvent(_connection, event);
        std::free(event);
        return true;
    }

    auto GuiApplicationPlatform::screens() -> std::vector<Screen>
    {
        std::vector<Screen> screens;

        xcb_connection_t* connection = xcb_connect(nullptr, nullptr);
        if (!connection || xcb_connection_has_error(connection) != 0)
        {
            if (connection)
            {
                xcb_disconnect(connection);
            }

            return screens;
        }

        xcb_setup_t const*    setup = xcb_get_setup(connection);
        xcb_screen_iterator_t screenIterator = xcb_setup_roots_iterator(setup);

        for (; screenIterator.rem != 0; xcb_screen_next(&screenIterator))
        {
            xcb_screen_t* root = screenIterator.data;
            if (!root)
            {
                continue;
            }

            screens.emplace_back(Screen(Rect(0, 0, root->width_in_pixels, root->height_in_pixels), "screen"));
        }

        xcb_disconnect(connection);
        return screens;
    }
} // namespace aw::gui::platform
