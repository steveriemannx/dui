## Window Message Notes
| Message          | Parameter description     | Reference     |
| :---          | :---     | :---     |
| WM_SIZE       | wParam (specific value), lParam (new width and height)          | https://learn.microsoft.com/zh-cn/windows/win32/winmsg/wm-size|
| WM_MOVE       | lParam (x and y coordinates of the upper-left corner of the window's client area)    |  https://learn.microsoft.com/zh-cn/windows/win32/winmsg/wm-move   |
| WM_PAINT      | No parameters    |https://learn.microsoft.com/zh-cn/windows/win32/gdi/wm-paint|
| WM_SETFOCUS   | wParam handle of the window that has lost keyboard focus | https://learn.microsoft.com/zh-cn/windows/win32/inputdev/wm-setfocus|
| WM_KILLFOCUS  | wParam handle of the window that receives keyboard focus | https://learn.microsoft.com/zh-cn/windows/win32/inputdev/wm-killfocus|
| WM_CHAR       | wParam character code of the key, lParam scan code, extended-key flag, etc.  | https://learn.microsoft.com/zh-cn/windows/win32/inputdev/wm-char |
| WM_KEYDOWN    | wParam virtual-key code, lParam scan code, extended-key flag, etc.  | https://learn.microsoft.com/zh-cn/windows/win32/inputdev/wm-keydown|
| WM_KEYUP      | Same as WM_KEYDOWN | https://learn.microsoft.com/zh-cn/windows/win32/inputdev/wm-keyup|
| WM_SYSKEYDOWN | Same as WM_KEYDOWN | https://learn.microsoft.com/zh-cn/windows/win32/inputdev/wm-syskeydown|
| WM_SYSKEYUP   | Same as WM_KEYDOWN | https://learn.microsoft.com/zh-cn/windows/win32/inputdev/wm-syskeyup|
| WM_HOTKEY     | wParam, lParam three values  |https://learn.microsoft.com/zh-cn/windows/win32/inputdev/wm-hotkey|
| WM_IME_STARTCOMPOSITION |  No parameters   |  https://learn.microsoft.com/zh-cn/windows/win32/intl/wm-ime-startcomposition   |
| WM_IME_ENDCOMPOSITION   |  No parameters   |  https://learn.microsoft.com/zh-cn/windows/win32/intl/wm-ime-endcomposition   |
| WM_SETCURSOR  | wParam window handle, lParam trigger event, etc.   | https://learn.microsoft.com/zh-cn/windows/win32/menurc/wm-setcursor    |
| -WM_SYSCOMMAND | wParam type of the system command requested | https://learn.microsoft.com/zh-cn/windows/win32/menurc/wm-syscommand    |
| WM_CONTEXTMENU| wParam window handle, lParam cursor coordinates x,y | https://learn.microsoft.com/zh-cn/windows/win32/menurc/wm-contextmenu?redirectedfrom=MSDN|
| WM_MOUSELEAVE | No parameters | https://learn.microsoft.com/zh-cn/windows/win32/inputdev/wm-mouseleave    |
| WM_MOUSEHOVER | wParam indicates whether various virtual keys are pressed, lParam cursor coordinates x,y  | https://learn.microsoft.com/zh-cn/windows/win32/inputdev/wm-mousehover    |
| WM_MOUSEMOVE  | wParam indicates whether various virtual keys are pressed, lParam cursor coordinates x,y  | https://learn.microsoft.com/zh-cn/windows/win32/inputdev/wm-mousemove    |
| WM_MOUSEWHEEL | wParam WHEEL_DELTA + whether virtual keys are pressed, lParam cursor coordinates x,y| https://learn.microsoft.com/zh-cn/windows/win32/inputdev/wm-mousewheel    |
| WM_LBUTTONDOWN| wParam indicates whether various virtual keys are pressed, lParam cursor coordinates x,y  |https://learn.microsoft.com/zh-cn/windows/win32/inputdev/wm-lbuttondown     |
| WM_LBUTTONUP  | wParam indicates whether various virtual keys are pressed, lParam cursor coordinates x,y  | https://learn.microsoft.com/zh-cn/windows/win32/inputdev/wm-lbuttonup    |
| WM_LBUTTONDBLCLK |  Same   |https://learn.microsoft.com/zh-cn/windows/win32/inputdev/wm-lbuttondblclk |
| WM_RBUTTONDOWN   |  Same   |https://learn.microsoft.com/zh-cn/windows/win32/inputdev/wm-rbuttondown     |
| WM_RBUTTONUP     |  Same   |https://learn.microsoft.com/zh-cn/windows/win32/inputdev/wm-rbuttonup     |
| WM_RBUTTONDBLCLK |  Same   |https://learn.microsoft.com/zh-cn/windows/win32/inputdev/wm-rbuttondblclk    |
| WM_CAPTURECHANGED|  Sent to the window that lost mouse capture; lParam handle of the window that gained mouse capture   | https://learn.microsoft.com/zh-cn/windows/win32/inputdev/wm-capturechanged |
| WM_CLOSE    |     |     |
|     |     |     |
| WM_TASKBARBUTTONCREATED |  Used by the example, no parameters   |     |
| WM_SHOWWINDOW| wParam indicates whether the window is displayed. If wParam is TRUE, the window is displayed. If wParam is FALSE, the window is hidden.| https://learn.microsoft.com/zh-cn/windows/win32/winmsg/wm-showwindow?redirectedfrom=MSDN
|     |     |     |
|     |     |     |
|     |     |     |
|     |     |     |
|     |     |     |
|     |     |     |
|     |     |     |
|     |     |     |
|     |     |     |
|     |     |     |
|     |     |     |
|     |     |     |
|     |     |     |
|     |     |     |
