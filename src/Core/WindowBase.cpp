#include "dui/Core/WindowBase.h"
#include "dui/Core/GlobalManager.h"
#include "dui/Core/WindowCreateAttributes.h"
#include "dui/Utils/StringConvert.h"
#include "dui/Utils/MonitorUtil.h"
#include <random>

namespace ui
{
WindowBase::WindowBase():
    m_pParentWindow(nullptr), 
    m_pNativeWindow(nullptr),
    m_bWindowFirstShown(false),
    m_bWindowSized(false),
    m_windowSizeState(WindowSizeState::kUnknown),
    m_bSendDragEnterMsg(false)
{
    m_pNativeWindow = new NativeWindow(this);
}

WindowBase::~WindowBase()
{
    ClearWindowBase();
    if (m_pNativeWindow != nullptr) {
        delete m_pNativeWindow;
        m_pNativeWindow = nullptr;
    }
}

bool WindowBase::CreateWnd(WindowBase* pParentWindow, const WindowCreateParam& createParam)
{
    //Parse the XML and read the window attribute parameters
    WindowCreateAttributes createAttributes;
    GetCreateWindowAttributes(createAttributes);
    SetWindowId(createParam.m_windowId);
    m_windowClassName = createParam.m_className;
    m_pParentWindow = pParentWindow;

    NativeWindow* pNativeWindow = pParentWindow != nullptr ? pParentWindow->NativeWnd() : nullptr;
    return m_pNativeWindow->CreateWnd(pNativeWindow, createParam, createAttributes);
}

int32_t WindowBase::DoModal(WindowBase* pParentWindow, const WindowCreateParam& createParam,
                            bool bCloseByEsc, bool bCloseByEnter)
{
    //Parse the XML and read the window attribute parameters
    WindowCreateAttributes createAttributes;
    GetCreateWindowAttributes(createAttributes);
    SetWindowId(createParam.m_windowId);
    m_windowClassName = createParam.m_className;
    m_pParentWindow = pParentWindow;

    NativeWindow* pNativeWindow = pParentWindow != nullptr ? pParentWindow->NativeWnd() : nullptr;
    return m_pNativeWindow->DoModal(pNativeWindow, createParam, createAttributes, bCloseByEsc, bCloseByEnter);
}

bool WindowBase::CreateChildWnd(WindowBase* pParentWindow, int32_t nX, int32_t nY, int32_t nWidth, int32_t nHeight)
{
    SetWindowId(_T(""));
    m_pParentWindow = pParentWindow;
    NativeWindow* pNativeWindow = pParentWindow != nullptr ? pParentWindow->NativeWnd() : nullptr;
    return m_pNativeWindow->CreateChildWnd(pNativeWindow, nX, nY, nWidth, nHeight);
}

void WindowBase::OnNativeCreateWndMsg(bool bDoModal, const NativeMsg& nativeMsg, bool& bHandled)
{
    std::weak_ptr<WeakFlag> windowFlag = GetWeakFlag();

    //The window creation is complete; initialize (internal use)
    InitWindowBase();

    //Callback for subclasses to parse the XML file and bind it to the window (internal use; subclasses can override)
    if (!windowFlag.expired()) {
        PreInitWindow();
    }

    //Internal initialization (internal use)
    if (!windowFlag.expired()) {
        PostInitWindow();
    }

    //Call the subclass initialization function
    if (!windowFlag.expired()) {
        OnInitWindow();
    }
    if (!windowFlag.expired()) {
        OnWindowCreateMsg(bDoModal, nativeMsg, bHandled);
    }    
    if (!windowFlag.expired()) {
        //Callback to the application layer
        SendWindowEvent(kWindowCreateMsg, (WPARAM)bDoModal ? 1 : 0);
    }
}

void WindowBase::ClearWindowBase()
{
    m_pParentWindow = nullptr;
    m_dpi.reset();

    //Clear the resources of the native window
    m_pNativeWindow->ClearNativeWindow();
}

void WindowBase::SetUseSystemCaption(bool bUseSystemCaption)
{
    m_pNativeWindow->SetUseSystemCaption(bUseSystemCaption);
}

bool WindowBase::IsUseSystemCaption() const
{
    return m_pNativeWindow->IsUseSystemCaption();
}

void WindowBase::SetLayeredWindowAlpha(int32_t nAlpha)
{
    m_pNativeWindow->SetLayeredWindowAlpha(nAlpha);
    OnWindowAlphaChanged();
}

uint8_t WindowBase::GetLayeredWindowAlpha() const
{
    return m_pNativeWindow->GetLayeredWindowAlpha();
}

void WindowBase::SetLayeredWindowOpacity(int32_t nAlpha)
{
    m_pNativeWindow->SetLayeredWindowOpacity(nAlpha);
    OnWindowAlphaChanged();
}

uint8_t WindowBase::GetLayeredWindowOpacity() const
{
    return m_pNativeWindow->GetLayeredWindowOpacity();
}


bool WindowBase::SetLayeredWindow(bool bIsLayeredWindow, bool bRedraw)
{
    bool bRet = m_pNativeWindow->SetLayeredWindow(bIsLayeredWindow, bRedraw);
    OnLayeredWindowChanged();
    return bRet;
}

bool WindowBase::IsLayeredWindow() const
{
    return m_pNativeWindow->IsLayeredWindow();
}

void WindowBase::CloseWnd(int32_t nRet)
{
    m_pNativeWindow->CloseWnd(nRet);
}

void WindowBase::Close()
{
    m_pNativeWindow->Close();
}

bool WindowBase::IsClosingWnd() const
{
    return m_pNativeWindow->IsClosingWnd();
}

int32_t WindowBase::GetCloseParam() const
{
    return m_pNativeWindow->GetCloseParam();
}

bool WindowBase::AddMessageFilter(IUIMessageFilter* pFilter)
{
    if (std::find(m_aMessageFilters.begin(), m_aMessageFilters.end(), pFilter) != m_aMessageFilters.end()) {
        ASSERT(false);
        return false;
    }
    if (pFilter != nullptr) {
        m_aMessageFilters.push_back(pFilter);
    }
    return true;
}
bool WindowBase::RemoveMessageFilter(IUIMessageFilter* pFilter)
{
    auto iter = std::find(m_aMessageFilters.begin(), m_aMessageFilters.end(), pFilter);
    if (iter != m_aMessageFilters.end()) {
        m_aMessageFilters.erase(iter);
        return true;
    }
    return false;
}

LRESULT WindowBase::PostMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return m_pNativeWindow->PostMsg(uMsg, wParam, lParam);
}

void WindowBase::PostQuitMsg(int32_t nExitCode)
{
    return NativeWindow::PostQuitMsg(nExitCode);
}

WindowBase* WindowBase::WindowBaseFromPoint(const UiPoint& pt, bool bIgnoreChildWindow)
{
    WindowBase* pWindowBase = nullptr;
    INativeWindow* pNativeWindow = m_pNativeWindow->WindowBaseFromPoint(pt, bIgnoreChildWindow);
    if (pNativeWindow != nullptr) {
        pWindowBase = dynamic_cast<WindowBase*>(pNativeWindow);
    }
    return pWindowBase;
}

NativeWindow* WindowBase::NativeWnd() const
{
    return m_pNativeWindow;
}

WindowBase* WindowBase::GetParentWindow() const
{
    return m_pParentWindow.get();
}

bool WindowBase::SetParentWindow(WindowBase* pParentWindow)
{
    ASSERT((pParentWindow != nullptr) && pParentWindow->IsWindow());
    if ((pParentWindow == nullptr) || !pParentWindow->IsWindow()) {
        return false;
    }
    m_pParentWindow = pParentWindow;
    return m_pNativeWindow->SetParentWindow(pParentWindow->NativeWnd());
}

bool WindowBase::IsWindow() const
{
    return m_pNativeWindow->IsWindow();
}

bool WindowBase::IsChildWindow() const
{
    return m_pNativeWindow->IsChildWindow();
}

void WindowBase::InitWindowBase()
{
    ASSERT(IsWindow());
    if (!IsWindow()) {
        return;
    }
    //Initialize the window's own DPI manager
    m_dpi = std::make_unique<DpiManager>();
    m_dpi->SetDisplayScaleForWindow(this);
}

void WindowBase::GetClientRect(UiRect& rcClient) const
{
    m_pNativeWindow->GetClientRect(rcClient);
}

