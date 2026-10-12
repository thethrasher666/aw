//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "platform/window.hxx"

#import <AppKit/AppKit.h>
#import <QuartzCore/CAMetalLayer.h>

namespace aw::core
{
    class PlatformWindow;
}

@interface AWWindowDelegate : NSObject <NSWindowDelegate>
- (instancetype)initWithPlatformWindow:(aw::PlatformWindow*)platformWindow;
@end

/// Layer-backed view whose backing layer is a CAMetalLayer, so a rendering surface can be created
/// from a plain aw::Window without the caller touching AppKit.
@interface AWMetalView : NSView
- (void)synchroniseDrawableSize;
@end

namespace aw::gui::platform
{
    namespace
    {
        struct NativeStyle
        {
            NSWindowStyleMask styleMask{};
        };

        auto map_styles(WindowFlags flags, NSWindow* parentWindow) -> NativeStyle
        {
            if (parentWindow || flags == WindowFlags::Widget)
            {
                return { NSWindowStyleMaskBorderless };
            }

            if (flags == WindowFlags::Dialog)
            {
                return { NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskResizable };
            }

            return { NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable };
        }

        /// The area a child window is positioned within, in Cocoa screen coordinates (bottom-left, y up).
        auto content_rect(NSWindow* window) -> NSRect
        {
            return [window contentRectForFrameRect:[window frame]];
        }
    } // namespace

    PlatformWindow::PlatformWindow(Window* owner, PlatformWindow* parent, WindowFlags flags) : _owner(owner), _parent(parent)
    {
        NSWindow* const parentWindow = parent ? (__bridge NSWindow*) parent->_nativeWindow : nil;
        NativeStyle const nativeStyle = map_styles(flags, parentWindow);

        NSRect const initialFrame = NSMakeRect(0.0, 0.0, 1024.0, 768.0);
        NSWindow* const nativeWindow = [[NSWindow alloc] initWithContentRect:initialFrame
                                                                    styleMask:nativeStyle.styleMask
                                                                      backing:NSBackingStoreBuffered
                                                                        defer:NO];
        if (!nativeWindow)
        {
            return;
        }

        [nativeWindow setTitle:@"Application"];

        AWWindowDelegate* const delegate = [[AWWindowDelegate alloc] initWithPlatformWindow:this];
        [nativeWindow setDelegate:delegate];

        if (parentWindow)
        {
            [parentWindow addChildWindow:nativeWindow ordered:NSWindowAbove];
        }

        _nativeWindow = (__bridge void*) nativeWindow;
        _nativeDelegate = (__bridge void*) delegate;
        notifyGeometryChanged();
    }

    PlatformWindow::~PlatformWindow()
    {
        NSWindow* const nativeWindow = (__bridge NSWindow*) _nativeWindow;

        if (nativeWindow)
        {
            [nativeWindow setDelegate:nil];
            [nativeWindow close];
        }


        _nativeWindow = nullptr;
        _nativeDelegate = nullptr;
    }

    auto PlatformWindow::geometry() const -> Rect
    {
        NSWindow* const nativeWindow = (__bridge NSWindow*) _nativeWindow;

        if (!nativeWindow)
        {
            return {};
        }

        NSRect const    frame = [nativeWindow frame];
        NSWindow* const parentWindow = _parent ? (__bridge NSWindow*) _parent->_nativeWindow : nil;

        if (parentWindow)
        {
            // Child windows are separate NSWindows, so convert back to the parent-relative,
            // top-left-origin coordinates the platform-agnostic layer expects.
            NSRect const parentContent = content_rect(parentWindow);
            return Rect{ static_cast<int32_t>(frame.origin.x - parentContent.origin.x),
                         static_cast<int32_t>(NSMaxY(parentContent) - NSMaxY(frame)),
                         static_cast<int32_t>(frame.size.width),
                         static_cast<int32_t>(frame.size.height) };
        }

        // The content rect, so that a layout sizes its children to the client area only.
        NSRect const content = content_rect(nativeWindow);
        return Rect{ static_cast<int32_t>(content.origin.x),
                     static_cast<int32_t>(content.origin.y),
                     static_cast<int32_t>(content.size.width),
                     static_cast<int32_t>(content.size.height) };
    }

    auto PlatformWindow::height() const -> int32_t
    {
        return geometry().height();
    }

    void PlatformWindow::hide()
    {
        NSWindow* const nativeWindow = (__bridge NSWindow*) _nativeWindow;

        if (nativeWindow)
        {
            [nativeWindow orderOut:nil];
        }
    }

    auto PlatformWindow::nativeHandle() const -> void*
    {
        return (__bridge NSWindow*) _nativeWindow;
    }

    auto PlatformWindow::nativeSurfaceHandle() -> void*
    {
        if (_metalLayer)
        {
            return _metalLayer;
        }

        NSWindow* const nativeWindow = (__bridge NSWindow*) _nativeWindow;

        if (!nativeWindow)
        {
            return nullptr;
        }

        AWMetalView* const view = [[AWMetalView alloc] initWithFrame:[[nativeWindow contentView] bounds]];

        if (!view)
        {
            return nullptr;
        }

        [view setWantsLayer:YES];
        [view setAutoresizingMask:NSViewWidthSizable | NSViewHeightSizable];
        [nativeWindow setContentView:view];
        [view synchroniseDrawableSize];

        _metalLayer = (__bridge void*) [view layer];

        // The window owns the content view now.
        [view release];

        return _metalLayer;
    }

