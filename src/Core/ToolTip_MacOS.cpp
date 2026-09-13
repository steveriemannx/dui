#include "dui/dui_config.h"

#include "dui/Core/ToolTip.h"
#include "dui/Core/Window.h"
#include "dui/Core/ControlPtrT.h"
#include "dui/Core/GlobalManager.h"
#include "dui/Core/WindowBuilder.h"
#include "dui/Core/WindowMessage.h"
#include "dui/Control/Label.h"

#if defined(DUI_BUILD_FOR_MACOS)

namespace ui
{
class ToolTipWindow: public Window
{
public:
    ToolTipWindow()
    {
        InitSkin("public/tooltip/", "tooltip.xml");
    }

    /** Called after the window has been created, for subclasses to do some initialization work
    */
    virtual void OnInitWindow() override
    {
        m_pToolTipText = dynamic_cast<Label*>(FindControl("tooltip_text"));
        if (!m_text.empty() && (m_pToolTipText != nullptr)) {
            m_pToolTipText->SetText(m_text);
        }
        if (m_pToolTipText != nullptr) {
            m_pToolTipText->SetMaxWidth(m_nMaxWidth, false);
        }
    }

    /** Set the text
    */
    void SetToolTipText(const std::string& text)
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
    std::string m_text;

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
                     const std::string& text);

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
        // Start the timer to post the synthetic hover message after the tooltip
        // delay, matching the native backend/Windows tooltip timing.
        m_hoverFlag = pParentWnd->GetWeakFlag();
        auto hoverCallback = [this, pParentWnd]() {
                if (pParentWnd != nullptr) {
                    pParentWnd->PostMsg(kWM_USER + 4);
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

void ToolTip::TImpl::ShowToolTip(WindowBase* pParentWnd,
                                 const UiRect& /*rect*/,
                                 uint32_t maxWidth,
                                 const UiPoint& trackPos,
                                 const std::string& text)
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

    if ((m_pTooltipWnd == nullptr) || m_pTooltipWnd->IsClosingWnd()) {
        m_pTooltipWnd = new ToolTipWindow;
    }    
    std::string skinFolder = m_pTooltipWnd->GetSkinFolder();
    std::string skinFile = m_pTooltipWnd->GetSkinFile();
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

    // Offset of the shadow (converted to client pixels)
    pParentWnd->Dpi().ScaleInt(rcShadowCorner.left);
    pParentWnd->Dpi().ScaleInt(rcShadowCorner.top);

    // Initial creation position (screen coordinates); the final position is
    // set below once the real tooltip size has been measured.
    UiPoint windowPos = trackPos;
    pParentWnd->ClientToScreen(windowPos);

    WindowCreateParam createParam;
    createParam.m_nX = windowPos.x;
    createParam.m_nY = windowPos.y;

    if (!m_pTooltipWnd->IsWindow()) {
        createParam.m_nWidth = pParentWnd->Dpi().GetScaleWindowSize(200); // The width and height are adjusted automatically after the text is set
        createParam.m_nHeight = pParentWnd->Dpi().GetScaleWindowSize(80);
        createParam.m_dwStyle = kWS_POPUPWINDOW;
        createParam.m_dwExStyle = kWS_EX_NOACTIVATE | kWS_EX_TRANSPARENT | kWS_EX_LAYERED | kWS_EX_TOOLTIP_WINDOW;
        m_pTooltipWnd->CreateWnd(pParentWnd, createParam);
    }

    m_pTooltipWnd->SetToolTipMaxWidth((int32_t)maxWidth);
    m_pTooltipWnd->SetToolTipText(text);

    // Show first, then force a synchronous layout and paint pass: the
    // auto-sized root resizes the window to the real content size and the
    // first paint completes before the window server composites the next
    // frame. Without this, the first appearance uses the stale creation size
    // (misplaced) and only the resized region gets painted (transparent
    // background).
    m_pTooltipWnd->ShowWindow(kSW_SHOW_NOACTIVATE);
    m_pTooltipWnd->UpdateWindow();

    // Measure the real window size and place the tooltip directly above the
    // mouse position: horizontally centered on the cursor, with its bottom
    // edge a small gap above it (so it appears above the hovered control).
    UiRect rcTooltip;
    m_pTooltipWnd->GetWindowRect(rcTooltip);
    int32_t tooltipWidth = rcTooltip.Width();
    int32_t tooltipHeight = rcTooltip.Height();
    // GetWindowRect is in window points; the mouse position is in client
    // pixels, so convert the tooltip size into the same pixel space first.
    pParentWnd->Dpi().WindowSizeToClientSize(tooltipWidth);
    pParentWnd->Dpi().WindowSizeToClientSize(tooltipHeight);

    windowPos = trackPos;
    windowPos.x -= tooltipWidth / 2;
    windowPos.y -= tooltipHeight + pParentWnd->Dpi().GetScaleInt(4);
    windowPos.x -= rcShadowCorner.left;
    windowPos.y -= rcShadowCorner.top;

    // The native macOS backend expects screen coordinates (from the top-left
    // of the screen, in window points) when positioning a popup window;
    // convert the parent window's client coordinates to screen coordinates.
    pParentWnd->ClientToScreen(windowPos);

    // Set the final window position (keep the real size)
    m_pTooltipWnd->SetWindowPos(InsertAfterWnd(), windowPos.x, windowPos.y, 0, 0, kSWP_NOSIZE | kSWP_NOZORDER);
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
                          const std::string& text)
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

#endif // DUI_BUILD_FOR_MACOS
