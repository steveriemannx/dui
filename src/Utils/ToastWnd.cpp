#include "dui/Utils/ToastWnd.h"
#include "dui/Core/GlobalManager.h"
#include "dui/Core/WindowCreateParam.h"
#include "dui/Core/WindowMessage.h"
#include "dui/Core/Callback.h"
#include "dui/Animation/AnimationPlayer.h"
#include "dui/Control/Button.h"
#include "dui/Core/Box.h"

#include <algorithm>
#include <chrono>

namespace ui
{
    //Static member initialization
    std::vector<ToastWnd*> ToastWnd::s_toasts;
    const int32_t ToastWnd::kToastGap          = 12;  //The vertical gap between notifications
    const int32_t ToastWnd::kAnchorMargin      = 24;  //The margin from the top/bottom edge of the parent client area
    const int32_t ToastWnd::kRightMargin       = 24;  //The margin from the right edge of the parent client area
    const int32_t ToastWnd::kEnterAnimMs       = 200; //The fade-in animation duration
    const int32_t ToastWnd::kExitAnimMs        = 180; //The fade-out animation duration
    const int32_t ToastWnd::kEnterSlideOffset  = 12;  //The slide distance of the enter animation
    const size_t ToastWnd::kMaxToastCount      = 5;   //The maximum number of notifications shown at the same time

    ToastWnd::ToastWnd() :
        m_pParentWindow(nullptr),
        m_type(kTypeInfo),
        m_durationMs(3000),
        m_remainingMs(3000),
        m_position(kPosTop),
        m_bTextId(false),
        m_pActionBox(nullptr),
        m_nAutoCloseTaskId(0),
        m_nExpireTick(0),
        m_bClosing(false),
        m_pRootBox(nullptr),
        m_pTextLabel(nullptr),
        m_pIconControl(nullptr),
        m_nWindowX(0),
        m_nWindowY(0),
        m_nWindowWidth(0),
        m_nWindowHeight(0),
        m_nClientWidth(0),
        m_nClientHeight(0)
    {
    }

    ToastWnd::~ToastWnd()
    {
        CancelAutoCloseTimer();
    }

    std::string ToastWnd::GetSkinFolder()
    {
        return "public/toast";
    }

    std::string ToastWnd::GetSkinFile()
    {
        return "toast.xml";
    }

    void ToastWnd::Show(ui::Window* pParentWindow,
                        const std::string& text,
                        ToastType type,
                        int32_t nDurationMs,
                        ToastPosition position,
                        bool bTextId)
    {
        Show(pParentWindow, text, type, {}, nDurationMs, position, bTextId);
    }

