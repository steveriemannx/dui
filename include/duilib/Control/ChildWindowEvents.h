#ifndef UI_CONTROL_CHILD_WINDOW_EVENTS_H_
#define UI_CONTROL_CHILD_WINDOW_EVENTS_H_

#include "duilib/Core/Callback.h"
#include "duilib/Core/UiTypes.h"
#include "duilib/Core/WindowMessage.h"
#include "duilib/Core/Keycode.h"

namespace ui
{
/** Event interface of the child window
 */
class DUILIB_API ChildWindowEvents: public virtual SupportWeakCallback
{
public:
    virtual ~ChildWindowEvents() = default;

public:
    /** Event of the window being created successfully (WM_CREATE/WM_INITDIALOG)
     * @param [in] nativeMsg The original message content received from the system
     * @param [out] bHandled Whether the message has been handled; returning true means the message has been handled successfully and no longer needs to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for processing
     * @return Returns the processing result of the message; if the application handles this message, it should return zero
     */
    virtual void OnWindowCreateMsg(const ui::NativeMsg& nativeMsg, bool& bHandled) { (void)nativeMsg; (void)bHandled; }

    /** Window close message (WM_CLOSE)
     * @param [in] wParam The wParam parameter of the message
     * @param [in] nativeMsg The original message content received from the system
     * @param [out] bHandled Whether the message has been handled; returning true means the message has been handled successfully and no longer needs to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for processing
     * @return Returns the processing result of the message; if the application handles this message, it should return zero
     */
    virtual LRESULT OnWindowCloseMsg(uint32_t wParam, const ui::NativeMsg& nativeMsg, bool& bHandled) { (void)wParam; (void)nativeMsg; (void)bHandled; return 0; }

    /** The window position or size changed (WM_WINDOWPOSCHANGED)
    * @param [in] nativeMsg The original message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true means the message has been handled successfully and no longer needs to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for processing
    * @return Returns the processing result of the message; if the application handles this message, it should return zero
    */
    virtual LRESULT OnWindowPosChangedMsg(const ui::NativeMsg& nativeMsg, bool& bHandled) { (void)nativeMsg; (void)bHandled; return 0; }

    /** The window size changed (WM_SIZE)
     * @param [in] sizeType The type that triggered the window size change
     * @param [in] newWindowSize The new window size (width and height)
     * @param [in] nativeMsg The original message content received from the system
     * @param [out] bHandled Whether the message has been handled; returning true means the message has been handled successfully and no longer needs to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for processing
     * @return Returns the processing result of the message; if the application handles this message, it should return zero
     */
    virtual LRESULT OnSizeMsg(ui::WindowSizeType sizeType, const ui::UiSize& newWindowSize, const ui::NativeMsg& nativeMsg, bool& bHandled) { (void)sizeType; (void)newWindowSize; (void)nativeMsg; (void)bHandled; return 0; }

    /** Window moved (WM_MOVE)
     * @param [in] ptTopLeft The x and y coordinates of the upper-left corner of the window client area (coordinates are screen coordinates)
     * @param [in] nativeMsg The original message content received from the system
     * @param [out] bHandled Whether the message has been handled; returning true means the message has been handled successfully and no longer needs to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for processing
     * @return Returns the processing result of the message; if the application handles this message, it should return zero
     */
    virtual LRESULT OnMoveMsg(const ui::UiPoint& ptTopLeft, const ui::NativeMsg& nativeMsg, bool& bHandled) { (void)ptTopLeft; (void)nativeMsg; (void)bHandled; return 0; }

    /** Window shown or hidden (WM_SHOWWINDOW)
     * @param [in] bShow true means the window is being shown, false means the window is being hidden
     * @param [in] nativeMsg The original message content received from the system
     * @param [out] bHandled Whether the message has been handled; returning true means the message has been handled successfully and no longer needs to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for processing
     * @return Returns the processing result of the message; if the application handles this message, it should return zero
     */
    virtual LRESULT OnShowWindowMsg(bool bShow, const ui::NativeMsg& nativeMsg, bool& bHandled) { (void)bShow; (void)nativeMsg; (void)bHandled; return 0; }

