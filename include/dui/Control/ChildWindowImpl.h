#ifndef UI_CORE_CHILD_WINDOW_IMPL_H_
#define UI_CORE_CHILD_WINDOW_IMPL_H_

#include "dui/Core/WindowBase.h"
#include "dui/Control/ChildWindowEvents.h"
#include "dui/Core/ControlPtrT.h"

namespace ui
{
/** Child window control; the control itself is an operating system child window. The UI library internally handles the creation and destruction of the child window, but the UI library does not perform the drawing of the child window
 *  The application layer is responsible for the drawing of the child window
 */
class ChildWindowImpl: public WindowBase
{
public:
    ChildWindowImpl(ChildWindowEvents* pChildWindowEvents);
    virtual ~ChildWindowImpl() override;
    ChildWindowImpl(const ChildWindowImpl&) = delete;
    ChildWindowImpl& operator=(const ChildWindowImpl&) = delete;

public:
    /** Set the visible property of the child window
    * @param [in] bVisible Whether it is visible
    */
    void SetChildWindowVisible(bool bVisible);

    /** Set the enabled property of the child window
    * @param [in] bEnabled The enabled state
    */
    void SetChildWindowEnabled(bool bEnabled);

    /** Set the message callback interface of the child window
    * @param [in] pChildWindowEvents The message callback interface of the child window
    */
    void SetChildWindowEvents(ChildWindowEvents* pChildWindowEvents);

protected:
    /** Initializing the window data
    */
    virtual void PreInitWindow() override;

    /** Initialize the window data (this function is called after the window is created), for subclasses to do some initialization work
    */
    virtual void OnInitWindow() override;

    /** Finish initializing the window data
    */
    virtual void PostInitWindow() override;

    /** Finish the initialization of the UI layout; the layout information such as the position and size of each control has been initialized
    */
    virtual void OnInitLayout() override;

    /** The window is closing; handle the internal state
    */
    virtual void PreCloseWindow() override;

    /** The window has been closed; handle the internal state
    */
    virtual void PostCloseWindow() override;

    /** Called when the window is destroyed; this is the last message of the window
    */
    virtual void FinalMessage() override;

protected:
    /** Enter the fullscreen state
    */
    virtual void NotifyWindowEnterFullscreen() override;

    /** Exit the fullscreen state
    */
    virtual void NotifyWindowExitFullscreen() override;

protected:
    /** Switch between the system caption bar and the self-drawn caption bar
    */
    virtual void OnUseSystemCaptionBarChanged() override;

    /** Prepare for drawing
    * @return Returns true to continue drawing, returns false to stop drawing
    */
    virtual bool OnPreparePaint() override;

    /** The layered window property of the window changed
    */
    virtual void OnLayeredWindowChanged() override;

    /** The alpha of the window changed
    */
    virtual void OnWindowAlphaChanged() override;

    /** Enter the fullscreen state
    */
    virtual void OnWindowEnterFullscreen() override;

    /** Exit the fullscreen state
    */
    virtual void OnWindowExitFullscreen() override;

    /** The DPI scale factor of the window changed; update the control size and layout (for subclasses)
    * @param [in] nOldScaleFactor The old DPI scale percentage
    * @param [in] nNewScaleFactor The new DPI scale percentage, consistent with the value of Dpi().GetDisplayScaleFactor(); this value may be the same as nOldScaleFactor
    */
    virtual void OnWindowDisplayScaleChanged(uint32_t nOldScaleFactor, uint32_t nNewScaleFactor) override;

    /** Get the configured shadow size of the window
    * @param [out] rcShadow Returns the configured shadow size of the window, not DPI scaled
    */
    virtual void GetShadowCorner(UiPadding& rcShadow) const override;

    /** Get the current shadow size of the window
    * @param [out] rcShadow Returns the current shadow size of the window, already DPI scaled
    */
    virtual void GetCurrentShadowCorner(UiPadding& rcShadow) const override;

    /** Judge whether a point is on a control placed on the caption bar
    */
    virtual bool IsPtInCaptionBarControl(const UiPoint& pt) const override;

