#include "dui/Core/Keyboard.h"
#include "dui/Utils/StringConvert.h"

#if defined (DUI_BUILD_FOR_WIN)

namespace ui
{
bool Keyboard::IsKeyDown(VirtualKeyCode nVirtKey)
{
    return (::GetKeyState(nVirtKey) & 0x8000) != 0;
}

bool Keyboard::IsNumLockOn()
{
    return (::GetKeyState(VK_NUMLOCK) & 0x0001) != 0;
}

bool Keyboard::IsScrollLockOn()
{
    return (::GetKeyState(VK_SCROLL) & 0x0001) != 0;
}

bool Keyboard::IsCapsLockOn()
{
    return (::GetKeyState(VK_CAPITAL) & 0x0001) != 0;
}

std::string Keyboard::GetKeyName(VirtualKeyCode nVirtKey, bool fExtended)
{
    UINT nScanCode = ::MapVirtualKeyEx(nVirtKey, 0, ::GetKeyboardLayout(0));
    switch (nVirtKey)
    {
    // Keys which are "extended" (except for Return which is Numeric Enter as extended)
    case kVK_INSERT:
    case kVK_DELETE:
    case kVK_HOME:
    case kVK_END:
    case kVK_NEXT:  // Page down
    case kVK_PRIOR: // Page up
    case kVK_LEFT:
    case kVK_RIGHT:
    case kVK_UP:
    case kVK_DOWN:
        nScanCode |= 0x100; // Add extended bit
        break;
    default:
        break;
    }
    if (fExtended) {
        nScanCode |= 0x01000000L;
    }

    // wchar_t rather than TCHAR: the string model is UTF-8 everywhere now, and the key
    // name comes back from the W API, so it is converted once on the way out.
    wchar_t szStr[MAX_PATH] = { 0 };
    ::GetKeyNameTextW(nScanCode << 16, szStr, MAX_PATH);
    return StringConvert::WStringToUTF8(szStr);
}

} // namespace ui

#endif // DUI_BUILD_FOR_WIN
