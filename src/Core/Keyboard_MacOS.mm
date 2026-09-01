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

DString Keyboard::GetKeyName(VirtualKeyCode nVirtKey, bool /*fExtended*/)
{
    switch (nVirtKey) {
    case kVK_RETURN: return DUI_T("Enter");
    case kVK_ESCAPE: return DUI_T("Esc");
    case kVK_TAB:    return DUI_T("Tab");
    case kVK_SPACE:  return DUI_T("Space");
    case kVK_BACK:   return DUI_T("Backspace");
    case kVK_DELETE: return DUI_T("Del");
    case kVK_LEFT:   return DUI_T("Left");
    case kVK_RIGHT:  return DUI_T("Right");
    case kVK_UP:     return DUI_T("Up");
    case kVK_DOWN:   return DUI_T("Down");
    case kVK_HOME:   return DUI_T("Home");
    case kVK_END:    return DUI_T("End");
    case kVK_PRIOR:  return DUI_T("PageUp");
    case kVK_NEXT:   return DUI_T("PageDown");
    case kVK_SHIFT:  return DUI_T("Shift");
    case kVK_CONTROL:return DUI_T("Ctrl");
    case kVK_MENU:   return DUI_T("Alt");
    case kVK_LWIN:   return DUI_T("Cmd");
    default: break;
    }
    if ((nVirtKey >= kVK_A) && (nVirtKey <= kVK_Z)) {
        return DString(1, (wchar_t)('A' + (nVirtKey - kVK_A)));
    }
    if ((nVirtKey >= kVK_0) && (nVirtKey <= kVK_9)) {
        return DString(1, (wchar_t)('0' + (nVirtKey - kVK_0)));
    }
    if ((nVirtKey >= kVK_F1) && (nVirtKey <= kVK_F24)) {
        DString str = DUI_T("F");
#ifdef DUI_UNICODE
        str += std::to_wstring((int32_t)(nVirtKey - kVK_F1 + 1));
#else
        str += std::to_string((int32_t)(nVirtKey - kVK_F1 + 1));
#endif
        return str;
    }
    return DString();
}

} // namespace ui

#endif // defined(DUI_BUILD_FOR_MACOS)
