#ifndef UI_CORE_WINDOW_BASE_H_
#define UI_CORE_WINDOW_BASE_H_

#include "duilib/Core/INativeWindow.h"
#include "duilib/Core/ControlPtrT.h"
#include "duilib/Utils/FilePath.h"
#include "duilib/Core/EventArgs.h"

#if defined (DUILIB_BUILD_FOR_SDL) || defined (DUILIB_BUILD_FOR_WAYLAND)
    #include "duilib/Core/NativeWindow_SDL.h"
#elif defined (DUILIB_BUILD_FOR_WIN)
    #include "duilib/Core/NativeWindow_Windows.h"
#else
    class NativeWindow;
#endif

namespace ui
{
class IRender;
class WindowCreateParam;
class WindowCreateAttributes;

/** Basic window functionality wrapper (platform-related window functionality wrapper)
*/
class DUILIB_API WindowBase: public INativeWindow
{
public:
    WindowBase();
    WindowBase(const WindowBase& r) = delete;
    WindowBase& operator=(const WindowBase& r) = delete;
    virtual ~WindowBase() override;

public:
    /** Create the window; the OnInitWindow interface can be used to implement custom requirements after window creation
    *   Note: after a subclass Window object is created successfully, the Window::OnFinalMessage function will be called to delete itself when the window is destroyed.
    * @param [in] pParentWindow The parent window
    * @param [in] createParam The parameters required to create the window
    */
    bool CreateWnd(WindowBase* pParentWindow, const WindowCreateParam& createParam);

    /** Show a modal window; the OnInitWindow interface can be used to implement custom requirements after window creation
    *   Note:
    *       (1) DoModal is blocking; after the window is displayed, the function does not return until the window is destroyed
    *       (2) The DoModal window will not destroy itself (including the Window subclass) when the window is destroyed; the object's lifetime needs to be managed by yourself
    * @param [in] pParentWindow The parent window
    * @param [in] createParam The parameters required to create the window
    * @param [in] bCloseByEsc Whether to close the window when the ESC key is pressed
    * @param [in] bCloseByEnter Whether to close the window when the Enter key is pressed
    * @return The return value when the window exits; returns -1 on failure
    */
    int32_t DoModal(WindowBase* pParentWindow, const WindowCreateParam& createParam,
                    bool bCloseByEsc = true, bool bCloseByEnter = false);

    /** Create a child window (a non-popup child window)
    * @param [in] pParentWindow The parent window
    * @param [in] nX The X coordinate of the child window (relative to the parent window)
    * @param [in] nY The Y coordinate of the child window (relative to the parent window)
    * @param [in] nWidth The width of the child window
    * @param [in] nHeight The height of the child window
    */
    bool CreateChildWnd(WindowBase* pParentWindow, int32_t nX, int32_t nY, int32_t nWidth, int32_t nHeight);

    /** Whether the window has a valid window handle
    */
    bool IsWindow() const;

    /** Whether the current window is a child window (a child window that is not of the popup window type; for the Windows system, it is a window with only the WS_CHILD style)
    */
    bool IsChildWindow() const;

    /** Get the parent window
    */
    WindowBase* GetParentWindow() const;

    /** Set or change the parent window
    */
    bool SetParentWindow(WindowBase* pParentWindow);

    /** Get the implementation interface of the window
    */
    NativeWindow* NativeWnd() const;

    /** Set whether to use the system title bar
    */
    void SetUseSystemCaption(bool bUseSystemCaption);

    /** Get whether the system title bar is used
    */
    bool IsUseSystemCaption() const;

    /** Set whether it is a layered window
    * @param [in] bIsLayeredWindow true means set as a layered window, otherwise set as a non-layered window
    * @param [in] bRedraw Whether to redraw the window (after an attribute change, if not redrawn, the interface may display abnormally)
    */
    bool SetLayeredWindow(bool bIsLayeredWindow, bool bRedraw);

    /** Whether it is a layered window
    */
    bool IsLayeredWindow() const;

    /** Set the window transparency (valid only when IsLayeredWindow() is true)
    * @param [in] nAlpha The transparency value [0, 255]; when nAlpha is 0, the window is fully transparent. When nAlpha is 255, the window is opaque.
    *             This parameter is used as an argument in the UpdateLayeredWindow function (BLENDFUNCTION.SourceConstantAlpha).
    */
    void SetLayeredWindowAlpha(int32_t nAlpha);

    /** Get the window transparency (valid only when IsLayeredWindow() is true)
    * @param [in] nAlpha The transparency value [0, 255]; when nAlpha is 0, the window is fully transparent. When nAlpha is 255, the window is opaque.
    */
    uint8_t GetLayeredWindowAlpha() const;

    /** Set the window opacity (valid only when IsLayeredWindow() is true, so if the current window is not a layered window, it will automatically be set as a layered window internally)
    * @param [in] nAlpha The transparency value [0, 255]; when nAlpha is 0, the window is fully transparent. When nAlpha is 255, the window is opaque.
    *             This parameter is used as an argument in the SetLayeredWindowAttributes function (bAlpha).
    */
    void SetLayeredWindowOpacity(int32_t nAlpha);

    /** Get the window opacity (valid only when IsLayeredWindow() is true)
    * @param [in] nAlpha The transparency value [0, 255]; when nAlpha is 0, the window is fully transparent. When nAlpha is 255, the window is opaque.
    */
    uint8_t GetLayeredWindowOpacity() const;

    /** Close the window, asynchronously; after the function returns, the IsClosing() state is true
    * @param [in] nRet The close parameter; see: enum WindowCloseParam
    */
    void CloseWnd(int32_t nRet = kWindowCloseNormal);

    /** Close the window, synchronously
    */
    void Close();

    /** Whether the window is about to close
    */
    bool IsClosingWnd() const;

    /** Get the window close parameter
    * @return See enum WindowCloseParam; it may also be a custom value
    */
    int32_t GetCloseParam() const;

public:
    /** @name Interfaces related to window state such as display, hide, size, position
    * @{
    */
    /** Show, hide, maximize, restore, and minimize the window
     * @param [in] nCmdShow The command to show or hide the window
    */
    bool ShowWindow(ShowWindowCommands nCmdShow);

    /** Show a modal dialog (the parent window is specified when it is created; the parent window is set to the Disable state)
    */
    void ShowModalFake();

    /** Whether it is a simulated modal display window (a window displayed through the ShowModalFake function)
    */
    bool IsFakeModal() const;

    /** Whether it is in modal dialog mode (a dialog window displayed through the DoModal function)
    */
    bool IsDoModal() const;

    /** Center the window; supports extended screens
    */
    void CenterWindow();

    /** Set whether the window is a topmost window
    * @param [in] bOnTop true means set as a topmost window, false means cancel the topmost window
    */
    void SetWindowAlwaysOnTop(bool bOnTop);

    /** Determine whether the current window is a topmost window
    * @return true means the current window is a topmost window, false means it is not
    */
    bool IsWindowAlwaysOnTop() const;

    /** Set the window as the foreground window
    */
    bool SetWindowForeground();

    /** Whether the current window is the foreground window
    */
    bool IsWindowForeground() const;

    /** Set the window as the focus window
    */
    bool SetWindowFocus();

    /** Make the window lose focus
    */
    bool KillWindowFocus();

    /** Whether the current window is the input focus window
    */
    bool IsWindowFocused() const;

    /** Check and ensure the current window is the focus window
    */
    void CheckSetWindowFocus();

    /** Put the window into fullscreen state
    */
    bool EnterFullscreen();

    /** Make the window exit fullscreen state (by default, pressing the ESC key exits fullscreen)
    */
    bool ExitFullscreen();

    /** Whether the window is maximized
    */
    bool IsWindowMaximized() const;

    /** Whether the window is minimized
    */
    bool IsWindowMinimized() const;

    /** Whether the window is in fullscreen state
    */
    bool IsWindowFullscreen() const;

     /** Set the Enable state of the window
    * @param [in] bEnable true means set to the Enable state, false means set to the disable state
    */
    bool EnableWindow(bool bEnable);

    /** Get the Enable state of the window
    */
    bool IsWindowEnabled() const;

    /** Whether the window is visible
    */
    bool IsWindowVisible() const;

    /** Set the window position (a thin wrapper around the ::SetWindowPos API; no DPI scaling inside)
    * @param [in] insertAfter Corresponds to the hWndInsertAfter option of SetWindowPos
    * @param [in] X The X coordinate of the window
    * @param [in] Y The Y coordinate of the window
    * @param [in] cx The width of the window
    * @param [in] cy The height of the window
    * @param [in] uFlags Refer to the enum WindowPosFlags options
    */
    bool SetWindowPos(const InsertAfterWnd& insertAfter, int32_t X, int32_t Y, int32_t cx, int32_t cy, uint32_t uFlags);

