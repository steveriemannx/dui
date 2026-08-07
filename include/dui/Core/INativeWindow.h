#ifndef UI_CORE_INATIVE_WINDOW_H_
#define UI_CORE_INATIVE_WINDOW_H_

#include "dui/Core/Keyboard.h"
#include "dui/Core/Callback.h"
#include "dui/Core/UiTypes.h"
#include "dui/Core/WindowMessage.h"
#include "dui/Core/DpiManager.h"
#include "dui/Render/IRender.h"

namespace ui
{
/** Window message filter interface, used to intercept window procedure messages, processed with priority over the Window class
*/
class DUI_API IUIMessageFilter
{
public:
    virtual ~IUIMessageFilter() = default;

    /**  Message handling function, processed with higher priority than the Window class message handler
    * @param [in] uMsg The message content
    * @param [in] wParam The additional parameter of the message
    * @param [in] lParam The additional parameter of the message
    * @param [out] bHandled Returns false to continue dispatching the message, returns true to stop dispatching the message
    * @return Returns the message handling result
    */
    virtual LRESULT FilterMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled) = 0;
};

/** OS-provided shadow state (macOS NSWindow / Windows DWM).
 */
enum class NativeWindowShadowType
{
    kShadowSystemDisabled   = 0,    //System shadow disabled (self-drawn shadow used instead)
    kShadowSystemDefault    = 1,    //OS default shadow
    kShadowSystemDoNotRound = 2,    //OS shadow, square corners
    kShadowSystemRound      = 3,    //OS shadow, rounded corners
    kShadowSystemSmallRound = 4,    //OS shadow, small rounded corners
};

class Control;

class DUI_API INativeWindow: public virtual SupportWeakCallback
{
public:
    /** Get the DPI manager corresponding to this window
    */
    virtual const DpiManager& OnNativeGetDpi() const = 0;

    /** True when the platform provides OS shadows (macOS: yes; Windows: DWM
     *  composition on; other platforms: no).
    */
    virtual bool IsSystemShadowSupported() const = 0;

    /** Enable/disable the OS-provided shadow; returns false when unsupported
     *  or the native call failed.
    */
    virtual bool SetSystemShadowType(NativeWindowShadowType nativeShadowType) = 0;

    /** Current OS shadow state. */
    virtual NativeWindowShadowType GetSystemShadowType() const = 0;

    /** Get the size of the window shadow
    * @param [out] rcShadow Get the size of the rounded corner
    */
    virtual void OnNativeGetShadowCorner(UiPadding& rcShadow) const = 0;

    /** Get the size of the stretchable range on the four sides of the window
    */
    virtual UiRect OnNativeGetSizeBox() const = 0;

    /** Get the window title bar area (draggable area), corresponding to the caption attribute in XML
    * @param [out] captionRect Returns the rectangular range of the title bar area (rcClient represents the rectangular range of the window client area):
    *              Title bar left  : rcClient.left + rcCaption.left
    *              Title bar top   : rcClient.top + rcCaption.top
    *              Title bar right : rcClient.right - rcCaption.right
    *              Title bar bottom: rcClient.top + rcCaption.bottom
    */
    virtual void OnNativeGetCaptionRect(UiRect& captionRect) const = 0;

    /** Get the rectangular area occupied by the system menu in the window title bar area; double-clicking this area closes the window, corresponding to the sys_menu_rect attribute in XML,
    *@param [out] sysMenuRect Returns the rectangular area, which must be a sub-area of the client area corresponding to the caption
    */
    virtual void OnNativeGetSysMenuRect(UiRect& sysMenuRect) const = 0;

    /** Determine whether a point is on a control placed on the title bar
    */
    virtual bool OnNativeIsPtInCaptionBarControl(const UiPoint& pt) const = 0;

    /** Determine whether it has maximize and minimize buttons
    * @param [out] bMinimizeBox Returns true if it has the minimize button
    * @param [out] bMaximizeBox Returns true if it has the maximize button
    */
    virtual bool OnNativeHasMinMaxBox(bool& bMinimizeBox, bool& bMaximizeBox) const = 0;

    /** Determine whether a point is on the maximize or restore button
    */
    virtual bool OnNativeIsPtInMaximizeRestoreButton(const UiPoint& pt) const = 0;

