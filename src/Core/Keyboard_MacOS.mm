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
    case kVK_RETURN: return _T("Enter");
    case kVK_ESCAPE: return _T("Esc");
    case kVK_TAB:    return _T("Tab");
    case kVK_SPACE:  return _T("Space");
    case kVK_BACK:   return _T("Backspace");
    case kVK_DELETE: return _T("Del");
    case kVK_LEFT:   return _T("Left");
    case kVK_RIGHT:  return _T("Right");
    case kVK_UP:     return _T("Up");
    case kVK_DOWN:   return _T("Down");
    case kVK_HOME:   return _T("Home");
    case kVK_END:    return _T("End");
    case kVK_PRIOR:  return _T("PageUp");
    case kVK_NEXT:   return _T("PageDown");
    case kVK_SHIFT:  return _T("Shift");
    case kVK_CONTROL:return _T("Ctrl");
    case kVK_MENU:   return _T("Alt");
    case kVK_LWIN:   return _T("Cmd");
    default: break;
    }
    if ((nVirtKey >= kVK_A) && (nVirtKey <= kVK_Z)) {
        return DString(1, (wchar_t)('A' + (nVirtKey - kVK_A)));
    }
    if ((nVirtKey >= kVK_0) && (nVirtKey <= kVK_9)) {
        return DString(1, (wchar_t)('0' + (nVirtKey - kVK_0)));
    }
    if ((nVirtKey >= kVK_F1) && (nVirtKey <= kVK_F24)) {
        DString str = _T("F");
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
