#ifndef UI_CONTROL_FLYOUT_H_
#define UI_CONTROL_FLYOUT_H_

#include "dui/Utils/WinImplBase.h"
#include "dui/Core/ControlPtrT.h"
#include <functional>
#include <vector>

namespace ui
{

/** Generic flyout (popup) window
*   Features:
*   1. Pops up around an anchor control in 8 directions; when the preferred side has not enough
*      room it flips to the opposite side automatically and is clamped to the monitor work area
*   2. Does not take the focus by default (WS_EX_NOACTIVATE), so the parent window stays active
*      and usable while the flyout is shown
*   3. Closes automatically when the user clicks outside the flyout or presses Esc (this behavior
*      can be disabled with SetAutoDismiss)
*   4. Can host arbitrary Box content (described by XML), paints itself with the theme colors and
*      supports a window shadow
*   5. Modeless window: the framework deletes the object automatically once the window has been
*      closed, so the caller does not need to release it manually
*
*   Usage:
*   @code
*       ui::Flyout* pFlyout = new ui::Flyout(this);
*       pFlyout->SetSkinFolder(GetResourcePath().ToString());
*       pFlyout->ShowAt(pAnchorButton, "my_flyout.xml", ui::Flyout::Placement::Bottom);
*       ui::Button* pBtn = dynamic_cast<ui::Button*>(pFlyout->FindControl("action"));
*   @endcode
*/
class DUI_API Flyout : public WindowImplBase
{
    typedef WindowImplBase BaseClass;

public:
    /** Popup direction of the flyout relative to the anchor control
    */
    enum class Placement : uint32_t
    {
        Bottom      = 0,    //Below the anchor, aligned to the left edge
        BottomEnd   = 1,    //Below the anchor, aligned to the right edge
        Top         = 2,    //Above the anchor, aligned to the left edge
        TopEnd      = 3,    //Above the anchor, aligned to the right edge
        Right       = 4,    //Right of the anchor, aligned to the top edge
        RightEnd    = 5,    //Right of the anchor, aligned to the bottom edge
        Left        = 6,    //Left of the anchor, aligned to the top edge
        LeftEnd     = 7     //Left of the anchor, aligned to the bottom edge
    };

    /** Reason why the flyout was closed
    */
    enum class CloseReason : uint32_t
    {
        kManual         = 0,    //Closed explicitly by the caller through Dismiss
        kClickOutside   = 1,    //Click outside the flyout/anchor, or the flyout lost the focus
        kEscape         = 2,    //The Esc key was pressed
        kAnchorLost     = 3     //The anchor control or its parent window became invalid (destroyed/hidden/minimized/DPI change)
    };

    /** Prototype of the flyout open/close callback
    * @param [in] reason Close reason (the opened callback always receives CloseReason::kManual)
    */
    typedef std::function<void(CloseReason reason)> FlyoutEvent;

public:
    /** Constructor
    * @param [in] pParentWindow Parent window (the anchor control must belong to this window); the flyout is an
    *             owned window of it and is destroyed together with it
    */
    explicit Flyout(Window* pParentWindow);
    virtual ~Flyout() override;

    /** Sets the folder from which the content XML resource is loaded (a path relative to the resource root)
    *   When it is not set, the resource path of the parent window is used
    */
    void SetSkinFolder(const std::string& skinFolder);

    /** Shows the flyout around an anchor control
    * @param [in] pAnchor Anchor control
    * @param [in] xmlFile Content XML file name (resolved relative to SkinFolder); XML text starting with the character '<' is also accepted
    * @param [in] placement Preferred popup direction; when there is not enough space it flips to the opposite side automatically
    * @return Returns true when the flyout was shown; if the window cannot be created the flyout object destroys itself and
    *         false is returned, and the object must not be accessed any more in that case
    */
    bool ShowAt(Control* pAnchor, const std::string& xmlFile, Placement placement = Placement::Bottom);

    /** Closes the flyout (the close reason is recorded as kManual)
    */
    void Dismiss();

    /** Sets whether clicking outside or pressing Esc closes the flyout automatically; default is true
    * @note On non-Windows platforms, outside clicks cannot be detected in no-focus mode
    *       (SetNoFocus(true), the default), so automatic closing on an outside click does not work there;
    *       use SetNoFocus(false) to switch to focus mode when that behavior is needed
    *       (outside clicks are then detected through focus loss).
    */
    void SetAutoDismiss(bool bAutoDismiss);
    bool IsAutoDismiss() const;

    /** Sets whether the flyout leaves the focus where it is; default is true
    *   When set to false the flyout takes the focus, and outside clicks are detected through focus loss
    * @note On non-Windows platforms, outside click detection is not supported in no-focus mode (true)
    *       (see StartDetectTimer), so the flyout cannot close itself on an outside click; use false
    *       (focus mode) when that capability is required.
    */
    void SetNoFocus(bool bNoFocus);
    bool IsNoFocus() const;

