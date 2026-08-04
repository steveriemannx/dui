#ifndef UI_CORE_NATIVE_WINDOW_WINDOWS_H_
#define UI_CORE_NATIVE_WINDOW_WINDOWS_H_

#include "dui/Core/INativeWindow.h"
#include "dui/Core/WindowCreateParam.h"
#include "dui/Core/WindowCreateAttributes.h"
#include "dui/Utils/FilePath.h"

#if defined (DUI_BUILD_FOR_WIN) && !defined (DUI_BUILD_FOR_SDL)

#include "dui/dui_config_windows.h"
#include <oleidl.h>

namespace ui {

class WindowDropTarget;
class Control;

/** Windows platform implementation of the window functionality
*/
class DUI_API NativeWindow_Windows
{
public:
    explicit NativeWindow_Windows(INativeWindow* pOwner);
    NativeWindow_Windows(const NativeWindow_Windows& r) = delete;
    NativeWindow_Windows& operator=(const NativeWindow_Windows& r) = delete;
    ~NativeWindow_Windows();

public:
    /** Create the window
    * @param [in] pParentWindow The parent window
    * @param [in] createParam The parameters required to create the window
    * @param [in] createAttributes The relevant Window attributes in the XML file
    */
    bool CreateWnd(NativeWindow_Windows* pParentWindow,
                  const WindowCreateParam& createParam,
                  const WindowCreateAttributes& createAttributes);

    /** Show the modal window
    * @param [in] pParentWindow The parent window
    * @param [in] createParam The parameters required to create the window
    * @param [in] bCloseByEsc Whether to close the window when the ESC key is pressed
    * @param [in] bCloseByEnter Whether to close the window when the Enter key is pressed
    * @return The return value when the window exits; returns -1 if it fails
    */
    int32_t DoModal(NativeWindow_Windows* pParentWindow,
                    const WindowCreateParam& createParam,
                    const WindowCreateAttributes& createAttributes,
                    bool bCloseByEsc = true,
                    bool bCloseByEnter = false);

    /** Create a child window (a non-popup child window)
    * @param [in] pParentWindow The parent window
    * @param [in] nX The X coordinate of the child window (relative to the parent window)
    * @param [in] nY The Y coordinate of the child window (relative to the parent window)
    * @param [in] nWidth The width of the child window
    * @param [in] nHeight The height of the child window
    */
    bool CreateChildWnd(NativeWindow_Windows* pParentWindow, int32_t nX, int32_t nY, int32_t nWidth, int32_t nHeight);

    /** Get the Windows handle to which the window belongs
    */
    HWND GetHWND() const;

    /** Get the window handle of the native implementation
    */
    void* GetWindowHandle() const;

    /** Whether it has a valid window handle
    */
    bool IsWindow() const;

    /** Whether the current window is a child window (a child window that is not a popup window type; for the Windows system, it is a window with only the WS_CHILD style)
    */
    bool IsChildWindow() const;

    /** Set or modify the parent window
    */
    bool SetParentWindow(NativeWindow_Windows* pParentWindow);

    /** Get the handle of the resource
    * @return By default, returns the handle of the current process exe
    */
    HMODULE GetResModuleHandle() const;

    /** Get the drawing area DC
    */
    HDC GetPaintDC() const;

public:
    /** Close the window asynchronously; after the function returns, the IsClosing() state is true
    * @param [in] nRet The close parameter; see: enum WindowCloseParam
    */
    void CloseWnd(int32_t nRet = kWindowCloseNormal);

    /** Close the window synchronously
    */
    void Close();

    /** Whether it is about to close
    */
    bool IsClosingWnd() const;

    /** Get the window close parameter
    * @return See enum WindowCloseParam; it may also be a custom value
    */
    int32_t GetCloseParam() const;

public:
    /** Set whether it is a layered window
    * @param [in] bIsLayeredWindow true means set as a layered window, otherwise set as a non-layered window
    * @param [in] bRedraw Whether to repaint the window (after the attribute changes, if not repainted, the UI may display abnormally)
    */
    bool SetLayeredWindow(bool bIsLayeredWindow, bool bRedraw);