    void ToastWnd::Show(ui::Window* pParentWindow,
                        const std::string& text,
                        ToastType type,
                        const std::vector<ToastAction>& actions,
                        int32_t nDurationMs,
                        ToastPosition position,
                        bool bTextId)
    {
        ASSERT(nDurationMs >= 0);
        if (nDurationMs < 0) {
            nDurationMs = 3000;
        }

        //When the count exceeds the limit, let the earliest notification close first
        //(the close procedure removes it from the list automatically)
        if (s_toasts.size() >= kMaxToastCount) {
            s_toasts.front()->RequestClose();
        }

        //Do not steal the focus: a TOPMOST overlay may accidentally become the foreground window while it is
        //being created/shown, which makes the parent window (and the edit boxes inside it) lose their active
        //state and keyboard focus, so typing can no longer continue.
        //The foreground state of the parent window is recorded before creation, and if it was taken over the
        //parent window is restored explicitly after showing (the same as the no-focus mode of Flyout).
        const bool bParentWasForeground = (pParentWindow != nullptr) && pParentWindow->IsWindowForeground();

        ToastWnd* pToastWnd = new ToastWnd();
        pToastWnd->m_pParentWindow = pParentWindow;
        pToastWnd->m_text = text;
        pToastWnd->m_type = type;
        pToastWnd->m_durationMs = nDurationMs;
        pToastWnd->m_remainingMs = nDurationMs;
        pToastWnd->m_position = position;
        pToastWnd->m_bTextId = bTextId;
        pToastWnd->m_actions = actions;

        WindowCreateParam createWndParam;
        createWndParam.m_dwStyle = kWS_POPUP;
        createWndParam.m_dwExStyle = kWS_EX_TOPMOST | kWS_EX_LAYERED | kWS_EX_NOACTIVATE;
        if (pParentWindow == nullptr) {
            //Without a parent window it becomes a tool window, so that it does not appear in the taskbar or
            //in the Alt+Tab list
            createWndParam.m_dwExStyle |= kWS_EX_TOOLWINDOW;
        }

        if (!pToastWnd->CreateWnd(pParentWindow, createWndParam)) {
            delete pToastWnd;
            return;
        }

        //After creation (the content has been filled in and the size measured in OnInitWindow), add it to the
        //active list and position it according to its stacking index
        s_toasts.push_back(pToastWnd);
        int32_t nX = 0;
        int32_t nY = 0;
        CalcTargetWindowPos(pToastWnd, nX, nY);
        pToastWnd->m_nWindowX = nX;
        pToastWnd->m_nWindowY = nY;

        //Move it to the final position while it is fully transparent, so that it does not flicker on first show
        pToastWnd->SetLayeredWindowAlpha(0);
        pToastWnd->MoveToWindowPos(nX, nY);
        pToastWnd->ShowWindow(kSW_SHOW_NA);

        //Do not steal the focus: if the foreground state of the parent window was taken over while the overlay
        //was being created/shown, restore the foreground activation and the input focus of the parent window
        //explicitly, so that an edit box can still be typed into after the Toast took the focus
        //(for example, after pressing Enter in a search box).
        if (bParentWasForeground && (pParentWindow != nullptr) && !pParentWindow->IsWindowForeground()) {
            pParentWindow->SetWindowForeground();
            pParentWindow->SetWindowFocus();
        }

        pToastWnd->PlayEnterAnimation();
        pToastWnd->StartAutoCloseTimer(nDurationMs);
    }

    void ToastWnd::OnInitWindow()
    {
        BaseClass::OnInitWindow();

        m_pRootBox = GetRoot();
        ASSERT(m_pRootBox != nullptr);
        if (m_pRootBox == nullptr) {
            return;
        }

        InitContent();
        InitInteraction();
        InitActions();
        MeasureAndPosition();
    }

    void ToastWnd::OnFinalMessage()
    {
        //When the window is destroyed externally (for example the parent window closes or the program exits),
        //remove it from the active list and re-arrange the remaining notifications.
        //The normal close procedure has already removed it in RequestClose, so removing it again here is a
        //no-op.
        auto it = std::find(s_toasts.begin(), s_toasts.end(), this);
        if (it != s_toasts.end()) {
            s_toasts.erase(it);
            LayoutAllToasts(true);
        }

        //The non-modal window base class deletes this automatically after OnFinalMessage returns
        BaseClass::OnFinalMessage();
    }

    LRESULT ToastWnd::OnMouseLeaveMsg(const NativeMsg& nativeMsg, bool& bHandled)
    {
        //When the mouse leaves the notification window directly, the control level kEventMouseLeave is not
        //dispatched, so the countdown has to be resumed here
        ResumeAutoCloseTimer();
        return BaseClass::OnMouseLeaveMsg(nativeMsg, bHandled);
    }

    void ToastWnd::InitContent()
    {
        ASSERT(m_pRootBox != nullptr);
        if (m_pRootBox == nullptr) {
            return;
        }

        //The icon
        m_pIconControl = FindControl("toast_icon");
        if (m_pIconControl != nullptr) {
            std::string sIconFile;
            switch (m_type) {
            case kTypeSuccess:
                sIconFile = "file='toast_success.svg'";
                break;
            case kTypeWarning:
                sIconFile = "file='toast_warning.svg'";
                break;
            case kTypeError:
                sIconFile = "file='toast_error.svg'";
                break;
            case kTypeInfo:
            default:
                sIconFile = "file='toast_info.svg'";
                break;
            }
            m_pIconControl->SetBkImage(sIconFile);
            m_pIconControl->SetVisible(true);
        }

        //The text (supports multi-language IDs; the control refreshes itself after a language switch)
        m_pTextLabel = dynamic_cast<Label*>(FindControl("toast_text"));
        if (m_pTextLabel != nullptr) {
            if (m_bTextId) {
                m_pTextLabel->SetTextId(m_text);
            }
            else {
                m_pTextLabel->SetText(m_text);
            }
        }
    }