void WindowBase::GetWindowRect(UiRect& rcWindow) const
{
    m_pNativeWindow->GetWindowRect(rcWindow);
}

void WindowBase::ScreenToClient(UiPoint& pt) const
{
    m_pNativeWindow->ScreenToClient(pt);
}

void WindowBase::ClientToScreen(UiPoint& pt) const
{
    m_pNativeWindow->ClientToScreen(pt);
}

void WindowBase::ClientToScreen(UiRect& rc) const
{
    UiPoint pt;
    pt.x = rc.left;
    pt.y = rc.top;
    ClientToScreen(pt);

    int32_t cx = rc.Width();
    Dpi().ClientSizeToWindowSize(cx);
    int32_t cy = rc.Height();
    Dpi().ClientSizeToWindowSize(cy);

    rc.right = pt.x + cx;
    rc.left = pt.x;
    rc.bottom = pt.y + cy;
    rc.top = pt.y;
}

void WindowBase::ScreenToClient(UiRect& rc) const
{
    UiPoint pt;
    pt.x = rc.left;
    pt.y = rc.top;
    ScreenToClient(pt);

    int32_t cx = rc.Width();
    Dpi().WindowSizeToClientSize(cx);
    int32_t cy = rc.Height();
    Dpi().WindowSizeToClientSize(cy);

    rc.right = pt.x + cx;
    rc.left = pt.x;
    rc.bottom = pt.y + cy;
    rc.top = pt.y;
}

void WindowBase::GetCursorPos(UiPoint& pt) const
{
    m_pNativeWindow->GetCursorPos(pt);
}

bool WindowBase::GetMonitorRect(UiRect& rcMonitor) const
{
    return m_pNativeWindow->GetMonitorRect(rcMonitor);
}

bool WindowBase::GetPrimaryMonitorWorkRect(UiRect& rcWork)
{
    return NativeWindow::GetPrimaryMonitorWorkRect(rcWork);
}

bool WindowBase::GetMonitorWorkRect(UiRect& rcWork) const
{
    return m_pNativeWindow->GetMonitorWorkRect(rcWork);
}

bool WindowBase::GetMonitorWorkRect(const UiPoint& pt, UiRect& rcWork) const
{
    return m_pNativeWindow->GetMonitorWorkRect(pt, rcWork);
}

void WindowBase::SetCapture()
{
    m_pNativeWindow->SetCapture();
}

void WindowBase::ReleaseCapture()
{
    m_pNativeWindow->ReleaseCapture();
}

bool WindowBase::IsCaptured() const
{
    return m_pNativeWindow->IsCaptured();
}

bool WindowBase::ShowWindow(ShowWindowCommands nCmdShow)
{
    return m_pNativeWindow->ShowWindow(nCmdShow);
}

void WindowBase::ShowModalFake()
{
    NativeWindow* pNativeWindow = GetParentWindow() != nullptr ? GetParentWindow()->NativeWnd() : nullptr;
    m_pNativeWindow->ShowModalFake(pNativeWindow);
}

bool WindowBase::IsFakeModal() const
{
    return m_pNativeWindow->IsFakeModal();
}

bool WindowBase::IsDoModal() const
{
    return m_pNativeWindow->IsDoModal();
}

void WindowBase::CenterWindow()
{
    m_pNativeWindow->CenterWindow();
}

void WindowBase::SetWindowAlwaysOnTop(bool bOnTop)
{
    m_pNativeWindow->SetWindowAlwaysOnTop(bOnTop);
}

bool WindowBase::IsWindowAlwaysOnTop() const
{
    return m_pNativeWindow->IsWindowAlwaysOnTop();
}

bool WindowBase::SetWindowForeground()
{
    return m_pNativeWindow->SetWindowForeground();
}

bool WindowBase::IsWindowForeground() const
{
    return m_pNativeWindow->IsWindowForeground();
}

bool WindowBase::SetWindowFocus()
{
    return m_pNativeWindow->SetWindowFocus();
}

bool WindowBase::KillWindowFocus()
{
    return m_pNativeWindow->KillWindowFocus();
}

bool WindowBase::IsWindowFocused() const
{
    return m_pNativeWindow->IsWindowFocused();
}

void WindowBase::CheckSetWindowFocus()
{
    return m_pNativeWindow->CheckSetWindowFocus();
}

bool WindowBase::EnterFullscreen()
{
    return m_pNativeWindow->EnterFullscreen();
}

bool WindowBase::ExitFullscreen()
{
    return m_pNativeWindow->ExitFullscreen();
}

bool WindowBase::IsWindowMaximized() const
{
    return m_pNativeWindow->IsWindowMaximized();
}

bool WindowBase::IsWindowMinimized() const
{
    return m_pNativeWindow->IsWindowMinimized();
}

bool WindowBase::IsWindowFullscreen() const
{
    return m_pNativeWindow->IsWindowFullscreen();
}

bool WindowBase::EnableWindow(bool bEnable)
{
    return m_pNativeWindow->EnableWindow(bEnable);
}

bool WindowBase::IsWindowEnabled() const
{
    return m_pNativeWindow->IsWindowEnabled();
}

bool WindowBase::IsWindowVisible() const
{
    return m_pNativeWindow->IsWindowVisible();
}

bool WindowBase::SetWindowPos(const InsertAfterWnd& insertAfter, int32_t X, int32_t Y, int32_t cx, int32_t cy, uint32_t uFlags)
{
    NativeWindow* pNativeWindow = insertAfter.m_pWindow != nullptr ? insertAfter.m_pWindow->NativeWnd() : nullptr;
    return m_pNativeWindow->SetWindowPos(pNativeWindow, insertAfter.m_hwndFlag, X, Y, cx, cy, uFlags);
}

bool WindowBase::MoveWindow(int32_t X, int32_t Y, int32_t nWidth, int32_t nHeight, bool bRepaint)
{
    return m_pNativeWindow->MoveWindow(X, Y, nWidth, nHeight, bRepaint);
}

UiRect WindowBase::GetWindowPos(bool bContainShadow) const
{
    ASSERT(IsWindow());
    UiRect rcPos;
    GetWindowRect(rcPos);
    if (!bContainShadow) {
        UiPadding rcShadow;
        GetCurrentShadowCorner(rcShadow);
        rcPos.Deflate(rcShadow);
    }
    return rcPos;
}

void WindowBase::Resize(int cx, int cy, bool bContainShadow, bool bNeedDpiScale)
{
    ASSERT(cx >= 0 && cy >= 0);
    if (cx < 0) {
        cx = 0;
    }
    if (cy < 0) {
        cy = 0;
    }
    if (bNeedDpiScale) {
        Dpi().ScaleWindowSize(cy);
        Dpi().ScaleWindowSize(cx);
    }

    if (!bContainShadow) {
        UiPadding rcShadow;
        GetShadowCorner(rcShadow);
        Dpi().ScaleWindowSize(rcShadow.left);
        Dpi().ScaleWindowSize(rcShadow.top);
        Dpi().ScaleWindowSize(rcShadow.right);
        Dpi().ScaleWindowSize(rcShadow.bottom);
        cx += rcShadow.left + rcShadow.right;
        cy += rcShadow.top + rcShadow.bottom;
    }
    ASSERT(IsWindow());
    WindowBase::SetWindowPos(InsertAfterWnd(), 0, 0, cx, cy, kSWP_NOZORDER | kSWP_NOMOVE | kSWP_NOACTIVATE);
}

bool WindowBase::SetWindowIcon(const FilePath& iconFilePath)
{
    return m_pNativeWindow->SetWindowIcon(iconFilePath);
}

bool WindowBase::SetWindowIcon(const std::vector<uint8_t>& iconFileData, const DString& iconFileName)
{
    return m_pNativeWindow->SetWindowIcon(iconFileData, iconFileName);
}

void WindowBase::SetText(const DString& strText)
{
    m_pNativeWindow->SetText(strText);
}

DString WindowBase::GetText() const
{
    return m_pNativeWindow->GetText();
}

void WindowBase::SetTextId(const DString& strTextId)
{
    ASSERT(IsWindow());
    m_textId = strTextId;
    m_pNativeWindow->SetText(GlobalManager::Instance().Lang().GetStringViaID(strTextId));  
}

const DString& WindowBase::GetTextId() const
{
    return m_textId;
}

const DString& WindowBase::GetWindowId() const
{
    return m_windowId;
}