    /** Set the window position and size
    * @param [in] X The X coordinate of the window
    * @param [in] Y The Y coordinate of the window
    * @param [in] nWidth The width of the window
    * @param [in] nHeight The height of the window
    * @param [in] bRepaint Whether to redraw the window
    */
    bool MoveWindow(int32_t X, int32_t Y, int32_t nWidth, int32_t nHeight, bool bRepaint);

    /** Get the window position information
     * @param [in] bContainShadow Whether to include the shadow; true means include, the default is false, not included
     */
    UiRect GetWindowPos(bool bContainShadow /*= false*/) const;

    /** Reset the window size
    * @param [in] cx The width
    * @param [in] cy The height
    * @param [in] bContainShadow false means cx and cy do not include the shadow
    * @param [in] bNeedDpiScale false means do not adjust according to DPI
    */
    void Resize(int cx, int cy, bool bContainShadow /*= false*/, bool bNeedDpiScale);

    /** @} */

    /** Set the window icon (supports *.ico format; other formats are also supported, but the ICO format is recommended)
    *  @param [in] iconFilePath The path of the icon file (absolute path)
    */
    bool SetWindowIcon(const FilePath& iconFilePath);

    /** Set the window icon (supports *.ico format; other formats are also supported, but the ICO format is recommended)
    *  @param [in] iconFileData The data of the icon file
    *  @param [in] iconFileName The file name including the extension, used to identify the image type
    */
    bool SetWindowIcon(const std::vector<uint8_t>& iconFileData, const DString& iconFileName);

    /** Set the window title bar text
    * @param [in] strText The window title bar text
    */
    void SetText(const DString& strText);

    /** Get the window title bar text
    */
    DString GetText() const;

    /** Set the window title bar text according to the text ID in the language list
    * @param [in] strTextId The language ID; this ID must exist in the language file
    */
    void SetTextId(const DString& strTextId);

    /** Get the text ID of the window title bar text
    */
    const DString& GetTextId() const;

    /** Get the window ID
    */
    const DString& GetWindowId() const;

    /** Set the window ID
    */
    void SetWindowId(const DString& windowId);

    /** Get the Class name of the window
    */
    const DString& GetWindowClassName() const;

    /** Get the DPI manager corresponding to this window
    */
    const DpiManager& Dpi() const;

    /** Actively adjust the interface display scale of the window
    *   Note: this function can change the display scale of the window, thereby changing the interface percentage of the window and controls.
    * @param [in] nNewDisplayScaleFactor The new interface display scale value; for example, 100 means the interface DPI scale ratio is 100%
    * @param [in] bDisableDpiAware true means DPI awareness is disabled; when the monitor's DPI changes, the window's DPI no longer follows the change
                                   false means DPI awareness is still enabled; when the monitor's DPI changes, it still follows the change
    */
    bool ChangeDisplayScale(uint32_t nNewDisplayScaleFactor, bool bDisableDpiAware = true);

    /** Send a redraw message
    * @param [in] rcItem The redraw range, in client area coordinates
    */
    void Invalidate(const UiRect& rcItem);

    /** Update the window and perform the redraw
    */
    bool UpdateWindow() const;

    /** Keep the parent window active
    */
    void KeepParentActive();

public:
    /** @name Interfaces related to the window size, title bar, etc.
    * @{
    */

    /** Get the size of the resizable range on the four sides of the window
    */
    const UiRect& GetSizeBox() const;

    /** Set the size of the resizable range on the four sides of the window
    * @param [in] rcSizeBox The size to set
    * @param [in] bNeedDpiScale Whether DPI scaling is supported
    */
    void SetSizeBox(const UiRect& rcSizeBox, bool bNeedDpiScale);

    /** Get the window title bar area (draggable area), corresponding to the caption attribute in XML
    */
    const UiRect& GetCaptionRect() const;

    /** Set the window title bar area, corresponding to the caption attribute in XML
    * @param [in] rcCaption The rectangular range of the title bar area to set (rcClient represents the client area rectangle range of the window):
    *              The left of the title bar  : rcClient.left + rcCaption.left
    *              The top of the title bar   : rcClient.top + rcCaption.top
    *              The right of the title bar : rcClient.right - rcCaption.right
    *              The bottom of the title bar: rcClient.top + rcCaption.bottom
    * @param [in] bNeedDpiScale false means do not adjust according to DPI
    */
    void SetCaptionRect(const UiRect& rcCaption, bool bNeedDpiScale);

    /** Get the window menu area (double-clicking this area exits the window; clicking displays the system window menu)
    */
    const UiRect& GetSysMenuRect() const;

    /** Set the window menu area (double-clicking this area exits the window; clicking displays the system window menu)
    * @param [in] rcSysMenuRect The area range to set
    * @param [in] bNeedDpiScale false means do not adjust according to DPI
    */
    void SetSysMenuRect(const UiRect& rcSysMenuRect, bool bNeedDpiScale);

    /** Set whether to support displaying the snap layout menu (a new Windows 11 feature: alignment layouts can be easily accessed by hovering the mouse over the maximize button of the window or pressing Win + Z.)
    *   This feature is enabled by default.
    * @param [in] bEnable true means supported, false means not supported
    */
    void SetEnableSnapLayoutMenu(bool bEnable);

    /** Determine whether displaying the snap layout menu is supported (only supported on Windows 11 and later versions)
    */
    bool IsEnableSnapLayoutMenu() const;

    /** Set whether to display the system window menu when the title bar is right-clicked (operations such as adjusting the window state and closing the window are available)
    * @param [in] bEnable true means supported, false means not supported
    */
    void SetEnableSysMenu(bool bEnable);

    /** Get whether the system window menu is displayed when the title bar is right-clicked
    */
    bool IsEnableSysMenu() const;

    /** Get the window round corner size, corresponding to the roundcorner attribute in XML
    */
    const UiSize& GetRoundCorner() const;

    /** Set the window round corner size
    * @param [in] cx The round corner width
    * @param [in] cy The round corner height
    * @param [in] bNeedDpiScale false means do not adjust according to DPI
    */
    void SetRoundCorner(int cx, int cy, bool bNeedDpiScale);

    /** Set the minimum value of the window size (width and height)
    * @param [in] szMinWindow The minimum width and minimum height of the window; a value of 0 means no restriction
    * @param [in] bNeedDpiScale false means no automatic adjustment according to DPI is needed
    */
    void SetWindowMaximumSize(const UiSize& szMinWindow, bool bNeedDpiScale);

    /** Get the minimum value of the window size (width and height)
    */
    const UiSize& GetWindowMaximumSize() const;

    /** Set the maximum value of the window size (width and height)
    * @param [in] szMaxWindow The maximum width and minimum height of the window; a value of 0 means no restriction
    * @param [in] bNeedDpiScale false means no automatic adjustment according to DPI is needed
    */
    void SetWindowMinimumSize(const UiSize& szMaxWindow, bool bNeedDpiScale);

    /** Get the maximum value of the window size (width and height)
    */
    const UiSize& GetWindowMinimumSize() const;

    /** @}*/

public:
    /** @name Interfaces related to window messages
    * @{ */

    /** Add a message filter; the message has already been dispatched, and the message handling priority of this interface is higher than the message handling function of the Window class
    * @param [in] pFilter An object instance that inherits IUIMessageFilter, which needs to implement the methods in the interface
    */
    bool AddMessageFilter(IUIMessageFilter* pFilter);

    /** Remove a message filter
    * @param [in] pFilter An object instance that inherits IUIMessageFilter
    */
    bool RemoveMessageFilter(IUIMessageFilter* pFilter);

    /** Post a message to the message queue
    * @param [in] uMsg The message type
    * @param [in] wParam Additional message parameters
    * @param [in] lParam Additional message parameters
    * @return Returns the window's handling result for the message
    */
    LRESULT PostMsg(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0L);

    /** Send a quit message to the message queue
    * @param [in] nExitCode The exit code
    */
    static void PostQuitMsg(int32_t nExitCode);

    /** @}*/

public:
    /** Get the client area rectangle of the current window
    * @param [out] rcClient Returns the client area coordinates of the window
    */
    void GetClientRect(UiRect& rcClient) const;

    /** Get the window area rectangle of the current window
    * @param [out] rcWindow Returns the screen coordinates of the top-left and bottom-right corners of the window
    */
    void GetWindowRect(UiRect& rcWindow) const;

