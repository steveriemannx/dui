#ifndef DUI_UTILS_TOAST_WND_H_
#define DUI_UTILS_TOAST_WND_H_

#include "dui/Utils/WinImplBase.h"
#include "dui/Control/Label.h"
#include "dui/Core/Callback.h"
#include <vector>
#include <memory>

namespace ui
{

class AnimationPlayer;

/** The action button of a Toast (a Win32 notification style ActionButton, such as "Undo" or "View details")
*   Clicking the button runs the callback, and then the notification closes automatically;
*   when the callback is empty, clicking only closes the notification.
*/
struct ToastAction
{
    std::string     text;       //The button text (resolved as a multi-language ID when bTextId=true)
    StdClosure  callback;   //The click callback (can be empty)
    bool        bTextId = false; //Whether text is a multi-language ID
};

/** A non-modal notification window (Toast) with a custom-drawn skin
*   Features:
*   1. Non-modal and it does not steal the focus of the parent window; the parent window stays usable while it is shown
*   2. Supports 4 types: information/success/warning/error (each with a different colored icon)
*   3. Closes automatically after a timeout (3000ms by default and customizable; 0 means it never closes
*      automatically), with fade in/out and upward slide animations
*   4. Multiple notifications are stacked vertically starting from the anchor position, and the remaining
*      notifications move smoothly to fill the gap after one of them closes
*   5. The countdown is paused while the mouse hovers over the notification and continues when it leaves;
*      clicking the notification closes it immediately
*   6. The message text supports multi-language IDs (bTextId=true) and refreshes in place after a language switch
*
*   Usage (static interface, fire-and-forget, the window object destroys itself after it closes):
*   @code
*       ui::ToastWnd::Show(this, "Saved successfully", ui::ToastWnd::kTypeSuccess);
*       ui::ToastWnd::Show(this, "STRID_NETWORK_ERROR", ui::ToastWnd::kTypeError,
*                          5000, ui::ToastWnd::kPosTop, true);
*   @endcode
*/
class DUI_API ToastWnd : public ui::WindowImplBase
{
    typedef ui::WindowImplBase BaseClass;

public:
    /** The notification type, which decides the style of the icon on the left
    */
    enum ToastType : uint32_t
    {
        kTypeInfo    = 0,  //Information (a blue i)
        kTypeSuccess = 1,  //Success (a green check mark)
        kTypeWarning = 2,  //Warning (a yellow !)
        kTypeError   = 3   //Error (a red X)
    };

    /** The position where the notification appears (relative to the client area of the parent window;
    *   when the parent window is null, relative to the work area of the monitor the parent window is on)
    */
    enum ToastPosition : uint32_t
    {
        kPosTop         = 0,  //Centered at the top (default)
        kPosCenter      = 1,  //Vertically centered
        kPosBottom      = 2,  //Centered at the bottom
        kPosTopRight    = 3,  //Top right corner
        kPosBottomRight = 4   //Bottom right corner (similar to a system notification)
    };

public:
    /** Show a Toast notification (non-modal; it does not block the calling thread or the parent window)
    * @param [in] pParentWindow The parent window; the notification is shown near its client area.
    *                           Can be nullptr (positioned by the monitor work area)
    * @param [in] text The notification content; supports '\n' line breaks, and long text is wrapped automatically
    * @param [in] type The notification type, see enum ToastType
    * @param [in] nDurationMs How long the notification stays before it closes automatically, in milliseconds;
    *                         the default is 3000, and 0 means it never closes automatically
    *                         (the countdown is paused while the mouse hovers and continues with the remaining
    *                         time after the mouse leaves)
    * @param [in] position The display position, see enum ToastPosition
    * @param [in] bTextId When true, text is resolved as a multi-language ID and refreshes in place after a
    *                     language switch
    */
    static void Show(ui::Window* pParentWindow,
                     const std::string& text,
                     ToastType type = kTypeInfo,
                     int32_t nDurationMs = 3000,
                     ToastPosition position = kPosTop,
                     bool bTextId = false);

    /** Show a Toast notification with action buttons (same as above, plus a group of ActionButtons rendered
    *   in the action area)
    * @param [in] actions The list of action buttons (such as {Got it, View details}); when it is empty this is
    *                     equivalent to the 6-parameter version above
    * @note The meaning of the other parameters is the same as for Show above
    */
    static void Show(ui::Window* pParentWindow,
                     const std::string& text,
                     ToastType type,
                     const std::vector<ToastAction>& actions,
                     int32_t nDurationMs = 3000,
                     ToastPosition position = kPosTop,
                     bool bTextId = false);

private:
    ToastWnd();
    virtual ~ToastWnd() override;

    /** Resource related interfaces
    */
    virtual std::string GetSkinFolder() override;
    virtual std::string GetSkinFile() override;

    /** After the window is created: fill in the content, bind the interactions, calculate the position,
    *   play the enter animation and start the auto close countdown
    */
    virtual void OnInitWindow() override;

    /** After the window message loop ends: unregister from the active list (the non-modal window base class
    *   deletes this afterwards)
    */
    virtual void OnFinalMessage() override;