//Generate a 12-digit random string
static std::string generate_12digit_random()
{
    std::random_device rd;
    std::mt19937_64 generator(rd());
    std::uniform_int_distribution<int64_t> distribution(
        100000000000,  // Minimum 12-digit number
        999999999999   // Maximum 12-digit number
    );

    return std::to_string(distribution(generator));
}

void WindowBase::SetWindowId(const DString& windowId)
{
    m_windowId = windowId;
    if (m_windowId.empty()) {
        //Randomly generate an ID
        m_windowId = StringConvert::UTF8ToT(generate_12digit_random());
    }
}

const DString& WindowBase::GetWindowClassName() const
{
    return m_windowClassName;
}

const DpiManager& WindowBase::Dpi() const
{
    return (m_dpi != nullptr) ? *m_dpi : GlobalManager::Instance().Dpi();
}

bool WindowBase::ChangeDisplayScale(uint32_t nNewDisplayScaleFactor, bool bDisableDpiAware)
{
    ASSERT(IsWindow());
    if (!IsWindow()) {
        return false;
    }
    //The DPI scale ratio is limited to between 60 and 300 (when it is less than 50, problems may occur; for example, a value of 1 may become 0 after DPI conversion, causing many logic failures)
    const uint32_t nDisplayScaleFactorMin = (uint32_t)(DUI_DISPLAY_SCALE_MIN * 100 + 0.5f);
    const uint32_t nDisplayScaleFactorMax = (uint32_t)(DUI_DISPLAY_SCALE_MAX * 100 + 0.5f);
    ASSERT((nNewDisplayScaleFactor >= nDisplayScaleFactorMin) && (nNewDisplayScaleFactor <= nDisplayScaleFactorMax)) ;
    if ((nNewDisplayScaleFactor < nDisplayScaleFactorMin) || (nNewDisplayScaleFactor > nDisplayScaleFactorMax)) {
        return false;
    }

    uint32_t nOldScaleFactor = Dpi().GetDisplayScaleFactor();
    if (m_dpi == nullptr) {
        m_dpi = std::make_unique<DpiManager>();
    }
    //Update the window's DPI value to the new value
    m_dpi->SetDisplayScale(nNewDisplayScaleFactor / 100.0f, m_dpi->GetPixelDensity());

    //Mark as user-defined DPI; it no longer follows the screen DPI changes
    if (bDisableDpiAware) {
        m_dpi->SetUserDefinedDpi(true);
    }

    uint32_t nNewScaleFactor = m_dpi->GetDisplayScaleFactor();

    //Update the window layout according to the new DPI
    OnDisplayScaleChanged(nOldScaleFactor, nNewScaleFactor);
    OnWindowDisplayScaleChanged(nOldScaleFactor, nNewScaleFactor);

    //Update the window size: it needs to be scaled in proportion to the display scale, but does not need to be processed by the window pixel density
    if (nNewScaleFactor != nOldScaleFactor) {
        UiRect rcOldWindow;
        GetWindowRect(rcOldWindow);
        int32_t cx = Dpi().GetScaleInt(rcOldWindow.Width(), nOldScaleFactor);
        int32_t cy = Dpi().GetScaleInt(rcOldWindow.Height(), nOldScaleFactor);
        m_pNativeWindow->MoveWindow(rcOldWindow.left, rcOldWindow.top, cx, cy, true);
    }

    //Redraw the window content
    UiRect rcClient;
    GetClientRect(rcClient);
    Invalidate(rcClient);
    return true;
}

void WindowBase::OnProcessDisplayScaleChangedMsg(float fNewDisplayScale, float fNewPixelDensity)
{
    if ((fNewDisplayScale < 0.9999f) || (fNewPixelDensity < 0.9999f)) {
        //Invalid value
        return;
    }
    //Update the DPI value of the global DPI manager
    float fMonitorDisplayScale = MonitorUtil::GetPrimaryMonitorDisplayScale();
    DpiManager& dpi = GlobalManager::Instance().Dpi();
    if (!dpi.IsUserDefinedDpi() && dpi.IsDpiAware() && !IsFloatEqual(fMonitorDisplayScale, dpi.GetDisplayScale())) {
        dpi.SetDisplayScale(fMonitorDisplayScale, dpi.GetPixelDensity());
    }

    //This message must be handled, otherwise the ratio between the window size and the interface will be distorted
    uint32_t nOldScaleFactor = 0;
    uint32_t nNewScaleFactor = 0;
    if (m_dpi != nullptr) {
        nOldScaleFactor = m_dpi->GetDisplayScaleFactor();
        nNewScaleFactor = nOldScaleFactor;
    }
    if ((m_dpi != nullptr) && !m_dpi->IsUserDefinedDpi() && m_dpi->IsDpiAware()) {
        if (!m_dpi->IsDisplayScaleChanged(fNewDisplayScale, fNewPixelDensity)) {
            //No change; do not handle
            return;
        }
        //Update the window's DPI scale ratio to the new value
        nOldScaleFactor = m_dpi->GetDisplayScaleFactor();
        m_dpi->SetDisplayScale(fNewDisplayScale, fNewPixelDensity);
        nNewScaleFactor = m_dpi->GetDisplayScaleFactor();

        //Update the window layout according to the new DPI
        if (nOldScaleFactor != nNewScaleFactor) {
            OnDisplayScaleChanged(nOldScaleFactor, nNewScaleFactor);            
        }
    }

    //This callback must notify the application layer regardless of whether it changed
    OnWindowDisplayScaleChanged(nOldScaleFactor, nNewScaleFactor);
}

void WindowBase::OnDisplayScaleChanged(uint32_t nOldScaleFactor, uint32_t nNewScaleFactor)
{
    if ((nOldScaleFactor == nNewScaleFactor) || (nNewScaleFactor == 0)) {
        return;
    }
    if (!Dpi().CheckDisplayScaleFactor(nNewScaleFactor)) {
        return;
    }
    UiSize szMinWindow = NativeWnd()->GetWindowMinimumSize();
    szMinWindow = Dpi().GetScaleSize(szMinWindow, nOldScaleFactor);
    NativeWnd()->SetWindowMinimumSize(szMinWindow);

    UiSize szMaxWindow = NativeWnd()->GetWindowMaximumSize();
    szMaxWindow = Dpi().GetScaleSize(szMaxWindow, nOldScaleFactor);
    NativeWnd()->SetWindowMaximumSize(szMaxWindow);

    m_rcSizeBox = Dpi().GetScaleRect(m_rcSizeBox, nOldScaleFactor);
    m_szRoundCorner = Dpi().GetScaleSize(m_szRoundCorner, nOldScaleFactor);
    m_rcCaption = Dpi().GetScaleRect(m_rcCaption, nOldScaleFactor);
}

bool WindowBase::NeedSetWindowRgnOnWindowResized()
{
    if (IsChildWindow()) {
        //Child windows do not set the RGN automatically
        return false;
    }
    return true;
}

bool WindowBase::SetWindowRoundRectRgn(const UiRect& rcWnd, float rx, float ry, bool bRedraw)
{
    return m_pNativeWindow->SetWindowRoundRectRgn(rcWnd, rx, ry, bRedraw);
}

bool WindowBase::SetWindowRectRgn(const UiRect& rcWnd, bool bRedraw)
{
    return m_pNativeWindow->SetWindowRectRgn(rcWnd, bRedraw);
}

void WindowBase::ClearWindowRgn(bool bRedraw)
{
    m_pNativeWindow->ClearWindowRgn(bRedraw);
}

void WindowBase::Invalidate(const UiRect& rcItem)
{
    GlobalManager::Instance().AssertUIThread();
    m_pNativeWindow->Invalidate(rcItem);
}

bool WindowBase::UpdateWindow() const
{
    return m_pNativeWindow->UpdateWindow();
}

void WindowBase::KeepParentActive()
{
    m_pNativeWindow->KeepParentActive();
}

const UiRect& WindowBase::GetSizeBox() const
{
    return m_rcSizeBox;
}

void WindowBase::SetSizeBox(const UiRect& rcSizeBox, bool bNeedDpiScale)
{
    m_rcSizeBox = rcSizeBox;
    if (bNeedDpiScale) {
        Dpi().ScaleRect(m_rcSizeBox);
    }
    if (m_rcSizeBox.left < 0) {
        m_rcSizeBox.left = 0;
    }
    if (m_rcSizeBox.top < 0) {
        m_rcSizeBox.top = 0;
    }
    if (m_rcSizeBox.right < 0) {
        m_rcSizeBox.right = 0;
    }
    if (m_rcSizeBox.bottom < 0) {
        m_rcSizeBox.bottom = 0;
    }
}