    /** Enter fullscreen state
    */
    virtual void OnNativeWindowEnterFullscreen() = 0;

    /** Exit fullscreen state
    */
    virtual void OnNativeWindowExitFullscreen() = 0;

    /** Called when the window is about to be closed, for subclasses to do some cleanup work
    */
    virtual void OnNativePreCloseWindow() = 0;

    /** Called when the window has been closed, for subclasses to do some cleanup work
    */
    virtual void OnNativePostCloseWindow() = 0;

    /** Switch between the system title bar and the self-drawn title bar
    */
    virtual void OnNativeUseSystemCaptionBarChanged() = 0;

    /** Prepare to paint
    * @return Returns true to continue painting, returns false to stop painting
    */
    virtual bool OnNativePreparePaint() = 0;

    /** Get the paint engine object
    */
    virtual IRender* OnNativeGetRender() const = 0;

    /** Get the control interface at the specified coordinate point
    * @param [in] pt The coordinate point in the client area
    */
    virtual Control* OnNativeFindControl(const UiPoint& pt) const = 0;

public:
    /** @name Window message handling
     * @{
     */
     /** Called when the window is destroyed; this is the last message of the window (the default implementation of this class cleans up resources and calls the OnDeleteSelf function to destroy the window object)
     */
    virtual void OnNativeFinalMessage() = 0;

