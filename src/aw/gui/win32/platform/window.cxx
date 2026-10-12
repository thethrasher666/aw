//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "window.hxx"
#include "aw/core/string.hxx"

#include <objidl.h>

#include <vector>
#include <gdiplus.h>
#include <windows.h>

namespace aw::gui::platform
{
    namespace
    {
        constexpr wchar_t WINDOW_CLASS_NAME[] = L"aw.Window";
    } // namespace

    namespace
    {
        struct NativeStyle
        {
            DWORD style{};
            DWORD exStyle{};
        };

        // Builds an icon from already-decoded RGBA pixel data (as produced by QoiImage), not an encoded file.
        auto createIconFromMemory(uint8_t const* rgba, int32_t srcWidth, int32_t srcHeight, int32_t width, int32_t height) -> HICON
        {
            // GDI+ 32bppARGB stores pixels as B,G,R,A in memory; the decoded QOI data is R,G,B,A.
            std::vector<uint8_t> bgra(static_cast<size_t>(srcWidth) * static_cast<size_t>(srcHeight) * 4);

            for (size_t i = 0; i < bgra.size(); i += 4)
            {
                bgra[i + 0] = rgba[i + 2];
                bgra[i + 1] = rgba[i + 1];
                bgra[i + 2] = rgba[i + 0];
                bgra[i + 3] = rgba[i + 3];
            }

            Gdiplus::Bitmap bitmap(srcWidth, srcHeight, srcWidth * 4, PixelFormat32bppARGB, bgra.data());

            HICON hIcon = nullptr;

            if (bitmap.GetLastStatus() == Gdiplus::Ok)
            {
                Gdiplus::Bitmap resized(width, height, PixelFormat32bppARGB);

                Gdiplus::Graphics graphics(&resized);
                graphics.DrawImage(&bitmap, 0, 0, width, height);

                resized.GetHICON(&hIcon);
            }

            return hIcon;
        }

        auto mapStyles(WindowFlags flags, HWND parentHandle) -> NativeStyle
        {
            if (parentHandle || flags == WindowFlags::Widget)
            {
                return { WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 0 };
            }

            if (flags == WindowFlags::Dialog)
            {
                return { WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_CLIPCHILDREN, WS_EX_DLGMODALFRAME };
            }

            return { WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, 0 };
        }
    } // namespace

