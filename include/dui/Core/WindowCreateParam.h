#ifndef UI_CORE_WINDOW_CREATE_PARAM_H_
#define UI_CORE_WINDOW_CREATE_PARAM_H_

#include "dui/dui_defs.h"
#include <string>

namespace ui {

/** The style of the window class
*   On the Windows platform, refer to: https://learn.microsoft.com/zh-cn/windows/win32/winmsg/window-class-styles
*/
enum WindowClassStyle : uint32_t
{
    kCS_VREDRAW = 0x0001,   //Redraw the entire window if a movement or size adjustment changes the height of the client area
    kCS_HREDRAW = 0x0002,   //Redraw the entire window if a movement or size adjustment changes the width of the client area
    kCS_DBLCLKS = 0x0008    //When the user double-clicks the mouse while the cursor is within a window belonging to the class, the double-click message is sent to the window procedure
};

/** Window styles
*   On the Windows platform, refer to: https://learn.microsoft.com/zh-cn/windows/win32/winmsg/window-styles
*/
enum WindowStyle : uint32_t
{
    kWS_OVERLAPPED      = 0x00000000L, //The window is an overlapped window; overlapped windows have a title bar and a border
    kWS_POPUP           = 0x80000000L, //The window is a pop-up window
    kWS_VISIBLE         = 0x10000000L, //The window is initially visible
    kWS_DISABLED        = 0x08000000L, //The window is initially disabled; a disabled window cannot receive input from the user
    kWS_CAPTION         = 0x00C00000L, /* kWS_BORDER | kWS_DLGFRAME */ //The window has a title bar (includes the kWS_BORDER style).
    kWS_BORDER          = 0x00800000L, //The window has a thin-line border
    kWS_DLGFRAME        = 0x00400000L, //The window has a border of the style typically used with dialog boxes.
    kWS_THICKFRAME      = 0x00040000L, //The window has a sizing border
    kWS_MINIMIZEBOX     = 0x00020000L, //The window has a maximize button. The WS_SYSMENU style must be specified.
    kWS_MAXIMIZEBOX     = 0x00010000L, //The window has a minimize button. The WS_SYSMENU style must be specified.
    kWS_SYSMENU	        = 0x00080000L, //The window has a window menu on its title bar. The WS_CAPTION style must also be specified.

    //An overlapped window with a title bar, a sizing border, a window menu, and minimize and maximize buttons.
    kWS_OVERLAPPEDWINDOW    = (kWS_OVERLAPPED |  kWS_CAPTION |  kWS_SYSMENU |  kWS_THICKFRAME |  kWS_MINIMIZEBOX | kWS_MAXIMIZEBOX),
    //A pop-up window
    kWS_POPUPWINDOW         = (kWS_POPUP | kWS_BORDER | kWS_SYSMENU)
};

/** Window extended styles
*   On the Windows platform, refer to: https://docs.microsoft.com/en-us/windows/desktop/winmsg/window-class-styles
*/
enum WindowExStyle : uint32_t
{
    kWS_EX_TOPMOST        = 0x00000008L, //The window should be placed above all non-topmost windows and should stay above them even when the window is deactivated.
    kWS_EX_ACCEPTFILES    = 0x00000010L, //The window accepts drag and drop files
    kWS_EX_TRANSPARENT    = 0x00000020L, //The window is displayed as transparent
    kWS_EX_TOOLTIP_WINDOW = 0x00000040L, //The window is intended to be used as a ToolTip window (only used with SDL)
    kWS_EX_TOOLWINDOW     = 0x00000080L, //The window is intended to be used as a floating toolbar. A tool window has a title bar shorter than a normal title bar and a window title drawn with a smaller font. A tool window does not appear in the taskbar or in the dialog displayed when the user presses Alt+TAB. If a tool window has a system menu, its icon is not displayed on the title bar.
    kWS_EX_LAYERED        = 0x00080000L, //The window is a layered window
    kWS_EX_NOACTIVATE     = 0x08000000L  //A top-level window created with this style does not become the foreground window when the user clicks it. The system does not bring this window to the foreground when the user minimizes or closes the foreground window.
};

/** Default value flags for the window position and size
*/
enum WindowDefaultSize
{
    kCW_USEDEFAULT  = ((int32_t)0x80000000)
};

/** Parameters required to create a window
*/
class DUI_API WindowCreateParam
{
public:
    /** Default constructor
    */
    WindowCreateParam();

    /** Constructor providing the window title
    * @param [in] windowTitle Window title
    * @param [in] windowId Window ID; if empty, a window ID is generated internally
    */
    explicit WindowCreateParam(const DString& windowTitle, const DString& windowId = _T(""));

    /** Constructor providing the window title and window centering
    * @param [in] windowTitle Window title
    * @param [in] bCenterWindow The initial position of the window is centered
    * @param [in] windowId Window ID; if empty, a window ID is generated internally
    */
    WindowCreateParam(const DString& windowTitle, bool bCenterWindow, const DString& windowId = _T(""));

public:
    /** Window class name (optional parameter; if not provided, the default value is used)
    */
    DString m_className;

    /** The style of the window class (optional parameter; if not provided, the default value is used). Refer to the definition above: enum WindowClassStyle
    */
    uint32_t m_dwClassStyle;

public:
    /** Window style (optional parameter; if not provided, the default value is used). Refer to the definition above: enum WindowStyle
    */
    uint32_t m_dwStyle;

    /** Window extended style (optional parameter; if not provided, the default value is used). Refer to the definition above: enum WindowExStyle
    */
    uint32_t m_dwExStyle;

    /** The title of the window (optional parameter; empty by default)
    */
    DString m_windowTitle;

    /** Window ID (can be empty; if empty, a unique ID is generated automatically internally)
    */
    DString m_windowId;

public:
    /** The X coordinate of the top-left corner of the window (if not set, the default value is used)
    */
    int32_t m_nX;

    /** The Y coordinate of the top-left corner of the window (if not set, the default value is used)
    */
    int32_t m_nY;

    /** The width of the window (if not set, the default value is used)
    */
    int32_t m_nWidth;

    /** The height of the window (if not set, the default value is used)
    */
    int32_t m_nHeight;

    /** Whether the initial window is centered (default is false)
    */
    bool m_bCenterWindow;
};

} // namespace ui

#endif // UI_CORE_WINDOW_CREATE_PARAM_H_

