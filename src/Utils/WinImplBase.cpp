#include "dui/Utils/WinImplBase.h"
#include "dui/Core/WindowBuilder.h"
#include "dui/Core/Box.h"
#include "dui/Utils/FilePath.h"

#ifdef DUI_BUILD_FOR_MACOS
#include "dui/Control/MacTrafficLights.h"
#endif

namespace ui
{

WindowImplBase::WindowImplBase():
    m_pMaxButton(nullptr),
    m_pMinButton(nullptr),
    m_pRestoreButton(nullptr)
{
}

WindowImplBase::~WindowImplBase()
{
}

void WindowImplBase::PreInitWindow()
{
    BaseClass::PreInitWindow();
    BindCaptionButtons();
}

void WindowImplBase::BindCaptionButtons()
{
    if (m_bCaptionButtonsBound || IsUseSystemCaption()) {
        return;
    }
    bool bFoundAny = false;

    //Close button
    Control* pControl = FindControl(DUI_CTR_BUTTON_CLOSE);
    if (pControl) {
        ASSERT(pControl->GetType() == DUI_CTR_BUTTON);
        pControl->AttachClick(UiBind(&WindowImplBase::OnButtonClick, this, std::placeholders::_1));
        bFoundAny = true;
    }

    //Minimize button
    pControl = FindControl(DUI_CTR_BUTTON_MIN);
    if (pControl) {
        ASSERT(pControl->GetType() == DUI_CTR_BUTTON);
        pControl->AttachClick(UiBind(&WindowImplBase::OnButtonClick, this, std::placeholders::_1));

        m_pMinButton = pControl;
        m_minButtonFlag = pControl->GetWeakFlag();
        bFoundAny = true;
    }

    //Maximize button
    pControl = FindControl(DUI_CTR_BUTTON_MAX);
    if (pControl) {
        ASSERT(pControl->GetType() == DUI_CTR_BUTTON);
        pControl->AttachClick(UiBind(&WindowImplBase::OnButtonClick, this, std::placeholders::_1));

        m_pMaxButton = pControl;
        m_maxButtonFlag = pControl->GetWeakFlag();
        bFoundAny = true;
    }

    //Restore button
    pControl = FindControl(DUI_CTR_BUTTON_RESTORE);
    if (pControl) {
        ASSERT(pControl->GetType() == DUI_CTR_BUTTON);
        pControl->AttachClick(UiBind(&WindowImplBase::OnButtonClick, this, std::placeholders::_1));

        m_pRestoreButton = pControl;
        m_restoreButtonFlag = pControl->GetWeakFlag();
        bFoundAny = true;
    }

    //Fullscreen button
    pControl = FindControl(DUI_CTR_BUTTON_FULLSCREEN);
    if (pControl) {
        ASSERT(pControl->GetType() == DUI_CTR_BUTTON);
        pControl->AttachClick(UiBind(&WindowImplBase::OnButtonClick, this, std::placeholders::_1));
        bFoundAny = true;
    }

#if defined (DUI_BUILD_FOR_SDL) && !defined (DUI_BUILD_FOR_WIN)
    //Title bar: since SDL does not support double-clicking the title bar to maximize/restore the window, implement this logic ourselves (non-Windows platforms)
    pControl = FindControl(DUI_CTR_CAPTION_BAR);
    if (pControl) {
        pControl->AttachBubbledEvent(ui::kEventMouseDoubleClick, UiBind(&WindowImplBase::OnTitleBarDoubleClick, this, std::placeholders::_1), 0);
        bFoundAny = true;
    }
#endif
    // Only mark as bound if at least one caption control was actually found.
    // For pure-code windows, the control tree is built in OnInitWindow, so the
    // first call (from PreInitWindow) finds nothing and leaves the flag false,
    // allowing the second call (from OnInitWindow) to retry after the controls exist.
    if (bFoundAny) {
        m_bCaptionButtonsBound = true;
    }
}

DString WindowImplBase::GetSkinFolder()
{
    return BaseClass::GetSkinFolder();
}

DString WindowImplBase::GetSkinFile()
{
    return BaseClass::GetSkinFile();
}

Control* WindowImplBase::CreateControl(const DString& strClass)
{
    return BaseClass::CreateControl(strClass);
}

void WindowImplBase::OnInitWindow()
{
    BaseClass::OnInitWindow();

    // For pure-code windows (no layout XML), the control tree is built in the
    // derived class's OnInitWindow, so PreInitWindow could not find the caption
    // buttons yet. Bind them now (BindCaptionButtons is idempotent).
    BindCaptionButtons();

#ifdef DUI_BUILD_FOR_MACOS
    ApplyMacCaptionBar();
#endif
}

void WindowImplBase::OnInitLayout()
{
    BaseClass::OnInitLayout();
}

void WindowImplBase::OnPreCloseWindow()
{
    BaseClass::OnPreCloseWindow();
}

void WindowImplBase::OnCloseWindow()
{
    BaseClass::OnCloseWindow();
}

void WindowImplBase::OnFinalMessage()
{
    BaseClass::OnFinalMessage();
}

bool WindowImplBase::OnButtonClick(const EventArgs& msg)
{
    if (IsUseSystemCaption()) {
        return true;
    }
    Control* pSender = msg.GetSender();
    ASSERT(pSender != nullptr);
    if (pSender == nullptr) {
        return false;
    }
    DString sCtrlName = pSender->GetName();
    if (sCtrlName == DUI_CTR_BUTTON_CLOSE) {
        //Close button
        CloseWnd();
    }
    else if (sCtrlName == DUI_CTR_BUTTON_MIN) {
        //Minimize button
        ShowWindow(kSW_MINIMIZE);
    }
    else if (sCtrlName == DUI_CTR_BUTTON_MAX) {
        //Maximize button        
        ShowWindow(kSW_SHOW_MAXIMIZED);
    }
    else if (sCtrlName == DUI_CTR_BUTTON_RESTORE) {
        //Restore button        
        ShowWindow(kSW_RESTORE);
    }
    else if (sCtrlName == DUI_CTR_BUTTON_FULLSCREEN) {
        //Fullscreen button
        EnterFullscreen();
    }

    return true;
}

bool WindowImplBase::OnTitleBarDoubleClick(const EventArgs& /*param*/)
{
    Control* pControl = FindControl(DUI_CTR_BUTTON_MAX);
    if ((pControl != nullptr) && pControl->IsVisible()) {
        //Maximize button
        if (!IsWindowMaximized()) {
            ShowWindow(kSW_SHOW_MAXIMIZED);
        }
    }
    else {
        //Restore button
        pControl = FindControl(DUI_CTR_BUTTON_RESTORE);
        if ((pControl != nullptr) && pControl->IsVisible()) {
            if (IsWindowMaximized()) {
                ShowWindow(kSW_RESTORE);
            }            
        }
    }
    return true;
}

LRESULT WindowImplBase::OnSizeMsg(WindowSizeType sizeType, const UiSize& newWindowSize, const NativeMsg& nativeMsg, bool& bHandled)
{
    std::weak_ptr<WeakFlag> windowFlag = GetWeakFlag();
    LRESULT lResult = BaseClass::OnSizeMsg(sizeType, newWindowSize, nativeMsg, bHandled);
    if (windowFlag.expired()) {
        return lResult;
    }
    if (sizeType == WindowSizeType::kSIZE_MAXIMIZED) {
        //Window maximized
        if (!IsWindowFullscreen()) {
            OnWindowMaximized();
        }
    }
    else if (sizeType == WindowSizeType::kSIZE_RESTORED) {
        //Window restored
        if (!IsWindowFullscreen()) {
            OnWindowRestored();
        }
    }
    else if (sizeType == WindowSizeType::kSIZE_MINIMIZED) {
        //Window minimized
        if (!IsWindowFullscreen()) {
            OnWindowMinimized();
        }
    }
    return lResult;
}

void WindowImplBase::OnWindowEnterFullscreen()
{
    if (!IsUseSystemCaption()) {
        Control* pCaptionBar = FindControl(DUI_CTR_CAPTION_BAR);
        if (pCaptionBar != nullptr) {
            pCaptionBar->SetVisible(false);
        }
    }
}

void WindowImplBase::OnWindowExitFullscreen()
{
    if (!IsUseSystemCaption()) {
        Control* pCaptionBar = FindControl(DUI_CTR_CAPTION_BAR);
        if (pCaptionBar != nullptr) {
            pCaptionBar->SetVisible(true);
        }
        ProcessMaxRestoreStatus();
    }
}

void WindowImplBase::OnWindowMaximized()
{
    ProcessMaxRestoreStatus();
}

void WindowImplBase::OnWindowRestored()
{
    ProcessMaxRestoreStatus();
}

void WindowImplBase::OnWindowMinimized()
{
}

void WindowImplBase::OnUseSystemCaptionBarChanged()
{
    BaseClass::OnUseSystemCaptionBarChanged();
    if (GetRoot() == nullptr) {
        return;
    }
    Control* pCaptionBar = FindControl(DUI_CTR_CAPTION_BAR);
    if (pCaptionBar != nullptr) {
        pCaptionBar->SetVisible(!IsUseSystemCaption());
        if (!IsUseSystemCaption()) {
            ProcessMaxRestoreStatus();
        }
    }
}

void WindowImplBase::OnWindowDisplayScaleChanged(uint32_t nOldScaleFactor, uint32_t nNewScaleFactor)
{
    BaseClass::OnWindowDisplayScaleChanged(nOldScaleFactor, nNewScaleFactor);
}

void WindowImplBase::ProcessMaxRestoreStatus()
{
    if (IsUseSystemCaption() || (GetRoot() == nullptr)) {
        return;
    }
    Control* pMaxButton = FindControl(DUI_CTR_BUTTON_MAX);
    Control* pRestoreButton = FindControl(DUI_CTR_BUTTON_RESTORE);
    bool bWindowMax = IsWindowMaximized();
    if (pMaxButton != nullptr) {
        pMaxButton->SetFadeVisible(bWindowMax ? false : true);
    }
    if (pRestoreButton != nullptr) {
        pRestoreButton->SetFadeVisible(bWindowMax ? true : false);
    }
}

bool WindowImplBase::HasMinMaxBox(bool& bMinimizeBox, bool& bMaximizeBox) const
{
    bMinimizeBox = false;
    bMaximizeBox = false;
    if ((m_pMinButton != nullptr) && !m_minButtonFlag.expired()) {
        bMinimizeBox = true;
    }
    if ((m_pMaxButton != nullptr) && !m_maxButtonFlag.expired()) {
        bMaximizeBox = true;
    }
    return true;
}

bool WindowImplBase::IsPtInMaximizeRestoreButton(const UiPoint& pt) const
{
    bool bInButton = false;
    if ((m_pMaxButton != nullptr) && !m_maxButtonFlag.expired() && m_pMaxButton->IsVisible()) {
        bInButton = m_pMaxButton->GetRect().ContainsPt(pt);
    }
    else if ((m_pRestoreButton != nullptr) && !m_restoreButtonFlag.expired() && m_pRestoreButton->IsVisible()) {
        bInButton = m_pRestoreButton->GetRect().ContainsPt(pt);
    }
    return bInButton;
}

#ifdef DUI_BUILD_FOR_MACOS
void WindowImplBase::ApplyMacCaptionBar()
{
    if (IsUseSystemCaption() || (GetRoot() == nullptr)) {
        return;
    }
    //The self-drawn traffic lights replace the custom window buttons, so hide
    //the XML/code-declared caption buttons.
    const DString captionButtonNames[] = {
        DUI_CTR_BUTTON_CLOSE,
        DUI_CTR_BUTTON_MIN,
        DUI_CTR_BUTTON_MAX,
        DUI_CTR_BUTTON_RESTORE,
        DUI_CTR_BUTTON_FULLSCREEN,
    };
    for (const DString& strButtonName : captionButtonNames) {
        Control* pButton = FindControl(strButtonName);
        if (pButton != nullptr) {
            pButton->SetVisible(false);
        }
    }
    Control* pCaptionBar = FindControl(DUI_CTR_CAPTION_BAR);
    if (pCaptionBar == nullptr) {
        return;
    }
    //macOS title bar: white background, 3pt thinner than the XML height
    //(36pt) to match the compact macOS-style caption bar.
    pCaptionBar->SetBkColor(_T("#FFFFFFFF"));
    pCaptionBar->SetFixedHeight(UiFixedInt(33), true, true);
    //No extra macOS title-bar separator line: the examples use a clean
    //caption bar without a visible border line.
    pCaptionBar->SetBorderSize(UiRectF(0.0f, 0.0f, 0.0f, 0.0f), true);

    //Match the window caption (drag) region to the thinner bar.
    UiRect rcCaption = GetCaptionRect();
    rcCaption.bottom = Dpi().GetScaleInt(33);
    SetCaptionRect(rcCaption, false);

    //Insert the self-drawn traffic lights at the far left of the caption bar.
    //The control's fixed 76pt width also acts as the left gutter, so the rest
    //of the caption content (logo/title/buttons) shifts right like a native
    //macOS title bar.
    Box* pCaptionBox = dynamic_cast<Box*>(pCaptionBar);
    if (pCaptionBox != nullptr) {
        MacTrafficLights* pTrafficLights = new MacTrafficLights(this);
        pTrafficLights->SetName(_T("mac_traffic_lights"));
        pTrafficLights->SetFixedWidth(UiFixedInt(76), true, true);
        pTrafficLights->SetFixedHeight(UiFixedInt(33), true, true);
        if (pCaptionBox->AddItemAt(pTrafficLights, 0)) {
            pTrafficLights->AttachClick(
                UiBind(&WindowImplBase::OnMacTrafficLightsClick, this, std::placeholders::_1));
            m_pMacTrafficLights = pTrafficLights;
        }
        else {
            delete pTrafficLights;
        }
    }
}

bool WindowImplBase::OnMacTrafficLightsClick(const EventArgs& args)
{
    Control* pSender = args.GetSender();
    MacTrafficLights* pTrafficLights = dynamic_cast<MacTrafficLights*>(pSender);
    if (pTrafficLights == nullptr) {
        return false;
    }
    switch (pTrafficLights->GetClickedZone()) {
    case MacTrafficLights::ClickZone::kClose:
        CloseWnd();
        break;
    case MacTrafficLights::ClickZone::kMinimize:
        ShowWindow(kSW_MINIMIZE);
        break;
    case MacTrafficLights::ClickZone::kZoom:
        //macOS behavior: the green traffic light toggles fullscreen
        //(not maximize/restore)
        if (IsWindowFullscreen()) {
            ExitFullscreen();
        }
        else {
            EnterFullscreen();
        }
        break;
    default:
        break;
    }
    return true;
}

LRESULT WindowImplBase::OnSetFocusMsg(WindowBase* /*pLostFocusWindow*/, const NativeMsg& nativeMsg, bool& bHandled)
{
    LRESULT lResult = BaseClass::OnSetFocusMsg(nullptr, nativeMsg, bHandled);
    if (m_pMacTrafficLights != nullptr) {
        m_pMacTrafficLights->SetWindowActive(true);
    }
    return lResult;
}

LRESULT WindowImplBase::OnKillFocusMsg(WindowBase* /*pSetFocusWindow*/, const NativeMsg& nativeMsg, bool& bHandled)
{
    LRESULT lResult = BaseClass::OnKillFocusMsg(nullptr, nativeMsg, bHandled);
    if (m_pMacTrafficLights != nullptr) {
        m_pMacTrafficLights->SetWindowActive(false);
    }
    return lResult;
}
#endif

}