    /** Whether it is a layered window
    */
    bool IsLayeredWindow() const;

    /** Set the window transparency (only valid when IsLayeredWindow() is true)
    * @param [in] nAlpha The transparency value [0, 255]; when nAlpha is 0, the window is fully transparent. When nAlpha is 255, the window is opaque.
    *             This parameter is used as a parameter in the UpdateLayeredWindow function.
    */
    void SetLayeredWindowAlpha(int32_t nAlpha);

    /** Get the window transparency (only valid when IsLayeredWindow() is true)
    * @param [in] nAlpha The transparency value [0, 255]; when nAlpha is 0, the window is fully transparent. When nAlpha is 255, the window is opaque.
    */
    uint8_t GetLayeredWindowAlpha() const;

    /** Set the window opacity (only valid when IsLayeredWindow() is true; if the current window is not a layered window, it will be automatically set as a layered window internally)
    * @param [in] nAlpha The transparency value [0, 255]; when nAlpha is 0, the window is fully transparent. When nAlpha is 255, the window is opaque.
    *             This parameter is used as a parameter (bAlpha) in the SetLayeredWindowAttributes function.
    */
    void SetLayeredWindowOpacity(int32_t nAlpha);

    /** Get the window opacity (only valid when IsLayeredWindow() is true)
    * @param [in] nAlpha The transparency value [0, 255]; when nAlpha is 0, the window is fully transparent. When nAlpha is 255, the window is opaque.
    */
    uint8_t GetLayeredWindowOpacity() const;

    /** Set whether to use the system title bar
    */
    void SetUseSystemCaption(bool bUseSystemCaption);

    /** Get whether the system title bar is used
    */
    bool IsUseSystemCaption() const;

public:
    /** Operations to show, hide, maximize, restore, and minimize the window
     * @param [in] nCmdShow The command to show or hide the window
    */
    bool ShowWindow(ShowWindowCommands nCmdShow);

    /** Show the modal dialog (the parent window is specified at creation time)
    */
    void ShowModalFake(NativeWindow_Windows* pParentWindow);

    /** The modal dialog is closed; synchronize the state
    */
    void OnCloseModalFake(NativeWindow_Windows* pParentWindow);

    /** Whether it is a simulated modal display window (a window shown via the ShowModalFake function)
    */
    bool IsFakeModal() const;

    /** Whether it is modal dialog mode (a dialog window shown via the DoModal function)
    */
    bool IsDoModal() const;

    /** Center the window, supporting extended screens
    */
    void CenterWindow();

    /** Whether to set the window as a topmost window
    * @param [in] bOnTop true means set as a topmost window, false means cancel the topmost window
    */
    void SetWindowAlwaysOnTop(bool bOnTop);

    /** Determine whether the current window is a topmost window
    * @return true means the current window is topmost, false means it is not topmost
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

    /** Post a message to the message queue
    * @param [in] uMsg The message type
    * @param [in] wParam The additional parameter of the message
    * @param [in] lParam The additional parameter of the message
    * @return Returns the result of the window processing the message
    */
    LRESULT PostMsg(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0L);

    /** Send a quit message to the message queue
    * @param [in] nExitCode The exit code
    */
    static void PostQuitMsg(int32_t nExitCode);

public:
    /** Make the window enter fullscreen state
    */
    bool EnterFullscreen();

    /** Make the window exit fullscreen state (by default, exiting fullscreen when the ESC key is pressed)
    */
    bool ExitFullscreen();

    /** Whether the window is in the maximized state
    */
    bool IsWindowMaximized() const;

    /** Whether the window is in the minimized state
    */
    bool IsWindowMinimized() const;