    /** Convert screen coordinates to client area coordinates of the current window
    * @param [out] pt Returns the client area coordinates
    */
    void ScreenToClient(UiPoint& pt) const;

    /** Convert the client area coordinates of the current window to screen coordinates
    * @param [out] pt Returns the screen coordinates
    */
    void ClientToScreen(UiPoint& pt) const;

    /* Convert the rc coordinates from client area coordinates to screen coordinates
    * @param [out] rc Returns the screen coordinates
    */
    void ClientToScreen(UiRect& rc) const;

    /* Convert the rc coordinates from screen coordinates to client area coordinates
    * @param [out] rc Returns the client area coordinates
    */
    void ScreenToClient(UiRect& rc) const;

    /** Get the current mouse position
    * @param [out] pt Returns the screen coordinates
    */
    void GetCursorPos(UiPoint& pt) const;

    /** Get the monitor rectangle of the monitor where the specified window is located
    * @param [out] rcMonitor The rectangular area of the monitor
    */
    bool GetMonitorRect(UiRect& rcMonitor) const;

    /** Get the work area rectangle of the current primary monitor
    * @param [out] rcWork Returns the primary screen coordinates
    */
    static bool GetPrimaryMonitorWorkRect(UiRect& rcWork);

    /** Get the work area rectangle of the monitor where the current window is located, expressed in virtual screen coordinates.
        Note that if the monitor is not the primary monitor, some rectangle coordinates may be negative.
    * @param [out] rcWork Returns the screen coordinates
    */
    bool GetMonitorWorkRect(UiRect& rcWork) const;

    /** Get the work area rectangle of the monitor where the specified point is located, expressed in virtual screen coordinates.
        Note that if the monitor is not the primary monitor, some rectangle coordinates may be negative.
    * @param [out] pt Input as screen coordinates
    * @param [out] rcWork Returns the screen coordinates
    */
    bool GetMonitorWorkRect(const UiPoint& pt, UiRect& rcWork) const;

    /** Set the mouse window handle to be captured as the current drawing window
    * @param [in]
    */
    void SetCapture();

    /** Release resources when the window no longer needs mouse input
    */
    void ReleaseCapture();

    /** Determine whether mouse input is currently captured
    */
    bool IsCaptured() const;

public:
    /** Set the system hotkey for activating the window; after registration, pressing this hotkey automatically activates this window
    * @param [in] wVirtualKeyCode The virtual key code, e.g., kVK_DOWN, etc.; see: https://learn.microsoft.com/zh-cn/windows/win32/inputdev/virtual-key-codes
    *             If wVirtualKeyCode is 0, it means cancel the management of the window activation hotkey
    * @param [in] wModifiers The hotkey modifier flags; see the values of the HotKeyModifiers enum type
    * @return Description of the return value:
       -1: The function was unsuccessful; the hotkey is invalid.
        0: The function was unsuccessful; the window is invalid.
        1: The function was successful, and no other window has the same hotkey.
        2: The function was successful, but another window already has the same hotkey.
    */
    int32_t SetWindowHotKey(uint8_t wVirtualKeyCode, uint8_t wModifiers);

    /** Get the system hotkey for activating the window
    * @param [out] wVirtualKeyCode The virtual key code, e.g., kVK_DOWN, etc.
    * @param [out] wModifiers The hotkey modifier flags; see the values of the HotKeyModifiers enum type
    * @return If false is returned, no window activation hotkey is registered; otherwise, a window activation hotkey is registered
    */
    bool GetWindowHotKey(uint8_t& wVirtualKeyCode, uint8_t& wModifiers) const;

    /** Register a system global hotkey; after successful registration, pressing this hotkey makes the window receive the WM_HOTKEY message
    * @param [in] wVirtualKeyCode The virtual key code, e.g., kVK_DOWN, etc.
    * @param [in] wModifiers The hotkey modifier flags; see the values of the HotKeyModifiers enum type
    * @param [in] id The command ID; the application must specify an ID value in the range of 0x0000 to 0xBFFF.
                  To avoid conflicts with hotkey identifiers defined by other shared DLLs, the DLL should use the GlobalAddAtom function to obtain a hotkey identifier.
    */
    bool RegisterHotKey(uint8_t wVirtualKeyCode, uint8_t wModifiers, int32_t id);

    /** Unregister a system global hotkey
    * @param [in] id The command ID, i.e., the command ID used at registration
    */
    bool UnregisterHotKey(int32_t id);

    /** Get the last mouse position
    */
    const UiPoint& GetLastMousePos() const;

    /** Set the last mouse position
    */
    void SetLastMousePos(const UiPoint& pt);

    /** Get the window handle (on the Windows platform, the window handle HWND is returned; in the SDL implementation, SDL_Window* is returned)
    */
    void* GetWindowHandle() const;

#ifdef DUILIB_BUILD_FOR_SDL
    /** Get the driver name of the current window implementation
    */
    DString GetVideoDriverName() const;

    /** Get the name of the current Render drawing engine
    */
    DString GetWindowRenderName() const;
#endif

    /** Whether the interface has completed its first display
    */
    bool IsWindowFirstShown() const;

public:
    /** Listen for the window creation event
    * @param [in] callback The specified callback function; wParam of 1 means a modal dialog displayed through the DoModal function, wParam of 0 means a normal window
    * @param [in] callbackID The ID corresponding to this callback function (used to delete the callback function)
    */
    void AttachWindowCreateMsg(const EventCallback& callback, EventCallbackID callbackID = 0);

    /** Listen for the window close event
    * @param [in] callback The specified callback function; wParam is the exit code when the window closes
    * @param [in] callbackID The ID corresponding to this callback function (used to delete the callback function)
    */
    void AttachWindowCloseMsg(const EventCallback& callback, EventCallbackID callbackID = 0);

    /** Listen for the first display event of the window (the interface layout has been completed)
    * @param [in] callback The specified callback function
    * @param [in] callbackID The ID corresponding to this callback function (used to delete the callback function)
    */
    void AttachWindowFirstShown(const EventCallback& callback, EventCallbackID callbackID = 0);

    /** Listen for the window display attribute event
    * @param [in] callback The specified callback function; wParam of 1 means shown, wParam of 0 means hidden
    * @param [in] callbackID The ID corresponding to this callback function (used to delete the callback function)
    */
    void AttachWindowShowWindowMsg(const EventCallback& callback, EventCallbackID callbackID = 0);

    /** Listen for the window paint event
    * @param [in] callback The specified callback function
    * @param [in] callbackID The ID corresponding to this callback function (used to delete the callback function)
    */
    void AttachWindowPaintMsg(const EventCallback& callback, EventCallbackID callbackID = 0);

    /** Listen for the window enter fullscreen event
    * @param [in] callback The specified callback function
    * @param [in] callbackID The ID corresponding to this callback function (used to delete the callback function)
    */
    void AttachWindowEnterFullscreenMsg(const EventCallback& callback, EventCallbackID callbackID = 0);

    /** Listen for the window exit fullscreen event
    * @param [in] callback The specified callback function
    * @param [in] callbackID The ID corresponding to this callback function (used to delete the callback function)
    */
    void AttachWindowExitFullscreenMsg(const EventCallback& callback, EventCallbackID callbackID = 0);

    /** Listen for the window maximize event
    * @param [in] callback The specified callback function
    * @param [in] callbackID The ID corresponding to this callback function (used to delete the callback function)
    */
    void AttachWindowMaximizedMsg(const EventCallback& callback, EventCallbackID callbackID = 0);

    /** Listen for the window minimize event
    * @param [in] callback The specified callback function
    * @param [in] callbackID The ID corresponding to this callback function (used to delete the callback function)
    */
    void AttachWindowMinimizedMsg(const EventCallback& callback, EventCallbackID callbackID = 0);

    /** Listen for the window restore event
    * @param [in] callback The specified callback function
    * @param [in] callbackID The ID corresponding to this callback function (used to delete the callback function)
    */
    void AttachWindowRestoredMsg(const EventCallback& callback, EventCallbackID callbackID = 0);

    /** Listen for the window position and size change event
    * @param [in] callback The specified callback function
    * @param [in] callbackID The ID corresponding to this callback function (used to delete the callback function)
    */
    void AttachWindowPosChangedMsg(const EventCallback& callback, EventCallbackID callbackID = 0);