    /** Window drawing (SDL_EVENT_WINDOW_EXPOSED/WM_PAINT)
     * @param [in] rcPaint The rectangular area that needs to be updated for this drawing
     * @param [in] nativeMsg The original message content received from the system
     *             SDL implementation: nativeMsg.uMsg is SDL_EVENT_WINDOW_EXPOSED, and nativeMsg.wParam is a SDL_Window* pointer
     *             Windows implementation: nativeMsg.uMsg is WM_PAINT, and nativeMsg.wParam is the HWND handle of the window
     * @param [out] bHandled Whether the message has been handled; returning true means the message has been handled successfully and no longer needs to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for processing
     * @return Returns the processing result of the message; if the application handles this message, it should return zero
     */
    virtual LRESULT OnPaintMsg(const ui::UiRect& rcPaint, const ui::NativeMsg& nativeMsg, bool& bHandled) { (void)rcPaint; (void)nativeMsg; (void)bHandled; return 0; }

    /** Window gained focus (WM_SETFOCUS)
     * @param [in] pLostFocusWindow The window that lost keyboard focus (can be nullptr)
     * @param [in] nativeMsg The original message content received from the system
     * @param [out] bHandled Whether the message has been handled; returning true means the message has been handled successfully and no longer needs to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for processing
     * @return Returns the processing result of the message; if the application handles this message, it should return zero
     */
    virtual LRESULT OnSetFocusMsg(ui::WindowBase* pLostFocusWindow, const ui::NativeMsg& nativeMsg, bool& bHandled) { (void)pLostFocusWindow; (void)nativeMsg; (void)bHandled; return 0; }

    /** Window lost focus (WM_KILLFOCUS)
     * @param [in] pSetFocusWindow The window receiving keyboard focus (can be nullptr)
     * @param [in] nativeMsg The original message content received from the system
     * @param [out] bHandled Whether the message has been handled; returning true means the message has been handled successfully and no longer needs to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for processing
     * @return Returns the processing result of the message; if the application handles this message, it should return zero
     */
    virtual LRESULT OnKillFocusMsg(ui::WindowBase* pSetFocusWindow, const ui::NativeMsg& nativeMsg, bool& bHandled) { (void)pSetFocusWindow; (void)nativeMsg; (void)bHandled; return 0; }

    /** Set the cursor (WM_SETCURSOR)
     * @param [in] nativeMsg The original message content received from the system
     * @param [out] bHandled Whether the message has been handled; returning true means the message has been handled successfully and no longer needs to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for processing
     * @return If the application handles this message, it should return TRUE to stop further processing or FALSE to continue
     */
    virtual LRESULT OnSetCursorMsg(const ui::NativeMsg& nativeMsg, bool& bHandled) { (void)nativeMsg; (void)bHandled; return 0; }

    /** Key down (WM_KEYDOWN or WM_SYSKEYDOWN)
     * @param [in] vkCode The virtual key code
     * @param [in] modifierKey The key modifier flags; valid values: ModifierKey::kFirstPress, ModifierKey::kAlt
     * @param [in] nativeMsg The original message content received from the system
     * @param [out] bHandled Whether the message has been handled; returning true means the message has been handled successfully and no longer needs to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for processing
     * @return Returns the processing result of the message; if the application handles this message, it should return zero
     */
    virtual LRESULT OnKeyDownMsg(ui::VirtualKeyCode vkCode, uint32_t modifierKey, const ui::NativeMsg& nativeMsg, bool& bHandled) { (void)vkCode; (void)modifierKey; (void)nativeMsg; (void)bHandled; return 0; }