const UiRect& WindowBase::GetCaptionRect() const
{
    return m_rcCaption;
}

void WindowBase::SetCaptionRect(const UiRect& rcCaption, bool bNeedDpiScale)
{
    m_rcCaption = rcCaption;
    if (bNeedDpiScale) {
        Dpi().ScaleRect(m_rcCaption);
    }
}

const UiRect& WindowBase::GetSysMenuRect() const
{
    return m_rcSysMenuRect;
}

void WindowBase::SetSysMenuRect(const UiRect& rcSysMenuRect, bool bNeedDpiScale)
{
    m_rcSysMenuRect = rcSysMenuRect;
    if (bNeedDpiScale) {
        Dpi().ScaleRect(m_rcSysMenuRect);
    }
}

void WindowBase::SetEnableSnapLayoutMenu(bool bEnable)
{
    m_pNativeWindow->SetEnableSnapLayoutMenu(bEnable);
}

bool WindowBase::IsEnableSnapLayoutMenu() const
{
    return m_pNativeWindow->IsEnableSnapLayoutMenu();
}

void WindowBase::SetEnableSysMenu(bool bEnable)
{
    m_pNativeWindow->SetEnableSysMenu(bEnable);
}

bool WindowBase::IsEnableSysMenu() const
{
    return m_pNativeWindow->IsEnableSysMenu();
}

const UiSize& WindowBase::GetRoundCorner() const
{
    return m_szRoundCorner;
}

void WindowBase::SetRoundCorner(int cx, int cy, bool bNeedDpiScale)
{
    ASSERT(cx >= 0);
    ASSERT(cy >= 0);
    if ((cx < 0) || (cy < 0)) {
        return;
    }
    //The two parameters must both be 0 or both be greater than 0, otherwise the parameters are invalid
    ASSERT(((cx > 0) && (cy > 0)) || ((cx == 0) && (cy == 0)));
    if (cx == 0) {
        if (cy != 0) {
            return;
        }
    }
    else {
        if (cy == 0) {
            return;
        }
    }
    if (bNeedDpiScale) {
        Dpi().ScaleInt(cx);
        Dpi().ScaleInt(cy);
    }
    m_szRoundCorner.cx = cx;
    m_szRoundCorner.cy = cy;
}

void WindowBase::SetWindowMaximumSize(const UiSize& szMinWindow, bool bNeedDpiScale)
{
    if (bNeedDpiScale) {
        NativeWnd()->SetWindowMaximumSize(Dpi().GetScaleWindowSize(szMinWindow));
    }
    else {
        NativeWnd()->SetWindowMaximumSize(szMinWindow);
    }
}

const UiSize& WindowBase::GetWindowMaximumSize() const
{
    return NativeWnd()->GetWindowMaximumSize();
}

void WindowBase::SetWindowMinimumSize(const UiSize& szMaxWindow, bool bNeedDpiScale)
{
    if (bNeedDpiScale) {
        NativeWnd()->SetWindowMinimumSize(Dpi().GetScaleWindowSize(szMaxWindow));
    }
    else {
        NativeWnd()->SetWindowMinimumSize(szMaxWindow);
    }
}

const UiSize& WindowBase::GetWindowMinimumSize() const
{
    return NativeWnd()->GetWindowMinimumSize();
}

int32_t WindowBase::SetWindowHotKey(uint8_t wVirtualKeyCode, uint8_t wModifiers)
{
    return m_pNativeWindow->SetWindowHotKey(wVirtualKeyCode, wModifiers);
}

bool WindowBase::GetWindowHotKey(uint8_t& wVirtualKeyCode, uint8_t& wModifiers) const
{
    return m_pNativeWindow->GetWindowHotKey(wVirtualKeyCode, wModifiers);
}

bool WindowBase::RegisterHotKey(uint8_t wVirtualKeyCode, uint8_t wModifiers, int32_t id)
{
    return m_pNativeWindow->RegisterHotKey(wVirtualKeyCode, wModifiers, id);
}

bool WindowBase::UnregisterHotKey(int32_t id)
{
    return m_pNativeWindow->UnregisterHotKey(id);
}

const UiPoint& WindowBase::GetLastMousePos() const
{
    return m_pNativeWindow->GetLastMousePos();
}

void WindowBase::SetLastMousePos(const UiPoint& pt)
{
    m_pNativeWindow->SetLastMousePos(pt);
}

void* WindowBase::GetWindowHandle() const
{
    return m_pNativeWindow->GetWindowHandle();
}

#ifdef DUI_BUILD_FOR_SDL
DString WindowBase::GetVideoDriverName() const
{
    return m_pNativeWindow->GetVideoDriverName();
}

DString WindowBase::GetWindowRenderName() const
{
    return m_pNativeWindow->GetWindowRenderName();
}
#endif

void WindowBase::OnWindowSized(bool bRedraw)
{
    m_bWindowSized = true;
    //The main function of this function: set the window's RGN to implement rounded corners or right-angle corners for the window
    if (!NeedSetWindowRgnOnWindowResized()) {
        //Not supported; return immediately
        return;
    }
    if (IsUseSystemCaption() || IsWindowMinimized() || IsWindowMaximized()) {
        //When using the system title bar, or the window is minimized or maximized, disable the RGN setting
        ClearWindowRgn(bRedraw);
    }
    else {
        //In other cases
        UiSize szRoundCorner = GetRoundCorner();
        if (szRoundCorner.cx > 0 && szRoundCorner.cy > 0) {
            //The window is configured as a rounded-corner window
            UiRect rcWnd;
            GetWindowRect(rcWnd);
            rcWnd.Offset(-rcWnd.left, -rcWnd.top);
            SetWindowRoundRectRgn(rcWnd, (float)szRoundCorner.cx, (float)szRoundCorner.cy, bRedraw);
        }
        else {
            //Configured as a right-angle-corner window
            //When the RGN does not need to be set, use a rectangular RGN the same size as the window instead of the default value (because by default, the top-left and top-right corners of the window are rounded, while the bottom-left and bottom-right corners are right-angle)
            UiRect rcWnd;
            GetWindowRect(rcWnd);
            rcWnd.Offset(-rcWnd.left, -rcWnd.top);
            rcWnd.right++;
            rcWnd.bottom++;
            SetWindowRectRgn(rcWnd, bRedraw);
        }
    }
}

void WindowBase::OnNativeWindowEnterFullscreen()
{
    NotifyWindowEnterFullscreen(); //For Window subclasses to handle business logic
    OnWindowEnterFullscreen();     //For the application layer to handle business logic

    m_windowSizeState = WindowSizeState::kFullscreen;
    SendWindowEvent(kWindowEnterFullscreenMsg);
}

void WindowBase::OnNativeWindowExitFullscreen()
{
    NotifyWindowExitFullscreen();   //For Window subclasses to handle business logic
    OnWindowExitFullscreen();       //For the application layer to handle business logic
    SendWindowEvent(kWindowExitFullscreenMsg);
}

UiRect WindowBase::OnNativeGetSizeBox() const
{
    return GetSizeBox();
}

void WindowBase::OnNativeGetShadowCorner(UiPadding& rcShadow) const
{
    GetCurrentShadowCorner(rcShadow);
}

bool WindowBase::IsSystemShadowSupported() const
{
    return m_pNativeWindow->IsSystemShadowSupported();
}

bool WindowBase::SetSystemShadowType(NativeWindowShadowType nativeShadowType)
{
    return m_pNativeWindow->SetSystemShadowType(nativeShadowType);
}

NativeWindowShadowType WindowBase::GetSystemShadowType() const
{
    return m_pNativeWindow->GetSystemShadowType();
}

void WindowBase::RefreshSystemShadow()
{
    m_pNativeWindow->RefreshSystemShadow();
}

void WindowBase::ClearWindowRgnForSystemShadow()
{
    m_pNativeWindow->ClearWindowRgnForSystemShadow();
}


const DpiManager& WindowBase::OnNativeGetDpi() const
{
    return Dpi();
}

void WindowBase::OnNativeGetCaptionRect(UiRect& captionRect) const
{
    captionRect = GetCaptionRect();
}

void WindowBase::OnNativeGetSysMenuRect(UiRect& sysMenuRect) const
{
    sysMenuRect = GetSysMenuRect();
}