    /** Listen for the window size change event
    * @param [in] callback The specified callback function; wParam is a value of the WindowSizeType type
    * @param [in] callbackID The ID corresponding to this callback function (used to delete the callback function)
    */
    void AttachWindowSizeMsg(const EventCallback& callback, EventCallbackID callbackID = 0);

    /** Listen for the window position change event
    * @param [in] callback The specified callback function
    * @param [in] callbackID The ID corresponding to this callback function (used to delete the callback function)
    */
    void AttachWindowMoveMsg(const EventCallback& callback, EventCallbackID callbackID = 0);

    /** Listen for the window gaining focus event
    * @param [in] callback The specified callback function; wParam is the pointer to the window that lost focus (WindowBase*)
    * @param [in] callbackID The ID corresponding to this callback function (used to delete the callback function)
    */
    void AttachWindowSetFocusMsg(const EventCallback& callback, EventCallbackID callbackID = 0);

    /** Listen for the window losing focus event
    * @param [in] callback The specified callback function; wParam is the pointer to the window that gained focus (WindowBase*)
    * @param [in] callbackID The ID corresponding to this callback function (used to delete the callback function)
    */
    void AttachWindowKillFocusMsg(const EventCallback& callback, EventCallbackID callbackID = 0);

    /** Listen for the window cursor set event
    * @param [in] callback The specified callback function
    * @param [in] callbackID The ID corresponding to this callback function (used to delete the callback function)
    */
    void AttachWindowSetCursorMsg(const EventCallback& callback, EventCallbackID callbackID = 0);

    /** Listen for the window keyboard key-down event
    * @param [in] callback The specified callback function: vkCode is the virtual key code, modifierKey is the modifier flags; valid values: ModifierKey::kFirstPress, ModifierKey::kAlt
    * @param [in] callbackID The ID corresponding to this callback function (used to delete the callback function)
    */
    void AttachWindowKeyDownMsg(const EventCallback& callback, EventCallbackID callbackID = 0);

    /** Listen for the window keyboard key-up event
    * @param [in] callback The specified callback function: vkCode is the virtual key code, modifierKey is the modifier flags; valid values: ModifierKey::kAlt
    * @param [in] callbackID The ID corresponding to this callback function (used to delete the callback function)
    */
    void AttachWindowKeyUpMsg(const EventCallback& callback, EventCallbackID callbackID = 0);

    /** Listen for the window mouse wheel event
    * @param [in] callback The specified callback function: ptMouse is the mouse position
    *                                    modifierKey is the modifier flags; valid values: ModifierKey::kControl, ModifierKey::kShift
    *                                    eventData is the distance the wheel has rotated; a positive value means the wheel rotated forward (away from the user); a negative value means the wheel rotated backward (toward the user)
    * @param [in] callbackID The ID corresponding to this callback function (used to delete the callback function)
    */
    void AttachWindowMouseWheelMsg(const EventCallback& callback, EventCallbackID callbackID = 0);

    /** Listen for the window mouse move event
    * @param [in] callback The specified callback function: ptMouse is the mouse position, modifierKey is the modifier flags; valid values: ModifierKey::kControl, ModifierKey::kShift
    * @param [in] callbackID The ID corresponding to this callback function (used to delete the callback function)
    */
    void AttachWindowMouseMoveMsg(const EventCallback& callback, EventCallbackID callbackID = 0);

    /** Listen for the window mouse hover event
    * @param [in] callback The specified callback function: ptMouse is the mouse position, modifierKey is the modifier flags; valid values: ModifierKey::kControl, ModifierKey::kShift
    * @param [in] callbackID The ID corresponding to this callback function (used to delete the callback function)
    */
    void AttachWindowMouseHoverMsg(const EventCallback& callback, EventCallbackID callbackID = 0);

    /** Listen for the window mouse leave event
    * @param [in] callback The specified callback function
    * @param [in] callbackID The ID corresponding to this callback function (used to delete the callback function)
    */
    void AttachWindowMouseLeaveMsg(const EventCallback& callback, EventCallbackID callbackID = 0);

    /** Listen for the window mouse left button down event
    * @param [in] callback The specified callback function: ptMouse is the mouse position, modifierKey is the modifier flags; valid values: ModifierKey::kControl, ModifierKey::kShift
    * @param [in] callbackID The ID corresponding to this callback function (used to delete the callback function)
    */
    void AttachWindowLButtonDownMsg(const EventCallback& callback, EventCallbackID callbackID = 0);

    /** Listen for the window mouse left button up event
    * @param [in] callback The specified callback function: ptMouse is the mouse position, modifierKey is the modifier flags; valid values: ModifierKey::kControl, ModifierKey::kShift
    * @param [in] callbackID The ID corresponding to this callback function (used to delete the callback function)
    */
    void AttachWindowLButtonUpMsg(const EventCallback& callback, EventCallbackID callbackID = 0);

    /** Listen for the window mouse left button double-click event
    * @param [in] callback The specified callback function: ptMouse is the mouse position, modifierKey is the modifier flags; valid values: ModifierKey::kControl, ModifierKey::kShift
    * @param [in] callbackID The ID corresponding to this callback function (used to delete the callback function)
    */
    void AttachWindowLButtonDbClickMsg(const EventCallback& callback, EventCallbackID callbackID = 0);

    /** Listen for the window mouse right button down event
    * @param [in] callback The specified callback function: ptMouse is the mouse position, modifierKey is the modifier flags; valid values: ModifierKey::kControl, ModifierKey::kShift
    * @param [in] callbackID The ID corresponding to this callback function (used to delete the callback function)
    */
    void AttachWindowRButtonDownMsg(const EventCallback& callback, EventCallbackID callbackID = 0);

    /** Listen for the window mouse right button up event
    * @param [in] callback The specified callback function: ptMouse is the mouse position, modifierKey is the modifier flags; valid values: ModifierKey::kControl, ModifierKey::kShift
    * @param [in] callbackID The ID corresponding to this callback function (used to delete the callback function)
    */
    void AttachWindowRButtonUpMsg(const EventCallback& callback, EventCallbackID callbackID = 0);

    /** Listen for the window mouse right button double-click event
    * @param [in] callback The specified callback function: ptMouse is the mouse position, modifierKey is the modifier flags; valid values: ModifierKey::kControl, ModifierKey::kShift
    * @param [in] callbackID The ID corresponding to this callback function (used to delete the callback function)
    */
    void AttachWindowRButtonDbClickMsg(const EventCallback& callback, EventCallbackID callbackID = 0);

    /** Listen for the window mouse middle button down event
    * @param [in] callback The specified callback function: ptMouse is the mouse position, modifierKey is the modifier flags; valid values: ModifierKey::kControl, ModifierKey::kShift
    * @param [in] callbackID The ID corresponding to this callback function (used to delete the callback function)
    */
    void AttachWindowMButtonDownMsg(const EventCallback& callback, EventCallbackID callbackID = 0);

    /** Listen for the window mouse middle button up event
    * @param [in] callback The specified callback function: ptMouse is the mouse position, modifierKey is the modifier flags; valid values: ModifierKey::kControl, ModifierKey::kShift
    * @param [in] callbackID The ID corresponding to this callback function (used to delete the callback function)
    */
    void AttachWindowMButtonUpMsg(const EventCallback& callback, EventCallbackID callbackID = 0);

    /** Listen for the window mouse middle button double-click event
    * @param [in] callback The specified callback function: ptMouse is the mouse position, modifierKey is the modifier flags; valid values: ModifierKey::kControl, ModifierKey::kShift
    * @param [in] callbackID The ID corresponding to this callback function (used to delete the callback function)
    */
    void AttachWindowMButtonDbClickMsg(const EventCallback& callback, EventCallbackID callbackID = 0);

    /** Listen for the window losing mouse capture event
    * @param [in] callback The specified callback function
    * @param [in] callbackID The ID corresponding to this callback function (used to delete the callback function)
    */
    void AttachWindowCaptureChangedMsg(const EventCallback& callback, EventCallbackID callbackID = 0);

    /** Listen for the window drag-and-drop operation events
    * @param [in] callback The specified callback function
    * @param [in] callbackID The ID corresponding to this callback function (used to delete the callback function)
    */
    void AttachWindowDropEnterMsg(const EventCallback& callback, EventCallbackID callbackID = 0);
    void AttachWindowDropOverMsg(const EventCallback& callback, EventCallbackID callbackID = 0);
    void AttachWindowDropMsg(const EventCallback& callback, EventCallbackID callbackID = 0);
    void AttachWindowDropLeaveMsg(const EventCallback& callback, EventCallbackID callbackID = 0);

