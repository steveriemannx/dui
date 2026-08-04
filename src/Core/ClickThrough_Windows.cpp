#include "dui/Core/ClickThrough.h"

#ifdef DUI_BUILD_FOR_WIN

#include "dui/Core/Window.h"
#include "dui/Core/Control.h"

namespace ui
{
ClickThrough::ClickThrough()
{
}

ClickThrough::~ClickThrough()
{
}

bool ClickThrough::ClickThroughWindow(Window* pWindow, const UiPoint& ptMouse)
{
    if (pWindow == nullptr) {
        return false;
    }
    POINT ptScreen;
    ptScreen.x = ptMouse.x;
    ptScreen.y = ptMouse.y;

    HWND hWnd = pWindow->NativeWnd()->GetHWND();
    ::SetWindowLong(hWnd, GWL_EXSTYLE, ::GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_TRANSPARENT);
    HWND hWndUnder = ::WindowFromPoint(ptScreen);
    ::SetWindowLong(hWnd, GWL_EXSTYLE, ::GetWindowLong(hWnd, GWL_EXSTYLE) & ~WS_EX_TRANSPARENT);

    // If a child window is obtained, get its parent window
    while (::IsWindow(hWndUnder)) {
        HWND hParentWnd = ::GetParent(hWndUnder);
        if ((hParentWnd == nullptr) || (hParentWnd == ::GetDesktopWindow())) {
            break;
        }
        hWndUnder = hParentWnd;
    }
    if ((hWnd != hWndUnder) && ::IsWindow(hWnd) && ::IsWindow(hWndUnder)) {
        DWORD dwThreadId = ::GetWindowThreadProcessId(hWnd, nullptr);
        DWORD dwUnderThreadId = ::GetWindowThreadProcessId(hWndUnder, nullptr);
        ::AttachThreadInput(dwThreadId, dwUnderThreadId, TRUE);
        ::SetActiveWindow(hWndUnder);
        ::AttachThreadInput(dwThreadId, dwUnderThreadId, FALSE);
        return true;
    }
    return false;
}

}//namespace ui

#endif //DUI_BUILD_FOR_WIN