    PlatformWindow::PlatformWindow(Window* owner, PlatformWindow* parent, WindowFlags flags) : _owner(owner)
    {
        HINSTANCE const   instance = ::GetModuleHandleW(nullptr);
        HWND const        parentHandle = parent ? parent->_nativeHandle : nullptr;
        NativeStyle const nativeStyle = mapStyles(flags, parentHandle);

        WNDCLASSEXW windowClass{};
        windowClass.cbSize = sizeof(WNDCLASSEXW);

        if (::GetClassInfoExW(instance, WINDOW_CLASS_NAME, &windowClass) == 0)
        {
            windowClass.cbSize = sizeof(WNDCLASSEXW);
            windowClass.style = CS_HREDRAW | CS_VREDRAW;
            windowClass.lpfnWndProc = &PlatformWindow::windowProcedure;
            windowClass.hInstance = instance;
            windowClass.hCursor = ::LoadCursorW(nullptr, MAKEINTRESOURCEW(32512));
            windowClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
            windowClass.lpszClassName = WINDOW_CLASS_NAME;

            if (::RegisterClassExW(&windowClass) == 0)
            {
                return;
            }
        }

        _nativeHandle = ::CreateWindowExW(nativeStyle.exStyle, // Extended window style
                                          WINDOW_CLASS_NAME,   // Class name
                                          L"",                 // Window title
                                          nativeStyle.style,   // Window style
                                          CW_USEDEFAULT,       // Initial X position
                                          CW_USEDEFAULT,       // Initial Y position
                                          1024,                // Initial width
                                          768,                 // Initial height
                                          parentHandle,        // Parent window handle
                                          nullptr,             // Menu handle
                                          instance,            // Instance handle
                                          this);

        if (_nativeHandle)
        {
            ::SetWindowLongPtrW(_nativeHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
        }
    }

    PlatformWindow::~PlatformWindow()
    {
        if (_nativeHandle)
        {
            ::DestroyWindow(_nativeHandle);
            _nativeHandle = nullptr;
        }
    }

    auto PlatformWindow::geometry() const -> core::Rect
    {
        if (!_nativeHandle)
        {
            return {};
        }

        RECT rect;
        ::GetWindowRect(_nativeHandle, &rect);
        return core::Rect{ rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top };
    }

    auto PlatformWindow::height() const -> int32_t
    {
        return geometry().height();
    }

    void PlatformWindow::hide()
    {
        if (_nativeHandle)
        {
            ::ShowWindow(_nativeHandle, SW_HIDE);
        }
    }

    void PlatformWindow::setGeometry(core::Rect const& geometry)
    {
        if (!_nativeHandle)
        {
            return;
        }

        ::SetWindowPos(_nativeHandle, nullptr, geometry.x(), geometry.y(), geometry.width(), geometry.height(), SWP_NOZORDER | SWP_NOACTIVATE);
    }

    void PlatformWindow::setIcon(Icon const& icon)
    {
        auto platformIcon = createIconFromMemory(icon.data(), static_cast<int32_t>(icon.width()), static_cast<int32_t>(icon.height()), 32, 32);
        ::SendMessageW(_nativeHandle, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(platformIcon));
    }

    void PlatformWindow::setTitle(std::string const& title)
    {
        std::wstring const wideTitle = core::narrowToWide(title);

        if (_nativeHandle)
        {
            ::SetWindowTextW(_nativeHandle, wideTitle.c_str());
        }
    }

    void PlatformWindow::setWindowState(WindowState state)
    {
        if (!_nativeHandle)
        {
            return;
        }

        switch (state)
        {
        case WindowState::Minimized:
            ::ShowWindow(_nativeHandle, SW_MINIMIZE);
            break;

        case WindowState::Maximized:
            ::ShowWindow(_nativeHandle, SW_MAXIMIZE);
            break;

        case WindowState::FullScreen:
            // Placeholder until fullscreen style switching is implemented.
            ::ShowWindow(_nativeHandle, SW_MAXIMIZE);
            break;

        case WindowState::Normal:
        default:
            ::ShowWindow(_nativeHandle, SW_RESTORE);
            break;
        }
    }

    void PlatformWindow::show()
    {
        if (_nativeHandle)
        {
            ::ShowWindow(_nativeHandle, SW_SHOW);
            ::UpdateWindow(_nativeHandle);
        }
    }

    auto PlatformWindow::width() const -> int32_t
    {
        return geometry().width();
    }

    LRESULT PlatformWindow::windowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        auto* self = reinterpret_cast<PlatformWindow*>(::GetWindowLongPtrW(hwnd, GWLP_USERDATA));

        if (self)
        {
            switch (message)
            {
            case WM_CLOSE:
                if (!self->_owner || self->_owner->onCloseRequested())
                {
                    ::DestroyWindow(hwnd);
                }
                return 0;

            case WM_DESTROY:
                self->_nativeHandle = nullptr;
                if (self->_owner)
                {
                    self->_owner->onNativeWindowDestroyed();
                }
                return 0;

            case WM_MOVE:
            case WM_SIZE:
                if (self->_owner)
                {
                    self->_owner->onGeometryChanged(self->geometry());
                }
                return 0;

            case WM_COMMAND:
                self->_owner->onCommand(LOWORD(wParam));
                break;
            }
        }
        else if (message == WM_NCCREATE)
        {
            auto* pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
            self = reinterpret_cast<PlatformWindow*>(pCreate->lpCreateParams);

            if (self)
            {
                self->_nativeHandle = hwnd;
                ::SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
            }
        }

        return ::DefWindowProcW(hwnd, message, wParam, lParam);
    }
} // namespace aw::gui::platform