    /** Listen for the window DPI change event
    * @param [in] callback The specified callback function; the value of wParam is a pointer type: WindowDisplayScaleData*, containing DPI data
    * @param [in] callbackID The ID corresponding to this callback function (used to delete the callback function)
    */
    void AttachWindowDisplayScaleChangedMsg(const EventCallback& callback, EventCallbackID callbackID = 0);

    /** Listen for the screen resolution change event of the monitor where the window is located
    * @param [in] callback The specified callback function; the value of wParam is a pointer type: WindowDisplayResolutionData*, containing the screen resolution data
    * @param [in] callbackID The ID corresponding to this callback function (used to delete the callback function)
    */
    void AttachWindowDisplayResolutionChangedMsg(const EventCallback& callback, EventCallbackID callbackID = 0);

public:
    /** Whether this window has a callback function (according to the callback event type)
    * @param [in] eventType The callback event type
    */
    bool HasWindowEventCallback(EventType eventType) const;

    /** Whether this window has a callback function (according to the callback function ID)
    * @param [in] callbackID The ID corresponding to this callback function
    */
    bool HasWindowEventCallbackByID(EventCallbackID callbackID) const;

    /** Delete the callback function (according to the callback event type)
    * @param [in] eventType The callback event type
    */
    void DetachWindowEventCallback(EventType eventType);

    /** Delete the callback function (according to the callback function ID)
    * @param [in] callbackID The ID corresponding to this callback function
    */
    void DetachWindowEventCallbackByID(EventCallbackID callbackID);

protected:
    /** Initializing the window data
    */
    virtual void PreInitWindow() = 0;

    /** Initialize the window data (this function is called after the window is created), for subclasses to do some initialization work
    */
    virtual void OnInitWindow() = 0;

    /** Finish initializing the window data
    */
    virtual void PostInitWindow() = 0;

    /** Finish the initialization of the interface layout; the layout information such as the position and size of each control is initialized
    */
    virtual void OnInitLayout() = 0;

    /** The window is closing; handle the internal state
    */
    virtual void PreCloseWindow() = 0;

    /** The window has closed; handle the internal state
    */
    virtual void PostCloseWindow() = 0;

    /** Called when the window is destroyed; this is the last message of this window
    */
    virtual void FinalMessage() = 0;

protected:
    /** Enter the fullscreen state
    */
    virtual void NotifyWindowEnterFullscreen() = 0;

    /** Exit the fullscreen state
    */
    virtual void NotifyWindowExitFullscreen() = 0;

protected:
    /** Switch between the system title bar and the self-drawn title bar
    */
    virtual void OnUseSystemCaptionBarChanged() = 0;

    /** Prepare to paint
    * @return Returns true to continue painting, returns false to stop painting
    */
    virtual bool OnPreparePaint() = 0;

    /** The layered window attribute of the window has changed
    */
    virtual void OnLayeredWindowChanged() = 0;

    /** The transparency of the window has changed
    */
    virtual void OnWindowAlphaChanged() = 0;

    /** Enter the fullscreen state
    */
    virtual void OnWindowEnterFullscreen() = 0;

    /** Exit the fullscreen state
    */
    virtual void OnWindowExitFullscreen() = 0;

    /** The DPI scale ratio of the window has changed; update the control size and layout (for use by subclasses)
    * @param [in] nOldScaleFactor The old DPI scale percentage
    * @param [in] nNewScaleFactor The new DPI scale percentage, consistent with the value of Dpi().GetDisplayScaleFactor(); this value may be the same as nOldScaleFactor
    */
    virtual void OnWindowDisplayScaleChanged(uint32_t nOldScaleFactor, uint32_t nNewScaleFactor) = 0;

    /** Get the size of the configured window shadow
    * @param [out] rcShadow Returns the size of the configured window shadow, not DPI-scaled
    */
    virtual void GetShadowCorner(UiPadding& rcShadow) const = 0;

    /** Get the size of the current window shadow
    * @param [out] rcShadow Returns the size of the current window shadow, already DPI-scaled
    */
    virtual void GetCurrentShadowCorner(UiPadding& rcShadow) const = 0;

    /** Determine whether a point is on a control placed on the title bar
    */
    virtual bool IsPtInCaptionBarControl(const UiPoint& pt) const = 0;

    /** Determine whether it contains the maximize and minimize buttons
    * @param [out] bMinimizeBox Returns true if it contains the minimize button
    * @param [out] bMaximizeBox Returns true if it contains the maximize button
    */
    virtual bool HasMinMaxBox(bool& bMinimizeBox, bool& bMaximizeBox) const = 0;

    /** Determine whether a point is on the maximize or restore button
    */
    virtual bool IsPtInMaximizeRestoreButton(const UiPoint& pt) const = 0;

    /** Get the attributes for creating the window (the attribute values read from the Window tag of the XML file)
    * @param [out] createAttributes Returns the attributes for creating the window read from the Window tag of the XML file
    */
    virtual void GetCreateWindowAttributes(WindowCreateAttributes& createAttributes) = 0;

    /** The DPI scale ratio of the window has changed; update the control size and layout
    * @param [in] nOldScaleFactor The old DPI scale percentage
    * @param [in] nNewScaleFactor The new DPI scale percentage, consistent with the value of Dpi().GetDisplayScaleFactor()
    */
    virtual void OnDisplayScaleChanged(uint32_t nOldScaleFactor, uint32_t nNewScaleFactor);

    /** Get the drawing engine object
    */
    virtual IRender* GetRender() const = 0;

    /** Get the control interface at the specified coordinate point
    * @param [in] pt The client area coordinate point
    */
    virtual Control* OnFindControl(const UiPoint& pt) const = 0;

protected:
    /** Whether to automatically set the window shape when the window size changes (on the Windows platform, this refers to setting the window's RGN)
    *   By default, child windows are not automatically set, and top-level windows are automatically set
    */
    virtual bool NeedSetWindowRgnOnWindowResized();

    /** Set the shape of the window to a rounded rectangle
    * @param [in] rcWnd The area where the RGN needs to be set; the coordinates are screen coordinates
    * @param [in] rx The width of the round corner; its value cannot be 0
    * @param [in] ry The height of the round corner; its value cannot be 0
    * @param [in] bRedraw Whether to redraw
    */
    virtual bool SetWindowRoundRectRgn(const UiRect& rcWnd, float rx, float ry, bool bRedraw);

    /** Set the shape of the window to a right-angle rectangle
    * @param [in] rcWnd The area where the RGN needs to be set; the coordinates are screen coordinates
    * @param [in] bRedraw Whether to redraw
    */
    virtual bool SetWindowRectRgn(const UiRect& rcWnd, bool bRedraw);

    /** Clear the shape setting of the window and restore the system default shape
    * @param [in] bRedraw Whether to redraw
    */
    virtual void ClearWindowRgn(bool bRedraw);

protected:
    /** @name Related to window message handling
     * @{
     */
    /** The dispatch function for window messages
    * @param [in] uMsg The message body
    * @param [in] wParam Additional message parameters
    * @param [in] lParam Additional message parameters
    * @param [out] bHandled Whether the message has been handled; returning true indicates the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for handling
    * @return Returns the handling result of the message
    */
    virtual LRESULT OnWindowMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled) = 0;

    /** The event of successful window creation (WM_CREATE/WM_INITDIALOG)
     * @param [in] bDoModal Whether the current window is a modal dialog displayed through the DoModal function
     * @param [in] nativeMsg The raw message content received from the system
     * @param [out] bHandled Whether the message has been handled; returning true indicates the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for handling
     * @return Returns the handling result of the message; if the application handles this message, it should return zero
     */
    virtual void OnWindowCreateMsg(bool bDoModal, const NativeMsg& nativeMsg, bool& bHandled) = 0;

    /** The window close message (WM_CLOSE)
    * @param [in] wParam The wParam parameter of the message
    * @param [in] nativeMsg The raw message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true indicates the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for handling
    * @return Returns the handling result of the message; if the application handles this message, it should return zero
    */
    virtual LRESULT OnWindowCloseMsg(uint32_t wParam, const NativeMsg& nativeMsg, bool& bHandled) = 0;

    /** The window is shown or hidden (WM_SHOWWINDOW)
    * @param [in] bShow true means the window is being shown, false means the window is being hidden
    * @param [in] nativeMsg The raw message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true indicates the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for handling
    * @return Returns the handling result of the message; if the application handles this message, it should return zero
    */
    virtual LRESULT OnShowWindowMsg(bool bShow, const NativeMsg& nativeMsg, bool& bHandled) = 0;

