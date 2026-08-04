#ifndef UI_CORE_WINDOW_MESSAGE_H_
#define UI_CORE_WINDOW_MESSAGE_H_

#include "dui/dui_defs.h"
#include <string>

namespace ui {

/** The type of window size change (parameter of the WM_SIZE message)
*/
enum class WindowSizeType
{
    kSIZE_RESTORED  = 0,    //The window has been resized, but neither the kSIZE_MINIMIZED nor the kSIZE_MAXIMIZED value applies
    kSIZE_MINIMIZED = 1,    //The window has been minimized
    kSIZE_MAXIMIZED = 2,    //The window has been maximized
    kSIZE_MAXSHOW   = 3,    //When some other window is restored to its previous size, the message is sent to all pop-up windows
    kSIZE_MAXHIDE   = 4     //When some other window is maximized, the message is sent to all pop-up windows
};

/** ShowWindow commands
*/
enum ShowWindowCommands
{
    kSW_HIDE                = 0, //Hides the window and activates another window
    kSW_SHOW_NORMAL         = 1, //Activates and displays the window. If the window is minimized, maximized, or arranged, the system restores it to its original size and position. Applications should specify this flag when displaying a window for the first time.
    kSW_SHOW_MINIMIZED      = 2, //Activates the window and displays it as a minimized window
    kSW_SHOW_MAXIMIZED      = 3, //Activates the window and displays it as a maximized window
    kSW_SHOW_NOACTIVATE     = 4, //Displays the window at its most recent size and position. This value is similar to kSW_SHOW_NORMAL, except that the window is not activated
    kSW_SHOW                = 5, //Activates the window and displays it at its current size and position
    kSW_MINIMIZE            = 6, //Minimizes the specified window and activates the next top-level window in the Z order
    kSW_SHOW_MIN_NOACTIVE   = 7, //Displays the window as a minimized window. This value is similar to kSW_SHOW_MINIMIZED, except the window is not activated
    kSW_SHOW_NA             = 8, //Displays the window at its current size and position. This value is similar to SW_SHOW, except the window is not activated
    kSW_RESTORE             = 9  //Activates and displays the window. If the window is minimized, maximized, or arranged, the system restores it to its original size and position. Applications should specify this flag when restoring a minimized window.
};

/* SetWindowPos Flags
 */
enum WindowPosFlags
{
    kSWP_NOSIZE         = 0x0001,
    kSWP_NOMOVE         = 0x0002,
    kSWP_NOZORDER       = 0x0004,
    kSWP_NOREDRAW       = 0x0008,
    kSWP_NOACTIVATE     = 0x0010,
    kSWP_FRAMECHANGED   = 0x0020,  /* The frame changed: send WM_NCCALCSIZE */
    kSWP_SHOWWINDOW     = 0x0040,
    kSWP_HIDEWINDOW     = 0x0080,
    kSWP_NOCOPYBITS     = 0x0100,
    kSWP_NOOWNERZORDER  = 0x0200,  /* Don't do owner Z ordering */
    kSWP_NOSENDCHANGING = 0x0400,  /* Don't send WM_WINDOWPOSCHANGING */
    kSWP_DEFERERASE     = 0x2000,  // same as SWP_DEFERDRAWING
    kSWP_ASYNCWINDOWPOS = 0x4000   // same as SWP_CREATESPB
};

/** The hWndInsertAfter parameter flag of SetWindowPos
*/
enum class InsertAfterFlag
{
    kHWND_DEFAULT   =  0,
    kHWND_NOTOPMOST = -2,
    kHWND_TOPMOST   = -1,
    kHWND_TOP       =  0,
    kHWND_BOTTOM    =  1
};

class WindowBase;
/** The hWndInsertAfter parameter of SetWindowPos
*/
class DUI_API InsertAfterWnd
{
public:
    InsertAfterWnd():
        m_pWindow(nullptr),
        m_hwndFlag(InsertAfterFlag::kHWND_TOP)
    {
    }
    explicit InsertAfterWnd(WindowBase* pWindow):
        m_pWindow(pWindow),
        m_hwndFlag(InsertAfterFlag::kHWND_TOP)
    {
    }
    explicit InsertAfterWnd(InsertAfterFlag flag) :
        m_pWindow(nullptr),
        m_hwndFlag(flag)
    {
    }
    /** Window pointer
    */
    WindowBase* m_pWindow;

    /** Window flag (only takes effect when m_pWindow is nullptr)
    */
    InsertAfterFlag m_hwndFlag;
};

/** Raw data of a window message
*/
class DUI_API NativeMsg
{
public:
    NativeMsg():
        uMsg(0),
        wParam(0),
        lParam(0)
    {
    }
    NativeMsg(uint32_t u, WPARAM w, LPARAM l):
        uMsg(u),
        wParam(w),
        lParam(l)
    {
    }
    /** Message ID
    */
    uint32_t uMsg;

    /** The first parameter of the message
    */
    WPARAM wParam;

    /** The second parameter of the message
    */
    LPARAM lParam;
};


/** Window message definitions; only the messages that are used are defined (consistent with the Windows system definitions, WinUser.h)
*/
enum WindowMessage{
#ifdef DUI_BUILD_FOR_SDL
    kWM_USER = 0x8000 + 32, //SDL_EVENT_USER / Wayland user event
#else
    kWM_USER = 0x0400, //WM_USER
#endif
};


} // namespace ui

#endif // UI_CORE_WINDOW_MESSAGE_H_


