#ifndef UI_CORE_NATIVE_WINDOW_SDL_H_
#define UI_CORE_NATIVE_WINDOW_SDL_H_

#include "dui/Core/INativeWindow.h"
#include "dui/Core/WindowCreateParam.h"
#include "dui/Core/WindowCreateAttributes.h"
#include "dui/Utils/FilePath.h"

#if defined(DUI_BUILD_FOR_SDL) || defined(DUI_BUILD_FOR_WAYLAND)

#include <unordered_map>

//SDL types, forward declared
struct SDL_Window;
struct SDL_Renderer;
struct SDL_Point;

//Wayland types, forward declarations
#if defined(DUI_BUILD_FOR_WAYLAND)
struct wl_surface;
struct xdg_surface;
struct xdg_toplevel;
struct wl_egl_window;
struct wl_callback;
#endif
typedef uint32_t SDL_WindowID;
typedef uint64_t SDL_WindowFlags;
typedef uint32_t SDL_Keycode;
typedef uint16_t SDL_Keymod;
typedef uint32_t SDL_PropertiesID;
union SDL_Event;

namespace ui {

class WindowDropTarget;

/** Windows platform implementation of window functionality
*/
class DUI_API NativeWindow_SDL
{
public:
    explicit NativeWindow_SDL(INativeWindow* pOwner);
    NativeWindow_SDL(const NativeWindow_SDL& r) = delete;
    NativeWindow_SDL& operator=(const NativeWindow_SDL& r) = delete;
    ~NativeWindow_SDL();

public:
    /** Gets the window ID associated with the SDL event
    * @return Returns 0 if there is no associated window ID
    */
    static SDL_WindowID GetWindowIdFromEvent(const SDL_Event& sdlEvent);

    /** Gets the window pointer by the window ID
    */
    static NativeWindow_SDL* GetWindowFromID(SDL_WindowID id);

    /** Gets the simulated Hover message ID
    */
    static uint32_t GetHoverMsgId();

    /** The window message handler; the first handler entered after a message is received from the system
    * @param [in] sdlEvent The message data
    * @return Returns true if the message was handled internally, otherwise false
    */
    bool OnSDLWindowEvent(const SDL_Event& sdlEvent);

public:
    /** Creates the window
    * @param [in] pParentWindow The parent window
    * @param [in] createParam The parameters required to create the window
    * @param [in] createAttributes The Window attributes from the XML file
    */
    bool CreateWnd(NativeWindow_SDL* pParentWindow,
                  const WindowCreateParam& createParam,
                  const WindowCreateAttributes& createAttributes);

    /** Shows a modal window
    * @param [in] pParentWindow The parent window
    * @param [in] createParam The parameters required to create the window
    * @param [in] bCloseByEsc Whether to close the window when ESC is pressed
    * @param [in] bCloseByEnter Whether to close the window when Enter is pressed
    * @return The return value when the window exits; -1 on failure
    */
    int32_t DoModal(NativeWindow_SDL* pParentWindow,
                    const WindowCreateParam& createParam,
                    const WindowCreateAttributes& createAttributes,
                    bool bCloseByEsc = true,
                    bool bCloseByEnter = false);

    /** Creates a child window (a non-popup child window)
    * @param [in] pParentWindow The parent window
    * @param [in] nX The X coordinate of the child window (relative to the parent window)
    * @param [in] nY The Y coordinate of the child window (relative to the parent window)
    * @param [in] nWidth The width of the child window
    * @param [in] nHeight The height of the child window
    */
    bool CreateChildWnd(NativeWindow_SDL* pParentWindow, int32_t nX, int32_t nY, int32_t nWidth, int32_t nHeight);

    /** Gets the native window handle
    */
    void* GetWindowHandle() const;

    /** Gets the associated INativeWindow interface
    */
    INativeWindow* GetOwner() const { return m_pOwner; }

    /** Gets the driver name of the current window implementation
    */
    DString GetVideoDriverName() const;

    /** Gets the name of the current Render drawing engine
    */
    DString GetWindowRenderName() const;

    /** Whether a valid window handle exists
    */
    bool IsWindow() const;