    /** The window is painted (WM_PAINT)
    * @param [in] rcPaint The rectangular area that needs to be updated in this paint
    * @param [in] nativeMsg The raw message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true indicates the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for handling
    * @return Returns the handling result of the message; if the application handles this message, it should return zero
    */
    virtual LRESULT OnPaintMsg(const UiRect& rcPaint, const NativeMsg& nativeMsg, bool& bHandled) = 0;

    /** The window position or size has changed (WM_WINDOWPOSCHANGED)
    * @param [in] nativeMsg The raw message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true indicates the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for handling
    * @return Returns the handling result of the message; if the application handles this message, it should return zero
    */
    virtual LRESULT OnWindowPosChangedMsg(const NativeMsg& nativeMsg, bool& bHandled) = 0;

    /** The window size has changed (WM_SIZE)
    * @param [in] sizeType The type that triggered the window size change
    * @param [in] newWindowSize The new window size (width and height)
    * @param [in] nativeMsg The raw message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true indicates the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for handling
    * @return Returns the handling result of the message; if the application handles this message, it should return zero
    */
    virtual LRESULT OnSizeMsg(WindowSizeType sizeType, const UiSize& newWindowSize, const NativeMsg& nativeMsg, bool& bHandled) = 0;

    /** The window moves (WM_MOVE)
    * @param [in] ptTopLeft The x and y coordinates of the top-left corner of the window client area (the coordinates are screen coordinates)
    * @param [in] nativeMsg The raw message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true indicates the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for handling
    * @return Returns the handling result of the message; if the application handles this message, it should return zero
    */
    virtual LRESULT OnMoveMsg(const UiPoint& ptTopLeft, const NativeMsg& nativeMsg, bool& bHandled) = 0;

    /** The window gains focus (WM_SETFOCUS)
    * @param [in] pLostFocusWindow The window that has lost keyboard focus (may be nullptr)
    * @param [in] nativeMsg The raw message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true indicates the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for handling
    * @return Returns the handling result of the message; if the application handles this message, it should return zero
    */
    virtual LRESULT OnSetFocusMsg(WindowBase* pLostFocusWindow, const NativeMsg& nativeMsg, bool& bHandled) = 0;

    /** The window loses focus (WM_KILLFOCUS)
    * @param [in] pSetFocusWindow The window receiving keyboard focus (may be nullptr)
    * @param [in] nativeMsg The raw message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true indicates the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for handling
    * @return Returns the handling result of the message; if the application handles this message, it should return zero
    */
    virtual LRESULT OnKillFocusMsg(WindowBase* pSetFocusWindow, const NativeMsg& nativeMsg, bool& bHandled) = 0;

    /** Notify the application of the input focus change (WM_IME_SETCONTEXT)
    * @param [in] nativeMsg The raw message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true indicates the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for handling
    * @return Returns the handling result of the message; if the application handles this message, it should return zero
    */
    virtual LRESULT OnImeSetContextMsg(const NativeMsg& nativeMsg, bool& bHandled) = 0;

    /** The input method starts generating the composition string (WM_IME_STARTCOMPOSITION)
    * @param [in] nativeMsg The raw message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true indicates the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for handling
    * @return Returns the handling result of the message; if the application handles this message, it should return zero
    */
    virtual LRESULT OnImeStartCompositionMsg(const NativeMsg& nativeMsg, bool& bHandled) = 0;

    /** Change the key composition state (WM_IME_COMPOSITION)
    * @param [in] nativeMsg The raw message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true indicates the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for handling
    * @return Returns the handling result of the message; if the application handles this message, it should return zero
    */
    virtual LRESULT OnImeCompositionMsg(const NativeMsg& nativeMsg, bool& bHandled) = 0;

    /** The input method ends the composition (WM_IME_ENDCOMPOSITION)
    * @param [in] nativeMsg The raw message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true indicates the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for handling
    * @return Returns the handling result of the message; if the application handles this message, it should return zero
    */
    virtual LRESULT OnImeEndCompositionMsg(const NativeMsg& nativeMsg, bool& bHandled) = 0;

    /** Set the cursor (WM_SETCURSOR)
    * @param [in] nativeMsg The raw message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true indicates the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for handling
    * @return If the application handles this message, it should return TRUE to stop further processing or FALSE to continue
    */
    virtual LRESULT OnSetCursorMsg(const NativeMsg& nativeMsg, bool& bHandled) = 0;

    /** Notify the window that the user wants to display the context menu (WM_CONTEXTMENU); the user may have clicked the right mouse button (right-clicked in the window), pressed Shift+F10, or pressed the application key (the context menu key), available on some keyboards.
    * @param [in] pt The mouse position, client area coordinates; (-1,-1) means the user typed SHIFT+F10
    * @param [in] nativeMsg The raw message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true indicates the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for handling
    * @return Returns the handling result of the message; if the application handles this message, it should return zero
    */
    virtual LRESULT OnContextMenuMsg(const UiPoint& pt, const NativeMsg& nativeMsg, bool& bHandled) = 0;

    /** Key pressed down (WM_KEYDOWN or WM_SYSKEYDOWN)
    * @param [in] vkCode The virtual key code
    * @param [in] modifierKey The modifier flags; valid values: ModifierKey::kFirstPress, ModifierKey::kAlt
    * @param [in] nativeMsg The raw message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true indicates the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for handling
    * @return Returns the handling result of the message; if the application handles this message, it should return zero
    */
    virtual LRESULT OnKeyDownMsg(VirtualKeyCode vkCode, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) = 0;

    /** Key released (WM_KEYUP or WM_SYSKEYUP)
    * @param [in] vkCode The virtual key code
    * @param [in] modifierKey The modifier flags; valid values: ModifierKey::kAlt
    * @param [in] nativeMsg The raw message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true indicates the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for handling
    * @return Returns the handling result of the message; if the application handles this message, it should return zero
    */
    virtual LRESULT OnKeyUpMsg(VirtualKeyCode vkCode, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) = 0;

    /** Key pressed (WM_CHAR)
    * @param [in] vkCode The virtual key code
    * @param [in] modifierKey The modifier flags; valid values: ModifierKey::kFirstPress, ModifierKey::kAlt
    * @param [in] nativeMsg The raw message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true indicates the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for handling
    * @return Returns the handling result of the message; if the application handles this message, it should return zero
    */
    virtual LRESULT OnCharMsg(VirtualKeyCode vkCode, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) = 0;

    /** Hotkey message (WM_HOTKEY)
    * @param [in] hotkeyId The ID of the hotkey
    * @param [in] vkCode The virtual key code
    * @param [in] modifierKey The modifier flags; valid values: ModifierKey::kAlt, ModifierKey::kControl, ModifierKey::kShift, ModifierKey::kWin
    * @param [in] nativeMsg The raw message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true indicates the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for handling
    * @return Returns the handling result of the message; if the application handles this message, it should return zero
    */
    virtual LRESULT OnHotKeyMsg(int32_t hotkeyId, VirtualKeyCode vkCode, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) = 0;

    /** Rotate the mouse wheel (WM_MOUSEWHEEL)
    * @param [in] wheelDelta The distance the wheel has rotated, expressed as a multiple or division of WHEEL_DELTA (120). A positive value means the wheel rotated forward (away from the user); a negative value means the wheel rotated backward (toward the user)
    * @param [in] pt The mouse position, client area coordinates
    * @param [in] modifierKey The modifier flags; valid values: ModifierKey::kControl, ModifierKey::kShift
    * @param [in] nativeMsg The raw message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true indicates the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for handling
    * @return Returns the handling result of the message; if the application handles this message, it should return zero
    */
    virtual LRESULT OnMouseWheelMsg(int32_t wheelDelta, const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) = 0;

    /** Mouse move message (WM_MOUSEMOVE)
    * @param [in] pt The mouse position, client area coordinates
    * @param [in] modifierKey The modifier flags; valid values: ModifierKey::kControl, ModifierKey::kShift
    * @param [in] bFromNC true means this is an NC message (WM_NCMOUSEMOVE), false means it is a WM_MOUSEMOVE message
    * @param [in] nativeMsg The raw message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true indicates the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for handling
    * @return Returns the handling result of the message; if the application handles this message, it should return zero
    */
    virtual LRESULT OnMouseMoveMsg(const UiPoint& pt, uint32_t modifierKey, bool bFromNC, const NativeMsg& nativeMsg, bool& bHandled) = 0;