    /** Judge whether it has maximize and minimize buttons
    * @param [out] bMinimizeBox Returns true if it has the minimize button
    * @param [out] bMaximizeBox Returns true if it has the maximize button
    */
    virtual bool HasMinMaxBox(bool& bMinimizeBox, bool& bMaximizeBox) const override;

    /** Judge whether a point is on the maximize or restore button
    */
    virtual bool IsPtInMaximizeRestoreButton(const UiPoint& pt) const override;

    /** Get the attributes for creating the window (the attribute values read from the Window tag of the XML file)
    * @param [out] createAttributes Returns the attributes for creating the window read from the Window tag of the XML file
    */
    virtual void GetCreateWindowAttributes(WindowCreateAttributes& createAttributes) override;

    /** The DPI scale factor of the window changed; update the control size and layout
    * @param [in] nOldScaleFactor The old DPI scale percentage
    * @param [in] nNewScaleFactor The new DPI scale percentage, consistent with the value of Dpi().GetDisplayScaleFactor()
    */
    virtual void OnDisplayScaleChanged(uint32_t nOldScaleFactor, uint32_t nNewScaleFactor) override;

    /** Get the rendering engine object
    */
    virtual IRender* GetRender() const override;

    /** Get the control interface at the specified coordinate point
    * @param [in] pt The client area coordinate point
    */
    virtual Control* OnFindControl(const UiPoint& pt) const override;

protected:
    /** @name Window message handling related
     * @{
     */
     /** The dispatch function for window messages
     * @param [in] uMsg The message body
     * @param [in] wParam The additional parameter of the message
     * @param [in] lParam The additional parameter of the message
     * @param [out] bHandled Whether the message has been handled; returning true means the message has been handled successfully and no longer needs to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for processing
     * @return Returns the processing result of the message
     */
    virtual LRESULT OnWindowMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled) override;

    /** Event of the window being created successfully (WM_CREATE/WM_INITDIALOG)
    * @param [in] bDoModal Whether the current window is a modal dialog shown through the DoModal function
    * @param [in] nativeMsg The original message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true means the message has been handled successfully and no longer needs to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for processing
    * @return Returns the processing result of the message; if the application handles this message, it should return zero
    */
    virtual void OnWindowCreateMsg(bool bDoModal, const NativeMsg& nativeMsg, bool& bHandled) override;

    /** Window close message (WM_CLOSE)
    * @param [in] wParam The wParam parameter of the message
    * @param [in] nativeMsg The original message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true means the message has been handled successfully and no longer needs to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for processing
    * @return Returns the processing result of the message; if the application handles this message, it should return zero
    */
    virtual LRESULT OnWindowCloseMsg(uint32_t wParam, const NativeMsg& nativeMsg, bool& bHandled) override;

    /** Window shown or hidden (WM_SHOWWINDOW)
    * @param [in] bShow true means the window is being shown, false means the window is being hidden
    * @param [in] nativeMsg The original message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true means the message has been handled successfully and no longer needs to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for processing
    * @return Returns the processing result of the message; if the application handles this message, it should return zero
    */
    virtual LRESULT OnShowWindowMsg(bool bShow, const NativeMsg& nativeMsg, bool& bHandled) override;

    /** Window drawing (SDL_EVENT_WINDOW_EXPOSED/WM_PAINT)
    * @param [in] rcPaint The rectangular area that needs to be updated for this drawing
    * @param [in] nativeMsg The original message content received from the system
    *             SDL implementation: nativeMsg.uMsg is SDL_EVENT_WINDOW_EXPOSED, and nativeMsg.wParam is a SDL_Window* pointer
    *             Windows implementation: nativeMsg.uMsg is WM_PAINT, and nativeMsg.wParam is the HWND handle of the window
    * @param [out] bHandled Whether the message has been handled; returning true means the message has been handled successfully and no longer needs to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for processing
    * @return Returns the processing result of the message; if the application handles this message, it should return zero
    */
    virtual LRESULT OnPaintMsg(const UiRect& rcPaint, const NativeMsg& nativeMsg, bool& bHandled) override;

