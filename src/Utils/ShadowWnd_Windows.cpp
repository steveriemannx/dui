#include "dui/Utils/ShadowWnd.h"
#include "dui/Core/Box.h"
#include "dui/Core/Shadow.h"

#if defined (DUI_BUILD_FOR_WIN) && !defined (DUI_BUILD_FOR_SDL)

namespace ui {

/**@brief A window attached around the base window, with the WS_EX_LAYERED property to achieve the shadow
    * @copyright (c) 2016, NetEase Inc. All rights reserved
    * @date 2019-03-22
    */
class ShadowWndBase : public ui::WindowImplBase, public ui::IUIMessageFilter
{
public:
    ShadowWndBase();

    virtual DString GetSkinFolder() override;
    virtual DString GetSkinFile() override;

    virtual LRESULT FilterMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled) override;
    bool Create(Window* window);
    bool UpdateShadowPos(bool bCheckZOrder);
private:
    Window* m_pWindow;
    //Mark whether the followed window has completed its first paint
    bool m_isFirstPainted;
};

ShadowWndBase::ShadowWndBase():
    m_pWindow(nullptr),
    m_isFirstPainted(false)
{
}

DString ShadowWndBase::GetSkinFolder()
{
    return _T("");
}

DString ShadowWndBase::GetSkinFile()
{
    return _T("public/shadow/shadow.xml");
}

bool ShadowWndBase::Create(Window* window)
{
    m_pWindow = window;
    WindowCreateParam createParam;
    createParam.m_dwExStyle = kWS_EX_TRANSPARENT | kWS_EX_LAYERED | kWS_EX_TOOLWINDOW | kWS_EX_NOACTIVATE;
    createParam.m_className = _T("ShadowWnd");
    return Window::CreateWnd(nullptr, createParam);
}

LRESULT ShadowWndBase::FilterMessage(UINT uMsg, WPARAM wParam, LPARAM /*lParam*/, bool& bHandled)
{
    bHandled = false;
    if ((m_pWindow == nullptr) || !m_pWindow->IsWindow()) {
        return 0;
    }
    switch (uMsg)
    {
        case WM_ERASEBKGND:        
        case WM_MOVE:
        case WM_MOVING:
        case WM_SIZE:
        case WM_SIZING:
        case WM_WINDOWPOSCHANGING:
        case WM_WINDOWPOSCHANGED:
        case WM_CAPTURECHANGED:
        case WM_ACTIVATE:
        case WM_NCACTIVATE:
            UpdateShadowPos(false);
            break;
        case WM_PAINT:
            if (!m_isFirstPainted) {
                if (UpdateShadowPos(false)) {
                    m_isFirstPainted = true;
                }
            }
            else {
                UpdateShadowPos(true);
            }
            break;
        case WM_CLOSE:
            ShowWindow(kSW_HIDE);
            //Destroy the shadow when the associated window is closed
            CloseWnd();
            break;
        case WM_SHOWWINDOW:
            if (wParam == 0) {
                ShowWindow(kSW_HIDE);
            }
            else {
                ShowWindow(kSW_SHOW_NA);
                UpdateShadowPos(false);
            }
            break;
        default:
            UpdateShadowPos(true);
            break;
    }
    return 0;
}

bool ShadowWndBase::UpdateShadowPos(bool bCheckZOrder)
{
    if ((m_pWindow == nullptr) || !m_pWindow->IsWindow()) {
        return false;
    }
    if (!m_pWindow->IsWindowVisible()) {
        ShowWindow(kSW_HIDE);
        return true;
    }
    if (bCheckZOrder) {
        if (::GetWindow(m_pWindow->NativeWnd()->GetHWND(), GW_HWNDNEXT) == NativeWnd()->GetHWND()) {
            return true;
        }
    }

    UiRect rc;
    m_pWindow->GetWindowRect(rc);
    UiPadding rcShadow;
    GetCurrentShadowCorner(rcShadow);
    rc.Inflate(rcShadow);
    uint32_t uFlags = kSWP_NOACTIVATE;
    if (!IsWindowVisible()) {
        uFlags |= kSWP_SHOWWINDOW;
    }
    SetWindowPos(InsertAfterWnd(m_pWindow), rc.left, rc.top, rc.Width(), rc.Height(), uFlags);
    return true;
}

ShadowWnd::ShadowWnd():
    m_pShadowWnd(nullptr)
{
}

ShadowWnd::~ShadowWnd()
{
}

Box* ShadowWnd::AttachShadow(Box* pRoot)
{
    if (pRoot == nullptr) {
        return nullptr;
    }
    ASSERT(m_pShadowWnd == nullptr);
    if (m_pShadowWnd != nullptr) {
        return BaseClass::AttachShadow(pRoot);
    }

    bool needCreateShadowWnd = NeedCreateShadowWnd();
    if (!needCreateShadowWnd) {
        //The conditions for attaching a transparent window shadow are not met, so fall back to the original logic
        return BaseClass::AttachShadow(pRoot);
    }
    else {
        //Do not attach the default shadow; instead, create our own transparent window to serve as the shadow
        m_pShadowWnd = new ShadowWndBase;
        AddMessageFilter(m_pShadowWnd);

        //With an external shadow window, the original window needs rounded corners to avoid a black background at the rounded corners
        ASSERT(pRoot->GetWindow() == this);
        UiSize borderRound = GetShadowBorderRound();
        SetRoundCorner(borderRound.cx, borderRound.cy, true);
        pRoot->SetBorderRound(borderRound);
        InitShadow();
        return pRoot;
    }
}

bool ShadowWnd::NeedCreateShadowWnd() const
{
    if (IsLayeredWindow()) {
        //The layered window property is set, so do not create one
        return false;
    }
    if (IsUseDefaultShadowAttached()) {
        //The ShadowAttached property is not set in the configuration file or the external interface, so create one
        return !GetShadowImage().empty();
    }
    else {
        //The configuration file has the setting, so follow the property in the configuration file
        return IsShadowAttached() && !GetShadowImage().empty();
    }    
}

void ShadowWnd::InitShadow()
{
    //Disable the default window shadow
    if (IsShadowAttached()) {
        if (IsUseDefaultShadowAttached()) {
            SetShadowAttached(false);
            SetUseDefaultShadowAttached(true);
        }
        else {
            SetShadowAttached(false);
        }
    }

    //Remove the layered window property
    if (IsLayeredWindow()) {
        SetLayeredWindow(false, true);
    }

    //Enable the shadow of a non-layered window when the XML configuration file <Window> is configured as follows:
    //   layered_window = "false" shadow_attached = "true";
    if (m_pShadowWnd != nullptr) {
        m_pShadowWnd->Create(this);
        ASSERT(m_pShadowWnd->IsWindow());
        //The shadow window does not accept mouse or keyboard messages
        m_pShadowWnd->EnableWindow(false);
        m_pShadowWnd->SetShadowType(GetShadowType());
        if (IsWindowVisible()) {
            m_pShadowWnd->ShowWindow(kSW_SHOW_NA);
        }
    }
}

} // namespace ui

#endif //DUI_BUILD_FOR_WIN
