//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "window.hxx"
#include "application.hxx"

#include <algorithm>
#include <array>
#include <cstdlib>
#include <cstring>
#include <string>
#include <unordered_map>

#include <xcb/xcb.h>

namespace aw::gui::platform
{
    namespace
    {
        std::unordered_map<xcb_window_t, PlatformWindow*> g_windows;

        auto internAtom(xcb_connection_t* connection, char const* name) -> xcb_atom_t
        {
            if (!connection || !name)
            {
                return XCB_ATOM_NONE;
            }

            auto  cookie = xcb_intern_atom(connection, 0, std::strlen(name), name);
            auto* reply = xcb_intern_atom_reply(connection, cookie, nullptr);
            if (!reply)
            {
                return XCB_ATOM_NONE;
            }

            xcb_atom_t const atom = reply->atom;
            std::free(reply);
            return atom;
        }

        auto firstScreen(xcb_connection_t* connection) -> xcb_screen_t*
        {
            if (!connection)
            {
                return nullptr;
            }

            auto const* setup = xcb_get_setup(connection);
            auto        iterator = xcb_setup_roots_iterator(setup);
            return iterator.data;
        }

        auto sendNetWmState(xcb_connection_t* connection, xcb_window_t window, xcb_atom_t stateAtom, xcb_atom_t propertyAtom, uint32_t action) -> void
        {
            if (!connection || window == XCB_NONE || stateAtom == XCB_ATOM_NONE || propertyAtom == XCB_ATOM_NONE)
            {
                return;
            }

            xcb_screen_t* screen = firstScreen(connection);
            if (!screen)
            {
                return;
            }

            xcb_client_message_event_t event{};
            event.response_type = XCB_CLIENT_MESSAGE;
            event.window = window;
            event.type = propertyAtom;
            event.format = 32;
            event.data.data32[0] = action;
            event.data.data32[1] = stateAtom;
            event.data.data32[2] = XCB_ATOM_NONE;
            event.data.data32[3] = 1;
            event.data.data32[4] = 0;

            xcb_send_event(connection, 0, screen->root, XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT | XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY, reinterpret_cast<char const*>(&event));
            xcb_flush(connection);
        }
    } // namespace

    PlatformWindow::PlatformWindow(Window* owner, PlatformWindow* parent, WindowFlags flags) : _owner(owner)
    {
        _connection = GuiApplicationPlatform::connection();
        if (!_connection || xcb_connection_has_error(_connection) != 0)
        {
            return;
        }

        _wmProtocols = internAtom(_connection, "WM_PROTOCOLS");
        _wmDeleteWindow = internAtom(_connection, "WM_DELETE_WINDOW");
        _netWmState = internAtom(_connection, "_NET_WM_STATE");
        _netWmStateMaximizedHoriz = internAtom(_connection, "_NET_WM_STATE_MAXIMIZED_HORZ");
        _netWmStateMaximizedVert = internAtom(_connection, "_NET_WM_STATE_MAXIMIZED_VERT");
        _netWmStateFullscreen = internAtom(_connection, "_NET_WM_STATE_FULLSCREEN");

        xcb_screen_t* screen = firstScreen(_connection);
        if (!screen)
        {
            return;
        }

        _nativeHandle = xcb_generate_id(_connection);

        xcb_window_t const parentHandle = parent ? parent->_nativeHandle : screen->root;

        uint32_t const eventMask = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_STRUCTURE_NOTIFY;
        uint32_t const values[] = { screen->white_pixel, eventMask };

        uint16_t width = 1024;
        uint16_t height = 768;
        if (flags == WindowFlags::Widget)
        {
            width = 320;
            height = 240;
        }

        xcb_create_window(_connection,
                          XCB_COPY_FROM_PARENT,
                          _nativeHandle,
                          parentHandle,
                          0,
                          0,
                          width,
                          height,
                          0,
                          XCB_WINDOW_CLASS_INPUT_OUTPUT,
                          XCB_COPY_FROM_PARENT,
                          XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK,
                          values);

        if (_wmProtocols != XCB_ATOM_NONE && _wmDeleteWindow != XCB_ATOM_NONE)
        {
            xcb_change_property(_connection, XCB_PROP_MODE_REPLACE, _nativeHandle, _wmProtocols, XCB_ATOM_ATOM, 32, 1, &_wmDeleteWindow);
        }

        xcb_flush(_connection);
        g_windows[_nativeHandle] = this;
    }

    PlatformWindow::~PlatformWindow()
    {
        if (_connection && _nativeHandle != XCB_NONE)
        {
            g_windows.erase(_nativeHandle);
            xcb_destroy_window(_connection, _nativeHandle);
            xcb_flush(_connection);
            _nativeHandle = XCB_NONE;
        }
    }

    auto PlatformWindow::geometry() const -> Rect
    {
        if (!_connection || _nativeHandle == XCB_NONE)
        {
            return {};
        }

        auto  cookie = xcb_get_geometry(_connection, _nativeHandle);
        auto* reply = xcb_get_geometry_reply(_connection, cookie, nullptr);
        if (!reply)
        {
            return {};
        }

        Rect const result(reply->x, reply->y, reply->width, reply->height);
        std::free(reply);
        return result;
    }

    auto PlatformWindow::height() const -> int32_t
    {
        return geometry().height();
    }

