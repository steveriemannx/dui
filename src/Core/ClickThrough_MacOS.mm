#include "dui/Core/ClickThrough.h"
#include "dui/Core/Window.h"

#ifdef DUI_BUILD_FOR_MACOS
#import <Cocoa/Cocoa.h>
#import <CoreGraphics/CoreGraphics.h>
#include <ApplicationServices/ApplicationServices.h>
#include <AppKit/AppKit.h>
#include <CoreFoundation/CoreFoundation.h>

namespace ui
{
ClickThrough::ClickThrough()
{
}

ClickThrough::~ClickThrough()
{
}

static void MacOS_ActivateWindowByNumber(NSInteger windowNumber) 
{
    if (windowNumber == 0) {
        return;
    }
    
    CFArrayRef windowList = CGWindowListCopyWindowInfo(kCGWindowListOptionOnScreenOnly, kCGNullWindowID);    
    if (!windowList) {
        return;
    }
    
    BOOL found = NO;
    pid_t targetPID = 0;
    CFIndex count = CFArrayGetCount(windowList);
    
    for (CFIndex i = 0; i < count; i++) {
        CFDictionaryRef info = (CFDictionaryRef)CFArrayGetValueAtIndex(windowList, i);
        CFNumberRef idRef = (CFNumberRef)CFDictionaryGetValue(info, kCGWindowNumber);
        
        NSInteger currentID;
        if (idRef && CFNumberGetValue(idRef, kCFNumberNSIntegerType, &currentID) && (currentID == windowNumber)) {
            
            CFNumberRef pidRef = (CFNumberRef)CFDictionaryGetValue(info, kCGWindowOwnerPID);
            if (pidRef) {
                CFNumberGetValue(pidRef, kCFNumberIntType, &targetPID);
            }            
            found = YES;
            break;
        }
    }
    CFRelease(windowList);
    
    if (found && targetPID != 0) {
        NSRunningApplication *app = [NSRunningApplication runningApplicationWithProcessIdentifier:targetPID];
        [app activateWithOptions:NSApplicationActivateIgnoringOtherApps];
        if (!app.isActive) {
            dispatch_async(dispatch_get_main_queue(), ^{
                [app activateWithOptions:NSApplicationActivateIgnoringOtherApps];
            });
        }
    }
}

bool ClickThrough::ClickThroughWindow(Window* pWindow, const UiPoint& ptMouse)
{
    if (pWindow == nullptr) {
        return false;
    }
    
    // Get the native window handle
    NSWindow* pNSWindow = (NSWindow*)pWindow->NativeWnd()->GetNSWindow();
    if (pNSWindow == nullptr) {
        return false;
    }
    
    // Convert the coordinates into window client coordinates
    UiPoint pt = ptMouse;
    pWindow->ScreenToClient(pt);

    // The window being processed
    NSWindow* currentWindow = pNSWindow;
    
    // Save the window's original state so it can be restored
    NSWindowLevel originalLevel = [currentWindow level];
    NSWindowCollectionBehavior originalBehavior = [currentWindow collectionBehavior];
    BOOL originalIgnoresMouseEvents = [currentWindow ignoresMouseEvents];
    
    // Temporarily make the window click-through
    [currentWindow setLevel:NSFloatingWindowLevel];
    [currentWindow setCollectionBehavior:NSWindowCollectionBehaviorCanJoinAllSpaces |
                                     NSWindowCollectionBehaviorIgnoresCycle |
                                     NSWindowCollectionBehaviorStationary];
    [currentWindow setIgnoresMouseEvents:YES];
    
    // Coordinate conversion: window coordinates -> screen coordinates
    NSView* contentView = [currentWindow contentView];
    NSPoint windowPoint = NSMakePoint(pt.x, pt.y);
    NSPoint screenPoint = [contentView convertPoint:windowPoint toView:nil];
    
    // Get the number of the window under the mouse click
    NSInteger windowNumber = [NSWindow windowNumberAtPoint:screenPoint
                                belowWindowWithWindowNumber:0];
    
    // Restore the window's original state
    [currentWindow setIgnoresMouseEvents:originalIgnoresMouseEvents];
    [currentWindow setLevel:originalLevel];
    [currentWindow setCollectionBehavior:originalBehavior];

    // If another window was clicked, activate that window
    if (windowNumber != 0 && windowNumber != [currentWindow windowNumber]) {
        MacOS_ActivateWindowByNumber(windowNumber);
    }

    return true;
}

}//namespace ui

#endif //DUI_BUILD_FOR_MACOS
