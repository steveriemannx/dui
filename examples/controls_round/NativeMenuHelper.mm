#include "NativeMenuHelper.h"

#if defined(__APPLE__)

#import <Cocoa/Cocoa.h>

@interface DuiNativeMenuAboutTarget : NSObject
@property(nonatomic, assign) NativeMenuAboutCallback callback;
@property(nonatomic, assign) void* userData;
- (void)clickAbout:(id)sender;
@end

@implementation DuiNativeMenuAboutTarget
- (void)clickAbout:(id)sender
{
    (void)sender;
    if (self.callback != nullptr) {
        self.callback(self.userData);
    }
}
@end

void ShowNativeControlsMenu(void* nsWindow,
                            float x,
                            float y,
                            NativeMenuAboutCallback onAbout,
                            void* userData)
{
    if (nsWindow == nullptr) {
        return;
    }
    NSWindow* window = (__bridge NSWindow*)nsWindow;
    if (window == nil) {
        return;
    }

    NSMenu* menu = [[NSMenu alloc] initWithTitle:@""];

    [menu addItemWithTitle:@"Menu Item 1 (Level 1)" action:nil keyEquivalent:@""];
    [menu addItemWithTitle:@"Menu Item 2 (Level 1)" action:nil keyEquivalent:@""];

    [menu addItem:[NSMenuItem separatorItem]];

    // Volume submenu (simple stand-in for the custom slider demo).
    NSMenu* volumeMenu = [[NSMenu alloc] initWithTitle:@"Volume"];
    [volumeMenu addItemWithTitle:@"Volume: 70%" action:nil keyEquivalent:@""];
    NSMenuItem* volumeItem = [[NSMenuItem alloc] initWithTitle:@"Volume" action:nil keyEquivalent:@""];
    [volumeItem setSubmenu:volumeMenu];
    [menu addItem:volumeItem];

    [menu addItem:[NSMenuItem separatorItem]];

    NSMenu* subMenu = [[NSMenu alloc] initWithTitle:@"Sub Menu"];
    [subMenu addItemWithTitle:@"Menu Item 0 (Level 2)" action:nil keyEquivalent:@""];
    [subMenu addItemWithTitle:@"Menu Item 1 (Level 2)" action:nil keyEquivalent:@""];
    [subMenu addItemWithTitle:@"Menu Item 2 (Level 2)" action:nil keyEquivalent:@""];
    [subMenu addItemWithTitle:@"Menu Item 3 (Level 2)" action:nil keyEquivalent:@""];
    [subMenu addItemWithTitle:@"Menu Item 4 (Level 2)" action:nil keyEquivalent:@""];
    NSMenuItem* subMenuItem = [[NSMenuItem alloc] initWithTitle:@"Sub Menu" action:nil keyEquivalent:@""];
    [subMenuItem setSubmenu:subMenu];
    [menu addItem:subMenuItem];

    [menu addItem:[NSMenuItem separatorItem]];

    [menu addItemWithTitle:@"Sort: Ascending" action:nil keyEquivalent:@""];
    [menu addItemWithTitle:@"Sort: Descending" action:nil keyEquivalent:@""];

    [menu addItem:[NSMenuItem separatorItem]];

    NSMenuItem* aboutItem = [[NSMenuItem alloc] initWithTitle:@"About" action:@selector(clickAbout:) keyEquivalent:@""];
    DuiNativeMenuAboutTarget* target = [[DuiNativeMenuAboutTarget alloc] init];
    target.callback = onAbout;
    target.userData = userData;
    aboutItem.target = target;
    [menu addItem:aboutItem];

    const CGFloat screenHeight = [NSScreen mainScreen].frame.size.height;
    NSPoint screenPoint = NSMakePoint((CGFloat)x, screenHeight - (CGFloat)y);
    NSView* view = window.contentView;
    if (view == nil) {
        return;
    }

    NSPoint windowPoint = [window convertPointFromScreen:screenPoint];
    NSPoint viewPoint = [view convertPoint:windowPoint fromView:nil];
    [menu popUpMenuPositioningItem:nil atLocation:viewPoint inView:view];
}

#endif // __APPLE__