bool WindowBase::OnNativeIsPtInCaptionBarControl(const UiPoint& pt) const
{
    return IsPtInCaptionBarControl(pt);
}

bool WindowBase::OnNativeHasMinMaxBox(bool& bMinimizeBox, bool& bMaximizeBox) const
{
    return HasMinMaxBox(bMinimizeBox, bMaximizeBox);
}

bool WindowBase::OnNativeIsPtInMaximizeRestoreButton(const UiPoint& pt) const
{
    return IsPtInMaximizeRestoreButton(pt);
}

void WindowBase::OnNativePreCloseWindow()
{
    NativeWindow* pNativeWindow = GetParentWindow() != nullptr ? GetParentWindow()->NativeWnd() : nullptr;
    m_pNativeWindow->OnCloseModalFake(pNativeWindow);
    PreCloseWindow();
}

void WindowBase::OnNativePostCloseWindow()
{
    PostCloseWindow();
}

void WindowBase::OnNativeUseSystemCaptionBarChanged()
{
    OnUseSystemCaptionBarChanged();
}

bool WindowBase::OnNativePreparePaint()
{
    return OnPreparePaint();
}

IRender* WindowBase::OnNativeGetRender() const
{
    return GetRender();
}

Control* WindowBase::OnNativeFindControl(const UiPoint& pt) const
{
    return OnFindControl(pt);
}

void WindowBase::OnNativeDisplayResolutionChangedMsg(int32_t nColorDepth, int32_t nScreenWidth, int32_t nScreenHeight)
{
    std::weak_ptr<WeakFlag> windowFlag = GetWeakFlag();
    OnDisplayResolutionChangedMsg(nColorDepth, nScreenWidth, nScreenHeight);
    if (!windowFlag.expired()) {
        WindowDisplayResolutionData displayResolution;
        displayResolution.m_nColorDepth = nColorDepth;
        displayResolution.m_nScreenWidth = nScreenWidth;
        displayResolution.m_nScreenHeight = nScreenHeight;
        SendWindowEvent(kWindowDisplayResolutionChangedMsg, (WPARAM)&displayResolution);
    }
}

void WindowBase::OnNativeDisplayScaleChangedMsg(float fNewDisplayScale, float fNewPixelDensity)
{
    std::weak_ptr<WeakFlag> windowFlag = GetWeakFlag();
    OnDisplayScaleChangedMsg(fNewDisplayScale, fNewPixelDensity);
    if (!windowFlag.expired()) {
        WindowDisplayScaleData displayScale;
        displayScale.m_fNewDisplayScale = fNewDisplayScale;
        displayScale.m_fNewPixelDensity = fNewPixelDensity;
        SendWindowEvent(kWindowDisplayScaleChangedMsg, (WPARAM)&displayScale);
    }
}

void WindowBase::OnNativeProcessDisplayScaleChangedMsg(float fNewDisplayScale, float fNewPixelDensity)
{
    OnProcessDisplayScaleChangedMsg(fNewDisplayScale, fNewPixelDensity);
}

void WindowBase::OnNativeFinalMessage()
{
    if (IsChildWindow() || GlobalManager::Instance().Windows().HasWindowBase(this)) {
        //Send a window close event
        std::weak_ptr<WeakFlag> windowFlag = GetWeakFlag();
        WPARAM wParam = (WPARAM)GetCloseParam();
        SendWindowEvent(kWindowCloseMsg, wParam);
        if (windowFlag.expired()) {
            return;
        }
    }
    FinalMessage();
}

LRESULT WindowBase::OnNativeWindowMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
    LRESULT lResult = 0;
    std::weak_ptr<WeakFlag> windowFlag = GetWeakFlag();
    //First priority: send the message to the filters for filtering (the message handling can be intercepted by setting bHandled to true)
    for (auto filter : m_aMessageFilters) {
        if (filter == nullptr) {
            continue;
        }
        lResult = filter->FilterMessage(uMsg, wParam, lParam, bHandled);
        if (bHandled || windowFlag.expired()) {
            //After the filter handles and intercepts this message, it is no longer dispatched
            return lResult;
        }
    }

    //Second priority: dispatch to the subclass callback function (the subclass can intercept message handling by setting bHandled to true)   
    if (!bHandled && !windowFlag.expired()) {
        lResult = OnWindowMessage(uMsg, wParam, lParam, bHandled);
    }
    return lResult;
}

LRESULT WindowBase::OnNativeWindowPosChangedMsg(const NativeMsg& nativeMsg, bool& bHandled)
{
    std::weak_ptr<WeakFlag> windowFlag = GetWeakFlag();
    LRESULT lResult = OnWindowPosChangedMsg(nativeMsg, bHandled);
    if (!windowFlag.expired()) {
        SendWindowEvent(kWindowPosChangedMsg);
    }
    return lResult;
}

LRESULT WindowBase::OnNativeSizeMsg(WindowSizeType sizeType, const UiSize& newWindowSize, const NativeMsg& nativeMsg, bool& bHandled)
{
    std::weak_ptr<WeakFlag> windowFlag = GetWeakFlag();
    OnWindowSized(true);
    LRESULT lResult = OnSizeMsg(sizeType, newWindowSize, nativeMsg, bHandled);
    if (windowFlag.expired()) {
        return lResult;
    }
    SendWindowEvent(kWindowSizeMsg, (WPARAM)sizeType);
    if (windowFlag.expired()) {
        return lResult;
    }

    //When the window size changes, actively trigger a redraw (to avoid the problem of the window not being drawn in the case of a layered window)
    UiRect rcClient;
    GetClientRect(rcClient);
    Invalidate(rcClient);

    //Notify the final window state event
    if (sizeType == WindowSizeType::kSIZE_MAXIMIZED) {
        //Maximized
        if (!IsWindowFullscreen()) {
            //Not in fullscreen state
            m_windowSizeState = WindowSizeState::kMaximized;
            SendWindowEvent(kWindowMaximizedMsg);
        }
    }
    else if (sizeType == WindowSizeType::kSIZE_RESTORED) {
        //Restored
        if (m_windowSizeState != WindowSizeState::kRestored) {
            m_windowSizeState = WindowSizeState::kRestored;
            SendWindowEvent(kWindowRestoredMsg);
        }
    }
    else if (sizeType == WindowSizeType::kSIZE_MINIMIZED) {
        //Minimized
        m_windowSizeState = WindowSizeState::kMinimized;
        SendWindowEvent(kWindowMaximizedMsg);
    }
    return lResult;
}

LRESULT WindowBase::OnNativeMoveMsg(const UiPoint& ptTopLeft, const NativeMsg& nativeMsg, bool& bHandled)
{
    std::weak_ptr<WeakFlag> windowFlag = GetWeakFlag();
    LRESULT lResult = OnMoveMsg(ptTopLeft, nativeMsg, bHandled);
    if (!windowFlag.expired()) {
        SendWindowEvent(kWindowMoveMsg);
    }
    return lResult;
}

LRESULT WindowBase::OnNativeShowWindowMsg(bool bShow, const NativeMsg& nativeMsg, bool& bHandled)
{
    std::weak_ptr<WeakFlag> windowFlag = GetWeakFlag();
    LRESULT lResult = OnShowWindowMsg(bShow, nativeMsg, bHandled);
    if (!windowFlag.expired()) {
        SendWindowEvent(kWindowShowWindowMsg, bShow ? 1 : 0);
    }
    return lResult;
}

LRESULT WindowBase::OnNativePaintMsg(const UiRect& rcPaint, const NativeMsg& nativeMsg, bool& bHandled)
{
    std::weak_ptr<WeakFlag> windowFlag = GetWeakFlag();
    LRESULT lResult = OnPaintMsg(rcPaint, nativeMsg, bHandled);
    if (windowFlag.expired()) {
        return lResult;
    }
    SendWindowEvent(kWindowPaintMsg);
    if (windowFlag.expired()) {
        return lResult;
    }

    //First paint event; give one callback
    if (!IsWindowFirstShown()) {
        m_bWindowFirstShown = true;

        //Trigger the first paint event
        SendWindowEvent(kWindowFirstShown);
        if (windowFlag.expired()) {
            return lResult;
        }

        //If the window size change has not been triggered, trigger it once (set the RGN, etc.)
        if (!m_bWindowSized) {
            m_bWindowSized = true;
            OnWindowSized(false);
        }
    }
    return lResult;
}