    /** Whether it is a child window
    */
    bool IsChildWindow() const;

    /** Sets or changes the parent window
    */
    bool SetParentWindow(NativeWindow_SDL* pParentWindow);

#ifdef DUI_BUILD_FOR_WIN
    /** Gets the window handle
    */
    HWND GetHWND() const;

    /** Gets the resource handle
    */
    HMODULE GetResModuleHandle() const;

    /** Gets the drawing DC handle of the window
    */
    HDC GetPaintDC() const;
#endif

#if defined (DUI_BUILD_FOR_LINUX) || defined (DUI_BUILD_FOR_FREEBSD)
    /** Whether the backend engine of the current window is X11
    */
    bool IsVideoDriverX11() const;

    /** Whether the backend engine of the current window is X11
    */
    bool IsVideoDriverWayland() const;

    /** Gets the Display pointer of the X11 window (corresponding to the X11 ::Display* type)
    */
    size_t GetX11DisplayPointer() const;

    /** Gets the X11 Screen identifier (corresponding to X11 Screen)
    */
    uint64_t GetX11ScreenNumber() const;

    /** Gets the X11 window identifier (corresponding to the X11 ::Window type)
    */
    uint64_t GetX11WindowNumber() const;

    /** Gets the Display pointer of the Wayland window (corresponding to the Wayland wl_display* type)
    */
    size_t GetWaylandDisplayPointer() const;

#if defined(DUI_BUILD_FOR_WAYLAND)
    /** Start an interactive move or resize based on mouse position
    * @param pt Mouse position in client coordinates
    * @param serial Button press serial from wl_pointer.button event
    */
    void ProcessWaylandMoveResize(const UiPoint& pt, uint32_t serial);
#endif

#if defined(DUI_BUILD_FOR_WAYLAND)
public:
    wl_surface* m_pWaylandSurface;
    xdg_surface* m_pXdgSurface;
    xdg_toplevel* m_pXdgToplevel;
    wl_egl_window* m_pEglWindow;
    bool m_bWaylandConfigured;
    int32_t m_nWaylandPendingWidth;
    int32_t m_nWaylandPendingHeight;
    bool m_bWaylandVisible;
    bool m_bWaylandBufferBusy;
    bool m_bWaylandMaximized;
    bool m_bWaylandFullscreen;
private:
#endif

#endif

#if defined DUI_BUILD_FOR_MACOS
    /** Gets the NSView* pointer
    */
    void* GetNSView() const;

    /** Gets the NSWindow* pointer
    */
    void* GetNSWindow() const;
#endif 

public:
    /** Closes the window asynchronously; after the function returns, IsClosing() is true
    * @param [in] nRet The close parameter, see: enum WindowCloseParam
    */
    void CloseWnd(int32_t nRet = kWindowCloseNormal);

    /** Closes the window synchronously
    */
    void Close();

    /** Whether the window is about to close
    */
    bool IsClosingWnd() const;

    /** Gets the window close parameter
    * @return See enum WindowCloseParam; it may also be a custom value
    */
    int32_t GetCloseParam() const;

public:
    /** Sets whether the window is a layered window
    * @param [in] bIsLayeredWindow true to set the window as a layered window, otherwise as a non-layered window
    * @param [in] bRedraw Whether to repaint the window (after a property change, the UI may display incorrectly if not repainted)
    */
    bool SetLayeredWindow(bool bIsLayeredWindow, bool bRedraw);

    /** Whether it is a layered window
    */
    bool IsLayeredWindow() const;

    /** Sets the window opacity (only valid when IsLayeredWindow() is true)
    * @param [in] nAlpha Opacity value [0, 255]. When nAlpha is 0 the window is fully transparent; when nAlpha is 255 the window is fully opaque.
    *             This parameter is used in the UpdateLayeredWindow function.
    */

    /** True when the platform provides OS shadows (macOS: yes; Windows: DWM
     *  composition enabled; other platforms: no).
    */
    bool IsSystemShadowSupported() const;

    /** Enable/disable the OS-provided shadow (macOS NSWindow / Windows DWM);
     *  returns false when unsupported or the native call failed.
    */
    bool SetSystemShadowType(NativeWindowShadowType nativeShadowType);