    void PlatformWindow::hide()
    {
        if (!_connection || _nativeHandle == XCB_NONE)
        {
            return;
        }

        xcb_unmap_window(_connection, _nativeHandle);
        xcb_flush(_connection);
    }

    void PlatformWindow::setGeometry(Rect const& geometry)
    {
        if (!_connection || _nativeHandle == XCB_NONE)
        {
            return;
        }

        // X rejects zero-sized windows with BadValue.
        uint32_t const values[] = { static_cast<uint32_t>(geometry.x()),
                                    static_cast<uint32_t>(geometry.y()),
                                    static_cast<uint32_t>(std::max(geometry.width(), 1)),
                                    static_cast<uint32_t>(std::max(geometry.height(), 1)) };

        xcb_configure_window(_connection, _nativeHandle, XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y | XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT, values);
        xcb_flush(_connection);
    }

    void PlatformWindow::setIcon(Icon const&)
    {
    }

    void PlatformWindow::setTitle(std::string const& title)
    {
        if (!_connection || _nativeHandle == XCB_NONE)
        {
            return;
        }

        xcb_atom_t const utf8String = internAtom(_connection, "UTF8_STRING");
        xcb_atom_t const netWmName = internAtom(_connection, "_NET_WM_NAME");

        xcb_change_property(_connection, XCB_PROP_MODE_REPLACE, _nativeHandle, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, static_cast<uint32_t>(title.size()), title.c_str());

        if (utf8String != XCB_ATOM_NONE && netWmName != XCB_ATOM_NONE)
        {
            xcb_change_property(_connection, XCB_PROP_MODE_REPLACE, _nativeHandle, netWmName, utf8String, 8, static_cast<uint32_t>(title.size()), title.c_str());
        }

        xcb_flush(_connection);
    }

    void PlatformWindow::setWindowState(WindowState state)
    {
        if (!_connection || _nativeHandle == XCB_NONE)
        {
            return;
        }

        switch (state)
        {
        case WindowState::Minimized:
            xcb_unmap_window(_connection, _nativeHandle);
            break;

        case WindowState::Maximized:
            show();
            sendNetWmState(_connection, _nativeHandle, _netWmStateMaximizedHoriz, _netWmState, 1);
            sendNetWmState(_connection, _nativeHandle, _netWmStateMaximizedVert, _netWmState, 1);
            break;

        case WindowState::FullScreen:
            show();
            sendNetWmState(_connection, _nativeHandle, _netWmStateFullscreen, _netWmState, 1);
            break;

        case WindowState::Normal:
        default:
            show();
            sendNetWmState(_connection, _nativeHandle, _netWmStateMaximizedHoriz, _netWmState, 0);
            sendNetWmState(_connection, _nativeHandle, _netWmStateMaximizedVert, _netWmState, 0);
            sendNetWmState(_connection, _nativeHandle, _netWmStateFullscreen, _netWmState, 0);
            break;
        }

        xcb_flush(_connection);
    }

    void PlatformWindow::show()
    {
        if (!_connection || _nativeHandle == XCB_NONE)
        {
            return;
        }

        xcb_map_window(_connection, _nativeHandle);
        xcb_flush(_connection);
    }

    auto PlatformWindow::width() const -> int32_t
    {
        return geometry().width();
    }

    auto PlatformWindow::dispatchEvent(xcb_connection_t* connection, xcb_generic_event_t const* event) -> bool
    {
        if (!connection || !event)
        {
            return false;
        }

        uint8_t const responseType = event->response_type & ~0x80u;

        if (responseType == XCB_CLIENT_MESSAGE)
        {
            auto const* clientMessage = reinterpret_cast<xcb_client_message_event_t const*>(event);
            auto        iterator = g_windows.find(clientMessage->window);
            if (iterator == g_windows.end())
            {
                return false;
            }

            PlatformWindow* self = iterator->second;
            if (clientMessage->type == self->_wmProtocols && static_cast<xcb_atom_t>(clientMessage->data.data32[0]) == self->_wmDeleteWindow)
            {
                if (!self->_owner || self->_owner->onCloseRequested())
                {
                    xcb_destroy_window(connection, self->_nativeHandle);
                    xcb_flush(connection);
                }

                return true;
            }

            return false;
        }

        if (responseType == XCB_DESTROY_NOTIFY)
        {
            auto const* destroy = reinterpret_cast<xcb_destroy_notify_event_t const*>(event);
            auto        iterator = g_windows.find(destroy->window);
            if (iterator == g_windows.end())
            {
                return false;
            }

            PlatformWindow* self = iterator->second;
            self->_nativeHandle = XCB_NONE;
            g_windows.erase(iterator);

            if (self->_owner)
            {
                self->_owner->onNativeWindowDestroyed();
            }

            return true;
        }

        if (responseType == XCB_CONFIGURE_NOTIFY)
        {
            auto const* configure = reinterpret_cast<xcb_configure_notify_event_t const*>(event);
            auto        iterator = g_windows.find(configure->window);
            if (iterator == g_windows.end())
            {
                return false;
            }

            PlatformWindow* self = iterator->second;
            if (self->_owner)
            {
                self->_owner->onGeometryChanged(Rect(configure->x, configure->y, configure->width, configure->height));
            }

            return true;
        }

        return false;
    }
} // namespace aw::gui::platform