    /** Whether the window is in the fullscreen state
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

public:
    /** Set the window position (a wrapper of the ::SetWindowPos API, without DPI scaling internally)
    * @param [in] pInsertAfterWindow Corresponds to the hWndInsertAfter option of SetWindowPos
    * @param [in] insertAfterFlag Corresponds to the hWndInsertAfter option of SetWindowPos; only valid when pWindow is nullptr
    * @param [in] X The X coordinate of the window
    * @param [in] Y The Y coordinate of the window
    * @param [in] cx The width of the window
    * @param [in] cy The height of the window
    * @param [in] uFlags Refer to the enum WindowPosFlags options
    */
    bool SetWindowPos(const NativeWindow_Windows* pInsertAfterWindow,
                      InsertAfterFlag insertAfterFlag,
                      int32_t X, int32_t Y, int32_t cx, int32_t cy, uint32_t uFlags);

    /** Set the position and size of the window
    * @param [in] X The X coordinate of the window
    * @param [in] Y The Y coordinate of the window
    * @param [in] nWidth The width of the window
    * @param [in] nHeight The height of the window
    * @param [in] bRepaint Whether to repaint the window
    */
    bool MoveWindow(int32_t X, int32_t Y, int32_t nWidth, int32_t nHeight, bool bRepaint);

    /** Set the window icon (supports the *.ico format; other formats are also supported, but the ICO format is recommended)
    *  @param [in] iconFilePath The path of the ico file (absolute path)
    */
    bool SetWindowIcon(const FilePath& iconFilePath);

    /** Set the window icon (supports the *.ico format; other formats are also supported, but the ICO format is recommended)
    *  @param [in] iconFileData The data of the icon file
    *  @param [in] iconFileName The file name with the extension, used to identify the image type
    */
    bool SetWindowIcon(const std::vector<uint8_t>& iconFileData, const DString& iconFileName);

    /** Set the window title bar text
    * @param [in] strText The window title bar text
    */
    void SetText(const DString& strText);

    /** Get the window title bar text
    */
    DString GetText() const;

    /** Set the minimum size of the window (width and height; the size is not adjusted by DPI internally, DPI adaptation needs to be done by the caller)
    * @param [in] szMaxWindow The maximum width and minimum height of the window; if the value is 0, it means no restriction
    */
    void SetWindowMaximumSize(const UiSize& szMaxWindow);

    /** Get the minimum size of the window (width and height)
    */
    const UiSize& GetWindowMaximumSize() const;

    /** Set the maximum size of the window (width and height; the size is not adjusted by DPI internally, DPI adaptation needs to be done by the caller)
    * @param [in] szMinWindow The minimum width and minimum height of the window; if the value is 0, it means no restriction
    */
    void SetWindowMinimumSize(const UiSize& szMinWindow);

    /** Get the maximum size of the window (width and height)
    */
    const UiSize& GetWindowMinimumSize() const;

public:
    /** Set the window handle that captures the mouse to the current drawing window
    * @param [in]
    */
    void SetCapture();

    /** Release the capture when the window no longer needs mouse input
    */
    void ReleaseCapture();

    /** Determine whether the mouse input is currently captured
    */
    bool IsCaptured() const;

    /** Set the shape of the window to a rounded rectangle
    * @param [in] rcWnd The region to set as the RGN, in screen coordinates
    * @param [in] rx The width of the rounded corner; its value cannot be 0
    * @param [in] ry The height of the rounded corner; its value cannot be 0
    * @param [in] bRedraw Whether to repaint
    */
    bool SetWindowRoundRectRgn(const UiRect& rcWnd, float rx, float ry, bool bRedraw);

    /** Sets the window shape to a rectangle
    * @param [in] rcWnd The region to set as the RGN, in screen coordinates
    * @param [in] bRedraw Whether to repaint
    */
    bool SetWindowRectRgn(const UiRect& rcWnd, bool bRedraw);

    /** Clears the window shape setting and restores the system default shape
    * @param [in] bRedraw Whether to repaint
    */
    void ClearWindowRgn(bool bRedraw);

    /** Sends a repaint message
    * @param [in] rcItem The repaint range, in client coordinates
    */
    void Invalidate(const UiRect& rcItem);

    /** Updates the window and repaints
    */
    bool UpdateWindow() const;

    /** Keeps the parent window active
    */
    void KeepParentActive();

    /** Gets the client area rectangle of the current window
    * @param [out] rcClient Returns the client area coordinates of the window
    */
    void GetClientRect(UiRect& rcClient) const;