LRESULT WindowBase::OnNativeSetFocusMsg(INativeWindow* pLostFocusWindow, const NativeMsg& nativeMsg, bool& bHandled)
{
    WindowBase* pLostFocusWindowBase = nullptr;
    if (pLostFocusWindow != nullptr) {
        pLostFocusWindowBase = dynamic_cast<WindowBase*>(pLostFocusWindow);
    }
    std::weak_ptr<WeakFlag> windowFlag = GetWeakFlag();
    LRESULT lResult = OnSetFocusMsg(pLostFocusWindowBase, nativeMsg, bHandled);
    if (!windowFlag.expired()) {
        SendWindowEvent(kWindowSetFocusMsg, (WPARAM)pLostFocusWindowBase);
    }    
    return lResult;
}

LRESULT WindowBase::OnNativeKillFocusMsg(INativeWindow* pSetFocusWindow, const NativeMsg& nativeMsg, bool& bHandled)
{
    WindowBase* pSetFocusWindowBase = nullptr;
    if (pSetFocusWindow != nullptr) {
        pSetFocusWindowBase = dynamic_cast<WindowBase*>(pSetFocusWindow);
    }
    std::weak_ptr<WeakFlag> windowFlag = GetWeakFlag();
    LRESULT lResult = OnKillFocusMsg(pSetFocusWindowBase, nativeMsg, bHandled);
    if (!windowFlag.expired()) {
        SendWindowEvent(kWindowKillFocusMsg, (WPARAM)pSetFocusWindowBase);
    }
    return lResult;
}

LRESULT WindowBase::OnNativeImeSetContextMsg(const NativeMsg& nativeMsg, bool& bHandled)
{
    return OnImeSetContextMsg(nativeMsg, bHandled);
}

LRESULT WindowBase::OnNativeImeStartCompositionMsg(const NativeMsg& nativeMsg, bool& bHandled)
{
    return OnImeStartCompositionMsg(nativeMsg, bHandled);
}

LRESULT WindowBase::OnNativeImeCompositionMsg(const NativeMsg& nativeMsg, bool& bHandled)
{
    return OnImeCompositionMsg(nativeMsg, bHandled);
}

LRESULT WindowBase::OnNativeImeEndCompositionMsg(const NativeMsg& nativeMsg, bool& bHandled)
{
    return OnImeEndCompositionMsg(nativeMsg, bHandled);
}

LRESULT WindowBase::OnNativeSetCursorMsg(const NativeMsg& nativeMsg, bool& bHandled)
{
    std::weak_ptr<WeakFlag> windowFlag = GetWeakFlag();
    LRESULT lResult = OnSetCursorMsg(nativeMsg, bHandled);
    if (!windowFlag.expired()) {
        SendWindowEvent(kWindowSetCursorMsg);
    }    
    return lResult;
}

LRESULT WindowBase::OnNativeContextMenuMsg(const UiPoint& pt, const NativeMsg& nativeMsg, bool& bHandled)
{
    return OnContextMenuMsg(pt, nativeMsg, bHandled);
}

LRESULT WindowBase::OnNativeKeyDownMsg(VirtualKeyCode vkCode, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled)
{
    std::weak_ptr<WeakFlag> windowFlag = GetWeakFlag();
    LRESULT lResult = OnKeyDownMsg(vkCode, modifierKey, nativeMsg, bHandled);
    if (windowFlag.expired()) {
        return lResult;
    }
    if (!m_windowEventMap.empty()) {
        EventArgs msg;
        msg.SetSenderWeakFlag(GetWeakFlag());
        msg.eventType = kWindowKeyDownMsg;
        msg.vkCode = vkCode;
        msg.modifierKey = modifierKey;
        SendWindowEvent(msg);
    }
    return lResult;
}

LRESULT WindowBase::OnNativeKeyUpMsg(VirtualKeyCode vkCode, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled)
{
    std::weak_ptr<WeakFlag> windowFlag = GetWeakFlag();
    LRESULT lResult = OnKeyUpMsg(vkCode, modifierKey, nativeMsg, bHandled);
    if (windowFlag.expired()) {
        return lResult;
    }
    if (!m_windowEventMap.empty()) {
        EventArgs msg;
        msg.SetSenderWeakFlag(GetWeakFlag());
        msg.eventType = kWindowKeyUpMsg;
        msg.vkCode = vkCode;
        msg.modifierKey = modifierKey;
        SendWindowEvent(msg);
    }
    return lResult;
}

LRESULT WindowBase::OnNativeCharMsg(VirtualKeyCode vkCode, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled)
{
    return OnCharMsg(vkCode, modifierKey, nativeMsg, bHandled);
}

LRESULT WindowBase::OnNativeHotKeyMsg(int32_t hotkeyId, VirtualKeyCode vkCode, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled)
{
    return OnHotKeyMsg(hotkeyId, vkCode, modifierKey, nativeMsg, bHandled);
}

LRESULT WindowBase::OnNativeMouseWheelMsg(int32_t wheelDelta, const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled)
{
    std::weak_ptr<WeakFlag> windowFlag = GetWeakFlag();
    LRESULT lResult = OnMouseWheelMsg(wheelDelta, pt, modifierKey, nativeMsg, bHandled);
    if (windowFlag.expired()) {
        return lResult;
    }
    if (!m_windowEventMap.empty()) {
        EventArgs msg;
        msg.SetSenderWeakFlag(GetWeakFlag());
        msg.eventType = kWindowMouseWheelMsg;
        msg.ptMouse = pt;
        msg.modifierKey = modifierKey;
        msg.eventData = wheelDelta;
        SendWindowEvent(msg);
    }
    return lResult;
}

LRESULT WindowBase::OnNativeMouseMoveMsg(const UiPoint& pt, uint32_t modifierKey, bool bFromNC, const NativeMsg& nativeMsg, bool& bHandled)
{
    std::weak_ptr<WeakFlag> windowFlag = GetWeakFlag();
    LRESULT lResult = OnMouseMoveMsg(pt, modifierKey, bFromNC, nativeMsg, bHandled);
    if (!windowFlag.expired()) {
        SendWindowMouseEvent(kWindowMouseMoveMsg, pt, modifierKey);
    }    
    return lResult;
}

LRESULT WindowBase::OnNativeMouseHoverMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled)
{
    std::weak_ptr<WeakFlag> windowFlag = GetWeakFlag();
    LRESULT lResult = OnMouseHoverMsg(pt, modifierKey, nativeMsg, bHandled);
    if (!windowFlag.expired()) {
        SendWindowMouseEvent(kWindowMouseHoverMsg, pt, modifierKey);
    }
    return lResult;
}

LRESULT WindowBase::OnNativeMouseLeaveMsg(const NativeMsg& nativeMsg, bool& bHandled)
{
    std::weak_ptr<WeakFlag> windowFlag = GetWeakFlag();
    LRESULT lResult = OnMouseLeaveMsg(nativeMsg, bHandled);
    if (!windowFlag.expired()) {
        SendWindowEvent(kWindowMouseLeaveMsg);
    }
    return lResult;
}

LRESULT WindowBase::OnNativeMouseLButtonDownMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled)
{
    std::weak_ptr<WeakFlag> windowFlag = GetWeakFlag();
    LRESULT lResult = OnMouseLButtonDownMsg(pt, modifierKey, nativeMsg, bHandled);
    if (!windowFlag.expired()) {
        SendWindowMouseEvent(kWindowLButtonDownMsg, pt, modifierKey);
    }
    return lResult;
}

LRESULT WindowBase::OnNativeMouseLButtonUpMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled)
{
    std::weak_ptr<WeakFlag> windowFlag = GetWeakFlag();
    LRESULT lResult = OnMouseLButtonUpMsg(pt, modifierKey, nativeMsg, bHandled);
    if (!windowFlag.expired()) {
        SendWindowMouseEvent(kWindowLButtonUpMsg, pt, modifierKey);
    }
    return lResult;
}

LRESULT WindowBase::OnNativeMouseLButtonDbClickMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled)
{
    std::weak_ptr<WeakFlag> windowFlag = GetWeakFlag();
    LRESULT lResult = OnMouseLButtonDbClickMsg(pt, modifierKey, nativeMsg, bHandled);
    if (!windowFlag.expired()) {
        SendWindowMouseEvent(kWindowLButtonDbClickMsg, pt, modifierKey);
    }
    return lResult;
}