    /** Key up (WM_KEYUP or WM_SYSKEYUP)
     * @param [in] vkCode The virtual key code
     * @param [in] modifierKey The key modifier flags; valid values: ModifierKey::kAlt
     * @param [in] nativeMsg The original message content received from the system
     * @param [out] bHandled Whether the message has been handled; returning true means the message has been handled successfully and no longer needs to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for processing
     * @return Returns the processing result of the message; if the application handles this message, it should return zero
     */
    virtual LRESULT OnKeyUpMsg(ui::VirtualKeyCode vkCode, uint32_t modifierKey, const ui::NativeMsg& nativeMsg, bool& bHandled) { (void)vkCode; (void)modifierKey; (void)nativeMsg; (void)bHandled; return 0; }

    /** Mouse wheel rotated (WM_MOUSEWHEEL)
     * @param [in] wheelDelta The distance the wheel is rotated, expressed in multiples or divisions of WHEEL_DELTA (120). A positive value indicates the wheel rotated forward (away from the user); a negative value indicates the wheel rotated backward (toward the user)
     * @param [in] pt The mouse position, in client area coordinates
     * @param [in] modifierKey The key modifier flags; valid values: ModifierKey::kControl, ModifierKey::kShift
     * @param [in] nativeMsg The original message content received from the system
     * @param [out] bHandled Whether the message has been handled; returning true means the message has been handled successfully and no longer needs to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for processing
     * @return Returns the processing result of the message; if the application handles this message, it should return zero
     */
    virtual LRESULT OnMouseWheelMsg(int32_t wheelDelta, const ui::UiPoint& pt, uint32_t modifierKey, const ui::NativeMsg& nativeMsg, bool& bHandled) { (void)wheelDelta; (void)pt; (void)modifierKey; (void)nativeMsg; (void)bHandled; return 0; }

    /** Mouse move message (WM_MOUSEMOVE)
     * @param [in] pt The mouse position, in client area coordinates
     * @param [in] modifierKey The key modifier flags; valid values: ModifierKey::kControl, ModifierKey::kShift
     * @param [in] nativeMsg The original message content received from the system
     * @param [out] bHandled Whether the message has been handled; returning true means the message has been handled successfully and no longer needs to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for processing
     * @return Returns the processing result of the message; if the application handles this message, it should return zero
     */
    virtual LRESULT OnMouseMoveMsg(const ui::UiPoint& pt, uint32_t modifierKey, const ui::NativeMsg& nativeMsg, bool& bHandled) { (void)pt; (void)modifierKey; (void)nativeMsg; (void)bHandled; return 0; }

    /** Mouse hover message (WM_MOUSEHOVER)
     * @param [in] pt The mouse position, in client area coordinates
     * @param [in] modifierKey The key modifier flags; valid values: ModifierKey::kControl, ModifierKey::kShift
     * @param [in] nativeMsg The original message content received from the system
     * @param [out] bHandled Whether the message has been handled; returning true means the message has been handled successfully and no longer needs to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for processing
     * @return Returns the processing result of the message; if the application handles this message, it should return zero
     */
    virtual LRESULT OnMouseHoverMsg(const ui::UiPoint& pt, uint32_t modifierKey, const ui::NativeMsg& nativeMsg, bool& bHandled) { (void)pt; (void)modifierKey; (void)nativeMsg; (void)bHandled; return 0; }

    /** Mouse leave message (WM_MOUSELEAVE)
     * @param [in] nativeMsg The original message content received from the system
     * @param [out] bHandled Whether the message has been handled; returning true means the message has been handled successfully and no longer needs to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for processing
     * @return Returns the processing result of the message; if the application handles this message, it should return zero
     */
    virtual LRESULT OnMouseLeaveMsg(const ui::NativeMsg& nativeMsg, bool& bHandled) { (void)nativeMsg; (void)bHandled; return 0; }