    /** Gets the window rect of the current window
    * @param [out] rcWindow Returns the screen coordinates of the window top-left and bottom-right corners
    */
    void GetWindowRect(UiRect& rcWindow) const;

    /** Converts screen coordinates to the client coordinates of the current window
    * @param [out] pt Returns the client coordinates
    */
    void ScreenToClient(UiPoint& pt) const;

    /** Converts the client coordinates of the current window to screen coordinates
    * @param [out] pt Returns the screen coordinates
    */
    void ClientToScreen(UiPoint& pt) const;

    /** Gets the current mouse coordinates
    * @param [out] pt Returns the screen coordinates
    */
    void GetCursorPos(UiPoint& pt) const;

    /** Gets the monitor rectangle of the monitor containing the specified window
    * @param [out] rcMonitor The monitor rectangle
    */
    bool GetMonitorRect(UiRect& rcMonitor) const;

    /** Gets the work area rectangle of the current primary monitor
    * @param [out] rcWork Returns the primary screen coordinates
    */
    static bool GetPrimaryMonitorWorkRect(UiRect& rcWork);

    /** Gets the work area rectangle of the monitor containing the current window, in virtual screen coordinates.
        Note that if the monitor is not the primary monitor, some rectangle coordinates may be negative.
    * @param [out] rcWork Returns the screen coordinates
    */
    bool GetMonitorWorkRect(UiRect& rcWork) const;

    /** Gets the work area rectangle of the monitor containing the given point, in virtual screen coordinates.
        Note that if the monitor is not the primary monitor, some rectangle coordinates may be negative.
    * @param [out] pt The input is in screen coordinates
    * @param [out] rcWork Returns the screen coordinates
    */
    bool GetMonitorWorkRect(const UiPoint& pt, UiRect& rcWork) const;

    /** Gets the last mouse coordinates
    */
    const UiPoint& GetLastMousePos() const;

    /** Sets the last mouse coordinates
    */
    void SetLastMousePos(const UiPoint& pt);

    /** Gets the window interface at a point
    * @param [in] pt A point in screen coordinates
    * @param [in] bIgnoreChildWindow true to ignore child windows, false to not ignore them
    */
    INativeWindow* WindowBaseFromPoint(const UiPoint& pt, bool bIgnoreChildWindow = false);

    /** Sets whether the snap layout menu is supported (a Windows 11 feature: hover over the maximize button or press Win + Z to access snap layouts easily).
    *   This feature is enabled by default.
    * @param [in] bEnable true to enable, false to disable
    */
    void SetEnableSnapLayoutMenu(bool bEnable);

    /** Determines whether the snap layout menu is supported (only Windows 11 and later)
    */
    bool IsEnableSnapLayoutMenu() const;

    /** Sets whether the system window menu is shown when right-clicking the title bar (allows resizing the window, closing it, etc.)
    * @param [in] bEnable true to enable, false to disable
    */
    void SetEnableSysMenu(bool bEnable);

    /** Gets whether the system window menu is shown when right-clicking the title bar
    */
    bool IsEnableSysMenu() const;

public:
    /** Sets the system window activation hotkey; after registration, pressing it automatically activates this window
    * @param [in] wVirtualKeyCode Virtual key code, e.g. kVK_DOWN etc. See: https://learn.microsoft.com/windows/win32/inputdev/virtual-key-codes
    *             If wVirtualKeyCode is 0, the managed window activation hotkey is cancelled
    * @param [in] wModifiers Hotkey modifier flags, see the HotKeyModifiers enum
    * @return Return value description:
       -1: The function was unsuccessful; the hotkey is invalid.
        0: The function was unsuccessful; the window is invalid.
        1: The function succeeded, and no other window has the same hotkey.
        2: The function succeeded, but another window already has the same hotkey.
    */
    int32_t SetWindowHotKey(uint8_t wVirtualKeyCode, uint8_t wModifiers);

