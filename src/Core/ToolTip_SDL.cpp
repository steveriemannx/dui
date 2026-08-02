#include "duilib/Core/ToolTip.h"
#include "duilib/Core/Window.h"
#include "duilib/Core/ControlPtrT.h"
#include "duilib/Core/GlobalManager.h"
#include "duilib/Core/WindowBuilder.h"
#include "duilib/Control/Label.h"

#ifdef DUILIB_BUILD_FOR_SDL

namespace ui
{
class ToolTipWindow: public Window
{
public:
    ToolTipWindow()
    {
        InitSkin(_T("public/tooltip/"), _T("tooltip.xml"));
    }

    /** Called after the window has been created, for subclasses to do some initialization work
    */
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

    /** Set the text
    */
    void SetToolTipText(const DString& text)
    {
        m_text = text;
        if (m_pToolTipText != nullptr) {
            m_pToolTipText->SetText(text);
        }
    }

    /** Set the maximum width of the text
    */
    void SetToolTipMaxWidth(int32_t nMaxWidth)
    {
        if (nMaxWidth <= 0) {
            m_nMaxWidth = INT32_MAX;
        }
        else {
            m_nMaxWidth = nMaxWidth;
        }
        if (m_pToolTipText != nullptr) {
            m_pToolTipText->SetMaxWidth(m_nMaxWidth, false);
        }
    }

private:
    /** Text control
    */
    ControlPtrT<Label> m_pToolTipText;

    /** ToolTip text content
    */
    DString m_text;

    /** Maximum width of the text
    */
    int32_t m_nMaxWidth = INT32_MAX;
};

class ToolTip::TImpl
{
public:
    TImpl();
    ~TImpl();

public:
    /** Set the mouse tracking state
    * @param [in] pParentWnd Parent window
    * @param [in] bTracking Whether to track the mouse state
    */
    void SetMouseTracking(WindowBase* pParentWnd, bool bTracking);

    /**@brief Display ToolTip information
    * @param [in] pParentWnd Parent window
    * @param [in] rect ToolTip display area
    * @param [in] maxWidth Maximum ToolTip display width
    * @param [in] trackPos Tracked position
    * @param [in] text ToolTip display content
    */
    void ShowToolTip(WindowBase* pParentWnd,
                     const UiRect& rect, 
                     uint32_t maxWidth,
                     const UiPoint& trackPos,
                     const DString& text);

    /**@brief Hide ToolTip information
    */
    void HideToolTip();

    /**@brief Clear the mouse tracking state
    */
    void ClearMouseTracking();

    /** Destroy the ToolTip window
    */
    void DestroyToolTip();

    // Stop the timer
    void StopHoverTimer();

public:
    // The window associated with the ToolTip
    ControlPtrT<WindowBase> m_pParentWnd;

    // Validity guarantee of the timer
    std::weak_ptr<WeakFlag> m_hoverFlag;

    // The ToolTip's own window
    ControlPtrT<ToolTipWindow> m_pTooltipWnd;

    // Mouse tracking state
    bool m_bMouseTracking;

    // Timer ID
    size_t m_nTimerId;

    // The time interval for showing the ToolTip, i.e. the trigger interval of the timer (milliseconds)
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
        // Start the timer to track the state
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
        // Stop the timer
        StopHoverTimer();
    }
}