    /** Mouse left button down message (WM_LBUTTONDOWN)
     * @param [in] pt The mouse position, in client area coordinates
     * @param [in] modifierKey The key modifier flags; valid values: ModifierKey::kControl, ModifierKey::kShift
     * @param [in] nativeMsg The original message content received from the system
     * @param [out] bHandled Whether the message has been handled; returning true means the message has been handled successfully and no longer needs to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for processing
     * @return Returns the processing result of the message; if the application handles this message, it should return zero
     */
    virtual LRESULT OnMouseLButtonDownMsg(const ui::UiPoint& pt, uint32_t modifierKey, const ui::NativeMsg& nativeMsg, bool& bHandled) { (void)pt; (void)modifierKey; (void)nativeMsg; (void)bHandled; return 0; }

    /** Mouse left button up message (WM_LBUTTONUP)
     * @param [in] pt The mouse position, in client area coordinates
     * @param [in] modifierKey The key modifier flags; valid values: ModifierKey::kControl, ModifierKey::kShift
     * @param [in] nativeMsg The original message content received from the system
     * @param [out] bHandled Whether the message has been handled; returning true means the message has been handled successfully and no longer needs to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for processing
     * @return Returns the processing result of the message; if the application handles this message, it should return zero
     */
    virtual LRESULT OnMouseLButtonUpMsg(const ui::UiPoint& pt, uint32_t modifierKey, const ui::NativeMsg& nativeMsg, bool& bHandled) { (void)pt; (void)modifierKey; (void)nativeMsg; (void)bHandled; return 0; }

    /** Mouse left button double-click message (WM_LBUTTONDBLCLK)
     * @param [in] pt The mouse position, in client area coordinates
     * @param [in] modifierKey The key modifier flags; valid values: ModifierKey::kControl, ModifierKey::kShift
     * @param [in] nativeMsg The original message content received from the system
     * @param [out] bHandled Whether the message has been handled; returning true means the message has been handled successfully and no longer needs to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for processing
     * @return Returns the processing result of the message; if the application handles this message, it should return zero
     */
    virtual LRESULT OnMouseLButtonDbClickMsg(const ui::UiPoint& pt, uint32_t modifierKey, const ui::NativeMsg& nativeMsg, bool& bHandled) { (void)pt; (void)modifierKey; (void)nativeMsg; (void)bHandled; return 0; }

    /** Mouse right button down message (WM_RBUTTONDOWN)
     * @param [in] pt The mouse position, in client area coordinates
     * @param [in] modifierKey The key modifier flags; valid values: ModifierKey::kControl, ModifierKey::kShift
     * @param [in] nativeMsg The original message content received from the system
     * @param [out] bHandled Whether the message has been handled; returning true means the message has been handled successfully and no longer needs to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for processing
     * @return Returns the processing result of the message; if the application handles this message, it should return zero
     */
    virtual LRESULT OnMouseRButtonDownMsg(const ui::UiPoint& pt, uint32_t modifierKey, const ui::NativeMsg& nativeMsg, bool& bHandled) { (void)pt; (void)modifierKey; (void)nativeMsg; (void)bHandled; return 0; }

    /** Mouse right button up message (WM_RBUTTONUP)
     * @param [in] pt The mouse position, in client area coordinates
     * @param [in] modifierKey The key modifier flags; valid values: ModifierKey::kControl, ModifierKey::kShift
     * @param [in] nativeMsg The original message content received from the system
     * @param [out] bHandled Whether the message has been handled; returning true means the message has been handled successfully and no longer needs to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for processing
     * @return Returns the processing result of the message; if the application handles this message, it should return zero
     */
    virtual LRESULT OnMouseRButtonUpMsg(const ui::UiPoint& pt, uint32_t modifierKey, const ui::NativeMsg& nativeMsg, bool& bHandled) { (void)pt; (void)modifierKey; (void)nativeMsg; (void)bHandled; return 0; }