    /** The window position or size changed (WM_WINDOWPOSCHANGED)
    * @param [in] nativeMsg The original message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true means the message has been handled successfully and no longer needs to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for processing
    * @return Returns the processing result of the message; if the application handles this message, it should return zero
    */
    virtual LRESULT OnWindowPosChangedMsg(const NativeMsg& nativeMsg, bool& bHandled) override;

    /** The window size changed (WM_SIZE)
    * @param [in] sizeType The type that triggered the window size change
    * @param [in] newWindowSize The new window size (width and height)
    * @param [in] nativeMsg The original message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true means the message has been handled successfully and no longer needs to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for processing
    * @return Returns the processing result of the message; if the application handles this message, it should return zero
    */
    virtual LRESULT OnSizeMsg(WindowSizeType sizeType, const UiSize& newWindowSize, const NativeMsg& nativeMsg, bool& bHandled) override;

    /** Window moved (WM_MOVE)
    * @param [in] ptTopLeft The x and y coordinates of the upper-left corner of the window client area (coordinates are screen coordinates)
    * @param [in] nativeMsg The original message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true means the message has been handled successfully and no longer needs to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for processing
    * @return Returns the processing result of the message; if the application handles this message, it should return zero
    */
    virtual LRESULT OnMoveMsg(const UiPoint& ptTopLeft, const NativeMsg& nativeMsg, bool& bHandled) override;

    /** Window gained focus (WM_SETFOCUS)
    * @param [in] pLostFocusWindow The window that lost keyboard focus (can be nullptr)
    * @param [in] nativeMsg The original message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true means the message has been handled successfully and no longer needs to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for processing
    * @return Returns the processing result of the message; if the application handles this message, it should return zero
    */
    virtual LRESULT OnSetFocusMsg(WindowBase* pLostFocusWindow, const NativeMsg& nativeMsg, bool& bHandled) override;

    /** Window lost focus (WM_KILLFOCUS)
    * @param [in] pSetFocusWindow The window receiving keyboard focus (can be nullptr)
    * @param [in] nativeMsg The original message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true means the message has been handled successfully and no longer needs to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for processing
    * @return Returns the processing result of the message; if the application handles this message, it should return zero
    */
    virtual LRESULT OnKillFocusMsg(WindowBase* pSetFocusWindow, const NativeMsg& nativeMsg, bool& bHandled) override;

    /** Notify the application of an input focus change (WM_IME_SETCONTEXT)
    * @param [in] nativeMsg The original message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true means the message has been handled successfully and no longer needs to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for processing
    * @return Returns the processing result of the message; if the application handles this message, it should return zero
    */
    virtual LRESULT OnImeSetContextMsg(const NativeMsg& nativeMsg, bool& bHandled) override;

    /** The input method begins generating a composition string (WM_IME_STARTCOMPOSITION)
    * @param [in] nativeMsg The original message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true means the message has been handled successfully and no longer needs to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for processing
    * @return Returns the processing result of the message; if the application handles this message, it should return zero
    */
    virtual LRESULT OnImeStartCompositionMsg(const NativeMsg& nativeMsg, bool& bHandled) override;

    /** Change the key composition state (WM_IME_COMPOSITION)
    * @param [in] nativeMsg The original message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true means the message has been handled successfully and no longer needs to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for processing
    * @return Returns the processing result of the message; if the application handles this message, it should return zero
    */
    virtual LRESULT OnImeCompositionMsg(const NativeMsg& nativeMsg, bool& bHandled) override;

    /** The input method ends the composition (WM_IME_ENDCOMPOSITION)
    * @param [in] nativeMsg The original message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true means the message has been handled successfully and no longer needs to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for processing
    * @return Returns the processing result of the message; if the application handles this message, it should return zero
    */
    virtual LRESULT OnImeEndCompositionMsg(const NativeMsg& nativeMsg, bool& bHandled) override;

    /** Set the cursor (WM_SETCURSOR)
    * @param [in] nativeMsg The original message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true means the message has been handled successfully and no longer needs to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for processing
    * @return If the application handles this message, it should return TRUE to stop further processing or FALSE to continue
    */
    virtual LRESULT OnSetCursorMsg(const NativeMsg& nativeMsg, bool& bHandled) override;

