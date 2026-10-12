//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "platform/application.hxx"

#import <AppKit/AppKit.h>

@interface AppDelegate : NSObject <NSApplicationDelegate>
@property(assign) BOOL shouldQuit;
@end

@implementation AppDelegate

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication*)sender
{
    // Cancel OS-level termination and let the C++ event loop unwind cleanly.
    _shouldQuit = YES;
    return NSTerminateCancel;
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)sender
{
    return YES;
}

@end

namespace
{
    AppDelegate* g_delegate = nil;
}

namespace aw::gui::platform
{
    GuiApplicationPlatform::GuiApplicationPlatform()
    {
        [NSApplication sharedApplication];
        [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];

        g_delegate = [[AppDelegate alloc] init];
        [NSApp setDelegate:g_delegate];

        [NSApp finishLaunching];
    }

    GuiApplicationPlatform::~GuiApplicationPlatform()
    {
        [NSApp setDelegate:nil];
        g_delegate = nil;
    }

    auto GuiApplicationPlatform::processEvents(EventLoopFlags flags) -> bool
    {
        if ([g_delegate shouldQuit])
        {
            return false;
        }

        NSDate* const deadline = (flags == EventLoopFlags::WaitForMoreEvents) ? [NSDate distantFuture] : [NSDate distantPast];

        NSEvent* const event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                                 untilDate:deadline
                                                    inMode:NSDefaultRunLoopMode
                                                   dequeue:YES];
        if (event)
        {
            [NSApp sendEvent:event];
            [NSApp updateWindows];
        }

        return ![g_delegate shouldQuit];
    }

    auto GuiApplicationPlatform::screens() -> std::vector<Screen>
    {
        std::vector<Screen> screens;

        for (NSScreen* const screen : [NSScreen screens])
        {
            const NSRect frame = [screen frame];

            std::string name;
            if (NSString* const localizedName = [screen localizedName])
            {
                if (const char* const utf8Name = [localizedName UTF8String])
                {
                    name = utf8Name;
                }
            }

            screens.emplace_back(Screen(Rect(static_cast<int32_t>(frame.origin.x),
                                             static_cast<int32_t>(frame.origin.y),
                                             static_cast<int32_t>(frame.size.width),
                                             static_cast<int32_t>(frame.size.height)),
                                       name));
        }

        return screens;
    }
} // namespace aw::gui::platform