    void ToastWnd::InitInteraction()
    {
        //Note: GetRoot() returns the shadow container (the framework has already configured it not to receive
        //mouse messages), so the events must be bound to the XML root container (the actually visible
        //notification bar).
        Box* pContentBox = GetXmlRoot();
        ASSERT(pContentBox != nullptr);
        if (pContentBox == nullptr) {
            return;
        }

        //Clicking anywhere on the notification closes it immediately (an ordinary container does not dispatch
        //kEventClick, so the left mouse button up event is used)
        pContentBox->AttachButtonUp([this](const EventArgs& /*args*/) {
            RequestClose();
            return true;
        });

        //The mouse entered the notification area: pause the auto close countdown
        pContentBox->AttachMouseEnter([this](const EventArgs& /*args*/) {
            PauseAutoCloseTimer();
            return true;
        });

        //The mouse left the notification area: continue the countdown with the remaining time
        pContentBox->AttachMouseLeave([this](const EventArgs& /*args*/) {
            ResumeAutoCloseTimer();
            return true;
        });
    }

    void ToastWnd::InitActions()
    {
        if (m_actions.empty()) {
            return;
        }
        Box* pActions = dynamic_cast<Box*>(FindControl("toast_actions"));
        if (pActions == nullptr) {
            return;
        }
        m_pActionBox = pActions;

        for (const ToastAction& action : m_actions) {
            ui::Button* pBtn = new ui::Button(this);
            if (action.bTextId) {
                pBtn->SetTextId(action.text);
            }
            else {
                pBtn->SetText(action.text);
            }
            //Style: the same as the today/clear buttons of the calendar (rounded border plus hover/pressed
            //feedback); the semantic colors adapt to the light and dark themes
            pBtn->SetAttribute("width", "auto");
            pBtn->SetAttribute("height", "28");
            pBtn->SetAttribute("margin", "4,0,4,0");
            pBtn->SetAttribute("min_width", "64");
            pBtn->SetAttribute("text_padding", "12,0,12,0");
            pBtn->SetAttribute("font", "system_regular_12");
            pBtn->SetAttribute("text_color", "color_accent");
            pBtn->SetAttribute("border_size", "1");
            pBtn->SetAttribute("border_round", "4,4");
            pBtn->SetAttribute("normal_border_color", "border_control_normal");
            pBtn->SetAttribute("hovered_border_color", "border_btn_hovered");
            pBtn->SetAttribute("hovered_color", "bg_btn_hovered");
            pBtn->SetAttribute("pressed_color", "bg_btn_pressed");
            //Clicking: run the callback first, then close the notification (the button has mouse_enabled=true,
            //so the click does not bubble up to the whole-bar close handler of the root container)
            pBtn->AttachClick([this, action](const ui::EventArgs& /*args*/) {
                if (action.callback) {
                    action.callback();
                }
                RequestClose();
                return true;
            });
            pActions->AddItem(pBtn);
        }

        pActions->SetVisible(true);
    }

    void ToastWnd::MeasureAndPosition()
    {
        ASSERT(m_pRootBox != nullptr);
        if (m_pRootBox == nullptr) {
            return;
        }

        //Estimate the final size with the monitor work area as the available space (the return value includes
        //the window shadow)
        UiRect rcWork;
        GetMonitorWorkRect(rcWork);
        Dpi().WindowSizeToClientSize(rcWork);
        UiSize szAvailable(rcWork.Width(), rcWork.Height());

        UiEstSize estSize = m_pRootBox->EstimateSize(szAvailable);
        UiSize szWindow(szAvailable.cx, szAvailable.cy);
        if (estSize.cx.IsInt32()) {
            szWindow.cx = estSize.cx.GetInt32();
        }
        if (estSize.cy.IsInt32()) {
            szWindow.cy = estSize.cy.GetInt32();
        }

        m_nWindowWidth = szWindow.cx;
        m_nWindowHeight = szWindow.cy;

        //The padding of the root container is the area occupied by the shadow around it (see Menu::ResizeMenu)
        UiPadding rcShadow = m_pRootBox->GetPadding();
        m_nClientWidth = szWindow.cx - rcShadow.left - rcShadow.right;
        m_nClientHeight = szWindow.cy - rcShadow.top - rcShadow.bottom;
        if (m_nClientWidth < 10) {
            m_nClientWidth = 10;
        }
        if (m_nClientHeight < 10) {
            m_nClientHeight = 10;
        }
    }