    /** Current OS shadow state. */
    NativeWindowShadowType GetSystemShadowType() const;

    /** Re-apply the OS shadow state after the window changes (e.g. fullscreen
     *  exit); call on the UI thread.
    */
    void RefreshSystemShadow();

    /** Clear the window region (RGN) - required when enabling the OS shadow.
    */
    void ClearWindowRgnForSystemShadow();

    void SetLayeredWindowAlpha(int32_t nAlpha);

    /** Gets the window opacity (only valid when IsLayeredWindow() is true)
    * @param [in] nAlpha Opacity value [0, 255]. When nAlpha is 0 the window is fully transparent; when nAlpha is 255 the window is fully opaque.
    */
    uint8_t GetLayeredWindowAlpha() const;

    /** Sets the window opacity (only valid when IsLayeredWindow() is true; if the window is not layered, it is automatically made layered)
    * @param [in] nAlpha Opacity value [0, 255]. When nAlpha is 0 the window is fully transparent; when nAlpha is 255 the window is fully opaque.
    *             This parameter is used in the SetLayeredWindowAttributes function (bAlpha).
    */
    void SetLayeredWindowOpacity(int32_t nAlpha);

    /** Gets the window opacity (only valid when IsLayeredWindow() is true)
    * @param [in] nAlpha Opacity value [0, 255]. When nAlpha is 0 the window is fully transparent; when nAlpha is 255 the window is fully opaque.
    */
    uint8_t GetLayeredWindowOpacity() const;

    /** Sets whether to use the system title bar
    */
    void SetUseSystemCaption(bool bUseSystemCaption);

    /** Gets whether the system title bar is used
    */
    bool IsUseSystemCaption() const;

public:
    /** Window show, hide, maximize, restore and minimize operations
     * @param [in] nCmdShow The command to show or hide the window
    */
    bool ShowWindow(ShowWindowCommands nCmdShow);

    /** Shows a modal dialog (the parent window is specified at creation)
    */
    void ShowModalFake(NativeWindow_SDL* pParentWindow);

    /** The modal dialog closed; synchronize the state
    */
    void OnCloseModalFake(NativeWindow_SDL* pParentWindow);

    /** Whether it is a simulated modal window (shown via the ShowModalFake function)
    */
    bool IsFakeModal() const;

    /** Whether it is a modal dialog mode (a dialog window shown via DoModal)
    */
    bool IsDoModal() const;

    /** Centers the window, supporting extended screens
    */
    void CenterWindow();

    /** Whether to make the window always-on-top
    * @param [in] bOnTop true to make the window always-on-top, false to cancel it
    */
    void SetWindowAlwaysOnTop(bool bOnTop);

    /** Determines whether the current window is always-on-top
    * @return true means the window is currently always-on-top, false means it is not
    */
    bool IsWindowAlwaysOnTop() const;

    /** Makes the window the foreground window
    */
    bool SetWindowForeground();

    /** Whether the current window is the foreground window
    */
    bool IsWindowForeground() const;

    /** Makes the window the focus window
    */
    bool SetWindowFocus();

    /** Removes the focus from the window
    */
    bool KillWindowFocus();

    /** Whether the current window is the input focus window
    */
    bool IsWindowFocused() const;

    /** Checks and ensures the current window is the focus window
    */
    void CheckSetWindowFocus();

    /** Posts a message to the message queue
    * @param [in] uMsg The message type
    * @param [in] wParam The message extra parameter
    * @param [in] lParam The message extra parameter
    * @return Returns the window result of processing the message; -1 on error
    */
    LRESULT PostMsg(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0L);

    /** Sends an exit message to the message queue
    * @param [in] nExitCode The exit code
    */
    static void PostQuitMsg(int32_t nExitCode);

public:
    /** Makes the window enter fullscreen
    */
    bool EnterFullscreen();

    /** Exits the window fullscreen state (by default, pressing ESC exits fullscreen)
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

    /** Sets the Enable state of the window
    * @param [in] bEnable true to set the Enable state, false to set the disable state
    */
    bool EnableWindow(bool bEnable);

    /** Gets the Enable state of the window
    */
    bool IsWindowEnabled() const;