    /** Notifies the window that the user wants to display a context menu (WM_CONTEXTMENU); the user may have clicked the right mouse button (right-clicked in the window), pressed Shift+F10, or pressed the application key (context menu key) available on some keyboards.
    * @param [in] pt The mouse position, in client area coordinates; if it is (-1,-1), it means the user typed SHIFT+F10
    * @param [in] nativeMsg The original message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true means the message has been handled successfully and no longer needs to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for processing
    * @return Returns the processing result of the message; if the application handles this message, it should return zero
    */
    virtual LRESULT OnContextMenuMsg(const UiPoint& pt, const NativeMsg& nativeMsg, bool& bHandled) override;

    /** Key down (WM_KEYDOWN or WM_SYSKEYDOWN)
    * @param [in] vkCode The virtual key code
    * @param [in] modifierKey The key modifier flags; valid values: ModifierKey::kFirstPress, ModifierKey::kAlt
    * @param [in] nativeMsg The original message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true means the message has been handled successfully and no longer needs to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for processing
    * @return Returns the processing result of the message; if the application handles this message, it should return zero
    */
    virtual LRESULT OnKeyDownMsg(VirtualKeyCode vkCode, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) override;

    /** Key up (WM_KEYUP or WM_SYSKEYUP)
    * @param [in] vkCode The virtual key code
    * @param [in] modifierKey The key modifier flags; valid values: ModifierKey::kAlt
    * @param [in] nativeMsg The original message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true means the message has been handled successfully and no longer needs to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for processing
    * @return Returns the processing result of the message; if the application handles this message, it should return zero
    */
    virtual LRESULT OnKeyUpMsg(VirtualKeyCode vkCode, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) override;

    /** Key pressed (WM_CHAR)
    * @param [in] vkCode The virtual key code
    * @param [in] modifierKey The key modifier flags; valid values: ModifierKey::kFirstPress, ModifierKey::kAlt
    * @param [in] nativeMsg The original message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true means the message has been handled successfully and no longer needs to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for processing
    * @return Returns the processing result of the message; if the application handles this message, it should return zero
    */
    virtual LRESULT OnCharMsg(VirtualKeyCode vkCode, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) override;

    /** Hotkey message (WM_HOTKEY)
    * @param [in] hotkeyId The ID of the hotkey
    * @param [in] vkCode The virtual key code
    * @param [in] modifierKey The key modifier flags; valid values: ModifierKey::kAlt, ModifierKey::kControl, ModifierKey::kShift, ModifierKey::kWin
    * @param [in] nativeMsg The original message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true means the message has been handled successfully and no longer needs to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for processing
    * @return Returns the processing result of the message; if the application handles this message, it should return zero
    */
    virtual LRESULT OnHotKeyMsg(int32_t hotkeyId, VirtualKeyCode vkCode, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) override;

    /** Mouse wheel rotated (WM_MOUSEWHEEL)
    * @param [in] wheelDelta The distance the wheel is rotated, expressed in multiples or divisions of WHEEL_DELTA (120). A positive value indicates the wheel rotated forward (away from the user); a negative value indicates the wheel rotated backward (toward the user)
    * @param [in] pt The mouse position, in client area coordinates
    * @param [in] modifierKey The key modifier flags; valid values: ModifierKey::kControl, ModifierKey::kShift
    * @param [in] nativeMsg The original message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true means the message has been handled successfully and no longer needs to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for processing
    * @return Returns the processing result of the message; if the application handles this message, it should return zero
    */
    virtual LRESULT OnMouseWheelMsg(int32_t wheelDelta, const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) override;

    /** Mouse move message (WM_MOUSEMOVE)
    * @param [in] pt The mouse position, in client area coordinates
    * @param [in] modifierKey The key modifier flags; valid values: ModifierKey::kControl, ModifierKey::kShift
    * @param [in] bFromNC true means this is an NC message (WM_NCMOUSEMOVE), false means it is a WM_MOUSEMOVE message
    * @param [in] nativeMsg The original message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true means the message has been handled successfully and no longer needs to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for processing
    * @return Returns the processing result of the message; if the application handles this message, it should return zero
    */
    virtual LRESULT OnMouseMoveMsg(const UiPoint& pt, uint32_t modifierKey, bool bFromNC, const NativeMsg& nativeMsg, bool& bHandled) override;