    void PlatformWindow::setGeometry(Rect const& geometry)
    {
        NSWindow* const nativeWindow = (__bridge NSWindow*) _nativeWindow;

        if (!nativeWindow)
        {
            return;
        }

        // Matches geometry(): the frame origin is in Cocoa screen coordinates (bottom-left, y up).
        NSRect frame = NSMakeRect(static_cast<CGFloat>(geometry.x()),
                                  static_cast<CGFloat>(geometry.y()),
                                  static_cast<CGFloat>(geometry.width()),
                                  static_cast<CGFloat>(geometry.height()));

        if (NSWindow* const parentWindow = _parent ? (__bridge NSWindow*) _parent->_nativeWindow : nil)
        {
            // Child geometry arrives parent-relative with a top-left origin; place it inside the
            // parent's content area instead of at that point on the screen.
            NSRect const parentContent = content_rect(parentWindow);
            frame.origin.x = parentContent.origin.x + static_cast<CGFloat>(geometry.x());
            frame.origin.y = NSMaxY(parentContent) - static_cast<CGFloat>(geometry.y()) - frame.size.height;

            [nativeWindow setFrame:frame display:YES];
            return;
        }

        [nativeWindow setFrame:[nativeWindow frameRectForContentRect:frame] display:YES];
    }

    void PlatformWindow::setTitle(std::string const& title)
    {
        NSWindow* const nativeWindow = (__bridge NSWindow*) _nativeWindow;

        if (!nativeWindow)
        {
            return;
        }

        NSString* const text = [NSString stringWithUTF8String:title.c_str()];

        if (text)
        {
            [nativeWindow setTitle:text];
        }
    }

    void PlatformWindow::setWindowState(WindowState state)
    {
        NSWindow* const nativeWindow = (__bridge NSWindow*) _nativeWindow;

        if (!nativeWindow)
        {
            return;
        }

        switch (state)
        {
        case WindowState::Minimized:
            [nativeWindow miniaturize:nil];
            break;

        case WindowState::Maximized:
            if (![nativeWindow isZoomed])
            {
                [nativeWindow zoom:nil];
            }
            break;

        case WindowState::FullScreen:
            if (([nativeWindow styleMask] & NSWindowStyleMaskFullScreen) == 0)
            {
                [nativeWindow toggleFullScreen:nil];
            }
            break;

        case WindowState::Normal:
        default:
            if ([nativeWindow isMiniaturized])
            {
                [nativeWindow deminiaturize:nil];
            }
            if ([nativeWindow isZoomed])
            {
                [nativeWindow zoom:nil];
            }
            if (([nativeWindow styleMask] & NSWindowStyleMaskFullScreen) != 0)
            {
                [nativeWindow toggleFullScreen:nil];
            }
            break;
        }
    }

    void PlatformWindow::show()
    {
        NSWindow* const nativeWindow = (__bridge NSWindow*) _nativeWindow;

        if (nativeWindow)
        {
            [nativeWindow makeKeyAndOrderFront:nil];
            [NSApp activateIgnoringOtherApps:YES];
        }
    }

    auto PlatformWindow::width() const -> int32_t
    {
        return geometry().width();
    }

    void PlatformWindow::notifyGeometryChanged()
    {
        if (_owner)
        {
            _owner->onGeometryChanged(geometry());
        }
    }

    void PlatformWindow::notifyNativeWindowDestroyed()
    {
        if (_owner)
        {
            _owner->onNativeWindowDestroyed();
        }
    }

    auto PlatformWindow::notifyCloseRequested() -> bool
    {
        if (_owner)
        {
            return _owner->onCloseRequested();
        }

        return true;
    }

    void PlatformWindow::clearNativeWindow()
    {
        _nativeWindow = nullptr;
        _metalLayer = nullptr;
    }
} // namespace aw::gui::platform

@implementation AWMetalView

- (BOOL)wantsUpdateLayer
{
    return YES;
}

- (CALayer*)makeBackingLayer
{
    CAMetalLayer* const layer = [CAMetalLayer layer];
    [layer setNeedsDisplayOnBoundsChange:YES];
    return layer;
}

- (void)setFrameSize:(NSSize)newSize
{
    [super setFrameSize:newSize];
    [self synchroniseDrawableSize];
}

- (void)viewDidChangeBackingProperties
{
    [super viewDidChangeBackingProperties];
    [self synchroniseDrawableSize];
}

- (void)synchroniseDrawableSize
{
    CAMetalLayer* const layer = (CAMetalLayer*) [self layer];

    if (![layer isKindOfClass:[CAMetalLayer class]])
    {
        return;
    }

    CGFloat const scale = [self window] ? [[self window] backingScaleFactor] : 1.0;
    NSSize const  size = [self bounds].size;

    [layer setContentsScale:scale];
    [layer setDrawableSize:CGSizeMake(size.width * scale, size.height * scale)];
}

@end

@implementation AWWindowDelegate
{
    aw::PlatformWindow* _platformWindow;
}

- (instancetype)initWithPlatformWindow:(aw::PlatformWindow*)platformWindow
{
    self = [super init];

    if (self)
    {
        _platformWindow = platformWindow;
    }

    return self;
}

- (BOOL)windowShouldClose:(NSWindow*)sender
{
    if (!_platformWindow)
    {
        return YES;
    }

    return _platformWindow->notifyCloseRequested() ? YES : NO;
}

- (void)windowWillClose:(NSNotification*)notification
{
    if (_platformWindow)
    {
        _platformWindow->clearNativeWindow();
        _platformWindow->notifyNativeWindowDestroyed();
    }
}

- (void)windowDidMove:(NSNotification*)notification
{
    if (_platformWindow)
    {
        _platformWindow->notifyGeometryChanged();
    }
}

- (void)windowDidResize:(NSNotification*)notification
{
    if (_platformWindow)
    {
        _platformWindow->notifyGeometryChanged();
    }
}

@end