    /** Whether the window is visible
    */
    bool IsWindowVisible() const;

public:
    /** Sets the window position (a wrapper around the ::SetWindowPos API, no internal DPI scaling)
    * @param [in] pInsertAfterWindow Corresponds to the hWndInsertAfter option of SetWindowPos
    * @param [in] insertAfterFlag Corresponds to the hWndInsertAfter option of SetWindowPos; only valid when pWindow is nullptr
    * @param [in] X The X coordinate of the window
    * @param [in] Y The Y coordinate of the window
    * @param [in] cx The width of the window
    * @param [in] cy The height of the window
    * @param [in] uFlags See the WindowPosFlags enum options
    */
    bool SetWindowPos(const NativeWindow_SDL* pInsertAfterWindow,
                      InsertAfterFlag insertAfterFlag,
                      int32_t X, int32_t Y, int32_t cx, int32_t cy, uint32_t uFlags);

    /** Sets the window position and size
    * @param [in] X The X coordinate of the window
    * @param [in] Y The Y coordinate of the window
    * @param [in] nWidth The width of the window
    * @param [in] nHeight The height of the window
    * @param [in] bRepaint Whether to repaint the window
    */
    bool MoveWindow(int32_t X, int32_t Y, int32_t nWidth, int32_t nHeight, bool bRepaint);

    /** Sets the window icon (supports *.ico; other formats are also supported, but ICO is recommended)
    *  @param [in] iconFilePath The path of the ico file (absolute path)
    */
    bool SetWindowIcon(const FilePath& iconFilePath);

    /** Sets the window icon (supports *.ico; other formats are also supported, but ICO is recommended)
    *  @param [in] iconFileData The icon file data
    *  @param [in] iconFileName The file name with extension, used to identify the image type
    */
    bool SetWindowIcon(const std::vector<uint8_t>& iconFileData, const DString& iconFileName);

    /** Sets the window title bar text
    * @param [in] strText The window title bar text
    */
    void SetText(const DString& strText);

    /** Gets the window title bar text
    */
    DString GetText() const;

    /** Sets the minimum window size (width and height; no internal DPI scaling; the caller handles DPI adaptation)
    * @param [in] szMaxWindow The maximum width and height of the window; 0 means no limit
    */
    void SetWindowMaximumSize(const UiSize& szMaxWindow);

    /** Gets the minimum window size (width and height)
    */
    const UiSize& GetWindowMaximumSize() const;

    /** Sets the maximum window size (width and height; no internal DPI scaling; the caller handles DPI adaptation)
    * @param [in] szMinWindow The minimum width and height of the window; 0 means no limit
    */
    void SetWindowMinimumSize(const UiSize& szMinWindow);

    /** Gets the maximum window size (width and height)
    */
    const UiSize& GetWindowMinimumSize() const;

public:
    /** Sets the mouse capture window handle to the current drawing window
    * @param [in]
    */
    void SetCapture();

    /** Releases resources when the window no longer needs mouse input
    */
    void ReleaseCapture();

    /** Determines whether mouse input is currently captured
    */
    bool IsCaptured() const;

    /** Sets the window shape to a rounded rectangle
    * @param [in] rcWnd The region to set as the RGN, in screen coordinates
    * @param [in] rx The corner radius width; it must not be 0
    * @param [in] ry The corner radius height; it must not be 0
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
    * @param [in] wParam The message extra parameter
    * @param [in] lParam The message extra parameter
    * @return Returns the message processing result
    */
    LRESULT CallDefaultWindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

    /** The SDL Hit Test callback function
    */
    int32_t SDL_HitTest(SDL_Window* win, const SDL_Point* area, void* data);

    /** Draws the window
    * @param [in] bPaintAll true means the message is triggered by the system and the whole area must be painted; false means the program triggered it via Invalidate, allowing partial painting
    */
    void PaintWindow(bool bPaintAll);

    /** The update region of the window (needs painting)
    */
    const UiRect& GetUpdateRect() const;

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

