//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "platform/application.hxx"
#include "aw/core/string.hxx"

#include <windows.h>

namespace aw::gui::platform
{
    namespace
    {
        BOOL monitorEnumProc(HMONITOR hMonitor, HDC, LPRECT, LPARAM lParam)
        {
            auto* monitors = reinterpret_cast<std::vector<Screen>*>(lParam);

            MONITORINFO geomInfo{};
            geomInfo.cbSize = sizeof(geomInfo);

            if (GetMonitorInfoW(hMonitor, &geomInfo))
            {
                MONITORINFOEXW nameInfo{};
                std::string    name;
                nameInfo.cbSize = sizeof(nameInfo);

                if (GetMonitorInfoW(hMonitor, &nameInfo))
                {
                    name = core::wideToNarrow(nameInfo.szDevice);
                }

                monitors->emplace_back(Screen(core::Rect(geomInfo.rcMonitor.left,
                                                         geomInfo.rcMonitor.top,
                                                         geomInfo.rcMonitor.right - geomInfo.rcMonitor.left, // width
                                                         geomInfo.rcMonitor.bottom - geomInfo.rcMonitor.top),
                                              name));
            }

            return TRUE; // Continue enumeration.
        }
    } // namespace

    GuiApplicationPlatform::GuiApplicationPlatform()
    {
        Gdiplus::GdiplusStartupInput gdiplusStartupInput;
        Gdiplus::GdiplusStartup(&_gdiplusToken, &gdiplusStartupInput, nullptr);
    }

    GuiApplicationPlatform::~GuiApplicationPlatform()
    {
        Gdiplus::GdiplusShutdown(_gdiplusToken);
    }

    auto GuiApplicationPlatform::processEvents(core::EventLoopFlags flags) -> bool
    {
        MSG msg{};

        if (flags == core::EventLoopFlags::WaitForMoreEvents)
        {
            // Blocking: wait for the next message.
            if (::GetMessage(&msg, nullptr, 0, 0) <= 0)
            {
                return false;
            }
        }
        else
        {
            // Non-blocking: return immediately if the queue is empty.
            if (!::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
            {
                return true;
            }

            if (msg.message == WM_QUIT)
            {
                return false;
            }
        }

        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
        return true;
    }

    auto GuiApplicationPlatform::screens() -> std::vector<Screen>
    {
        std::vector<Screen> screens;

        EnumDisplayMonitors(nullptr, nullptr, monitorEnumProc, reinterpret_cast<LPARAM>(&screens));

        return screens;
    }
} // namespace aw::gui::platform
