## Cross-Platform Development Notes
The following are the main cross-platform support items for nim_duilib (a solution based on [SDL3.0](https://www.libsdl.org/) as the window manager is currently under development, which will enable cross-platform support for the UI library)
| Item          | Remarks     | 
| :---          | :---     |
| HWND          |Resolved    |
| HDC           |Resolved    |
| Win API       |Resolved    |
| DPI           |Resolved    |
| Font          |Resolved (uses Skia's font manager)|
| UINT LRESULT, |Resolved    |
| WPARAM,LPARAM |Resolved    |
| Key state      |Resolved    |
| RichEdit control  |Needs to be implemented with SDL; only basic text input is supported; pending development; it is a difficult item | 
| Date control      |Not supported yet; alternative solutions need to be researched| 
| Three Combo controls | Resolved   | 
|ColorPicker screen capture|Needs to be implemented with SDL; pending development |
| Menu control      | Resolved   |
| Message handling      | Resolved   | 
| SetWindowPos  | Resolved   | 
|Window GetHWND | Resolved   |
| DragDrop      | Not supported yet; alternative solutions need to be researched         |
| HotKey        | Not supported yet; alternative solutions need to be researched         |
| Window style      | Resolved   |
| Window          | Resolved   | 
| Window icon      | Resolved   | 
| Cursor          | Resolved   | 
| Mouse          | Resolved  | 
| Keyboard          | Resolved  | 
| Message interface modification  | Resolved  | 
| Window interface abstraction| Resolved  | 
|               |          | 

## Cross-Platform Feature Inventory (with development progress)
|Module | Source file (Windows platform or uses Windows API)    | Current status | Feature description  | Source file (same directory, other platforms)| Remarks |
| :---| :---                                      | :---     | :---     | :---              |:---           |
|Utils  |duilib\Utils\ApiWrapper_Windows.h        |No action needed  |Windows platform only|  Not needed  |
|Utils  |duilib\Utils\ApiWrapper_Windows.cpp      |No action needed  |Windows platform only|  Not needed  |
|Utils  |duilib\Utils\BitmapHelper_Windows.h      |No action needed  |Windows platform only|  Not needed  |
|Utils  |duilib\Utils\BitmapHelper_Windows.cpp    |No action needed  |Windows platform only|  Not needed  |
|Utils  |duilib\Utils\Macros_Windows.h            |No action needed  |Windows platform only|  Not needed  |
|Utils  |duilib\Utils\MonitorUtil_Windows.cpp     |Done    |Gets the DPI value |MonitorUtil_SDL.cpp|
|Utils  |duilib\Utils\FileDialog_Windows.cpp      |Done    |          |FileDialog_SDL.cpp |
|Utils  |duilib\Utils\ScreenCapture_Windows.cpp   |Not supported |          |SDL has no screen capture feature|No solution yet; other<br>platforms do not support screen color picking|
|Utils  |duilib\Utils\ShadowWnd_Windows.cpp       |In development... |SDL needs modification to add<br>WM_MOVE/WM_MOVING events;<br>SDL cannot change a window's Z-Order,<br>so the expected effect cannot be achieved|ShadowWnd_SDL.cpp||
|Control |duilib\Control\DateTimeWnd_Windows.h  |Done|          | DateTimeWnd_SDL.h  |Custom implementation|
|Control |duilib\Control\DateTimeWnd_Windows.cpp  |Done|          | DateTimeWnd_SDL.cpp|Custom implementation,<br>but does not support the EditFormat::<br>kDateCalendar format|
|Control |duilib\Control\PropertyGrid.cpp         |Done    |Platform-related code removed |   |
|Control |duilib\Control\RichEdit_Windows.h       |Done    |          |RichEdit_SDL.h|Lacks the RichText feature<br>compared with the Windows RichEdit control|
|Control |duilib\Control\RichEdit_Windows.cpp     |Done    |          |RichEdit_SDL.cpp|Custom implementation|
|Control |duilib\Control\RichEditCtrl_Windows.h   |No action needed  |          |          |
|Control |duilib\Control\RichEditHost_Windows.h   |No action needed  |          |          |
|Control |duilib\Control\RichEditHost_Windows.cpp |No action needed  |          |          |
|Core |duilib\Core\DpiManager.cpp                 |Done    |Platform-related code removed |   |
|Core |duilib\Core\FontManager.cpp                |Done    |Platform-related code removed |   |
|Core |duilib\Core\TimerManager.h                 |          |std::condition_variable;<br>Linux needs a pthread implementation |     |
|Core |duilib\Core\TimerManager.cpp               |Done    |Platform-related code removed |   |
|Core |duilib\Core\FrameworkThread.cpp and .h      |Done    |Platform-related code removed |   |
|Core |duilib\Core\CursorManager_Windows.cpp      |Done    |Cursor management  |CursorManager_SDL.cpp|
|Core |duilib\Core\DpiAwareness_Windows.cpp       |Done    |DPI awareness  |DpiAwareness_SDL.cpp|Not supported on other platforms yet|
|Core |duilib\Core\IconManager_Windows.h          |No action needed  |Windows platform only|  Not needed  |Not supported on other platforms|
|Core |duilib\Core\IconManager_Windows.cpp        |No action needed  |Windows platform only|  Not needed  |Not supported on other platforms|
|Core |duilib\Core\Keyboard_Windows.cpp           |Done    |Keyboard related   |Keyboard_SDL.cpp<br>Keycode_SDL.cpp|
|Core |duilib\Core\NativeWindow_Windows.cpp       |Done    |Window management   |NativeWindow_SDL.cpp|
|Core |duilib\Core\NativeWindow_Windows.h         |Done    |Window management   |NativeWindow_SDL.h|
|Core |duilib\Core\ToolTip_Windows.cpp            |Done    |ToolTip functionality|ToolTip_SDL.cpp|Not supported on other platforms yet|
|Core |duilib\Core\WindowDropTarget_Windows.cpp   |Done    |Drag-and-drop functionality   |WindowDropTarget_SDL.cpp|Not supported on other platforms yet|
|Core |duilib\Core\ThreadMessage_Windows.cpp      |Done    |Inter-thread communication |ThreadMessage_SDL.cpp|
|Core |duilib\Core\MessageLoop_Windows.h          |Done    |Message loop   |MessageLoop_SDL.h|
|Core |duilib\Core\MessageLoop_Windows.cpp        |Done    |Message loop   |MessageLoop_SDL.cpp|
|RenderSkia |duilib\RenderSkia\Bitmap_Skia.h      |Done    |Platform-related code removed |Not needed|
|RenderSkia |duilib\RenderSkia\Bitmap_Skia.cpp    |Done    |Platform-related code removed |Not needed|
|RenderSkia |duilib\RenderSkia\Render_Skia.h      |Done    |Platform-related code removed |Not needed|
|RenderSkia |duilib\RenderSkia\Render_Skia.cpp    |Done    |Platform-related code removed |Not needed|
|RenderSkia |duilib\RenderSkia\Render_Skia_Windows.h            |Done    | Associates the rendering engine with the window|Render_Skia_SDL.h|
|RenderSkia |duilib\RenderSkia\Render_Skia_Windows.cpp          |Done    | Associates the rendering engine with the window|Render_Skia_SDL.cpp|
|RenderSkia |duilib\RenderSkia\SkRasterWindowContext_Windows.h  |Done    | Supports CPU rendering   |SkRasterWindowContext_SDL.h|
|RenderSkia |duilib\RenderSkia\SkRasterWindowContext_Windows.cpp|Done    | Supports CPU rendering   |SkRasterWindowContext_SDL.cpp|
|RenderSkia |duilib\RenderSkia\SkGLWindowContext_Windows.h      |Done    | Supports OpenGL rendering| Not supported yet, pending research  |
|RenderSkia |duilib\RenderSkia\SkGLWindowContext_Windows.cpp    |Done    | Supports OpenGL rendering| Not supported yet, pending research  |
|RenderSkia |skia\tools\gpu\gl\win\SkWGL_win.cpp                |Done    | Supports OpenGL rendering| Not supported yet, pending research  |

## RichEdit Basic Feature List and Development Status (positioning: replaces the Windows RichEdit control, but without the Rich feature)
| Feature          | Keyboard or mouse shortcut  | Development status |     Remarks     | 
| :---          | :---              | :---     | :---         |
|Text storage       | None                | Done     |Text is stored line by line; memory footprint and draw speed need to be controlled |
|Text modification       | None                | Done     |Incremental modification and incremental drawing; fast drawing without lag|
|Text selection               | Hold the left mouse button and move the mouse           | Done         |Selects the text between the click point and the current point |
|Text selection               | Shift + left mouse button click           | Done         |Selects the text from the caret to the click point |
|Text selection               | Shift + Left key                 | Done         |Selects the text from the caret, extending left character by character |
|Text selection               | Shift + Right key                | Done         |Selects the text from the caret, extending right character by character |
|Text selection               | Shift + Up key                   | Done         |Selects the text from the caret, extending up line by line |
|Text selection               | Shift + Down key                 | Done         |Selects the text from the caret, extending down line by line |
|Text selection               | Shift + Home key                 | Done         |Selects the text from the caret to the start of the line |
|Text selection               | Shift + End key                  | Done         |Selects the text from the caret to the end of the line |
|Text selection               | Shift + Page Up key              | Done         |Selects the text from the caret, extending up page by page |
|Text selection               | Shift + Page Down key            | Done         |Selects the text from the caret, extending down page by page |
|Caret positioning               | Left/right mouse button click              | Done     |              |
|Keep the caret in the visible area     |                   | Done     |              |
|Move the caret left one character   | Left key            | Done     |              |
|Move the caret right one character   | Right key          | Done     |              |
|Move the caret up one line       | Up key             | Done     |              |
|Move the caret down one line       | Down key           | Done     |              |
|Move the caret to the start of the line           | Home key           | Done     |              |
|Move the caret to the end of the line           | End key            | Done     |              |
|Move the caret up one page       | Page Up key        | Done     |              |
|Move the caret down one page       | Page Down key      | Done     |              |
|Move the caret left one word     | Ctrl + Left key     | Done     |              |
|Move the caret right one word     | Ctrl + Right key    | Done     |              |
|Move the caret to the start of the document     | Ctrl + Home key     | Done     |View and caret move together              |
|Move the caret to the end of the document     | Ctrl + End key      | Done     |View and caret move together              |
|Move the view to the row above     | Ctrl + Up key       | Done     |Caret position unchanged; i.e. the ScrollBox feature |
|Move the view to the row below     | Ctrl + Down key     | Done     |Caret position unchanged; i.e. the ScrollBox feature |
|Move the view up one page       | Ctrl + Page Up key  | Done     |Caret position unchanged; i.e. the ScrollBox feature |
|Move the view down one page       | Ctrl + Page Down key| Done     |Caret position unchanged; i.e. the ScrollBox feature |
|Delete backwards by character         | Delete            | Done     |              |
|Delete backwards by word           | Ctrl + Delete     | Done     |Deletes the next word or the selected characters|
|Delete forwards by character         | Backspace         | Done     |              |
|Delete forwards by word           | Ctrl + Backspace  | Done     |Deletes the previous word or the selected characters|
|Select all           | Ctrl+A                    | Done     |              |
|Copy           | Ctrl+C / Ctrl+Insert      | Done     |              |
|Paste           | Ctrl+V / Shift+Insert     | Done     |              |
|Cut           | Ctrl+X                    | Done     |              |
|Undo           | Ctrl+Z                    | Done     |              |
|Redo           | Ctrl+Y                    | Done     |              |
|Word wrap       | Ctrl+W                    | Done     | Toggle     |
|Zoom           | Ctrl + mouse wheel           | Done     |              |
|TAB key drawing    |                           | Done     | TAB corresponds to 4 fixed spaces |
|Horizontal text alignment support |                   | Done     |              |
|Vertical text alignment support |                   | Done     |              |
|Single-line/multi-line text|                         | Done     |              |
|Password mode features |                         | Done     |              |
|Numeric mode features |                         | Done     |              |
|Character limit feature   |                         | Done     |              |
|PromptText feature   |                         | Done     |              |
|Input character recognition and filtering|                      | Done     |With the latest SDL3.0 source, input characters work correctly|
|Other common features   |                           | Done     |              |
|Find feature       |                           | Done     |              |
|DPI adaptation feature  |                           | Done     |              |

## Some Linux Platform Adaptation and Testing
| Linux category             | System version                        |  Desktop type          | Progress    |     Remarks     | 
| :---                  | :---                            |  :---              |   :---  |    :---     | 
| OpenEuler             | 24.03 64-bit                      | UKUI (X11)        |Done   | Chinese input: normal <br>  On-screen keyboard input: normal <br> Black screen when a window pops up: No <br> Transparent window supported: Yes <br> Other issues: none   | 
| OpenEuler             | 24.03 64-bit                      | DDE (deepin, X11) |Done   | Chinese input: not tested; the Chinese input method could not be installed; the Russian input method works normally <br>  On-screen keyboard input: not tested; no on-screen keyboard found <br> Black screen when a window pops up: No <br> Transparent window supported: No; the window shadow renders black, so only the system title bar can actually be used <br> Other issues: none  | 
| Zhongke Fangde              | NFSDesktop-5.0-G230-240806 64-bit | X11                |Done   | Chinese input: normal <br>  On-screen keyboard input: normal <br> Black screen when a window pops up: No <br> Transparent window supported: Yes <br> Occasional crashes related to windows, e.g. move_control | 
| OpenKylin | Desktop-V2.0 64-bit               |Wayland             |Done   | Chinese input: normal <br>  On-screen keyboard input: normal <br> Black screen when a window pops up: No <br> Transparent window supported: Yes <br> The program actually uses X11 <br>Sub-window focus has issues, affecting some features such as Combo and mouse-drag control operations (when a sub-window pops up, the parent window loses focus, causing flow errors; cause unknown)<br>| 
| UbuntuKylin | ubuntukylin-24.10-desktop 64-bit  | X11                |Done   | Chinese input: normal <br>  On-screen keyboard input: normal <br> Black screen when a window pops up: No <br> Transparent window supported: Yes | 
| UOS                   |desktop-20 1070                  | X11                |Done   | Chinese input: normal <br>  On-screen keyboard input: normal <br> Black screen when a window pops up: No <br> Transparent window supported: Yes <br> Other issues: the property grid control's system file/folder selection dialogs cannot pop up | 
| Ubuntu                | 24.04.1 64-bit                    | GNOME (Wayland)   |Done   | Chinese input: normal <br>  On-screen keyboard input: not tested <br> Black screen when a window pops up: No <br> Transparent window supported: Yes <br>The program actually uses X11<br>It can run if forced to use Wayland, but with poor results: no DPI scaling, and the UI is blurry at high DPI| 
| Debian                | 12.7.0 64-bit                     | GNOME (Wayland)   |Done   | Chinese input: normal <br>  On-screen keyboard input: not tested <br> Black screen when a window pops up: No <br> Transparent window supported: Yes <br>The program actually uses X11<br> The ListCtrl system title bar has only a close button; no minimize or maximize buttons| 
| Fedora                | Fedora Workstation 40 64-bit      | GNOME (Wayland)   |Done   | Chinese input: normal <br>  On-screen keyboard input: not tested <br> Black screen when a window pops up: No <br> Transparent window supported: Yes <br>The program actually uses X11<br> The ListCtrl system title bar has only a close button; no minimize or maximize buttons| 
| OpenSUSE              | OpenSUSE-Leap-15.6 64-bit         | KDE (X11)         |Done   | Chinese input: normal <br>  On-screen keyboard input: normal <br> Black screen when a window pops up: No <br> Transparent window supported: No; the window shadow renders black, so only the system title bar can actually be used <br> Other issues: when closing a window, the screen flashes black fullscreen before the window closes | 

Known issues:    
1. Custom (semi-transparent, shadowed) title bar: double-clicking the title bar cannot maximize/restore (Linux only; works normally on Windows) (a common issue that needs SDL-level support to fix)
2. Black screen when resizing windows (too many messages, so paint messages cannot be handled in time; Ubuntu has this issue, and it occurs on most systems to some degree; a common issue that needs SDL-level support to fix)
3. After a maximized window is restored, it does not return to the window position it had before maximizing (cause to be determined)
4. X11 windows are essentially fully tested, but Wayland windows have not been tested yet