    /** Whether the window needs to be centered when created
    */
    bool NeedCenterWindowAfterCreated() const;

public:
    //Several API interfaces supporting high-DPI screens
    bool GetWindowSize(int32_t* w, int32_t* h) const;
    bool GetWindowSizeInPixels(int32_t* w, int32_t* h) const;
    float GetDisplayContentScale() const;   //Gets the content display scale of the screen containing the window
    float GetWindowDisplayScale() const;    //Gets the content display scale of this window (which may differ from the display scale of the screen containing the window)
    float GetWindowPixelDensity() const;    //Gets the pixel density of the window

private:
    /** Creates the window and the rendering interface
    */
    bool CreateWindowAndRender(NativeWindow_SDL* pParentWindow, const WindowCreateAttributes& createAttributes);

    /** Initializes the window resources
    */
    void InitNativeWindow();

    /** Called when the window is being destroyed; this is the last message for the window (the default implementation cleans up resources and calls OnDeleteSelf to destroy the window object)
    */
    void OnFinalMessage();

    /** Gets the monitor rectangle and work area rectangle of the monitor containing the specified window
    * @param [out] rcMonitor The monitor rectangle
    * @param [out] rcWork The work area rectangle of the monitor
    */
    bool GetMonitorRect(SDL_Window* sdlWindow, UiRect& rcMonitor, UiRect& rcWork) const;

    /** Gets the window rect of the current window
    * @param [out] rcWindow Returns the screen coordinates of the window top-left and bottom-right corners
    */
    void GetWindowRect(SDL_Window* sdlWindow, UiRect& rcWindow) const;

    /** Synchronizes the window creation attributes
    * @param [in] bSupportTransparent Whether transparency is supported; only valid on non-Windows systems
    */
    void SyncCreateWindowAttributes(const WindowCreateAttributes& createAttributes, bool bSupportTransparent);

    /** Sets the window creation attributes from the window creation input parameters
    * @param [in] bUseOpenGL Whether to use the OpenGL rendering interface; only valid on non-Windows systems
    */
    void SetCreateWindowProperties(SDL_PropertiesID props,
                                   NativeWindow_SDL* pParentWindow,
                                   const WindowCreateAttributes& createAttributes,
                                   bool bUseOpenGL);

    /** Creates the Render associated with the SDL window
    */
    SDL_Renderer* CreateSdlRenderer(const DString& sdlRenderName) const;

    /** Gets the Render name list (in priority order)
    */
    void GetRenderNameList(const DString& externalRenderName, std::vector<DString>& renderNames) const;

    /** Reads the Render attributes that will be used
    * @param [in] The externally supplied Render name
    * @param [out] bOpenGL Whether OpenGL is supported
    * @param [out] bOpenGLES2 Whether OpenGL ES2 is supported
    * @param [out] bSupportTransparent Whether transparency is supported
    */
    void QueryRenderProperties(const DString& externalRenderName, bool& bOpenGL, bool& bOpenGLES2, bool& bSupportTransparent) const;

    /** Determines whether a Render supports transparency
    */
    bool IsRenderSupportTransparent(const DString& renderName) const;

    /** Creates an SDL window
    */
    SDL_Window* CreateSdlWindow(NativeWindow_SDL* pParentWindow, const WindowCreateAttributes& createAttributes);

    /** Calculates the centered window position
    */
    bool CalculateCenterWindowPos(SDL_Window* pCenterWindow, int32_t& xPos, int32_t& yPos) const;

    /** Checks the window edge-snap operation and calls back to the application layer
    */
    void CheckWindowSnap(SDL_Window* window);

private:
    /** Sets the mapping between the window ID and the window pointer
    */
    static void SetWindowFromID(SDL_WindowID id, NativeWindow_SDL* pNativeWindow);

    /** Clears the mapping between the window ID and the window pointer
    */
    static void ClearWindowFromID(SDL_WindowID id, NativeWindow_SDL* pNativeWindow);

    /** Converts an SDL key to the internal ModifierKey
    */
    static uint32_t GetModifiers(SDL_Keymod keymod);

private:
    /** @name Drag-related interfaces
    * @{ */
    friend class WindowDropTarget;

    /** SDL_EVENT_DROP_BEGIN
    */
    void OnDropBegin();