    /** The mouse left the client area of the window (WM_MOUSELEAVE): the framework does not dispatch this
    *   message to the controls, so it is used here to resume the auto close countdown when the mouse leaves
    *   the notification window directly
    */
    virtual LRESULT OnMouseLeaveMsg(const NativeMsg& nativeMsg, bool& bHandled) override;

private:
    /** Fill in the text and the icon
    */
    void InitContent();

    /** Create and lay out the action buttons dynamically (returns immediately when there are none, so the
    *   original layout is not affected)
    */
    void InitActions();

    /** Measure the window size (including the shadow) and calculate the final display position
    */
    void MeasureAndPosition();

    /** Bind the click-to-close and hover pause/resume events of the root container
    */
    void InitInteraction();

    /** Start/pause/resume the auto close countdown
    */
    void StartAutoCloseTimer(int32_t nDelayMs);
    void PauseAutoCloseTimer();
    void ResumeAutoCloseTimer();
    void CancelAutoCloseTimer();

    /** Play the enter animation (fade in plus a slight upward slide)
    */
    void PlayEnterAnimation();

    /** Play the exit animation (fade out); the window is closed when the animation ends
    */
    void PlayExitAnimation();

    /** Play the position move animation (the notification moves smoothly to fill the gap after another
    *   notification closed)
    * @param [in] nFromY The starting window Y (client area coordinates)
    * @param [in] nToY The target window Y (client area coordinates)
    */
    void PlayMoveAnimation(int32_t nFromY, int32_t nToY);

    /** Request to close: remove the notification from the active list, let the remaining notifications fill
    *   the gap, and then play the exit animation
    */
    void RequestClose();

    /** Re-arrange all notifications according to the current active list
    * @param [in] bAnimate true=move smoothly when the position changes; false=move directly (used when a new
    *                      notification is positioned for the first time)
    */
    static void LayoutAllToasts(bool bAnimate);

    /** Calculate the target window position of the given notification (window coordinates, including the
    *   shadow offset)
    * @param [in] pToast The notification to position (its size must have been measured by MeasureAndPosition)
    * @param [out] nX The X coordinate of the window top-left corner (client area coordinates)
    * @param [out] nY The Y coordinate of the window top-left corner (client area coordinates)
    */
    static void CalcTargetWindowPos(const ToastWnd* pToast, int32_t& nX, int32_t& nY);

    /** Move the window to the target position (the DPI conversion is done internally)
    */
    void MoveToWindowPos(int32_t nX, int32_t nY);

private:
    //The parent window (no reference is held; it is only used for positioning, and this window is destroyed
    //together with it as an owned window)
    ui::Window* m_pParentWindow;

    //The notification content
    std::string m_text;

    //The notification type
    ToastType m_type;

    //How long the notification stays before it closes automatically, in milliseconds; 0 means it never
    //closes automatically
    int32_t m_durationMs;

    //The remaining stay time in milliseconds; used when the countdown is paused by hovering
    int32_t m_remainingMs;

    //The display position
    ToastPosition m_position;

    //Whether the text is a multi-language ID
    bool m_bTextId;

    //The list of action buttons (empty means no action buttons)
    std::vector<ToastAction> m_actions;

    //The action button container (toast_actions in the XML)
    ui::Box* m_pActionBox;

    //The ID of the auto close delayed task (0 means no task)
    size_t m_nAutoCloseTaskId;

    //The expiration time point of the countdown (a steady_clock millisecond timestamp); 0 means the
    //countdown is not running
    int64_t m_nExpireTick;

    //Whether the exit animation is playing (prevents a click or the timer from triggering the close twice)
    bool m_bClosing;

private:
    //The root container (used to bind the click and hover events)
    ui::Control* m_pRootBox;

    //The text control
    ui::Label* m_pTextLabel;

    //The icon control
    ui::Control* m_pIconControl;

    //The target window size and position (window coordinates, including the shadow)
    int32_t m_nWindowX;
    int32_t m_nWindowY;
    int32_t m_nWindowWidth;
    int32_t m_nWindowHeight;

    //The size of the visually effective area (client area coordinates, excluding the shadow)
    int32_t m_nClientWidth;
    int32_t m_nClientHeight;

    //The enter/exit/position fill animation players (stopped when the window object is destroyed)
    std::unique_ptr<AnimationPlayer> m_pEnterPlayer;
    std::unique_ptr<AnimationPlayer> m_pExitPlayer;
    std::unique_ptr<AnimationPlayer> m_pMovePlayer;

private:
    //All notifications that are currently active (in creation order; accessed only on the UI thread)
    static std::vector<ToastWnd*> s_toasts;

    //The vertical gap between notifications, the margin from the anchor edge and the margin from the right
    //edge (client area coordinates)
    static const int32_t kToastGap;
    static const int32_t kAnchorMargin;
    static const int32_t kRightMargin;

    //The enter/exit animation duration in milliseconds, and the slide distance of the enter animation
    //(client area coordinates)
    static const int32_t kEnterAnimMs;
    static const int32_t kExitAnimMs;
    static const int32_t kEnterSlideOffset;

    //The maximum number of notifications shown at the same time; beyond that the oldest one exits first
    static const size_t kMaxToastCount;
};

} //namespace ui

#endif //DUI_UTILS_TOAST_WND_H_