    /** Gets the system window activation hotkey
    * @param [out] wVirtualKeyCode Virtual key code, e.g. kVK_DOWN etc.
    * @param [out] wModifiers Hotkey modifier flags, see the HotKeyModifiers enum
    * @return false means no window activation hotkey was registered; otherwise one was registered
    */
    bool GetWindowHotKey(uint8_t& wVirtualKeyCode, uint8_t& wModifiers) const;

    /** Registers a system global hotkey; after successful registration, pressing it sends the window a WM_HOTKEY message
    * @param [in] wVirtualKeyCode Virtual key code, e.g. kVK_DOWN etc.
    * @param [in] wModifiers Hotkey modifier flags, see the HotKeyModifiers enum
    * @param [in] id Command ID; the application must specify an ID in the range 0x0000 to 0xBFFF.
                  To avoid conflicts with hotkey identifiers defined by other shared DLLs, a DLL should use the GlobalAddAtom function to obtain hotkey identifiers.
    */
    bool RegisterHotKey(uint8_t wVirtualKeyCode, uint8_t wModifiers, int32_t id);

    /** Unregisters the system global hotkey
    * @param [in] id The command ID, i.e. the ID used when registering
    */
    bool UnregisterHotKey(int32_t id);

    /** Cleans up the window resources
    */
    void ClearNativeWindow();

public:
    /** Calls the system default window procedure; a wrapper around the CallWindowProc API
    * @param [in] uMsg The message
    * @param [in] wParam The additional parameter of the message
    * @param [in] lParam The additional parameter of the message
    * @return Returns the message processing result
    */
    LRESULT CallDefaultWindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

    /** Sets the input method on/off state (turning it off and on preserves the previous input method state)
    * @param [in] bOpen true to enable the input method, false to disable it
    */
    void SetImeOpenStatus(bool bOpen);

    /** Sets the input region
    * @param [in] rect The text input rectangle
    * @param [in] nCursor The text input position (offset relative to rect.left)
    */
    void SetTextInputArea(const UiRect* rect, int32_t nCursor);

    /** Sets whether drag-and-drop is allowed
    * @param [in] bEnable true to allow drag-and-drop, false to disallow it
    */
    void SetEnableDragDrop(bool bEnable);

    /** Unregisters a drag-and-drop interface
    */
    bool IsEnableDragDrop() const;

    /** Gets the control interface at the specified coordinates
    * @param [in] pt A point in client coordinates
    */
    Control* FindControl(const UiPoint& pt) const;

    /** Whether the window needs to be centered when creating the window
    */
    bool NeedCenterWindowAfterCreated() const;

private:
    /** The window procedure
    * @param [in] hWnd The window handle
    * @param [in] uMsg The message
    * @param [in] wParam The additional parameter of the message
    * @param [in] lParam The additional parameter of the message
    * @return Returns the message processing result
    */
    static LRESULT CALLBACK __WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    /** The window procedure (the entry function of the modal dialog)
    * @param [in] hWnd The window handle
    * @param [in] uMsg The message
    * @param [in] wParam The additional parameter of the message
    * @param [in] lParam The additional parameter of the message
    * @return Returns the message processing result
    */
    static INT_PTR CALLBACK __DialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    /** The window procedure (modal dialog)
    * @param [in] hWnd The window handle
    * @param [in] uMsg The message
    * @param [in] wParam The additional parameter of the message
    * @param [in] lParam The additional parameter of the message
    * @return Returns the message processing result
    */
    static LRESULT CALLBACK __DialogWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    /** The window message handler; the first handler entered after a message is received from the system
    * @param [in] uMsg The message
    * @param [in] wParam The additional parameter of the message
    * @param [in] lParam The additional parameter of the message
    * @return Returns the result of processing the message
    */
    LRESULT WindowMessageProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

    /** The internal handler of window messages
     * @param [in] uMsg The message
     * @param [in] wParam The additional parameter of the message
     * @param [in] lParam The additional parameter of the message
     * @param[out] bHandled Whether the message has been handled. Returning true means the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure
     * @return Returns the result of processing the message
    */
    LRESULT ProcessInternalMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);