    /** Mouse hover message (WM_MOUSEHOVER)
    * @param [in] pt The mouse position, in client area coordinates
    * @param [in] modifierKey The key modifier flags; valid values: ModifierKey::kControl, ModifierKey::kShift
    * @param [in] nativeMsg The original message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true means the message has been handled successfully and no longer needs to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for processing
    * @return Returns the processing result of the message; if the application handles this message, it should return zero
    */
    virtual LRESULT OnMouseHoverMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) override;

    /** Mouse leave message (WM_MOUSELEAVE)
    * @param [in] nativeMsg The original message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true means the message has been handled successfully and no longer needs to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for processing
    * @return Returns the processing result of the message; if the application handles this message, it should return zero
    */
    virtual LRESULT OnMouseLeaveMsg(const NativeMsg& nativeMsg, bool& bHandled) override;

    /** Mouse left button down message (WM_LBUTTONDOWN)
    * @param [in] pt The mouse position, in client area coordinates
    * @param [in] modifierKey The key modifier flags; valid values: ModifierKey::kControl, ModifierKey::kShift
    * @param [in] nativeMsg The original message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true means the message has been handled successfully and no longer needs to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for processing
    * @return Returns the processing result of the message; if the application handles this message, it should return zero
    */
    virtual LRESULT OnMouseLButtonDownMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) override;

    /** Mouse left button up message (WM_LBUTTONUP)
    * @param [in] pt The mouse position, in client area coordinates
    * @param [in] modifierKey The key modifier flags; valid values: ModifierKey::kControl, ModifierKey::kShift
    * @param [in] nativeMsg The original message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true means the message has been handled successfully and no longer needs to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for processing
    * @return Returns the processing result of the message; if the application handles this message, it should return zero
    */
    virtual LRESULT OnMouseLButtonUpMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) override;

    /** Mouse left button double-click message (WM_LBUTTONDBLCLK)
    * @param [in] pt The mouse position, in client area coordinates
    * @param [in] modifierKey The key modifier flags; valid values: ModifierKey::kControl, ModifierKey::kShift
    * @param [in] nativeMsg The original message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true means the message has been handled successfully and no longer needs to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for processing
    * @return Returns the processing result of the message; if the application handles this message, it should return zero
    */
    virtual LRESULT OnMouseLButtonDbClickMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) override;

    /** Mouse right button down message (WM_RBUTTONDOWN)
    * @param [in] pt The mouse position, in client area coordinates
    * @param [in] modifierKey The key modifier flags; valid values: ModifierKey::kControl, ModifierKey::kShift
    * @param [in] nativeMsg The original message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true means the message has been handled successfully and no longer needs to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for processing
    * @return Returns the processing result of the message; if the application handles this message, it should return zero
    */
    virtual LRESULT OnMouseRButtonDownMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) override;

    /** Mouse right button up message (WM_RBUTTONUP)
    * @param [in] pt The mouse position, in client area coordinates
    * @param [in] modifierKey The key modifier flags; valid values: ModifierKey::kControl, ModifierKey::kShift
    * @param [in] nativeMsg The original message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true means the message has been handled successfully and no longer needs to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for processing
    * @return Returns the processing result of the message; if the application handles this message, it should return zero
    */
    virtual LRESULT OnMouseRButtonUpMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) override;

    /** Mouse right button double-click message (WM_RBUTTONDBLCLK)
    * @param [in] pt The mouse position, in client area coordinates
    * @param [in] modifierKey The key modifier flags; valid values: ModifierKey::kControl, ModifierKey::kShift
    * @param [in] nativeMsg The original message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true means the message has been handled successfully and no longer needs to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for processing
    * @return Returns the processing result of the message; if the application handles this message, it should return zero
    */
    virtual LRESULT OnMouseRButtonDbClickMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) override;

    /** Mouse middle button down message (WM_MBUTTONDOWN)
    * @param [in] pt The mouse position, in client area coordinates
    * @param [in] modifierKey The key modifier flags; valid values: ModifierKey::kControl, ModifierKey::kShift
    * @param [in] nativeMsg The original message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true means the message has been handled successfully and no longer needs to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for processing
    * @return Returns the processing result of the message; if the application handles this message, it should return zero
    */
    virtual LRESULT OnMouseMButtonDownMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) override;

    /** Mouse middle button up message (WM_MBUTTONUP)
    * @param [in] pt The mouse position, in client area coordinates
    * @param [in] modifierKey The key modifier flags; valid values: ModifierKey::kControl, ModifierKey::kShift
    * @param [in] nativeMsg The original message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true means the message has been handled successfully and no longer needs to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for processing
    * @return Returns the processing result of the message; if the application handles this message, it should return zero
    */
    virtual LRESULT OnMouseMButtonUpMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) override;

    /** Mouse middle button double-click message (WM_MBUTTONDBLCLK)
    * @param [in] pt The mouse position, in client area coordinates
    * @param [in] modifierKey The key modifier flags; valid values: ModifierKey::kControl, ModifierKey::kShift
    * @param [in] nativeMsg The original message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true means the message has been handled successfully and no longer needs to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for processing
    * @return Returns the processing result of the message; if the application handles this message, it should return zero
    */
    virtual LRESULT OnMouseMButtonDbClickMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) override;

    /** Window lost mouse capture (WM_CAPTURECHANGED)
    * @param [in] nativeMsg The original message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true means the message has been handled successfully and no longer needs to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for processing
    * @return Returns the processing result of the message; if the application handles this message, it should return zero
    */
    virtual LRESULT OnCaptureChangedMsg(const NativeMsg& nativeMsg, bool& bHandled) override;

    /** Snap operation of the window position
    * @param [in] bLeftSnap Snap to the left edge of the window
    * @param [in] bRightSnap Snap to the right edge of the window
    * @param [in] bTopSnap Snap to the top edge of the window
    * @param [in] bBottomSnap Snap to the bottom edge of the window
    */
    virtual void OnWindowPosSnapped(bool bLeftSnap, bool bRightSnap, bool bTopSnap, bool bBottomSnap) override;

    /** Operation interface related to window drag and drop (the interface parameters depend on the implementation)
    * @param [in] dropType The source type of the drag and drop operation
    * @param [in,out] pDropData The specific type depends on dropType:
    *                 When dropType is kControlDropTypeWindows (representing the Windows platform SDK implementation), the type of pDropData is ControlDropData_Windows*
    *                 When dropType is kControlDropTypeSDL (representing the SDL implementation), the type of pDropData is ControlDropData_SDL*
    *                 pDropData->m_bHandled is the message handling flag; if it returns true, the event has been handled and will not be forwarded to other UI controls in the interface, which is equivalent to intercepting this message
    *                 pDropData->m_hResult is the return value after the message is handled, finally returned to the operating system; on the Windows platform, S_OK is returned on success
    */
    virtual void OnDropEnterMsg(ui::ControlDropType dropType, void* pDropData) override;
    virtual void OnDropOverMsg(ui::ControlDropType dropType, void* pDropData) override;
    virtual void OnDropMsg(ui::ControlDropType dropType, void* pDropData) override;
    virtual void OnDropLeaveMsg() override;

    /** Handle the system notification message for screen resolution changes (WM_DISPLAYCHANGE)
    * @param [in] nColorDepth The new image depth of the display, in bits per pixel
    * @param [in] nScreenWidth The horizontal resolution of the screen
    * @param [in] nScreenHeight The vertical resolution of the screen
    */
    virtual void OnDisplayResolutionChangedMsg(int32_t nColorDepth, int32_t nScreenWidth, int32_t nScreenHeight) override;

    /** Handle the system notification message for DPI changes (WM_DPICHANGED)
    * @param [in] fNewDisplayScale The new UI display scale value of the window; 1.0f means no scaling
    * @param [in] fNewPixelDensity The new pixel density value of the window (used only in the SDL implementation)
    */
    virtual void OnDisplayScaleChangedMsg(float fNewDisplayScale, float fNewPixelDensity) override;

    /** @}*/

private:
    //The external message callback interface of the child window
    ControlPtrT<ChildWindowEvents> m_pChildWindowEvents;
};

}//namespace ui

#endif //UI_CORE_CHILD_WINDOW_IMPL_H_
