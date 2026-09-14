#include "dui/Core/Keyboard.h"
#include "dui/Utils/StringConvert.h"

#if defined(DUI_BUILD_FOR_MACOS)

#import <Cocoa/Cocoa.h>

namespace ui
{

bool Keyboard::IsKeyDown(VirtualKeyCode nVirtKey)
{
    //macOS offers no global key-state polling (unlike GetAsyncKeyState);
    //modifier keys are readable through the event flags, others are reported
    //as not pressed (callers process key messages instead).
    NSEventModifierFlags flags = [NSEvent modifierFlags];
    switch (nVirtKey) {
    case kVK_SHIFT:
    case kVK_RSHIFT:
        return (flags & NSEventModifierFlagShift) != 0;
    case kVK_CONTROL:
    case kVK_RCONTROL:
        return (flags & NSEventModifierFlagControl) != 0;
    case kVK_MENU:
    case kVK_RMENU:
        return (flags & NSEventModifierFlagOption) != 0;
    case kVK_LWIN:
        return (flags & NSEventModifierFlagCommand) != 0;
    case kVK_CAPITAL:
        return (flags & NSEventModifierFlagCapsLock) != 0;
    default:
        return false;
    }
}

bool Keyboard::IsNumLockOn()
{
    //macOS has no Num Lock key
    return false;
}

bool Keyboard::IsScrollLockOn()
{
    //macOS has no Scroll Lock key
    return false;
}

bool Keyboard::IsCapsLockOn()
{
    return ([NSEvent modifierFlags] & NSEventModifierFlagCapsLock) != 0;
}

std::string Keyboard::GetKeyName(VirtualKeyCode nVirtKey, bool /*fExtended*/)
{
    switch (nVirtKey) {
    case kVK_RETURN: return "Enter";
    case kVK_ESCAPE: return "Esc";
    case kVK_TAB:    return "Tab";
    case kVK_SPACE:  return "Space";
    case kVK_BACK:   return "Backspace";
    case kVK_DELETE: return "Del";
    case kVK_LEFT:   return "Left";
    case kVK_RIGHT:  return "Right";
    case kVK_UP:     return "Up";
    case kVK_DOWN:   return "Down";
    case kVK_HOME:   return "Home";
    case kVK_END:    return "End";
    case kVK_PRIOR:  return "PageUp";
    case kVK_NEXT:   return "PageDown";
    case kVK_SHIFT:  return "Shift";
    case kVK_CONTROL:return "Ctrl";
    case kVK_MENU:   return "Alt";
    case kVK_LWIN:   return "Cmd";
    default: break;
    }
    if ((nVirtKey >= kVK_A) && (nVirtKey <= kVK_Z)) {
        return std::string(1, (wchar_t)('A' + (nVirtKey - kVK_A)));
    }
    if ((nVirtKey >= kVK_0) && (nVirtKey <= kVK_9)) {
        return std::string(1, (wchar_t)('0' + (nVirtKey - kVK_0)));
    }
    if ((nVirtKey >= kVK_F1) && (nVirtKey <= kVK_F24)) {
        std::string str = "F";
        str += std::to_string((int32_t)(nVirtKey - kVK_F1 + 1));
        return str;
    }
    return std::string();
}

} // namespace ui

#endif // defined(DUI_BUILD_FOR_MACOS)