void ToolTip::TImpl::ShowToolTip(WindowBase* pParentWnd,
                                 const UiRect& /*rect*/,
                                 uint32_t maxWidth,
                                 const UiPoint& trackPos,
                                 const DString& text)
{
    ASSERT(pParentWnd != nullptr);
    if (pParentWnd == nullptr) {
        return;
    }
    if (m_pParentWnd != pParentWnd) {
        return;
    }
    if (text.empty()) {
        return;
    }
    
    // Initial position of the window (client area coordinates)
    UiPoint windowPos = trackPos;
    if ((m_pTooltipWnd == nullptr) || m_pTooltipWnd->IsClosingWnd()) {
        m_pTooltipWnd = new ToolTipWindow;
    }    
    DString skinFolder = m_pTooltipWnd->GetSkinFolder();
    DString skinFile = m_pTooltipWnd->GetSkinFile();
    FilePath xmlPath(skinFolder);
    xmlPath.NormalizeDirectoryPath();
    xmlPath += skinFile;

    UiPadding rcShadowCorner;
    WindowBuilder windowBuilder;
    if (windowBuilder.ParseXmlFile(xmlPath)) {
        WindowCreateAttributes createAttributes;
        if (windowBuilder.ParseWindowCreateAttributes(createAttributes)) {
            rcShadowCorner = createAttributes.m_rcShadowCorner;
        }
    }

    WindowCreateParam createParam;
    createParam.m_nX = windowPos.x + pParentWnd->Dpi().GetScaleInt(10); // Offset to the right from the mouse point
    createParam.m_nY = windowPos.y + pParentWnd->Dpi().GetScaleInt(10); // Offset downward from the mouse point
    // Offset of the shadow
    pParentWnd->Dpi().ScaleInt(rcShadowCorner.left);
    pParentWnd->Dpi().ScaleInt(rcShadowCorner.top);
    createParam.m_nX -= rcShadowCorner.left;
    createParam.m_nY -= rcShadowCorner.top;
    
    // Inside SDL, when adjusting the position of a Popup window, client area coordinates are used for the window position, but the offset values use the window size values (mainly affecting the logic on macOS with high-DPI screens)
    pParentWnd->Dpi().UnscaleInt(createParam.m_nX);
    pParentWnd->Dpi().UnscaleInt(createParam.m_nY);
    pParentWnd->Dpi().ScaleWindowSize(createParam.m_nX);
    pParentWnd->Dpi().ScaleWindowSize(createParam.m_nY);

    if (!m_pTooltipWnd->IsWindow()) {
        createParam.m_nWidth = pParentWnd->Dpi().GetScaleWindowSize(200); // After the window is created or shown, its width and height are set automatically
        createParam.m_nHeight = pParentWnd->Dpi().GetScaleWindowSize(80);
        createParam.m_dwStyle = kWS_POPUPWINDOW;
        createParam.m_dwExStyle = kWS_EX_NOACTIVATE | kWS_EX_TRANSPARENT | kWS_EX_LAYERED | kWS_EX_TOOLTIP_WINDOW;
        m_pTooltipWnd->CreateWnd(pParentWnd, createParam);
    }

    // Set the window position
    m_pTooltipWnd->SetWindowPos(InsertAfterWnd(), createParam.m_nX, createParam.m_nY, 0, 0, kSWP_NOSIZE | kSWP_NOZORDER);

    m_pTooltipWnd->SetToolTipMaxWidth((int32_t)maxWidth);
    m_pTooltipWnd->SetToolTipText(text);
    m_pTooltipWnd->ShowWindow(kSW_SHOW_NOACTIVATE);
}

void ToolTip::TImpl::HideToolTip()
{
    // Stop the timer
    StopHoverTimer();

    if (m_pTooltipWnd != nullptr) {
        m_pTooltipWnd->ShowWindow(ShowWindowCommands::kSW_HIDE);
    }
}

void ToolTip::TImpl::ClearMouseTracking()
{
    // Stop the timer
    StopHoverTimer();
    m_pParentWnd = nullptr;    
}

void ToolTip::TImpl::DestroyToolTip()
{
    if (m_pTooltipWnd != nullptr) {
        m_pTooltipWnd->CloseWnd();
    }
}

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

void ToolTip::ShowToolTip(WindowBase* pParentWnd,
                          const UiRect& rect, 
                          uint32_t maxWidth,
                          const UiPoint& trackPos,
                          const DString& text)
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

#endif //DUILIB_BUILD_FOR_SDL