    /** Mouse hover message (WM_MOUSEHOVER)
    * @param [in] pt The mouse position, client area coordinates
    * @param [in] modifierKey The modifier flags; valid values: ModifierKey::kControl, ModifierKey::kShift
    * @param [in] nativeMsg The raw message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true indicates the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for handling
    * @return Returns the handling result of the message; if the application handles this message, it should return zero
    */
    virtual LRESULT OnMouseHoverMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) = 0;

    /** Mouse leave message (WM_MOUSELEAVE)
    * @param [in] nativeMsg The raw message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true indicates the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for handling
    * @return Returns the handling result of the message; if the application handles this message, it should return zero
    */
    virtual LRESULT OnMouseLeaveMsg(const NativeMsg& nativeMsg, bool& bHandled) = 0;

    /** Mouse left button down message (WM_LBUTTONDOWN)
    * @param [in] pt The mouse position, client area coordinates
    * @param [in] modifierKey The modifier flags; valid values: ModifierKey::kControl, ModifierKey::kShift
    * @param [in] nativeMsg The raw message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true indicates the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for handling
    * @return Returns the handling result of the message; if the application handles this message, it should return zero
    */
    virtual LRESULT OnMouseLButtonDownMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) = 0;

    /** Mouse left button up message (WM_LBUTTONUP)
    * @param [in] pt The mouse position, client area coordinates
    * @param [in] modifierKey The modifier flags; valid values: ModifierKey::kControl, ModifierKey::kShift
    * @param [in] nativeMsg The raw message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true indicates the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for handling
    * @return Returns the handling result of the message; if the application handles this message, it should return zero
    */
    virtual LRESULT OnMouseLButtonUpMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) = 0;

    /** Mouse left button double-click message (WM_LBUTTONDBLCLK)
    * @param [in] pt The mouse position, client area coordinates
    * @param [in] modifierKey The modifier flags; valid values: ModifierKey::kControl, ModifierKey::kShift
    * @param [in] nativeMsg The raw message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true indicates the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for handling
    * @return Returns the handling result of the message; if the application handles this message, it should return zero
    */
    virtual LRESULT OnMouseLButtonDbClickMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) = 0;

    /** Mouse right button down message (WM_RBUTTONDOWN)
    * @param [in] pt The mouse position, client area coordinates
    * @param [in] modifierKey The modifier flags; valid values: ModifierKey::kControl, ModifierKey::kShift
    * @param [in] nativeMsg The raw message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true indicates the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for handling
    * @return Returns the handling result of the message; if the application handles this message, it should return zero
    */
    virtual LRESULT OnMouseRButtonDownMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) = 0;

    /** Mouse right button up message (WM_RBUTTONUP)
    * @param [in] pt The mouse position, client area coordinates
    * @param [in] modifierKey The modifier flags; valid values: ModifierKey::kControl, ModifierKey::kShift
    * @param [in] nativeMsg The raw message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true indicates the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for handling
    * @return Returns the handling result of the message; if the application handles this message, it should return zero
    */
    virtual LRESULT OnMouseRButtonUpMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) = 0;

    /** Mouse right button double-click message (WM_RBUTTONDBLCLK)
    * @param [in] pt The mouse position, client area coordinates
    * @param [in] modifierKey The modifier flags; valid values: ModifierKey::kControl, ModifierKey::kShift
    * @param [in] nativeMsg The raw message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true indicates the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for handling
    * @return Returns the handling result of the message; if the application handles this message, it should return zero
    */
    virtual LRESULT OnMouseRButtonDbClickMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) = 0;

    /** Mouse middle button down message (WM_MBUTTONDOWN)
    * @param [in] pt The mouse position, client area coordinates
    * @param [in] modifierKey The modifier flags; valid values: ModifierKey::kControl, ModifierKey::kShift
    * @param [in] nativeMsg The raw message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true indicates the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for handling
    * @return Returns the handling result of the message; if the application handles this message, it should return zero
    */
    virtual LRESULT OnMouseMButtonDownMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) = 0;

    /** Mouse middle button up message (WM_MBUTTONUP)
    * @param [in] pt The mouse position, client area coordinates
    * @param [in] modifierKey The modifier flags; valid values: ModifierKey::kControl, ModifierKey::kShift
    * @param [in] nativeMsg The raw message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true indicates the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for handling
    * @return Returns the handling result of the message; if the application handles this message, it should return zero
    */
    virtual LRESULT OnMouseMButtonUpMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) = 0;

    /** Mouse middle button double-click message (WM_MBUTTONDBLCLK)
    * @param [in] pt The mouse position, client area coordinates
    * @param [in] modifierKey The modifier flags; valid values: ModifierKey::kControl, ModifierKey::kShift
    * @param [in] nativeMsg The raw message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true indicates the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for handling
    * @return Returns the handling result of the message; if the application handles this message, it should return zero
    */
    virtual LRESULT OnMouseMButtonDbClickMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) = 0;

    /** The window lost mouse capture (WM_CAPTURECHANGED)
    * @param [in] nativeMsg The raw message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true indicates the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for handling
    * @return Returns the handling result of the message; if the application handles this message, it should return zero
    */
    virtual LRESULT OnCaptureChangedMsg(const NativeMsg& nativeMsg, bool& bHandled) = 0;

    /** The snap operation of the window position
    * @param [in] bLeftSnap Snap the left side of the window
    * @param [in] bRightSnap Snap the right side of the window
    * @param [in] bTopSnap Snap the top side of the window
    * @param [in] bBottomSnap Snap the bottom side of the window
    */
    virtual void OnWindowPosSnapped(bool bLeftSnap, bool bRightSnap, bool bTopSnap, bool bBottomSnap) = 0;

    /** The operation interface related to window drag-and-drop (the interface parameters are related to the implementation method)
    * @param [in] dropType The source type of the drag-and-drop operation
    * @param [in,out] pDropData The specific type is determined by dropType:
    *                 When dropType is kControlDropTypeWindows (representing the Windows platform SDK implementation), the type of pDropData is ControlDropData_Windows*
    *                 When dropType is kControlDropTypeSDL (representing the SDL implementation), the type of pDropData is ControlDropData_SDL*
    *                 pDropData->m_bHandled is the message handling flag; if true is returned, it means the event has been handled and will not be forwarded to other UI controls in the interface for handling, which is equivalent to intercepting this message
    *                 pDropData->m_hResult is the return value after the message is handled, finally returned to the operating system; on the Windows platform, success returns S_OK
    */
    virtual void OnDropEnterMsg(ui::ControlDropType dropType, void* pDropData) = 0;
    virtual void OnDropOverMsg(ui::ControlDropType dropType, void* pDropData) = 0;
    virtual void OnDropMsg(ui::ControlDropType dropType, void* pDropData) = 0;
    virtual void OnDropLeaveMsg() = 0;

    /** Handle the system notification message for screen resolution changes (WM_DISPLAYCHANGE)
    * @param [in] nColorDepth The new image depth of the display, in bits per pixel
    * @param [in] nScreenWidth The horizontal resolution of the screen
    * @param [in] nScreenHeight The vertical resolution of the screen
    */
    virtual void OnDisplayResolutionChangedMsg(int32_t nColorDepth, int32_t nScreenWidth, int32_t nScreenHeight) = 0;

    /** Handle the system notification message for DPI changes (WM_DPICHANGED)
    * @param [in] fNewDisplayScale The new interface display scale value of the window; 1.0f means no scaling
    * @param [in] fNewPixelDensity The new pixel density value of the window (only used in the SDL implementation)
    */
    virtual void OnDisplayScaleChangedMsg(float fNewDisplayScale, float fNewPixelDensity) = 0;

    /** @}*/

protected:
    /** Get the window interface corresponding to a point
    * @param [in] pt The screen coordinate point
    * @param [in] bIgnoreChildWindow true means ignore child windows, false means do not ignore child windows
    */
    WindowBase* WindowBaseFromPoint(const UiPoint& pt, bool bIgnoreChildWindow = false);

    /** Handle the system notification message for DPI changes
    * @param [in] fNewDisplayScale The new interface display scale value of the window; 1.0f means no scaling
    * @param [in] fNewPixelDensity The new pixel density value of the window (only used in the SDL implementation)
    */
    void OnProcessDisplayScaleChangedMsg(float fNewDisplayScale, float fNewPixelDensity);

    /** Clean up the window resources
    */
    void ClearWindowBase();

    /** Actively sends a message (kWindowMsgBegin - kWindowMsgEnd) to the message handlers registered in the event callback manager of this window
    * @param [in] eventType The converted message type
    * @param [in] wParam The message extra parameter
    * @param [in] lParam The message extra parameter
    * @param [in] pt The coordinates of the mouse event
    * @param [in] modifierKey The keyboard modifier state of the mouse event (valid values: ModifierKey::kControl, ModifierKey::kShift)
    * @param [in] msg The message content
    */
    bool SendWindowEvent(EventType eventType, WPARAM wParam = 0, LPARAM lParam = 0);
    bool SendWindowMouseEvent(EventType eventType, const UiPoint& pt, uint32_t modifierKey);
    bool SendWindowEvent(const EventArgs& msg);

