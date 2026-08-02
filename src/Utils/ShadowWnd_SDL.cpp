#include "duilib/Utils/ShadowWnd.h"
#include "duilib/Core/Box.h"
#include "duilib/Core/Shadow.h"

#ifdef DUILIB_BUILD_FOR_SDL
#if defined(DUILIB_BUILD_FOR_SDL)
#include <SDL3/SDL.h>
#endif

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
    SDL_WindowID GetWindowID() const
    {
        if (m_pWindow != nullptr) {
            return SDL_GetWindowID((SDL_Window*)m_pWindow->GetWindowHandle());
        }
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
    return _T("");
}

DString ShadowWndBase::GetSkinFile()
{
    return _T("public/shadow/shadow.xml");
}
//
//int SDLCALL ShadowWndBaseEventFilter(void* userdata, SDL_Event* event)
//{
//    ShadowWndBase* pShadowWndBase = (ShadowWndBase*)userdata;
//    if ((event != nullptr) && (pShadowWndBase != nullptr)) {
//        if (event->type == SDL_EVENT_WINDOW_MOVING) {
//            if (event->window.windowID == pShadowWndBase->GetWindowID()) {
//                bool bHandled = false;
//                pShadowWndBase->FilterMessage(SDL_EVENT_WINDOW_MOVING, 0, 0, bHandled);
//            }
//        }
//    }
//    return 0;
//}

bool ShadowWndBase::Create(Window* window)
{
    m_pWindow = window;
    WindowCreateParam createParam;
    createParam.m_dwStyle = kWS_POPUP;
    createParam.m_dwExStyle = kWS_EX_TRANSPARENT | kWS_EX_LAYERED | kWS_EX_NOACTIVATE | kWS_EX_TOOLWINDOW;
    createParam.m_className = _T("ShadowWnd");
    createParam.m_windowTitle = _T("ShadowWnd");

    //Support the shadow-following effect when the window is moved by dragging the title bar with the mouse
    //Due to the current SDL message loop mechanism, when the window is moved by dragging the title bar with the mouse, the SDL events corresponding to the window messages are not processed by the application layer.
    //SDL_AddEventWatch(ShadowWndBaseEventFilter, this);

    return Window::CreateWnd(nullptr, createParam);
}

LRESULT ShadowWndBase::FilterMessage(UINT uMsg, WPARAM /*wParam*/, LPARAM /*lParam*/, bool& bHandled)
{
    bHandled = false;
    if ((m_pWindow == nullptr) || !m_pWindow->IsWindow()) {
        return 0;
    }
    bool bAdjustWindowPos = false;
    if (uMsg == SDL_EVENT_WINDOW_CLOSE_REQUESTED) {
        //Window closed
        ShowWindow(kSW_HIDE);
    }
    else if (uMsg == SDL_EVENT_WINDOW_SHOWN) {
        ShowWindow(kSW_SHOW_NA);
    }
    else if (uMsg == SDL_EVENT_WINDOW_HIDDEN) {
        ShowWindow(kSW_HIDE);
    }
    else if (uMsg == SDL_EVENT_WINDOW_MINIMIZED) {
        ShowWindow(kSW_HIDE);
    }
    else if ((uMsg == SDL_EVENT_WINDOW_EXPOSED) && !m_isFirstPainted) {
        bAdjustWindowPos = true;
    }
    else if ((uMsg >= SDL_EVENT_WINDOW_FIRST) && (uMsg <= SDL_EVENT_WINDOW_LAST)) {
        if (uMsg != SDL_EVENT_WINDOW_EXPOSED) {
            bAdjustWindowPos = true;
        }
    }
    if (bAdjustWindowPos) {
        if (m_pWindow->IsWindowVisible() && !m_pWindow->IsWindowMinimized()) {
            UiRect rc;
            m_pWindow->GetWindowRect(rc);
            UiPadding rcShadow;
            GetCurrentShadowCorner(rcShadow);
            Dpi().ClientSizeToWindowSize(rcShadow);
            rc.Inflate(rcShadow);
            //TODO: Since SDL has no feature to adjust the Z-order of windows, the shadow window is not positioned correctly and often stays behind other windows, so the shadow is invisible.
            SetWindowPos(InsertAfterWnd(m_pWindow), rc.left, rc.top, rc.Width(), rc.Height(), kSWP_SHOWWINDOW | kSWP_NOACTIVATE);
            if (uMsg == SDL_EVENT_WINDOW_EXPOSED) {
                m_isFirstPainted = true;
            }
        }
        else {
            ShowWindow(kSW_HIDE);
        }
    }
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

#endif //DUILIB_BUILD_FOR_WIN
