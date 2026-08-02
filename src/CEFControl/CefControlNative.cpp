#include "duilib/CEFControl/CefControlNative.h"

#ifdef DUILIB_BUILD_FOR_CEF

#include "duilib/CEFControl/CefWindowUtils.h"
#include "duilib/CEFControl/internal/CefBrowserHandler.h"

#include "duilib/Core/Box.h"
#include "duilib/Core/GlobalManager.h"

namespace ui {

CefControlNative::CefControlNative(ui::Window* pWindow):
    CefControl(pWindow),
    m_bWindowFirstShown(false),
    m_bSetCefWindowParentNull(false),
    m_bInGotFocusEvent(false)
{
}

CefControlNative::~CefControlNative(void)
{
    DoCloseAllNativeBrowsers(true);
    if (m_pBrowserHandler.get()) {
        m_pBrowserHandler->SetHostWindow(nullptr);
        m_pBrowserHandler->SetHandlerDelegate(nullptr);
    }
}

void CefControlNative::Init()
{
    GlobalManager::Instance().AssertUIThread();
    if (m_pBrowserHandler.get() == nullptr) {
#ifdef DUILIB_BUILD_FOR_WIN
        //Check whether the control is being created in a layered window
        HWND hWnd = GetWindow()->NativeWnd()->GetHWND();
        LONG style = ::GetWindowLong(hWnd, GWL_STYLE);
        ::SetWindowLong(hWnd, GWL_STYLE, style | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
        ASSERT((::GetWindowLong(hWnd, GWL_EXSTYLE) & WS_EX_LAYERED) == 0 && _T("CefControlNative: cannot be used in a layered window"));
#endif
        m_pBrowserHandler = new CefBrowserHandler;
        m_pBrowserHandler->SetHostWindow(GetWindow());
        m_pBrowserHandler->SetHandlerDelegate(this);

        //Create the Browser object asynchronously to avoid blocking the parsing and display of the main UI
        GlobalManager::Instance().Thread().PostTask(ui::kThreadUI, UiBind(&CefControlNative::ReCreateBrowser, this));
    }

    if (!m_jsBridge.get()) {
        m_jsBridge.reset(new ui::CefJSBridge);
    }
    BaseClass::Init();
}

void CefControlNative::ReCreateBrowser()
{
    GlobalManager::Instance().AssertUIThread();
    Window* pWindow = GetWindow();
    ASSERT(pWindow != nullptr);
    if (pWindow == nullptr) {
        return;
    }
    ASSERT(pWindow->IsWindow());
    ASSERT(m_pBrowserHandler != nullptr);
    if (m_pBrowserHandler == nullptr) {
        return;
    }
    if (m_pBrowserHandler->GetBrowser() != nullptr) {
        //Already created; do not create the Browser object again
        return;
    }

    //If the window has not completed its first display, wait until it is first shown to create it, to avoid a black screen flash
    if (!pWindow->IsWindowFirstShown()) {
        if (!m_bWindowFirstShown) {
            std::weak_ptr<WeakFlag> weakFlag = GetWeakFlag();
            pWindow->AttachWindowFirstShown([weakFlag, this](const EventArgs& /*args*/) {
                if (!weakFlag.expired()) {
                    ReCreateBrowser();
                }
                return true;
                });
            m_bWindowFirstShown = true;
        }
        return;
    }

    // Use windowed mode
    CefWindowInfo window_info;
    //This parameter must be explicitly initialized; the default value cannot be used
#if CEF_VERSION_MAJOR > 109
    window_info.runtime_style = CEF_RUNTIME_STYLE_ALLOY;
#endif
    UiRect rc = GetRect();
    Dpi().ClientSizeToWindowSize(rc);
    CefRect rect = { rc.left, rc.top, rc.right, rc.bottom};
#ifdef DUILIB_BUILD_FOR_WIN
    //Windows
    window_info.SetAsChild(pWindow->NativeWnd()->GetHWND(), rect);
#elif defined (DUILIB_BUILD_FOR_LINUX) || defined (DUILIB_BUILD_FOR_FREEBSD)
    //Linux
    CefWindowHandle hParenWindow = (CefWindowHandle)pWindow->NativeWnd()->GetX11WindowNumber();
    if (pWindow->NativeWnd()->IsVideoDriverWayland()) {
        hParenWindow = (CefWindowHandle)pWindow->NativeWnd()->GetWaylandDisplayPointer();
    }
    window_info.SetAsChild(hParenWindow, rect);
#elif defined DUILIB_BUILD_FOR_MACOS
    //MacOS
    window_info.SetAsChild(pWindow->NativeWnd()->GetNSView(), rect);
#endif

    CefBrowserSettings browser_settings;
    CefString url = GetInitURL();//URL to load immediately after creation succeeds
    CefBrowserHost::CreateBrowser(window_info, m_pBrowserHandler, url, browser_settings, nullptr, nullptr);
}

void CefControlNative::SetPos(ui::UiRect rc)
{
    GlobalManager::Instance().AssertUIThread();
    BaseClass::SetPos(rc);

    //Update the position of the page child window
    SetCefWindowPos(GetCefWindowHandle(), this);
}

void CefControlNative::OnGotFocus()
{
    if (!IsVisible() || !IsEnabled()) {
        return;
    }

    Window* pWindow = GetWindow();
    if (pWindow != nullptr) {
        //When the page gains focus, disable text input on the main UI (fixes: on macOS, typing in the page triggers multiple inputs per key press; the input is likely triggered again internally by SDL)
        pWindow->NativeWnd()->SetImeOpenStatus(false);
        pWindow->NativeWnd()->SetTextInputArea(nullptr, 0);
    }

    if (!IsFocused()) {
        //Avoid the appearance of a dual-focus control
        m_bInGotFocusEvent = true;
        SetFocus();
        m_bInGotFocusEvent = false;
    }
}

bool CefControlNative::OnSetFocus(const EventArgs& msg)
{
    if (!m_bInGotFocusEvent) {
        //Avoid calling CefBrowserHost::SetFocus inside the OnGotFocus callback, which can easily cause an infinite loop
        CefRefPtr<CefBrowserHost> browserHost = GetCefBrowserHost();
        if (browserHost != nullptr) {
            browserHost->SetFocus(true);
        }
    }
    return BaseClass::OnSetFocus(msg);
}

bool CefControlNative::OnKillFocus(const EventArgs& msg)
{
    CefRefPtr<CefBrowserHost> browserHost = GetCefBrowserHost();
    if (browserHost != nullptr) {
        browserHost->SetFocus(false);
    }
    return BaseClass::OnKillFocus(msg);
}

void CefControlNative::OnSetVisible(bool bChanged)
{
    GlobalManager::Instance().AssertUIThread();
    BaseClass::OnSetVisible(bChanged);

    //Update the visibility of the page child window
    SetCefWindowVisible(GetCefWindowHandle(), this);
}

void CefControlNative::UpdateCefWindowPos()
{
    if (GetCefWindowHandle() == 0) {
        //The CEF page window has not been created yet
        return;
    }

    BaseClass::UpdateCefWindowPos();
    //After the Browser control is created, update the window position and visibility
    SetPos(GetPos());
    SetVisible(IsVisible());
}

void CefControlNative::DoCloseAllNativeBrowsers(bool bForceClose)
{
    //Remove the parent-child relationship between the CEF child window and the parent window (to avoid the parent window also being closed when it closes, which would exit the program)
    if (!m_bSetCefWindowParentNull) {
        m_bSetCefWindowParentNull = true;
        RemoveCefWindowFromParent(GetCefWindowHandle());
    }
    DoCloseAllBrowsers(bForceClose);
}

void CefControlNative::CloseAllBrowsers()
{
    DoCloseAllNativeBrowsers(true);
}

void CefControlNative::SetWindow(ui::Window* pWindow)
{
    GlobalManager::Instance().AssertUIThread();
    if ((pWindow == nullptr) || (BaseClass::GetWindow() == pWindow)) {
        return;
    }
    BaseClass::SetWindow(pWindow);

    if (m_pBrowserHandler) {
        m_pBrowserHandler->SetHostWindow(pWindow);
        m_pBrowserHandler->SetHandlerDelegate(this);
    }

    //Update the parent window of the page child window
    SetCefWindowParent(GetCefWindowHandle(), this);
}

std::shared_ptr<IBitmap> CefControlNative::MakeImageSnapshot()
{
    std::vector<uint8_t> bitmap;
    int32_t width = 0;
    int32_t height = 0;
    bool bRet = CaptureCefWindowBitmap(GetCefWindowHandle(), bitmap, width, height);
    if (bRet && (width > 0) && (height > 0) && ((int32_t)bitmap.size() == (width * height * 4))) {
        std::shared_ptr<IBitmap> spBitmap;
        IRenderFactory* pRenderFactory = GlobalManager::Instance().GetRenderFactory();
        ASSERT(pRenderFactory != nullptr);
        if (pRenderFactory != nullptr) {
            spBitmap.reset(pRenderFactory->CreateBitmap());
            if (spBitmap != nullptr) {
                if (!spBitmap->Init(width, height, bitmap.data())) {
                    spBitmap.reset();
                }
            }
        }
        return spBitmap;
    }
    return nullptr;
}

bool CefControlNative::IsCefNative() const
{
    return true;
}

} //namespace ui

#endif //DUILIB_BUILD_FOR_CEF
