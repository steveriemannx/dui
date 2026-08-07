#import "dui/Core/SDL_MacOS.h"

#if defined(__APPLE__) && defined(__MACH__)

#import <SDL3/SDL.h>
#import <Cocoa/Cocoa.h>

namespace ui
{

void* GetSDLWindowContentView(SDL_Window* sdlWindow) 
{
    if(sdlWindow == nullptr) {
        return nullptr;
    }
    SDL_PropertiesID propID = ::SDL_GetWindowProperties(sdlWindow);
    NSWindow* pNSWindow = (NSWindow*)::SDL_GetPointerProperty(propID, SDL_PROP_WINDOW_COCOA_WINDOW_POINTER, nullptr);
    NSView* pNSView = nullptr;
    if (pNSWindow != nullptr) {
        pNSView = [pNSWindow contentView] ;
    }
    return (void*)pNSView;
}

bool SetFocus_MacOS(void* /*pNSWindow*/)
{
    //无需实现：如果用代码实现激活主窗口，则CEF在退出时有错误，导致进程无法正常退出
    return false;
}

bool ModifyNsWindowShadowType(void* pNSWindow, NativeWindowShadowType nativeShadowType)
{
    if (pNSWindow == nullptr) {
        return false;
    }
    // Must run on the main thread (AppKit)
    if (![NSThread isMainThread]) {
        __block bool result = false;
        dispatch_sync(dispatch_get_main_queue(), ^{
            result = ModifyNsWindowShadowType(pNSWindow, nativeShadowType);
        });
        return result;
    }

    NSWindow* window = (__bridge NSWindow*)pNSWindow;
    if (window == nullptr || ![window isKindOfClass:[NSWindow class]]) {
        return false;
    }

    NSView* contentView = window.contentView;
    if (contentView == nil) {
        if (nativeShadowType == NativeWindowShadowType::kShadowSystemDisabled) {
            [window setHasShadow:NO];
        }
        else {
            [window setHasShadow:YES];
        }
        [window invalidateShadow];
        return YES;
    }

    // Convert the SDL borderless window into a titled document window with the
    // title bar hidden: gives the OS rounded corners and a deep shadow.
    if (nativeShadowType != NativeWindowShadowType::kShadowSystemDisabled) {
        window.titlebarAppearsTransparent = YES;
        window.titleVisibility = NSWindowTitleHidden;
        NSWindowStyleMask requiredMask = NSWindowStyleMaskTitled |
                                         NSWindowStyleMaskFullSizeContentView |
                                         NSWindowStyleMaskMiniaturizable |
                                         NSWindowStyleMaskResizable;
        if ((window.styleMask & requiredMask) != requiredMask) {
            [window setStyleMask:requiredMask];
            [[window standardWindowButton:NSWindowCloseButton] setHidden:YES];
            [[window standardWindowButton:NSWindowMiniaturizeButton] setHidden:YES];
            [[window standardWindowButton:NSWindowZoomButton] setHidden:YES];
        }
    }

    switch (nativeShadowType) {
    case NativeWindowShadowType::kShadowSystemDisabled:
        [window setHasShadow:NO];
        if (contentView.wantsLayer) {
            CALayer* layer = contentView.layer;
            if (layer != nil) {
                layer.cornerRadius = 0;
                layer.masksToBounds = NO;
            }
        }
        break;
    case NativeWindowShadowType::kShadowSystemDefault:
    case NativeWindowShadowType::kShadowSystemRound:
        [window setHasShadow:YES];
        window.backgroundColor = [NSColor clearColor];
        break;
    case NativeWindowShadowType::kShadowSystemSmallRound: {
        [window setHasShadow:YES];
        contentView.wantsLayer = YES;
        CALayer* layer = contentView.layer;
        if (layer != nil) {
            layer.cornerRadius = 5.0;
            layer.masksToBounds = YES;
        }
        window.backgroundColor = [NSColor clearColor];
        break;
    }
    case NativeWindowShadowType::kShadowSystemDoNotRound: {
        [window setHasShadow:YES];
        if (contentView.wantsLayer) {
            CALayer* l2 = contentView.layer;
            if (l2 != nil) {
                l2.cornerRadius = 0;
                l2.masksToBounds = NO;
            }
        }
        window.backgroundColor = [NSColor clearColor];
        break;
    }
    default:
        break;
    }

    [window invalidateShadow];
    return YES;
}

void RestoreWindowShadowAfterFullscreen(void* pNSWindow, NativeWindowShadowType nativeShadowType)
{
    if (pNSWindow == nullptr) {
        return;
    }
    if (![NSThread isMainThread]) {
        dispatch_async(dispatch_get_main_queue(), ^{
            RestoreWindowShadowAfterFullscreen(pNSWindow, nativeShadowType);
        });
        return;
    }
    if (nativeShadowType == NativeWindowShadowType::kShadowSystemDisabled) {
        return;
    }
    NSWindow* window = (__bridge NSWindow*)pNSWindow;
    if (window == nullptr) {
        return;
    }
    // Toggle the title mask to force WindowServer to re-composite the window
    // so the rounded corners come back after fullscreen.
    NSWindowStyleMask mask = window.styleMask;
    window.styleMask = mask & ~NSWindowStyleMaskTitled;
    window.styleMask = mask;
    [window invalidateShadow];
}

} // namespace ui

#endif
