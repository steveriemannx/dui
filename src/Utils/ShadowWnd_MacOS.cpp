#include "dui/Utils/ShadowWnd.h"
#include "dui/Core/Box.h"
#include "dui/Core/Shadow.h"

#if defined(DUI_BUILD_FOR_MACOS)


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

    /** Get the ID of the associated window
    */
    uint32_t GetWindowID() const
    {
        // Stub: the SDL implementation maps the native window handle to an SDL
        // window ID via SDL_GetWindowID(); that mapping is meaningless in the
        // native AppKit backend, so return the default.
        (void)m_pWindow;
        return 0;
    }
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
    return DUI_T("");
}

DString ShadowWndBase::GetSkinFile()
{
    return DUI_T("public/shadow/shadow.xml");
}

bool ShadowWndBase::Create(Window* window)
{
    m_pWindow = window;
    WindowCreateParam createParam;
    createParam.m_dwStyle = kWS_POPUP;
    createParam.m_dwExStyle = kWS_EX_TRANSPARENT | kWS_EX_LAYERED | kWS_EX_NOACTIVATE | kWS_EX_TOOLWINDOW;
    createParam.m_className = DUI_T("ShadowWnd");
    createParam.m_windowTitle = DUI_T("ShadowWnd");

    // Note: the shadow-following effect while dragging is handled by the native
    // macOS window position callbacks (see NativeWindow_MacOS) instead of the
    // SDL event watch used in the SDL implementation.
    return Window::CreateWnd(nullptr, createParam);
}

LRESULT ShadowWndBase::FilterMessage(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, bool& bHandled)
{
    // Stub: the SDL implementation reacts to SDL_EVENT_WINDOW_* events to keep the
    // shadow window positioned around the followed window. On the native macOS
    // backend, window moving/shown/hidden/minimized events arrive through the
    // AppKit event loop (MessageLoop_MacOS / NativeWindow_MacOS), so the shadow
    // positioning logic is not implemented yet.
    bHandled = false;
    return 0;
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
    // Re-entry (e.g. PreInitWindow attaches the shadow and AttachBox calls
    // AttachShadow again because the external-shadow path never sets the base
    // Shadow's shadow box): the external shadow window already covers pRoot,
    // so return it unchanged - same idempotent contract as Shadow::AttachShadow.
    if (m_pShadowWnd != nullptr) {
        return pRoot;
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
    // System shadows are provided by NSWindow. Creating the legacy external
    // shadow window as well causes duplicate shadow compositing and visible
    // darkening/flicker while popup windows are shown or closed.
    if (Shadow::IsSystemShadowType(GetShadowType())) {
        return false;
    }
    if (IsLayeredWindow()) {
        //The layered window property is set, so do not create one
        return false;
    }
    if (IsUseDefaultShadowAttached()) {
        //The ShadowAttached property is not set in the configuration file or the external interface, so create one
        return true;
    }
    else {
        //The configuration file has the setting, so follow the property in the configuration file
        return IsShadowAttached();
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
        if (IsWindowVisible()) {
            m_pShadowWnd->ShowWindow(kSW_SHOW_NA);
        }
    }
}

} // namespace ui

#endif //DUI_BUILD_FOR_MACOS