    /** Sets the gap between the flyout and the anchor (logical value, before DPI scaling); default is 6
    */
    void SetGap(int32_t nGap);
    int32_t GetGap() const;

    /** Sets whether the flyout may flip to the opposite side when the preferred side has not enough space; default is true
    */
    void SetAllowFlip(bool bAllowFlip);
    bool IsAllowFlip() const;

    /** Gets the anchor control (may be nullptr after the flyout was closed or the anchor was destroyed)
    */
    Control* GetAnchor() const;

    /** Gets the preferred popup direction
    */
    Placement GetPlacement() const;

    /** Whether the flyout is currently open
    */
    bool IsOpen() const;

    /** Registers a callback invoked after the flyout has been opened
    */
    void AttachOpened(const FlyoutEvent& callback);

    /** Registers a callback invoked when the flyout is closed (called before the window is actually closed)
    */
    void AttachClosed(const FlyoutEvent& callback);

    /** Gets the currently active flyout (only one is kept at a time); returns nullptr when there is none
    */
    static Flyout* GetActiveFlyout();

    /** Closes the currently active flyout (no-op when there is none)
    */
    static void DismissActive();

public:
    //WindowImplBase interface
    virtual std::string GetSkinFolder() override;
    virtual std::string GetSkinFile() override;
    virtual void OnInitWindow() override;
    virtual void OnFinalMessage() override;

    /** In focus mode: closes the flyout as soon as the focus goes to another window; also closes it on the Esc key
    */
    virtual LRESULT OnKillFocusMsg(WindowBase* pSetFocusWindow, const NativeMsg& nativeMsg, bool& bHandled) override;
    virtual LRESULT OnKeyDownMsg(VirtualKeyCode vkCode, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) override;

    /** Closes the flyout directly when the DPI changes, to avoid a mismatch between the anchor coordinates/size and the screen
    */
    virtual void OnWindowDisplayScaleChanged(uint32_t nOldScaleFactor, uint32_t nNewScaleFactor) override;

protected:
    /** Recomputes the size and position of the flyout window from the anchor and the placement
    *   A subclass calls this after updating the content size in place (for example when the number of
    *   candidate rows changes the height), so the current window is reused instead of being destroyed
    *   and recreated
    */
    void PositionAroundAnchor();

private:
    /** Starts/stops the external input detection (in no-focus mode the global mouse state and the
    *   Esc key are polled by a timer)
    */
    void StartDetectTimer();
    void StopDetectTimer();
    void OnDetectTick();

    /** Runs the close sequence (takes effect only once)
    */
    void DoClose(CloseReason reason);

    /** Computes the rectangle of the anchor control in screen coordinates
    * @param [in] bPhysical true returns physical pixels (the coordinate system of GetCursorPos/GetWindowRect);
    *                       false returns DIP logical pixels (used for layout math)
    */
    bool GetAnchorScreenRect(UiRect& rcAnchor, bool bPhysical) const;

    /** Checks whether a screen point lies on the anchor control
    */
    bool IsPointInAnchor(const UiPoint& ptScreen) const;

private:
    //Parent window (not owned; used for positioning only; this window is its owned window and is destroyed with it)
    Window* m_pParentWindow;

    //Anchor control (weak reference)
    ControlPtrT<Control> m_pAnchor;

    //Content XML file name or XML text
    UiString m_xml;

    //Resource loading folder
    UiString m_skinFolder;

    //Preferred popup direction
    Placement m_placement;

    //Actual popup direction (may differ from the preferred one after flipping)
    Placement m_actualPlacement;

    //Whether the flyout closes automatically
    bool m_bAutoDismiss;

    //Whether the flyout leaves the focus where it is
    bool m_bNoFocus;

    //Gap to the anchor (before DPI scaling)
    int32_t m_nGap;

    //Whether flipping is allowed
    bool m_bAllowFlip;

    //Whether the flyout is open / whether it is closing
    bool m_bOpen;
    bool m_bClosing;

    //External input detection task ID (0 means no task)
    size_t m_nDetectTaskId;

    //Whether the mouse button/Esc key was down during the previous poll (used for edge detection)
    bool m_bPointerDownLast;
    bool m_bEscapeDownLast;

    //Opened/closed callbacks
    std::vector<FlyoutEvent> m_openedCallbacks;
    std::vector<FlyoutEvent> m_closedCallbacks;

private:
    //Currently active flyout (only one is kept at a time)
    static Flyout* s_pActiveFlyout;

    //Polling interval of the external input detection (milliseconds)
    static const int32_t kDetectIntervalMs;
};

} // namespace ui

#endif // UI_CONTROL_FLYOUT_H_