LRESULT WindowBase::OnNativeMouseRButtonDownMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled)
{
    std::weak_ptr<WeakFlag> windowFlag = GetWeakFlag();
    LRESULT lResult = OnMouseRButtonDownMsg(pt, modifierKey, nativeMsg, bHandled);
    if (!windowFlag.expired()) {
        SendWindowMouseEvent(kWindowRButtonDownMsg, pt, modifierKey);
    }
    return lResult;
}

LRESULT WindowBase::OnNativeMouseRButtonUpMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled)
{
    std::weak_ptr<WeakFlag> windowFlag = GetWeakFlag();
    LRESULT lResult = OnMouseRButtonUpMsg(pt, modifierKey, nativeMsg, bHandled);
    if (!windowFlag.expired()) {
        SendWindowMouseEvent(kWindowRButtonUpMsg, pt, modifierKey);
    }
    return lResult;
}

LRESULT WindowBase::OnNativeMouseRButtonDbClickMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled)
{
    std::weak_ptr<WeakFlag> windowFlag = GetWeakFlag();
    LRESULT lResult = OnMouseRButtonDbClickMsg(pt, modifierKey, nativeMsg, bHandled);
    if (!windowFlag.expired()) {
        SendWindowMouseEvent(kWindowRButtonDbClickMsg, pt, modifierKey);
    }
    return lResult;
}

LRESULT WindowBase::OnNativeMouseMButtonDownMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled)
{
    std::weak_ptr<WeakFlag> windowFlag = GetWeakFlag();
    LRESULT lResult = OnMouseMButtonDownMsg(pt, modifierKey, nativeMsg, bHandled);
    if (!windowFlag.expired()) {
        SendWindowMouseEvent(kWindowMButtonDownMsg, pt, modifierKey);
    }
    return lResult;
}

LRESULT WindowBase::OnNativeMouseMButtonUpMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled)
{
    std::weak_ptr<WeakFlag> windowFlag = GetWeakFlag();
    LRESULT lResult = OnMouseMButtonUpMsg(pt, modifierKey, nativeMsg, bHandled);
    if (!windowFlag.expired()) {
        SendWindowMouseEvent(kWindowMButtonUpMsg, pt, modifierKey);
    }
    return lResult;
}

LRESULT WindowBase::OnNativeMouseMButtonDbClickMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled)
{
    std::weak_ptr<WeakFlag> windowFlag = GetWeakFlag();
    LRESULT lResult = OnMouseMButtonDbClickMsg(pt, modifierKey, nativeMsg, bHandled);
    if (!windowFlag.expired()) {
        SendWindowMouseEvent(kWindowMButtonDbClickMsg, pt, modifierKey);
    }
    return lResult;
}

LRESULT WindowBase::OnNativeCaptureChangedMsg(const NativeMsg& nativeMsg, bool& bHandled)
{
    std::weak_ptr<WeakFlag> windowFlag = GetWeakFlag();
    LRESULT lResult = OnCaptureChangedMsg(nativeMsg, bHandled);
    if (!windowFlag.expired()) {
        SendWindowEvent(kWindowCaptureChangedMsg);
    }
    return lResult;
}

LRESULT WindowBase::OnNativeWindowCloseMsg(uint32_t wParam, const NativeMsg& nativeMsg, bool& bHandled)
{
    return OnWindowCloseMsg(wParam, nativeMsg, bHandled);
}

void WindowBase::OnNativeWindowPosSnapped(bool bLeftSnap, bool bRightSnap, bool bTopSnap, bool bBottomSnap)
{
    OnWindowPosSnapped(bLeftSnap, bRightSnap, bTopSnap, bBottomSnap);
}

/** Helper function to determine whether the message has been handled
*/
static bool IsDragDropMsgHandled(ControlDropType dropType, void* pDropData)
{
    bool bHandled = false;
    if (dropType == ui::kControlDropTypeWindows) {
        const ui::ControlDropData_Windows* dropData = (const ui::ControlDropData_Windows*)pDropData;
        if (dropData != nullptr) {
            bHandled = dropData->m_bHandled;
        }
    }
    else if (dropType == ui::kControlDropTypeSDL) {
        const ui::ControlDropData_SDL* dropData = (const ui::ControlDropData_SDL*)pDropData;
        if (dropData != nullptr) {
            bHandled = dropData->m_bHandled;
        }
    }
    return bHandled;
}

void WindowBase::OnNativeDropEnterMsg(ControlDropType dropType, void* pDropData)
{
    ASSERT(!IsDragDropMsgHandled(dropType, pDropData));
    std::weak_ptr<WeakFlag> windowFlag = GetWeakFlag();
    OnDropEnterMsg(dropType, pDropData);
    if (!windowFlag.expired() && !IsDragDropMsgHandled(dropType, pDropData)) {
        m_bSendDragEnterMsg = true;
        SendWindowEvent(kWindowDropEnterMsg, (WPARAM)dropType, (LPARAM)pDropData);
    }
}
void WindowBase::OnNativeDropOverMsg(ControlDropType dropType, void* pDropData)
{
    ASSERT(!IsDragDropMsgHandled(dropType, pDropData));
    std::weak_ptr<WeakFlag> windowFlag = GetWeakFlag();
    OnDropOverMsg(dropType, pDropData);
    if (!windowFlag.expired() && !IsDragDropMsgHandled(dropType, pDropData)) {
        SendWindowEvent(kWindowDropOverMsg, (WPARAM)dropType, (LPARAM)pDropData);
    }
}

void WindowBase::OnNativeDropMsg(ControlDropType dropType, void* pDropData)
{
    ASSERT(!IsDragDropMsgHandled(dropType, pDropData));
    std::weak_ptr<WeakFlag> windowFlag = GetWeakFlag();
    OnDropMsg(dropType, pDropData);
    if (!windowFlag.expired() && !IsDragDropMsgHandled(dropType, pDropData)) {
        SendWindowEvent(kWindowDropMsg, (WPARAM)dropType, (LPARAM)pDropData);
    }
    if (!windowFlag.expired()) {
        //If the drop event has been sent, the DropLeave event is not needed
        m_bSendDragEnterMsg = false;
    }
}

void WindowBase::OnNativeDropLeaveMsg()
{
    std::weak_ptr<WeakFlag> windowFlag = GetWeakFlag();
    OnDropLeaveMsg();
    if (!windowFlag.expired()) {
        if (m_bSendDragEnterMsg) {
            //Only send Leave if Enter was sent
            m_bSendDragEnterMsg = false;
            SendWindowEvent(kWindowDropLeaveMsg);
        }
    }
}

bool WindowBase::IsWindowFirstShown() const
{
    return m_bWindowFirstShown;
}

bool WindowBase::SendWindowEvent(EventType eventType, WPARAM wParam, LPARAM lParam)
{
    if (m_windowEventMap.empty()) {
        return true;
    }
    EventArgs msg;
    msg.SetSenderWeakFlag(GetWeakFlag());
    msg.eventType = eventType;
    msg.ptMouse = GetLastMousePos();
    msg.wParam = wParam;
    msg.lParam = lParam;
    return SendWindowEvent(msg);
}

bool WindowBase::SendWindowMouseEvent(EventType eventType, const UiPoint& pt, uint32_t modifierKey)
{
    if (m_windowEventMap.empty()) {
        return true;
    }
    EventArgs msg;
    msg.SetSenderWeakFlag(GetWeakFlag());
    msg.eventType = eventType;
    msg.ptMouse = pt;
    msg.modifierKey = modifierKey;
    return SendWindowEvent(msg);
}

bool WindowBase::SendWindowEvent(const EventArgs& msg)
{
    if (!m_windowEventMap.empty()) {
        auto callback = m_windowEventMap.find(msg.eventType);
        if (callback != m_windowEventMap.end()) {
            callback->second(msg);
        }
    }
    return true;
}

bool WindowBase::HasWindowEventCallback(EventType eventType) const
{
    return m_windowEventMap.find(eventType) != m_windowEventMap.end();
}

bool WindowBase::HasWindowEventCallbackByID(EventCallbackID callbackID) const
{
    for (auto iter = m_windowEventMap.begin(); iter != m_windowEventMap.end(); ++iter) {
        if (iter->second.HasEventCallbackByID(callbackID)) {
            return true;
        }
    }
    return false;
}