private:
    /** Initializes window data (internal function; subclasses overriding it must call the base class function)
    */
    void InitWindowBase();

    /** The window size changed; handles internal logic (sets the RGN)
    * @param [in] bRedraw Whether to repaint
    */
    void OnWindowSized(bool bRedraw);

private:
    //Events from the native window
    virtual void OnNativeWindowEnterFullscreen() override final;
    virtual void OnNativeWindowExitFullscreen() override final;
    virtual UiRect OnNativeGetSizeBox() const override final;
    virtual void OnNativeGetShadowCorner(UiPadding& rcShadow) const override final;
    virtual const DpiManager& OnNativeGetDpi() const override final;
    virtual void OnNativeGetCaptionRect(UiRect& captionRect) const override final;
    virtual void OnNativeGetSysMenuRect(UiRect& sysMenuRect) const override final;
    virtual bool OnNativeIsPtInCaptionBarControl(const UiPoint& pt) const override final;
    virtual bool OnNativeHasMinMaxBox(bool& bMinimizeBox, bool& bMaximizeBox) const override final;
    virtual bool OnNativeIsPtInMaximizeRestoreButton(const UiPoint& pt) const override final;
    virtual void OnNativePreCloseWindow() override final;
    virtual void OnNativePostCloseWindow() override final;
    virtual void OnNativeUseSystemCaptionBarChanged() override final;
    virtual bool OnNativePreparePaint() override final;
    virtual IRender* OnNativeGetRender() const override final;
    virtual Control* OnNativeFindControl(const UiPoint& pt) const override final;

    virtual void    OnNativeFinalMessage() override final;
    virtual LRESULT OnNativeWindowMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled) override final;
    virtual void    OnNativeDisplayResolutionChangedMsg(int32_t nColorDepth, int32_t nScreenWidth, int32_t nScreenHeight) override final;
    virtual void    OnNativeProcessDisplayScaleChangedMsg(float fNewDisplayScale, float fNewPixelDensity) override final;
    virtual void    OnNativeDisplayScaleChangedMsg(float fNewDisplayScale, float fNewPixelDensity) override final;
    virtual void    OnNativeCreateWndMsg(bool bDoModal, const NativeMsg& nativeMsg, bool& bHandled) override final;
    virtual LRESULT OnNativeWindowPosChangedMsg(const NativeMsg& nativeMsg, bool& bHandled) override final;
    virtual LRESULT OnNativeSizeMsg(WindowSizeType sizeType, const UiSize& newWindowSize, const NativeMsg& nativeMsg, bool& bHandled) override final;
    virtual LRESULT OnNativeMoveMsg(const UiPoint& ptTopLeft, const NativeMsg& nativeMsg, bool& bHandled) override final;
    virtual LRESULT OnNativeShowWindowMsg(bool bShow, const NativeMsg& nativeMsg, bool& bHandled) override final;
    virtual LRESULT OnNativePaintMsg(const UiRect& rcPaint, const NativeMsg& nativeMsg, bool& bHandled) override;
    virtual LRESULT OnNativeSetFocusMsg(INativeWindow* pLostFocusWindow, const NativeMsg& nativeMsg, bool& bHandled) override final;
    virtual LRESULT OnNativeKillFocusMsg(INativeWindow* pSetFocusWindow, const NativeMsg& nativeMsg, bool& bHandled) override final;
    virtual LRESULT OnNativeImeStartCompositionMsg(const NativeMsg& nativeMsg, bool& bHandled) override final;
    virtual LRESULT OnNativeImeSetContextMsg(const NativeMsg& nativeMsg, bool& bHandled) override final;
    virtual LRESULT OnNativeImeCompositionMsg(const NativeMsg& nativeMsg, bool& bHandled) override final;
    virtual LRESULT OnNativeImeEndCompositionMsg(const NativeMsg& nativeMsg, bool& bHandled) override final;
    virtual LRESULT OnNativeSetCursorMsg(const NativeMsg& nativeMsg, bool& bHandled) override final;
    virtual LRESULT OnNativeContextMenuMsg(const UiPoint& pt, const NativeMsg& nativeMsg, bool& bHandled) override final;
    virtual LRESULT OnNativeKeyDownMsg(VirtualKeyCode vkCode, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) override final;
    virtual LRESULT OnNativeKeyUpMsg(VirtualKeyCode vkCode, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) override final;
    virtual LRESULT OnNativeCharMsg(VirtualKeyCode vkCode, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) override final;
    virtual LRESULT OnNativeHotKeyMsg(int32_t hotkeyId, VirtualKeyCode vkCode, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) override final;
    virtual LRESULT OnNativeMouseWheelMsg(int32_t wheelDelta, const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) override final;
    virtual LRESULT OnNativeMouseMoveMsg(const UiPoint& pt, uint32_t modifierKey, bool bFromNC, const NativeMsg& nativeMsg, bool& bHandled) override final;
    virtual LRESULT OnNativeMouseHoverMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) override final;
    virtual LRESULT OnNativeMouseLeaveMsg(const NativeMsg& nativeMsg, bool& bHandled) override final;
    virtual LRESULT OnNativeMouseLButtonDownMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) override final;
    virtual LRESULT OnNativeMouseLButtonUpMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) override final;
    virtual LRESULT OnNativeMouseLButtonDbClickMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) override final;
    virtual LRESULT OnNativeMouseRButtonDownMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) override final;
    virtual LRESULT OnNativeMouseRButtonUpMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) override final;
    virtual LRESULT OnNativeMouseRButtonDbClickMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) override final;
    virtual LRESULT OnNativeMouseMButtonDownMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) override final;
    virtual LRESULT OnNativeMouseMButtonUpMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) override final;
    virtual LRESULT OnNativeMouseMButtonDbClickMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) override final;
    virtual LRESULT OnNativeCaptureChangedMsg(const NativeMsg& nativeMsg, bool& bHandled) override final;
    virtual LRESULT OnNativeWindowCloseMsg(uint32_t wParam, const NativeMsg& nativeMsg, bool& bHandled) override final;
    virtual void OnNativeWindowPosSnapped(bool bLeftSnap, bool bRightSnap, bool bTopSnap, bool bBottomSnap) override final;
    virtual void OnNativeDropEnterMsg(ControlDropType dropType, void* pDropData) override final;
    virtual void OnNativeDropOverMsg(ControlDropType dropType, void* pDropData) override final;
    virtual void OnNativeDropMsg(ControlDropType dropType, void* pDropData) override final;
    virtual void OnNativeDropLeaveMsg() override final;

private:
    /** The type of window size change (internal value)
    */
    enum class WindowSizeState: uint8_t
    {
        kUnknown    = 0,
        kMinimized  = 1,  //The window is minimized
        kRestored   = 2,  //The window is in the restored state
        kMaximized  = 3,  //The window is maximized
        kFullscreen = 4   //The window is fullscreen
    };

private:
    //The parent window
    ControlPtrT<WindowBase> m_pParentWindow;

    //The message filter list of the window
    std::vector<IUIMessageFilter*> m_aMessageFilters;

    //The window own DPI manager
    std::unique_ptr<DpiManager> m_dpi;

private:
    //The text ID of the window title bar text
    DString m_textId;

    //The window ID
    DString m_windowId;

    //The class name of the window
    DString m_windowClassName;

    //The stretchable range info of the four window edges
    UiRect m_rcSizeBox;

    //Window corner radius info
    UiSize m_szRoundCorner;

    //Title bar area info
    UiRect m_rcCaption;

    //Window menu area (double-click it to close the window; click to show the system window menu)
    UiRect m_rcSysMenuRect;

private:
    /** The native window implementation class
    */
    NativeWindow* m_pNativeWindow;

    /** Window events
    */
    EventMap m_windowEventMap;

    //Whether the UI has completed its first display
    bool m_bWindowFirstShown;

    //Whether the window size change event is triggered
    bool m_bWindowSized;

    //The window state
    WindowSizeState m_windowSizeState;

    //Whether the DragEnter message was sent, to keep DragLeave messages matched
    bool m_bSendDragEnterMsg;
};

} // namespace ui

#endif // UI_CORE_WINDOW_BASE_H_