    /** Dispatch function of window messages
    */
    virtual LRESULT OnNativeWindowMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled) = 0;

    /** Handle the system notification message of display resolution changes (WM_DISPLAYCHANGE)
    * @param [in] nColorDepth The new image depth of the display, in bits per pixel
    * @param [in] nScreenWidth The horizontal resolution of the screen
    * @param [in] nScreenHeight The vertical resolution of the screen
    */
    virtual void OnNativeDisplayResolutionChangedMsg(int32_t nColorDepth, int32_t nScreenWidth, int32_t nScreenHeight) = 0;

    /** Handle the system notification message of DPI changes (WM_DPICHANGED), for internal processing
    * @param [in] fNewDisplayScale The new UI display scale value of the window; 1.0f means no scaling
    * @param [in] fNewPixelDensity The new pixel density value of the window (only used by the SDL implementation)
    */
    virtual void OnNativeProcessDisplayScaleChangedMsg(float fNewDisplayScale, float fNewPixelDensity) = 0;

    /** Handle the system notification message of DPI changes (WM_DPICHANGED), to notify the application layer
    * @param [in] fNewDisplayScale The new UI display scale value of the window; 1.0f means no scaling
    * @param [in] fNewPixelDensity The new pixel density value of the window (only used by the SDL implementation)
    */
    virtual void OnNativeDisplayScaleChangedMsg(float fNewDisplayScale, float fNewPixelDensity) = 0;

    /** Event of successful window creation (WM_CREATE/WM_INITDIALOG)
    * @param [in] bDoModal Whether it is currently a modal dialog displayed through the DoModal function
    * @param [in] nativeMsg The original message content received from the system
    * @param [out] bHandled Whether the message has been handled; returns true if the message has been handled successfully and does not need to be passed to the window procedure; returns false to continue passing the message to the window procedure
    * @return Returns the result of message handling; if the application handles this message, it should return zero
    */
    virtual void OnNativeCreateWndMsg(bool bDoModal, const NativeMsg& nativeMsg, bool& bHandled) = 0;

    /** The window position and size changed (WM_WINDOWPOSCHANGED)
    * @param [in] nativeMsg The original message content received from the system
    * @param [out] bHandled Whether the message has been handled; returns true if the message has been handled successfully and does not need to be passed to the window procedure; returns false to continue passing the message to the window procedure
    * @return Returns the result of message handling; if the application handles this message, it should return zero
    */
    virtual LRESULT OnNativeWindowPosChangedMsg(const NativeMsg& nativeMsg, bool& bHandled) = 0;

    /** The window size changed (WM_SIZE)
    * @param [in] sizeType The type that triggered the window size change
    * @param [in] newWindowSize The new window size (width and height)
    * @param [in] nativeMsg The original message content received from the system
    * @param [out] bHandled Whether the message has been handled; returns true if the message has been handled successfully and does not need to be passed to the window procedure; returns false to continue passing the message to the window procedure
    * @return Returns the result of message handling; if the application handles this message, it should return zero
    */
    virtual LRESULT OnNativeSizeMsg(WindowSizeType sizeType, const UiSize& newWindowSize, const NativeMsg& nativeMsg, bool& bHandled) = 0;

    /** The window moved (WM_MOVE)
    * @param [in] ptTopLeft The x and y coordinates of the top-left corner of the window client area (in screen coordinates)
    * @param [in] nativeMsg The original message content received from the system
    * @param [out] bHandled Whether the message has been handled; returns true if the message has been handled successfully and does not need to be passed to the window procedure; returns false to continue passing the message to the window procedure
    * @return Returns the result of message handling; if the application handles this message, it should return zero
    */
    virtual LRESULT OnNativeMoveMsg(const UiPoint& ptTopLeft, const NativeMsg& nativeMsg, bool& bHandled) = 0;

    /** The window is shown or hidden (WM_SHOWWINDOW)
    * @param [in] bShow true means the window is being shown, false means the window is being hidden
    * @param [out] bHandled Whether the message has been handled; returns true if the message has been handled successfully and does not need to be passed to the window procedure; returns false to continue passing the message to the window procedure
    * @return Returns the result of message handling; if the application handles this message, it should return zero
    */
    virtual LRESULT OnNativeShowWindowMsg(bool bShow, const NativeMsg& nativeMsg, bool& bHandled) = 0;

    /** The window paints (SDL_EVENT_WINDOW_EXPOSED/WM_PAINT)
    * @param [in] rcPaint The rectangular area that needs to be updated in this paint
    * @param [in] nativeMsg The original message content received from the system
    *             SDL implementation: nativeMsg.uMsg value is SDL_EVENT_WINDOW_EXPOSED, nativeMsg.wParam value is the SDL_Window* pointer
    *             Windows implementation: nativeMsg.uMsg value is WM_PAINT, nativeMsg.wParam value is the HWND handle of the window
    * @param [out] bHandled Whether the message has been handled; returns true if the message has been handled successfully and does not need to be passed to the window procedure; returns false to continue passing the message to the window procedure
    * @return Returns the result of message handling; if the application handles this message, it should return zero
    */    
    virtual LRESULT OnNativePaintMsg(const UiRect& rcPaint, const NativeMsg& nativeMsg, bool& bHandled) = 0;

    /** The window gained focus (WM_SETFOCUS)
    * @param [in] pLostFocusWindow The window that lost keyboard focus (can be nullptr)
    * @param [in] nativeMsg The original message content received from the system
    * @param [out] bHandled Whether the message has been handled; returns true if the message has been handled successfully and does not need to be passed to the window procedure; returns false to continue passing the message to the window procedure
    * @return Returns the result of message handling; if the application handles this message, it should return zero
    */
    virtual LRESULT OnNativeSetFocusMsg(INativeWindow* pLostFocusWindow, const NativeMsg& nativeMsg, bool& bHandled) = 0;

    /** The window lost focus (WM_KILLFOCUS)
    * @param [in] pSetFocusWindow The window that receives keyboard focus (can be nullptr)
    * @param [in] nativeMsg The original message content received from the system
    * @param [out] bHandled Whether the message has been handled; returns true if the message has been handled successfully and does not need to be passed to the window procedure; returns false to continue passing the message to the window procedure
    * @return Returns the result of message handling; if the application handles this message, it should return zero
    */
    virtual LRESULT OnNativeKillFocusMsg(INativeWindow* pSetFocusWindow, const NativeMsg& nativeMsg, bool& bHandled) = 0;

    /** Notify the application of input focus changes (WM_IME_SETCONTEXT)
    * @param [in] nativeMsg The original message content received from the system
    * @param [out] bHandled Whether the message has been handled; returns true if the message has been handled successfully and does not need to be passed to the window procedure; returns false to continue passing the message to the window procedure
    * @return Returns the result of message handling; if the application handles this message, it should return zero
    */
    virtual LRESULT OnNativeImeSetContextMsg(const NativeMsg& nativeMsg, bool& bHandled) = 0;

    /** The IME starts generating the composition string (WM_IME_STARTCOMPOSITION)
    * @param [in] nativeMsg The original message content received from the system
    * @param [out] bHandled Whether the message has been handled; returns true if the message has been handled successfully and does not need to be passed to the window procedure; returns false to continue passing the message to the window procedure
    * @return Returns the result of message handling; if the application handles this message, it should return zero
    */
    virtual LRESULT OnNativeImeStartCompositionMsg(const NativeMsg& nativeMsg, bool& bHandled) = 0;

    /** Change the key composition state (WM_IME_COMPOSITION)
    * @param [in] nativeMsg The original message content received from the system
    * @param [out] bHandled Whether the message has been handled; returns true if the message has been handled successfully and does not need to be passed to the window procedure; returns false to continue passing the message to the window procedure
    * @return Returns the result of message handling; if the application handles this message, it should return zero
    */
    virtual LRESULT OnNativeImeCompositionMsg(const NativeMsg& nativeMsg, bool& bHandled) = 0;

    /** The IME ends the composition (WM_IME_ENDCOMPOSITION)
    * @param [in] nativeMsg The original message content received from the system
    * @param [out] bHandled Whether the message has been handled; returns true if the message has been handled successfully and does not need to be passed to the window procedure; returns false to continue passing the message to the window procedure
    * @return Returns the result of message handling; if the application handles this message, it should return zero
    */
    virtual LRESULT OnNativeImeEndCompositionMsg(const NativeMsg& nativeMsg, bool& bHandled) = 0;

    /** Set the cursor (WM_SETCURSOR)
    * @param [in] nativeMsg The original message content received from the system
    * @param [out] bHandled Whether the message has been handled; returns true if the message has been handled successfully and does not need to be passed to the window procedure; returns false to continue passing the message to the window procedure
    * @return If the application handles this message, it should return TRUE to stop further processing or FALSE to continue
    */
    virtual LRESULT OnNativeSetCursorMsg(const NativeMsg& nativeMsg, bool& bHandled) = 0;

    /** Notify the window that the user wants to display a context menu (WM_CONTEXTMENU); the user may have clicked the right mouse button (right-click in the window), pressed Shift+F10, or pressed the application key (context menu key), available on some keyboards.
    * @param [in] pt The mouse position, in client coordinates; (-1,-1) means the user pressed SHIFT+F10
    * @param [in] nativeMsg The original message content received from the system
    * @param [out] bHandled Whether the message has been handled; returns true if the message has been handled successfully and does not need to be passed to the window procedure; returns false to continue passing the message to the window procedure
    * @return Returns the result of message handling; if the application handles this message, it should return zero
    */
    virtual LRESULT OnNativeContextMenuMsg(const UiPoint& pt, const NativeMsg& nativeMsg, bool& bHandled) = 0;

    /** Key pressed (WM_KEYDOWN or WM_SYSKEYDOWN)
    * @param [in] vkCode Virtual key code
    * @param [in] modifierKey Key modifier flags, valid values: ModifierKey::kFirstPress, ModifierKey::kAlt
    * @param [in] nativeMsg The original message content received from the system
    * @param [out] bHandled Whether the message has been handled; returns true if the message has been handled successfully and does not need to be passed to the window procedure; returns false to continue passing the message to the window procedure
    * @return Returns the result of message handling; if the application handles this message, it should return zero
    */
    virtual LRESULT OnNativeKeyDownMsg(VirtualKeyCode vkCode, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) = 0;

    /** Key released (WM_KEYUP or WM_SYSKEYUP)
    * @param [in] vkCode Virtual key code
    * @param [in] modifierKey Key modifier flags, valid values: ModifierKey::kAlt
    * @param [in] nativeMsg The original message content received from the system
    * @param [out] bHandled Whether the message has been handled; returns true if the message has been handled successfully and does not need to be passed to the window procedure; returns false to continue passing the message to the window procedure
    * @return Returns the result of message handling; if the application handles this message, it should return zero
    */
    virtual LRESULT OnNativeKeyUpMsg(VirtualKeyCode vkCode, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) = 0;

    /** Key pressed (WM_CHAR)
    * @param [in] vkCode Virtual key code
    * @param [in] modifierKey Key modifier flags, valid values: ModifierKey::kFirstPress, ModifierKey::kAlt
    * @param [in] nativeMsg The original message content received from the system
    * @param [out] bHandled Whether the message has been handled; returns true if the message has been handled successfully and does not need to be passed to the window procedure; returns false to continue passing the message to the window procedure
    * @return Returns the result of message handling; if the application handles this message, it should return zero
    */
    virtual LRESULT OnNativeCharMsg(VirtualKeyCode vkCode, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) = 0;

    /** Hotkey message (WM_HOTKEY)
    * @param [in] hotkeyId The ID of the hotkey
    * @param [in] vkCode Virtual key code
    * @param [in] modifierKey Key modifier flags, valid values: ModifierKey::kAlt, ModifierKey::kControl, ModifierKey::kShift, ModifierKey::kWin
    * @param [in] nativeMsg The original message content received from the system
    * @param [out] bHandled Whether the message has been handled; returns true if the message has been handled successfully and does not need to be passed to the window procedure; returns false to continue passing the message to the window procedure
    * @return Returns the result of message handling; if the application handles this message, it should return zero
    */
    virtual LRESULT OnNativeHotKeyMsg(int32_t hotkeyId, VirtualKeyCode vkCode, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) = 0;

    /** Rotate the mouse wheel (WM_MOUSEWHEEL)
    * @param [in] wheelDelta The distance of wheel rotation, expressed in multiples or divisions of WHEEL_DELTA (120). A positive value means the wheel rotated forward (away from the user); a negative value means the wheel rotated backward (toward the user)
    * @param [in] pt The mouse position, in client coordinates
    * @param [in] modifierKey Key modifier flags, valid values: ModifierKey::kControl, ModifierKey::kShift
    * @param [in] nativeMsg The original message content received from the system
    * @param [out] bHandled Whether the message has been handled; returns true if the message has been handled successfully and does not need to be passed to the window procedure; returns false to continue passing the message to the window procedure
    * @return Returns the result of message handling; if the application handles this message, it should return zero
    */
    virtual LRESULT OnNativeMouseWheelMsg(int32_t wheelDelta, const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) = 0;

    /** Mouse move message (WM_MOUSEMOVE)
    * @param [in] pt The mouse position, in client coordinates
    * @param [in] modifierKey Key modifier flags, valid values: ModifierKey::kControl, ModifierKey::kShift
    * @param [in] bFromNC true means this is an NC message (WM_NCMOUSEMOVE), false means it is a WM_MOUSEMOVE message
    * @param [in] nativeMsg The original message content received from the system
    * @param [out] bHandled Whether the message has been handled; returns true if the message has been handled successfully and does not need to be passed to the window procedure; returns false to continue passing the message to the window procedure
    * @return Returns the result of message handling; if the application handles this message, it should return zero
    */
    virtual LRESULT OnNativeMouseMoveMsg(const UiPoint& pt, uint32_t modifierKey, bool bFromNC, const NativeMsg& nativeMsg, bool& bHandled) = 0;

    /** Mouse hover message (WM_MOUSEHOVER)
    * @param [in] pt The mouse position, in client coordinates
    * @param [in] modifierKey Key modifier flags, valid values: ModifierKey::kControl, ModifierKey::kShift
    * @param [in] nativeMsg The original message content received from the system
    * @param [out] bHandled Whether the message has been handled; returns true if the message has been handled successfully and does not need to be passed to the window procedure; returns false to continue passing the message to the window procedure
    * @return Returns the result of message handling; if the application handles this message, it should return zero
    */
    virtual LRESULT OnNativeMouseHoverMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) = 0;

    /** Mouse leave message (WM_MOUSELEAVE)
    * @param [in] nativeMsg The original message content received from the system
    * @param [out] bHandled Whether the message has been handled; returns true if the message has been handled successfully and does not need to be passed to the window procedure; returns false to continue passing the message to the window procedure
    * @return Returns the result of message handling; if the application handles this message, it should return zero
    */
    virtual LRESULT OnNativeMouseLeaveMsg(const NativeMsg& nativeMsg, bool& bHandled) = 0;

    /** Mouse left button down message (WM_LBUTTONDOWN)
    * @param [in] pt The mouse position, in client coordinates
    * @param [in] modifierKey Key modifier flags, valid values: ModifierKey::kControl, ModifierKey::kShift
    * @param [in] nativeMsg The original message content received from the system
    * @param [out] bHandled Whether the message has been handled; returns true if the message has been handled successfully and does not need to be passed to the window procedure; returns false to continue passing the message to the window procedure
    * @return Returns the result of message handling; if the application handles this message, it should return zero
    */
    virtual LRESULT OnNativeMouseLButtonDownMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) = 0;

    /** Mouse left button up message (WM_LBUTTONUP)
    * @param [in] pt The mouse position, in client coordinates
    * @param [in] modifierKey Key modifier flags, valid values: ModifierKey::kControl, ModifierKey::kShift
    * @param [in] nativeMsg The original message content received from the system
    * @param [out] bHandled Whether the message has been handled; returns true if the message has been handled successfully and does not need to be passed to the window procedure; returns false to continue passing the message to the window procedure
    * @return Returns the result of message handling; if the application handles this message, it should return zero
    */
    virtual LRESULT OnNativeMouseLButtonUpMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) = 0;

    /** Mouse left button double-click message (WM_LBUTTONDBLCLK)
    * @param [in] pt The mouse position, in client coordinates
    * @param [in] modifierKey Key modifier flags, valid values: ModifierKey::kControl, ModifierKey::kShift
    * @param [in] nativeMsg The original message content received from the system
    * @param [out] bHandled Whether the message has been handled; returns true if the message has been handled successfully and does not need to be passed to the window procedure; returns false to continue passing the message to the window procedure
    * @return Returns the result of message handling; if the application handles this message, it should return zero
    */
    virtual LRESULT OnNativeMouseLButtonDbClickMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) = 0;

    /** Mouse right button down message (WM_RBUTTONDOWN)
    * @param [in] pt The mouse position, in client coordinates
    * @param [in] modifierKey Key modifier flags, valid values: ModifierKey::kControl, ModifierKey::kShift
    * @param [in] nativeMsg The original message content received from the system
    * @param [out] bHandled Whether the message has been handled; returns true if the message has been handled successfully and does not need to be passed to the window procedure; returns false to continue passing the message to the window procedure
    * @return Returns the result of message handling; if the application handles this message, it should return zero
    */
    virtual LRESULT OnNativeMouseRButtonDownMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) = 0;

    /** Mouse right button up message (WM_RBUTTONUP)
    * @param [in] pt The mouse position, in client coordinates
    * @param [in] modifierKey Key modifier flags, valid values: ModifierKey::kControl, ModifierKey::kShift
    * @param [in] nativeMsg The original message content received from the system
    * @param [out] bHandled Whether the message has been handled; returns true if the message has been handled successfully and does not need to be passed to the window procedure; returns false to continue passing the message to the window procedure
    * @return Returns the result of message handling; if the application handles this message, it should return zero
    */
    virtual LRESULT OnNativeMouseRButtonUpMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) = 0;

    /** Mouse right button double-click message (WM_RBUTTONDBLCLK)
    * @param [in] pt The mouse position, in client coordinates
    * @param [in] modifierKey Key modifier flags, valid values: ModifierKey::kControl, ModifierKey::kShift
    * @param [in] nativeMsg The original message content received from the system
    * @param [out] bHandled Whether the message has been handled; returns true if the message has been handled successfully and does not need to be passed to the window procedure; returns false to continue passing the message to the window procedure
    * @return Returns the result of message handling; if the application handles this message, it should return zero
    */
    virtual LRESULT OnNativeMouseRButtonDbClickMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) = 0;

    /** Mouse middle button down message (WM_MBUTTONDOWN)
    * @param [in] pt The mouse position, in client coordinates
    * @param [in] modifierKey Key modifier flags, valid values: ModifierKey::kControl, ModifierKey::kShift
    * @param [in] nativeMsg The original message content received from the system
    * @param [out] bHandled Whether the message has been handled; returns true if the message has been handled successfully and does not need to be passed to the window procedure; returns false to continue passing the message to the window procedure
    * @return Returns the result of message handling; if the application handles this message, it should return zero
    */
    virtual LRESULT OnNativeMouseMButtonDownMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) = 0;

    /** Mouse middle button up message (WM_MBUTTONUP)
    * @param [in] pt The mouse position, in client coordinates
    * @param [in] modifierKey Key modifier flags, valid values: ModifierKey::kControl, ModifierKey::kShift
    * @param [in] nativeMsg The original message content received from the system
    * @param [out] bHandled Whether the message has been handled; returns true if the message has been handled successfully and does not need to be passed to the window procedure; returns false to continue passing the message to the window procedure
    * @return Returns the result of message handling; if the application handles this message, it should return zero
    */
    virtual LRESULT OnNativeMouseMButtonUpMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) = 0;

    /** Mouse middle button double-click message (WM_MBUTTONDBLCLK)
    * @param [in] pt The mouse position, in client coordinates
    * @param [in] modifierKey Key modifier flags, valid values: ModifierKey::kControl, ModifierKey::kShift
    * @param [in] nativeMsg The original message content received from the system
    * @param [out] bHandled Whether the message has been handled; returns true if the message has been handled successfully and does not need to be passed to the window procedure; returns false to continue passing the message to the window procedure
    * @return Returns the result of message handling; if the application handles this message, it should return zero
    */
    virtual LRESULT OnNativeMouseMButtonDbClickMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) = 0;

    /** The window lost mouse capture (WM_CAPTURECHANGED)
    * @param [in] nativeMsg The original message content received from the system
    * @param [out] bHandled Whether the message has been handled; returns true if the message has been handled successfully and does not need to be passed to the window procedure; returns false to continue passing the message to the window procedure
    * @return Returns the result of message handling; if the application handles this message, it should return zero
    */
    virtual LRESULT OnNativeCaptureChangedMsg(const NativeMsg& nativeMsg, bool& bHandled) = 0;

    /** Window close message (WM_CLOSE)
    * @param [in] wParam The wParam parameter of the message
    * @param [in] nativeMsg The original message content received from the system
    * @param [out] bHandled Whether the message has been handled; returns true if the message has been handled successfully and does not need to be passed to the window procedure; returns false to continue passing the message to the window procedure
    * @return Returns the result of message handling; if the application handles this message, it should return zero
    */
    virtual LRESULT OnNativeWindowCloseMsg(uint32_t wParam, const NativeMsg& nativeMsg, bool& bHandled) = 0;

    /** Snap operation of the window position
    * @param [in] bLeftSnap The window snaps to the left edge
    * @param [in] bRightSnap The window snaps to the right edge
    * @param [in] bTopSnap The window snaps to the top edge
    * @param [in] bBottomSnap The window snaps to the bottom edge
    */
    virtual void OnNativeWindowPosSnapped(bool bLeftSnap, bool bRightSnap, bool bTopSnap, bool bBottomSnap) = 0;

    /** Operation interface related to window drag-and-drop (the interface parameters depend on the implementation)
    * @param [in] dropType The source type of the drag-and-drop operation
    * @param [in,out] pDropData The concrete type is determined by dropType:
    *                 When dropType is kControlDropTypeWindows (representing the Windows platform SDK implementation), the type of pDropData is ControlDropData_Windows*
    *                 When dropType is kControlDropTypeSDL (representing the SDL implementation), the type of pDropData is ControlDropData_SDL*
    *                 pDropData->m_bHandled is the message handling flag; if it returns true, the event has been handled and is no longer forwarded to other UI controls in the interface, equivalent to intercepting the message
    *                 pDropData->m_hResult is the return value after message handling, finally returned to the operating system; on the Windows platform, success returns S_OK
    */
    virtual void OnNativeDropEnterMsg(ControlDropType dropType, void* pDropData) = 0;
    virtual void OnNativeDropOverMsg(ControlDropType dropType, void* pDropData) = 0;
    virtual void OnNativeDropMsg(ControlDropType dropType, void* pDropData) = 0;
    virtual void OnNativeDropLeaveMsg() = 0;

    /** @}*/

};

} // namespace ui

#endif // UI_CORE_INATIVE_WINDOW_H_