void WindowBase::DetachWindowEventCallback(EventType eventType)
{
    auto iter = m_windowEventMap.find(eventType);
    if (iter != m_windowEventMap.end()) {
        m_windowEventMap.erase(iter);
    }
}

void WindowBase::DetachWindowEventCallbackByID(EventCallbackID callbackID)
{
    EventUtils::RemoveEventCallbackByID(m_windowEventMap, callbackID);
}

void WindowBase::AttachWindowCreateMsg(const EventCallback& callback, EventCallbackID callbackID)
{
    m_windowEventMap[kWindowCreateMsg].AddEventCallback(callback, callbackID);
}

void WindowBase::AttachWindowCloseMsg(const EventCallback& callback, EventCallbackID callbackID)
{
    m_windowEventMap[kWindowCloseMsg].AddEventCallback(callback, callbackID);
}

void WindowBase::AttachWindowFirstShown(const EventCallback& callback, EventCallbackID callbackID)
{
    ASSERT(!IsWindowFirstShown());
    m_windowEventMap[kWindowFirstShown].AddEventCallback(callback, callbackID);
}

void WindowBase::AttachWindowEnterFullscreenMsg(const EventCallback& callback, EventCallbackID callbackID)
{
    m_windowEventMap[kWindowEnterFullscreenMsg].AddEventCallback(callback, callbackID);
}

void WindowBase::AttachWindowExitFullscreenMsg(const EventCallback& callback, EventCallbackID callbackID)
{
    m_windowEventMap[kWindowExitFullscreenMsg].AddEventCallback(callback, callbackID);
}

void WindowBase::AttachWindowMaximizedMsg(const EventCallback& callback, EventCallbackID callbackID)
{
    m_windowEventMap[kWindowMaximizedMsg].AddEventCallback(callback, callbackID);
}

void WindowBase::AttachWindowMinimizedMsg(const EventCallback& callback, EventCallbackID callbackID)
{
    m_windowEventMap[kWindowMinimizedMsg].AddEventCallback(callback, callbackID);
}

void WindowBase::AttachWindowRestoredMsg(const EventCallback& callback, EventCallbackID callbackID)
{
    m_windowEventMap[kWindowRestoredMsg].AddEventCallback(callback, callbackID);
}

void WindowBase::AttachWindowPosChangedMsg(const EventCallback& callback, EventCallbackID callbackID)
{
    m_windowEventMap[kWindowPosChangedMsg].AddEventCallback(callback, callbackID);
}

void WindowBase::AttachWindowSizeMsg(const EventCallback& callback, EventCallbackID callbackID)
{
    m_windowEventMap[kWindowSizeMsg].AddEventCallback(callback, callbackID);
}

void WindowBase::AttachWindowMoveMsg(const EventCallback& callback, EventCallbackID callbackID)
{
    m_windowEventMap[kWindowMoveMsg].AddEventCallback(callback, callbackID);
}

void WindowBase::AttachWindowShowWindowMsg(const EventCallback& callback, EventCallbackID callbackID)
{
    m_windowEventMap[kWindowShowWindowMsg].AddEventCallback(callback, callbackID);
}

void WindowBase::AttachWindowPaintMsg(const EventCallback& callback, EventCallbackID callbackID)
{
    m_windowEventMap[kWindowPaintMsg].AddEventCallback(callback, callbackID);
}

void WindowBase::AttachWindowSetFocusMsg(const EventCallback& callback, EventCallbackID callbackID)
{
    m_windowEventMap[kWindowSetFocusMsg].AddEventCallback(callback, callbackID);
}

void WindowBase::AttachWindowKillFocusMsg(const EventCallback& callback, EventCallbackID callbackID)
{
    m_windowEventMap[kWindowKillFocusMsg].AddEventCallback(callback, callbackID);
}

void WindowBase::AttachWindowSetCursorMsg(const EventCallback& callback, EventCallbackID callbackID)
{
    m_windowEventMap[kWindowSetCursorMsg].AddEventCallback(callback, callbackID);
}

void WindowBase::AttachWindowKeyDownMsg(const EventCallback& callback, EventCallbackID callbackID)
{
    m_windowEventMap[kWindowKeyDownMsg].AddEventCallback(callback, callbackID);
}

void WindowBase::AttachWindowKeyUpMsg(const EventCallback& callback, EventCallbackID callbackID)
{
    m_windowEventMap[kWindowKeyUpMsg].AddEventCallback(callback, callbackID);
}

void WindowBase::AttachWindowMouseWheelMsg(const EventCallback& callback, EventCallbackID callbackID)
{
    m_windowEventMap[kWindowMouseWheelMsg].AddEventCallback(callback, callbackID);
}

void WindowBase::AttachWindowMouseMoveMsg(const EventCallback& callback, EventCallbackID callbackID)
{
    m_windowEventMap[kWindowMouseMoveMsg].AddEventCallback(callback, callbackID);
}

void WindowBase::AttachWindowMouseHoverMsg(const EventCallback& callback, EventCallbackID callbackID)
{
    m_windowEventMap[kWindowMouseHoverMsg].AddEventCallback(callback, callbackID);
}

void WindowBase::AttachWindowMouseLeaveMsg(const EventCallback& callback, EventCallbackID callbackID)
{
    m_windowEventMap[kWindowMouseLeaveMsg].AddEventCallback(callback, callbackID);
}

void WindowBase::AttachWindowLButtonDownMsg(const EventCallback& callback, EventCallbackID callbackID)
{
    m_windowEventMap[kWindowLButtonDownMsg].AddEventCallback(callback, callbackID);
}

void WindowBase::AttachWindowLButtonUpMsg(const EventCallback& callback, EventCallbackID callbackID)
{
    m_windowEventMap[kWindowLButtonUpMsg].AddEventCallback(callback, callbackID);
}

void WindowBase::AttachWindowLButtonDbClickMsg(const EventCallback& callback, EventCallbackID callbackID)
{
    m_windowEventMap[kWindowLButtonDbClickMsg].AddEventCallback(callback, callbackID);
}

void WindowBase::AttachWindowRButtonDownMsg(const EventCallback& callback, EventCallbackID callbackID)
{
    m_windowEventMap[kWindowRButtonDownMsg].AddEventCallback(callback, callbackID);
}

void WindowBase::AttachWindowRButtonUpMsg(const EventCallback& callback, EventCallbackID callbackID)
{
    m_windowEventMap[kWindowRButtonUpMsg].AddEventCallback(callback, callbackID);
}

void WindowBase::AttachWindowRButtonDbClickMsg(const EventCallback& callback, EventCallbackID callbackID)
{
    m_windowEventMap[kWindowRButtonDbClickMsg].AddEventCallback(callback, callbackID);
}

void WindowBase::AttachWindowMButtonDownMsg(const EventCallback& callback, EventCallbackID callbackID)
{
    m_windowEventMap[kWindowMButtonDownMsg].AddEventCallback(callback, callbackID);
}

void WindowBase::AttachWindowMButtonUpMsg(const EventCallback& callback, EventCallbackID callbackID)
{
    m_windowEventMap[kWindowMButtonUpMsg].AddEventCallback(callback, callbackID);
}

void WindowBase::AttachWindowMButtonDbClickMsg(const EventCallback& callback, EventCallbackID callbackID)
{
    m_windowEventMap[kWindowMButtonDbClickMsg].AddEventCallback(callback, callbackID);
}

void WindowBase::AttachWindowCaptureChangedMsg(const EventCallback& callback, EventCallbackID callbackID)
{
    m_windowEventMap[kWindowCaptureChangedMsg].AddEventCallback(callback, callbackID);
}

void WindowBase::AttachWindowDropEnterMsg(const EventCallback& callback, EventCallbackID callbackID)
{
    m_windowEventMap[kWindowDropEnterMsg].AddEventCallback(callback, callbackID);
}

void WindowBase::AttachWindowDropOverMsg(const EventCallback& callback, EventCallbackID callbackID)
{
    m_windowEventMap[kWindowDropOverMsg].AddEventCallback(callback, callbackID);
}

void WindowBase::AttachWindowDropMsg(const EventCallback& callback, EventCallbackID callbackID)
{
    m_windowEventMap[kWindowDropMsg].AddEventCallback(callback, callbackID);
}

void WindowBase::AttachWindowDropLeaveMsg(const EventCallback& callback, EventCallbackID callbackID)
{
    m_windowEventMap[kWindowDropLeaveMsg].AddEventCallback(callback, callbackID);
}

} // namespace ui