    /** Mouse right button double-click message (WM_RBUTTONDBLCLK)
     * @param [in] pt The mouse position, in client area coordinates
     * @param [in] modifierKey The key modifier flags; valid values: ModifierKey::kControl, ModifierKey::kShift
     * @param [in] nativeMsg The original message content received from the system
     * @param [out] bHandled Whether the message has been handled; returning true means the message has been handled successfully and no longer needs to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for processing
     * @return Returns the processing result of the message; if the application handles this message, it should return zero
     */
    virtual LRESULT OnMouseRButtonDbClickMsg(const ui::UiPoint& pt, uint32_t modifierKey, const ui::NativeMsg& nativeMsg, bool& bHandled) { (void)pt; (void)modifierKey; (void)nativeMsg; (void)bHandled; return 0; }

    /** Mouse middle button down message (WM_MBUTTONDOWN)
     * @param [in] pt The mouse position, in client area coordinates
     * @param [in] modifierKey The key modifier flags; valid values: ModifierKey::kControl, ModifierKey::kShift
     * @param [in] nativeMsg The original message content received from the system
     * @param [out] bHandled Whether the message has been handled; returning true means the message has been handled successfully and no longer needs to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for processing
     * @return Returns the processing result of the message; if the application handles this message, it should return zero
     */
    virtual LRESULT OnMouseMButtonDownMsg(const ui::UiPoint& pt, uint32_t modifierKey, const ui::NativeMsg& nativeMsg, bool& bHandled) { (void)pt; (void)modifierKey; (void)nativeMsg; (void)bHandled; return 0; }

    /** Mouse middle button up message (WM_MBUTTONUP)
     * @param [in] pt The mouse position, in client area coordinates
     * @param [in] modifierKey The key modifier flags; valid values: ModifierKey::kControl, ModifierKey::kShift
     * @param [in] nativeMsg The original message content received from the system
     * @param [out] bHandled Whether the message has been handled; returning true means the message has been handled successfully and no longer needs to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for processing
     * @return Returns the processing result of the message; if the application handles this message, it should return zero
     */
    virtual LRESULT OnMouseMButtonUpMsg(const ui::UiPoint& pt, uint32_t modifierKey, const ui::NativeMsg& nativeMsg, bool& bHandled) { (void)pt; (void)modifierKey; (void)nativeMsg; (void)bHandled; return 0; }

    /** Mouse middle button double-click message (WM_MBUTTONDBLCLK)
     * @param [in] pt The mouse position, in client area coordinates
     * @param [in] modifierKey The key modifier flags; valid values: ModifierKey::kControl, ModifierKey::kShift
     * @param [in] nativeMsg The original message content received from the system
     * @param [out] bHandled Whether the message has been handled; returning true means the message has been handled successfully and no longer needs to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for processing
     * @return Returns the processing result of the message; if the application handles this message, it should return zero
     */
    virtual LRESULT OnMouseMButtonDbClickMsg(const ui::UiPoint& pt, uint32_t modifierKey, const ui::NativeMsg& nativeMsg, bool& bHandled) { (void)pt; (void)modifierKey; (void)nativeMsg; (void)bHandled; return 0; }

    /** Window lost mouse capture (WM_CAPTURECHANGED)
     * @param [in] nativeMsg The original message content received from the system
     * @param [out] bHandled Whether the message has been handled; returning true means the message has been handled successfully and no longer needs to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for processing
     * @return Returns the processing result of the message; if the application handles this message, it should return zero
     */
    virtual LRESULT OnCaptureChangedMsg(const ui::NativeMsg& nativeMsg, bool& bHandled) { (void)nativeMsg; (void)bHandled; return 0; }

    /** The DPI scale factor of the window changed; update the control size and layout (for subclasses)
    * @param [in] nOldScaleFactor The old DPI scale percentage
    * @param [in] nNewScaleFactor The new DPI scale percentage, consistent with the value of Dpi().GetDisplayScaleFactor(); this value may be the same as nOldScaleFactor
    */
    virtual void OnWindowDisplayScaleChanged(uint32_t nOldScaleFactor, uint32_t nNewScaleFactor) { (void)nOldScaleFactor; (void)nNewScaleFactor; }
};

}//namespace ui

#endif //UI_CONTROL_CHILD_WINDOW_EVENTS_H_