    void ToastWnd::StartAutoCloseTimer(int32_t nDelayMs)
    {
        CancelAutoCloseTimer();
        m_remainingMs = nDelayMs;
        if (nDelayMs <= 0) {
            //0 means it never closes automatically
            return;
        }

        m_nExpireTick = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count() + nDelayMs;
        m_nAutoCloseTaskId = GlobalManager::Instance().Thread().PostDelayedTask(
            kThreadUI,
            UiBind(this, [this]() {
                m_nAutoCloseTaskId = 0;
                m_nExpireTick = 0;
                RequestClose();
            }),
            nDelayMs);
    }

    void ToastWnd::PauseAutoCloseTimer()
    {
        if (m_bClosing || (m_durationMs <= 0) || (m_nExpireTick == 0)) {
            return;
        }
        int64_t nNow = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count();
        int32_t nRemain = static_cast<int32_t>(m_nExpireTick - nNow);
        if (nRemain < 0) {
            nRemain = 0;
        }
        CancelAutoCloseTimer();
        m_remainingMs = nRemain;
    }

    void ToastWnd::ResumeAutoCloseTimer()
    {
        if (m_bClosing || (m_durationMs <= 0) || (m_nExpireTick != 0)) {
            return;
        }
        if (m_remainingMs > 0) {
            StartAutoCloseTimer(m_remainingMs);
        }
        else {
            //The stay time has already run out (the expiration point was crossed while paused), so close directly
            RequestClose();
        }
    }

    void ToastWnd::CancelAutoCloseTimer()
    {
        if (m_nAutoCloseTaskId != 0) {
            GlobalManager::Instance().Thread().CancelTask(m_nAutoCloseTaskId);
            m_nAutoCloseTaskId = 0;
        }
        m_nExpireTick = 0;
    }

    void ToastWnd::PlayEnterAnimation()
    {
        std::weak_ptr<WeakFlag> weakFlag = GetWeakFlag();
        if (m_pEnterPlayer != nullptr) {
            m_pEnterPlayer->Stop();
        }
        AnimationPlayer* pPlayer = new AnimationPlayer;
        m_pEnterPlayer.reset(pPlayer);
        pPlayer->SetAnimationType(AnimationType::kAnimationNone);
        pPlayer->SetStartValue(0);
        pPlayer->SetEndValue(100);
        pPlayer->SetTotalMillSeconds(kEnterAnimMs);
        pPlayer->SetFrameIntervalMillSeconds(1000 / 60);
        pPlayer->SetEasingFunctionType(EaseOutCubic);
        pPlayer->SetPlayCallback([this, weakFlag](int32_t nValue) {
            if (weakFlag.expired()) {
                return;
            }
            int32_t nAlpha = 255 * nValue / 100;
            int32_t nOffset = kEnterSlideOffset * (100 - nValue) / 100;
            SetLayeredWindowAlpha(nAlpha);
            MoveToWindowPos(m_nWindowX, m_nWindowY + nOffset);
        });
        pPlayer->SetCompleteCallback([this, weakFlag]() {
            if (weakFlag.expired()) {
                return;
            }
            SetLayeredWindowAlpha(255);
            MoveToWindowPos(m_nWindowX, m_nWindowY);
        });
        pPlayer->Start();
    }