    //Some NC message handlers, to implement basic functionality
    LRESULT OnNcActivateMsg(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
    LRESULT OnNcCalcSizeMsg(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
    LRESULT OnNcHitTestMsg(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);

    //Some message handlers, to implement basic functionality
    LRESULT OnGetMinMaxInfoMsg(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
    LRESULT OnEraseBkGndMsg(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
    LRESULT OnDisplayChangedMsg(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
    LRESULT OnDpiChangedMsg(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
    LRESULT OnWindowPosChangingMsg(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);

    LRESULT OnNotifyMsg(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
    LRESULT OnCommandMsg(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
    LRESULT OnCtlColorMsgs(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);

    LRESULT OnPointerMsgs(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
    LRESULT OnTouchMsg(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);

    LRESULT OnCreateMsg(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
    LRESULT OnInitDialogMsg(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);

    /** The window message dispatcher; converts window messages to the internal format and dispatches them
    * @param [in] uMsg The message
    * @param [in] wParam The additional parameter of the message
    * @param [in] lParam The additional parameter of the message
    * @param[out] bHandled Whether the message has been handled. Returning true means the message has been successfully handled and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure
    * @return Returns the result of processing the message
    */
    LRESULT ProcessWindowMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);

private:
    /** Initializes the window resources
    */
    void InitNativeWindow();

    /** Called when the window is being destroyed; this is the last message for the window (the default implementation cleans up resources and calls OnDeleteSelf to destroy the window object)
    */
    void OnFinalMessage();

    /** Sets the Owner window as the focus window
    */
    bool SetOwnerWindowFocus();

    /** Gets the Owner window handle of the current window
    */
    HWND GetWindowOwner() const;

    /** Gets the monitor rectangle and work area rectangle of the monitor containing the specified window
    * @param [out] rcMonitor The monitor rectangle
    * @param [out] rcWork The work area rectangle of the monitor
    */
    bool GetMonitorRect(HWND hWnd, UiRect& rcMonitor, UiRect& rcWork) const;

    /** Gets the window rect of the current window
    * @param [out] rcWindow Returns the screen coordinates of the window top-left and bottom-right corners
    */
    void GetWindowRect(HWND hWnd, UiRect& rcWindow) const;

    /** Gets the key modifier flags of a message
    * @param [out] modifierKey Returns the modifier flags, see the ModifierKey type definition
    */
    bool GetModifiers(UINT message, WPARAM wParam, LPARAM lParam, uint32_t& modifierKey) const;

    /** Set whether it is a layered window
    */
    bool SetLayeredWindowStyle(bool bIsLayeredWindow, bool& bChanged) const;

    /** Updates the window style of the maximize/minimize buttons to match the program logic
    */
    void UpdateMinMaxBoxStyle() const;

    /** Shows the system window menu
    */
    bool ShowWindowSysMenu(HWND hWnd, const POINT& pt) const;

    /** Stops the system window menu timer
    */
    void StopSysMenuTimer();

    /** Performs the drawing operation
    */
    LRESULT OnPaintMsg(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);

    /** Synchronizes the window creation attributes
    */
    void SyncCreateWindowAttributes(const WindowCreateAttributes& createAttributes);

    /** Calculates the centered window position
    */
    bool CalculateCenterWindowPos(HWND hCenterWindow, int32_t& xPos, int32_t& yPos) const;

    /** Enables/disables the input method
    */
    void EnableIME(HWND hwnd, bool bEnable);

    /** Checks the window edge-snap operation and calls back to the application layer
    */
    void CheckWindowSnap(HWND hWnd);

    /** Sets the window icon (only *.ico format is supported)
    *  @param [in] iconFilePath The path of the ico file (absolute path)
    */
    bool SetWindowIconByIcoFile(const FilePath& iconFilePath);

private:    
    /** @name Drag-related interfaces
    * @{ */
    friend class WindowDropTarget;

    /** Methods of the drag-related interface (basically the same as the IDropTarget interface)
    * @param [out] bHandled If true is returned, the event has been handled and will not be forwarded to other UI controls
    */
    HRESULT OnDragEnter(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect, bool& bHandled);
    HRESULT OnDragOver(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect, bool& bHandled);
    HRESULT OnDragLeave();
    HRESULT OnDrop(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect, bool& bHandled);

    /** @} */
private:
    /** The interface receiving window events
    */
    INativeWindow* m_pOwner;

    /** The window handle
    */
    HWND m_hWnd;

    /** The parent window specified at creation
    */
    HWND m_hParentWnd;

    /** The initialization parameters when creating the window
    */
    WindowCreateParam m_createParam;

    /** The original window procedure (only used by modal dialogs)
    */
    WNDPROC m_pfnOldWndProc;

    /** Whether drag-and-drop is supported
    */
    bool m_bEnableDragDrop;

    /** Whether the current window is displayed as a modal dialog
    */
    bool m_bDoModal;

    /** Whether to close the window when ESC is pressed (only valid for modal dialogs)
    */
    bool m_bCloseByEsc;

    /** Whether to close the window when Enter is pressed (only valid for modal dialogs)
    */
    bool m_bCloseByEnter;

    //Whether it is a layered window
    bool m_bIsLayeredWindow;

    //Window opacity; this value is used as a parameter (BLENDFUNCTION.SourceConstantAlpha) in UpdateLayeredWindow
    uint8_t m_nLayeredWindowAlpha;

    //Window opacity; this value is used as a parameter (bAlpha) in SetLayeredWindowAttributes
    uint8_t m_nLayeredWindowOpacity;

    //Whether to use the system title bar
    bool m_bUseSystemCaption;

    //The window has been scheduled for delayed close
    bool m_bCloseing;

    //The window close parameter
    int32_t m_closeParam;

    //Whether the current window is displayed as a modal dialog
    bool m_bFakeModal;

    //Whether the snap layout menu is supported (a Windows 11 feature: hover over the maximize button or press Win + Z to access snap layouts easily)
    //Reference: https://learn.microsoft.com/windows/apps/desktop/modernize/apply-snap-layout-menu
    bool m_bSnapLayoutMenu;

    //Whether the system window menu is shown when right-clicking the title bar (allows resizing the window, closing it, etc.)
    bool m_bEnableSysMenu;

    //Whether the mouse clicked on the maximize button
    bool m_bNCLButtonDownOnMaxButton;

    //The timer ID for the delayed display of the system menu
    UINT_PTR m_nSysMenuTimerId;

    //The mouse position
    UiPoint m_ptLastMousePos;

    //The drawing DC
    HDC m_hDcPaint;

private:
    /**@name Fullscreen related state
    * @{ */

    /** Whether the window is in the fullscreen state
    */
    bool m_bFullscreen;

    /** Whether the window is exiting fullscreen
    */
    bool m_bFullscreenExiting;

    /** The window style before fullscreen
    */
    DWORD m_dwLastStyle;

    /** The window position/size and other info before fullscreen
    */
    WINDOWPLACEMENT m_rcLastWindowPlacement;

    /** @} */

    /** The system global hotkey ID
    */
    std::vector<int32_t> m_hotKeyIds;

    /** The minimum window size (width and height)
    */
    UiSize m_szMinWindow;

    /** The maximum window size (width and height)
    */
    UiSize m_szMaxWindow;

    /** The input method context
    */
    HIMC m_hImc;

    /** The drag-and-drop implementation interface
    */
    WindowDropTarget* m_pWindowDropTarget;

    /** The UI scale of the window
    */
    uint32_t m_nWindowDpiScaleFactor;

    /** Whether it is a child window (has the WS_CHILD style)
    */
    bool m_bChildWindow;

private:
    /** The IDataObject associated with the drag operation
    */
    IDataObject* m_pDataObj;

    /** The text data associated with the drag
    */
    std::vector<DString> m_textList;

    /** The file data associated with the drag
    */
    std::vector<DString> m_fileList;
};

/** Defines an alias
*/
typedef NativeWindow_Windows NativeWindow;

} // namespace ui

#endif //DUI_BUILD_FOR_WIN

#endif // UI_CORE_NATIVE_WINDOW_WINDOWS_H_

