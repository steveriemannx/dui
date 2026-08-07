// macOS input injection via CGEventPost (HID event tap). Requires the
// Accessibility permission; without it events are silently dropped.

#import <CoreGraphics/CoreGraphics.h>
#import <Cocoa/Cocoa.h>

#include "../InputInject.h"
#include "../KeyMap.h"

namespace sdk {

namespace {

// Physical origin of the shared logical screen in the global coordinate
// system (mirror = main display origin (0,0); extend = union of all displays).
void LogicalOrigin(int& ox, int& oy, int screenW, int screenH)
{
    double minX = 0, minY = 0, maxX = 0, maxY = 0;
    bool first = true;
    for (NSScreen* s in [NSScreen screens]) {
        const NSRect f = [s frame];
        if (first) {
            minX = f.origin.x; minY = f.origin.y;
            maxX = f.origin.x + f.size.width;
            maxY = f.origin.y + f.size.height;
            first = false;
        }
        else {
            minX = MIN(minX, f.origin.x);
            minY = MIN(minY, f.origin.y);
            maxX = MAX(maxX, f.origin.x + f.size.width);
            maxY = MAX(maxY, f.origin.y + f.size.height);
        }
    }
    (void)screenW;
    (void)screenH;
    ox = (int)minX;
    oy = (int)minY;
}

} // namespace

bool InputInjector::MoveTo(double nx, double ny, int screenW, int screenH)
{
    int ox = 0, oy = 0;
    LogicalOrigin(ox, oy, screenW, screenH);
    const double x = ox + nx * screenW;
    const double y = oy + ny * screenH;
    CGEventRef ev = CGEventCreateMouseEvent(nullptr, kCGEventMouseMoved,
                                            CGPointMake(x, y), kCGMouseButtonLeft);
    if (ev == nullptr) {
        return false;
    }
    CGEventPost(kCGHIDEventTap, ev);
    CFRelease(ev);
    return true;
}

bool InputInjector::Button(bool down, int button)
{
    CGEventType type;
    CGMouseButton mb = kCGMouseButtonLeft;
    switch (button) {
    case 1: mb = kCGMouseButtonRight; break;
    case 2: mb = kCGMouseButtonCenter; break;
    default: mb = kCGMouseButtonLeft; break;
    }
    type = down ? (button == 1 ? kCGEventRightMouseDown
                               : (button == 2 ? kCGEventOtherMouseDown
                                              : kCGEventLeftMouseDown))
                : (button == 1 ? kCGEventRightMouseUp
                               : (button == 2 ? kCGEventOtherMouseUp
                                              : kCGEventLeftMouseUp));
    CGEventRef ev = CGEventCreateMouseEvent(nullptr, type,
                                            CGEventGetLocation(nullptr), mb);
    if (ev == nullptr) {
        return false;
    }
    CGEventPost(kCGHIDEventTap, ev);
    CFRelease(ev);
    return true;
}

bool InputInjector::Wheel(int deltaLines)
{
    CGEventRef ev = CGEventCreateScrollWheelEvent(nullptr, kCGScrollEventUnitLine, 1,
                                                  -deltaLines);
    if (ev == nullptr) {
        return false;
    }
    CGEventPost(kCGHIDEventTap, ev);
    CFRelease(ev);
    return true;
}

bool InputInjector::Key(uint16_t vkCode, bool down)
{
    const uint16_t macCode = KeyMap::ToMacVirtualKey(vkCode);
    if (macCode == 0xFFFF) {
        return false;
    }
    CGEventRef ev = CGEventCreateKeyboardEvent(nullptr, (CGKeyCode)macCode, down);
    if (ev == nullptr) {
        return false;
    }
    CGEventPost(kCGHIDEventTap, ev);
    CFRelease(ev);
    return true;
}

bool InputInjector::GetCursorPos(int& x, int& y)
{
    const CGPoint p = CGEventGetLocation(nullptr);
    x = (int)p.x;
    y = (int)p.y;
    return true;
}

bool InputInjector::PermissionGranted()
{
    return CGPreflightListenEventAccess() && CGPreflightPostEventAccess();
}

std::string InputInjector::PermissionHint()
{
    return "accessibility permission not granted";
}

} // namespace sdk