    /** SDL_EVENT_DROP_POSITION
    * @param [in] pt A point in client coordinates
    * @param [out] bHandled If true is returned, the event has been handled and will not be forwarded to other UI controls
    */
    void OnDropPosition(const UiPoint& pt, bool& bHandled);

    /** SDL_EVENT_DROP_TEXT
    * @param [in] textList The text content; each element in the container represents one line of text
    * @param [out] bHandled If true is returned, the event has been handled and will not be forwarded to other UI controls
    */
    void OnDropTexts(const std::vector<DString>& textList, const UiPoint& pt, bool& bHandled);

    /** SDL_EVENT_DROP_FILE
    * @param [in] source The drag-and-drop source
    * @param [in] fileList The file paths; each element in the container represents one file
    * @param [out] bHandled If true is returned, the event has been handled and will not be forwarded to other UI controls
    */
    void OnDropFiles(const DString& source, const std::vector<DString>& fileList, const UiPoint& pt, bool& bHandled);

    /** SDL_EVENT_DROP_COMPLETE or another message that causes the leave
    */
    void OnDropLeave();

    /** @} */

private:
    /** The mapping between window pointers and SDL window IDs, used to relay messages
    */
    static std::unordered_map<SDL_WindowID, NativeWindow_SDL*> s_windowIDMap;

    /** The interface receiving window events
    */
    INativeWindow* m_pOwner;

    /** SDL window
    */
    SDL_Window* m_sdlWindow;

    /** The SDL window rendering interface
    */
    SDL_Renderer* m_sdlRenderer;

    /** Whether it is a child window
    */
    bool m_bChildWindow;

    /** The window has been scheduled for delayed close
    */
    bool m_bCloseing;

    /** The window close parameter
    */
    int32_t m_closeParam;

    /** The window flags before fullscreen
    */
    SDL_WindowFlags m_lastWindowFlags;

    /** Whether the window is in fullscreen state
    */
    bool m_bFullscreen;

    /** Whether the window is exiting fullscreen
    */
    bool m_bFullscreenExiting;

    /** Whether the window is maximized when entering fullscreen
    */
    bool m_bFullscreenMaximized;

    /** The minimum window size (width and height)
    */
    UiSize m_szMinWindow;

    /** The maximum window size (width and height)
    */
    UiSize m_szMaxWindow;

    /** The initial window size and the initial coordinates of the window top-left corner (needed in Wayland mode)
    */
    UiSize m_szInitWindow;
    UiPoint m_ptInitWindow;
    bool m_bInitWindowPosFlag;

    /** The mouse position
    */
    UiPoint m_ptLastMousePos;

    /** Whether to use the system title bar
    */
    bool m_bUseSystemCaption;

    /** The mouse event capture state
    */
    bool m_bMouseCapture;

    /** Window opacity; on Windows this value is used as a parameter (BLENDFUNCTION.SourceConstantAlpha) in UpdateLayeredWindow; other platforms behave similarly
    */
    uint8_t m_nLayeredWindowAlpha;

    /** Window opacity; on Windows this value is used as a parameter (bAlpha) in SetLayeredWindowAttributes; other platforms behave similarly
    */
    uint8_t m_nLayeredWindowOpacity;

    /** Whether drag-and-drop is supported
    */
    bool m_bEnableDragDrop;

    /** Whether the current window is displayed as a modal dialog
    */
    bool m_bFakeModal;

    /** Whether the current window is displayed as a modal dialog
    */
    bool m_bDoModal;

    /** The initialization parameters when creating the window
    */
    WindowCreateParam m_createParam;

    /** Whether it is a layered window
    */
    bool m_bIsLayeredWindow;
    NativeWindowShadowType m_systemShadowType = NativeWindowShadowType::kShadowSystemDisabled;

    /** The update region of the window (needs painting)
    */
    UiRect m_rcUpdateRect;

    /** Drag-and-drop support
    */
    std::unique_ptr<WindowDropTarget> m_pWindowDropTarget;
};

/** Defines an alias
*/
typedef NativeWindow_SDL NativeWindow;

} // namespace ui

#endif //DUI_BUILD_FOR_SDL

#endif // UI_CORE_NATIVE_WINDOW_SDL_H_