    void ToastWnd::PlayExitAnimation()
    {
        std::weak_ptr<WeakFlag> weakFlag = GetWeakFlag();
        if (m_pEnterPlayer != nullptr) {
            m_pEnterPlayer->Stop();
        }
        if (m_pMovePlayer != nullptr) {
            m_pMovePlayer->Stop();
        }
        if (m_pExitPlayer != nullptr) {
            m_pExitPlayer->Stop();
        }
        AnimationPlayer* pPlayer = new AnimationPlayer;
        m_pExitPlayer.reset(pPlayer);
        pPlayer->SetAnimationType(AnimationType::kAnimationNone);
        pPlayer->SetStartValue(0);
        pPlayer->SetEndValue(100);
        pPlayer->SetTotalMillSeconds(kExitAnimMs);
        pPlayer->SetFrameIntervalMillSeconds(1000 / 60);
        pPlayer->SetEasingFunctionType(EaseInCubic);
        pPlayer->SetPlayCallback([this, weakFlag](int32_t nValue) {
            if (weakFlag.expired()) {
                return;
            }
            int32_t nAlpha = 255 * (100 - nValue) / 100;
            SetLayeredWindowAlpha(nAlpha);
        });
        pPlayer->SetCompleteCallback([this, weakFlag]() {
            if (weakFlag.expired()) {
                return;
            }
            SetLayeredWindowAlpha(0);
            CloseWnd();
        });
        pPlayer->Start();
    }

    void ToastWnd::PlayMoveAnimation(int32_t nFromY, int32_t nToY)
    {
        std::weak_ptr<WeakFlag> weakFlag = GetWeakFlag();
        if (m_pMovePlayer != nullptr) {
            m_pMovePlayer->Stop();
        }
        AnimationPlayer* pPlayer = new AnimationPlayer;
        m_pMovePlayer.reset(pPlayer);
        pPlayer->SetAnimationType(AnimationType::kAnimationNone);
        pPlayer->SetStartValue(0);
        pPlayer->SetEndValue(100);
        pPlayer->SetTotalMillSeconds(kEnterAnimMs);
        pPlayer->SetFrameIntervalMillSeconds(1000 / 60);
        pPlayer->SetEasingFunctionType(EaseOutCubic);
        pPlayer->SetPlayCallback([this, weakFlag, nFromY, nToY](int32_t nValue) {
            if (weakFlag.expired()) {
                return;
            }
            int32_t nY = nFromY + (nToY - nFromY) * nValue / 100;
            MoveToWindowPos(m_nWindowX, nY);
        });
        pPlayer->SetCompleteCallback([this, weakFlag, nToY]() {
            if (weakFlag.expired()) {
                return;
            }
            MoveToWindowPos(m_nWindowX, nToY);
        });
        pPlayer->Start();
    }

    void ToastWnd::RequestClose()
    {
        if (m_bClosing) {
            return;
        }
        m_bClosing = true;
        CancelAutoCloseTimer();

        //Remove it from the active list, and the remaining notifications move up to fill the gap
        auto it = std::find(s_toasts.begin(), s_toasts.end(), this);
        if (it != s_toasts.end()) {
            s_toasts.erase(it);
        }
        LayoutAllToasts(true);

        PlayExitAnimation();
    }

    void ToastWnd::LayoutAllToasts(bool bAnimate)
    {
        for (ToastWnd* pToast : s_toasts) {
            if ((pToast == nullptr) || pToast->m_bClosing) {
                continue;
            }
            int32_t nNewX = 0;
            int32_t nNewY = 0;
            CalcTargetWindowPos(pToast, nNewX, nNewY);

            const int32_t nOldY = pToast->m_nWindowY;
            pToast->m_nWindowX = nNewX;
            pToast->m_nWindowY = nNewY;

            if (!bAnimate) {
                pToast->MoveToWindowPos(nNewX, nNewY);
                continue;
            }

            //A notification whose enter animation is playing snaps to the target position directly, so that the
            //two animations do not overwrite each other
            bool bEntering = (pToast->m_pEnterPlayer != nullptr) &&
                             pToast->m_pEnterPlayer->IsPlaying();
            if ((nNewY == nOldY) || bEntering) {
                pToast->MoveToWindowPos(nNewX, nNewY);
                continue;
            }
            pToast->PlayMoveAnimation(nOldY, nNewY);
        }
    }

