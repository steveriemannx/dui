#include "dui/Core/ToolTip.h"
#include "dui/Core/Window.h"
#include "dui/Core/ControlPtrT.h"
#include "dui/Core/GlobalManager.h"
#include "dui/Core/WindowBuilder.h"
#include "dui/Core/NativeWindow_SDL.h"
#include "dui/Control/Label.h"

#ifdef DUI_BUILD_FOR_WAYLAND

namespace ui
{
class ToolTipWindow: public Window
{
public:
    ToolTipWindow()
    {
        InitSkin(_T("public/tooltip/"), _T("tooltip.xml"));
    }

    virtual void OnInitWindow() override
    {
        m_pToolTipText = dynamic_cast<Label*>(FindControl(_T("tooltip_text")));
        if (!m_text.empty() && (m_pToolTipText != nullptr)) {
            m_pToolTipText->SetText(m_text);
        }
        if (m_pToolTipText != nullptr) {
            m_pToolTipText->SetMaxWidth(m_nMaxWidth, false);
        }
    }

    void SetToolTipText(const DString& text)
    {
        m_text = text;
        if (m_pToolTipText != nullptr) {
            m_pToolTipText->SetText(text);
        }
    }

    void SetToolTipMaxWidth(int32_t nMaxWidth)
    {
        m_nMaxWidth = (nMaxWidth <= 0) ? INT32_MAX : nMaxWidth;
        if (m_pToolTipText != nullptr) {
            m_pToolTipText->SetMaxWidth(m_nMaxWidth, false);
        }
    }

private:
    ControlPtrT<Label> m_pToolTipText;
    DString m_text;
    int32_t m_nMaxWidth = INT32_MAX;
};

class ToolTip::TImpl
{
public:
    TImpl();
    ~TImpl();

    void SetMouseTracking(WindowBase* pParentWnd, bool bTracking);
    void ShowToolTip(WindowBase* pParentWnd, const UiRect& rect, uint32_t maxWidth,
                     const UiPoint& trackPos, const DString& text);
    void HideToolTip();
    void ClearMouseTracking();
    void DestroyToolTip();
    void StopHoverTimer();

public:
    ControlPtrT<WindowBase> m_pParentWnd;
    std::weak_ptr<WeakFlag> m_hoverFlag;
    ControlPtrT<ToolTipWindow> m_pTooltipWnd;
    bool m_bMouseTracking;
    size_t m_nTimerId;
    const uint32_t m_hoverMillSeconds = 320;
};

ToolTip::TImpl::TImpl():
    m_nTimerId(0),
    m_bMouseTracking(false)
{
}

ToolTip::TImpl::~TImpl()
{
    StopHoverTimer();
}

void ToolTip::TImpl::StopHoverTimer()
{
    m_hoverFlag.reset();
    if (m_nTimerId != 0) {
        GlobalManager::Instance().Timer().RemoveTimer(m_nTimerId);
        m_nTimerId = 0;
    }
    m_bMouseTracking = false;
}

void ToolTip::TImpl::SetMouseTracking(WindowBase* pParentWnd, bool bTracking)
{
    ASSERT(pParentWnd != nullptr);
    if (pParentWnd == nullptr) {
        return;
    }
    if (bTracking && !m_bMouseTracking) {
        m_hoverFlag = pParentWnd->GetWeakFlag();
        ControlPtrT<WindowBase> spParentWnd(pParentWnd);
        auto hoverCallback = [this, pParentWnd]() {
            if (pParentWnd != nullptr) {
                pParentWnd->PostMsg(NativeWindow_SDL::GetHoverMsgId());
                m_nTimerId = 0;
                m_bMouseTracking = false;
                m_hoverFlag.reset();
            }
        };
        m_nTimerId = GlobalManager::Instance().Timer().AddTimer(m_hoverFlag, hoverCallback, m_hoverMillSeconds, 1);
    }
    m_pParentWnd = pParentWnd;
    m_bMouseTracking = bTracking;
    if (!bTracking && (m_nTimerId != 0)) {
        StopHoverTimer();
    }
}

void ToolTip::TImpl::ShowToolTip(WindowBase* pParentWnd, const UiRect& rect, uint32_t maxWidth,
                                  const UiPoint& trackPos, const DString& text)
{
    (void)trackPos;
    if (text.empty()) {
        HideToolTip();
        return;
    }
    if (m_pTooltipWnd == nullptr) {
        m_pTooltipWnd = new ToolTipWindow;
        WindowCreateParam createParam;
        
        createParam.m_className = _T("tooltip");
        m_pTooltipWnd->CreateWnd(pParentWnd, createParam);
    }
    if (m_pTooltipWnd != nullptr) {
        m_pTooltipWnd->SetToolTipText(text);
        m_pTooltipWnd->SetToolTipMaxWidth(maxWidth);
        UiRect rcWindow = rect;
        m_pTooltipWnd->SetWindowPos(InsertAfterWnd(InsertAfterFlag::kHWND_NOTOPMOST), rcWindow.left, rcWindow.top, rcWindow.Width(), rcWindow.Height(), 0);
        m_pTooltipWnd->ShowWindow(kSW_SHOW_NOACTIVATE);
    }
}

void ToolTip::TImpl::HideToolTip()
{
    if (m_pTooltipWnd != nullptr) {
        m_pTooltipWnd->ShowWindow(kSW_HIDE);
    }
}

void ToolTip::TImpl::ClearMouseTracking()
{
    StopHoverTimer();
    HideToolTip();
}

void ToolTip::TImpl::DestroyToolTip()
{
    StopHoverTimer();
    if (m_pTooltipWnd != nullptr) {
        m_pTooltipWnd->CloseWnd();
        m_pTooltipWnd = nullptr;
    }
}

// ToolTip public interface
ToolTip::ToolTip()
{
    m_impl = new TImpl;
}

ToolTip::~ToolTip()
{
    if (m_impl != nullptr) {
        delete m_impl;
        m_impl = nullptr;
    }
}

void ToolTip::SetMouseTracking(WindowBase* pParentWnd, bool bTracking)
{
    m_impl->SetMouseTracking(pParentWnd, bTracking);
}

void ToolTip::ShowToolTip(WindowBase* pParentWnd, const UiRect& rect, uint32_t maxWidth,
                          const UiPoint& trackPos, const DString& text)
{
    m_impl->ShowToolTip(pParentWnd, rect, maxWidth, trackPos, text);
}

void ToolTip::HideToolTip()
{
    m_impl->HideToolTip();
}

void ToolTip::ClearMouseTracking()
{
    m_impl->ClearMouseTracking();
}

void ToolTip::DestroyToolTip()
{
    m_impl->DestroyToolTip();
}

} // namespace ui

#endif // DUI_BUILD_FOR_WAYLAND
