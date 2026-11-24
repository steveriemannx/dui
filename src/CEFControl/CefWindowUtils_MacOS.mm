#include "dui/CEFControl/CefWindowUtils.h"
#include "dui/Core/Window.h"

#if defined (DUI_BUILD_FOR_MACOS) && defined (DUI_BUILD_FOR_CEF)
#include <Cocoa/Cocoa.h>

namespace ui
{
void ReparentNSView(NSView* targetView, NSView* newParent) 
{
    @autoreleasepool {
        // Ensure the operation runs on the main thread
        if (![NSThread isMainThread]) {
            dispatch_sync(dispatch_get_main_queue(), ^{
                [targetView removeFromSuperview];
                [newParent addSubview:targetView];
            });
        } else {
            [targetView removeFromSuperview];
            [newParent addSubview:targetView];
        }
    }
}

void SetNSViewVisibility(NSView* view, bool visible) 
{
    @autoreleasepool {
        if (![NSThread isMainThread]) {
            dispatch_sync(dispatch_get_main_queue(), ^{
                [view setHidden:!visible];
            });
        } else {
            [view setHidden:!visible];
        }
    }
}

void SetNSViewFrame(NSView* view, CGFloat x, CGFloat y, CGFloat w, CGFloat h)
{
    @autoreleasepool {
        NSRect newFrame = NSMakeRect(x, y, w, h);
        if (![NSThread isMainThread]) {
            dispatch_sync(dispatch_get_main_queue(), ^{
                [view setFrame:newFrame];
                [view.superview setNeedsDisplay:YES]; // Trigger a repaint
            });
        } else {
            [view setFrame:newFrame];
            [view.superview setNeedsDisplay:YES];
        }
    }
}

bool IsValidNSView(NSView* view) 
{
    __block BOOL isValid = NO;
    void (^checkBlock)(void) = ^{
        isValid = (view != nil) &&
                  [view respondsToSelector:@selector(window)] &&
                  [view isKindOfClass:[NSView class]] &&
                  view.window;
    };

    if (![NSThread isMainThread]) {
        dispatch_sync(dispatch_get_main_queue(), checkBlock);
    } else {
        checkBlock();
    }
    return isValid;
}
 
void SetCefWindowPos(CefWindowHandle cefWindow, CefControl* pCefControl)
{
    if ((cefWindow == 0) || (pCefControl == nullptr)) {
        return;
    }
    Window* pWindow = pCefControl->GetWindow();
    if (pWindow == nullptr) {
        return;
    }
    NSView* pCefNSView = (NSView*)cefWindow;
    if (!IsValidNSView(pCefNSView)) {
        return;
    }
    UiRect rc = pCefControl->GetPos();
    pCefControl->Dpi().ClientSizeToWindowSize(rc);
    UiRect rcClient;
    pWindow->GetClientRect(rcClient);
    // macOS view coordinates use a bottom-left origin; use the client height
    // because the window height also includes the native title bar.
    SetNSViewFrame(pCefNSView, (CGFloat)rc.left, (CGFloat)(rcClient.Height() - rc.bottom), (CGFloat)rc.Width(), (CGFloat)rc.Height());
}

void SetCefWindowVisible(CefWindowHandle cefWindow, CefControl* pCefControl)
{
    if ((cefWindow == 0) || (pCefControl == nullptr)) {
        return;
    }
    NSView* pCefNSView = (NSView*)cefWindow;
    if (!IsValidNSView(pCefNSView)) {
        return;
    }
    bool bVisible = pCefControl->IsVisible();
    
    SetNSViewVisibility(pCefNSView, bVisible);
}

void SetCefWindowParent(CefWindowHandle cefWindow, CefControl* pCefControl)
{
    if ((cefWindow == 0) || (pCefControl == nullptr)) {
        return;
    }
    Window* pWindow = pCefControl->GetWindow();
    if (pWindow == nullptr) {
        return;
    }
    NSView* pCefNSView = (NSView*)cefWindow;
    if (!IsValidNSView(pCefNSView)) {
        return;
    }
    NSView* pParentNSView = (NSView*)pWindow->NativeWnd()->GetNSView();
    
    ReparentNSView(pCefNSView, pParentNSView);
}

bool CaptureCefWindowBitmap(CefWindowHandle cefWindow, std::vector<uint8_t>& bitmap, int32_t& width, int32_t& height)
{
    NSView* pCefNSView = (NSView*)cefWindow;
    if (!pCefNSView) {
        return false;
    }
    if (!IsValidNSView(pCefNSView)) {
        return false;
    }
    // Get the view's bounds and size
    NSRect viewBounds = [pCefNSView bounds];
    width = static_cast<int32_t>(NSWidth(viewBounds));
    height = static_cast<int32_t>(NSHeight(viewBounds));
    
    if (width <= 0 || height <= 0) {
        return false;
    }
    
    // Create a bitmap image representation
    NSBitmapImageRep* bitmapRep = [[NSBitmapImageRep alloc] 
        initWithBitmapDataPlanes:NULL
                      pixelsWide:width
                      pixelsHigh:height
                   bitsPerSample:8
                 samplesPerPixel:4
                        hasAlpha:YES
                        isPlanar:NO
                  colorSpaceName:NSCalibratedRGBColorSpace
                    bytesPerRow:width * 4
                   bitsPerPixel:32];
    
    if (!bitmapRep) {
        return false;
    }
    
    // Create a graphics context and draw the view
    NSGraphicsContext* context = [NSGraphicsContext graphicsContextWithBitmapImageRep:bitmapRep];
    if (!context) {
        [bitmapRep release];
        return false;
    }
    
    [NSGraphicsContext saveGraphicsState];
    [NSGraphicsContext setCurrentContext:context];
    
    // Draw the view content (modern API instead of lockFocus/unlockFocus)
    [pCefNSView displayRectIgnoringOpacity:viewBounds inContext:context];
    
    [NSGraphicsContext restoreGraphicsState];
    
    // Get the bitmap data
    unsigned char* bitmapData = [bitmapRep bitmapData];
    if (bitmapData) {
        size_t dataSize = width * height * 4;
        bitmap.resize(dataSize);
        memcpy(bitmap.data(), bitmapData, dataSize);
    } else {
        [bitmapRep release];
        return false;
    }
    
    [bitmapRep release];
    return true;
}

void SetCefWindowCursor(CefWindowHandle cefWindow, CefCursorHandle cursor)
{
    // Currently unused; kept commented out
    // if ((cefWindow == nullptr) || (cursor == nullptr)) {
    //     return;
    // }
    
    // // Convert a CEF window handle into NSView*
    // NSView* cefView = static_cast<NSView*>(cefWindow);
    // // Convert a CEF cursor handle into NSCursor*
    // NSCursor* nsCursor = static_cast<NSCursor*>(cursor);
    
    // // Get the window that holds the view
    // NSWindow* window = [cefView window];
    // if (!window) {
    //     return;
    // }
    
    // // Ensure UI elements are handled on the main thread
    // auto setCursorBlock = ^{        
    //     // For view-scoped cursors, NSCursor's set method can be used
    //      [nsCursor set];
    // };
    
    // if ([NSThread isMainThread]) {
    //     setCursorBlock();
    // } else {
    //     dispatch_sync(dispatch_get_main_queue(), setCursorBlock);
    // }
}

void RemoveCefWindowFromParent(CefWindowHandle /*cefWindow*/)
{
    //Not needed: removing it from the parent view in code makes CEF report an error on exit and the process cannot terminate cleanly
    return;
}

} //namespace ui

#endif //DUI_BUILD_FOR_MACOS/DUI_BUILD_FOR_CEF