    void ToastWnd::CalcTargetWindowPos(const ToastWnd* pToast, int32_t& nX, int32_t& nY)
    {
        ASSERT(pToast != nullptr);
        if (pToast == nullptr) {
            nX = 0;
            nY = 0;
            return;
        }

        //1. Calculate the anchor area (screen client area coordinates/DIP): prefer the client area of the parent
        //   window, otherwise use the monitor work area
        UiRect rcAnchor;
        UiRect rcWork;
        Window* pParent = pToast->m_pParentWindow;
        if (pParent != nullptr) {
            UiPoint ptOrigin(0, 0);
            pParent->ClientToScreen(ptOrigin);
            UiRect rcClient;
            pParent->GetClientRect(rcClient);
            rcAnchor = UiRect(ptOrigin.x, ptOrigin.y,
                              ptOrigin.x + rcClient.Width(), ptOrigin.y + rcClient.Height());
            pParent->Dpi().WindowSizeToClientSize(rcAnchor);

            pParent->GetMonitorWorkRect(rcWork);
            pParent->Dpi().WindowSizeToClientSize(rcWork);
        }
        else {
            UiPoint ptCursor;
            pToast->GetCursorPos(ptCursor);
            pToast->GetMonitorWorkRect(ptCursor, rcWork);
            pToast->Dpi().WindowSizeToClientSize(rcWork);
            rcAnchor = rcWork;
        }

        //2. Sum the heights of the notifications that are on the same anchor and come before this one in the
        //   list (the stacking offset)
        int32_t nStackOffset = 0;
        for (ToastWnd* pOther : s_toasts) {
            if (pOther == pToast) {
                break;
            }
            if ((pOther != nullptr) &&
                (pOther->m_pParentWindow == pToast->m_pParentWindow) &&
                (pOther->m_position == pToast->m_position) &&
                !pOther->m_bClosing) {
                nStackOffset += pOther->m_nClientHeight + kToastGap;
            }
        }

        const int32_t nClientW = pToast->m_nClientWidth;
        const int32_t nClientH = pToast->m_nClientHeight;

        //3. The horizontal position
        int32_t nClientX = 0;
        switch (pToast->m_position) {
        case kPosTopRight:
        case kPosBottomRight:
            nClientX = rcAnchor.right - kRightMargin - nClientW;
            break;
        case kPosTop:
        case kPosCenter:
        case kPosBottom:
        default:
            nClientX = rcAnchor.CenterX() - nClientW / 2;
            break;
        }

        //4. The vertical position
        int32_t nClientY = 0;
        switch (pToast->m_position) {
        case kPosBottom:
        case kPosBottomRight:
            nClientY = rcAnchor.bottom - kAnchorMargin - nStackOffset - nClientH;
            break;
        case kPosCenter:
            nClientY = rcAnchor.CenterY() - nClientH / 2 + nStackOffset;
            break;
        case kPosTop:
        case kPosTopRight:
        default:
            nClientY = rcAnchor.top + kAnchorMargin + nStackOffset;
            break;
        }

        //5. Clamp it into the monitor work area
        if (nClientX < rcWork.left) {
            nClientX = rcWork.left;
        }
        if (nClientX + nClientW > rcWork.right) {
            nClientX = rcWork.right - nClientW;
        }
        if (nClientY < rcWork.top) {
            nClientY = rcWork.top;
        }
        if (nClientY + nClientH > rcWork.bottom) {
            nClientY = rcWork.bottom - nClientH;
        }

        //6. Convert to window coordinates (including the shadow offset)
        UiPadding rcShadow = pToast->m_pRootBox != nullptr ?
                             pToast->m_pRootBox->GetPadding() : UiPadding();
        nX = nClientX - rcShadow.left;
        nY = nClientY - rcShadow.top;
    }

    void ToastWnd::MoveToWindowPos(int32_t nX, int32_t nY)
    {
        int32_t nWindowX = nX;
        int32_t nWindowY = nY;
        UiSize szWindow(m_nWindowWidth, m_nWindowHeight);
        Dpi().ClientSizeToWindowSize(nWindowX);
        Dpi().ClientSizeToWindowSize(nWindowY);
        Dpi().ClientSizeToWindowSize(szWindow);
        SetWindowPos(InsertAfterWnd(InsertAfterFlag::kHWND_TOPMOST),
                     nWindowX, nWindowY, szWindow.cx, szWindow.cy,
                     kSWP_SHOWWINDOW | kSWP_NOACTIVATE);
    }

} //namespace ui
