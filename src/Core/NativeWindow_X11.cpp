#include "dui/Core/NativeWindow_X11.h"
#include "dui/Core/MessageLoop_X11.h"
#include "dui/Utils/StringConvert.h"

// Inside the guard: this file is compiled on Windows and macOS too, where its whole body
// is switched off and it only has to be an empty translation unit. Outside the guard, an
// empty file still needed X11 installed to compile.
#ifdef DUI_BUILD_FOR_X11

#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <X11/Xutil.h>
struct MotifWmHints_X11 { unsigned long flags; unsigned long functions; unsigned long decorations; long inputMode; unsigned long status; };
static void SetUndecorated_X11(Display* display, Window window) { Atom property = XInternAtom(display, "_MOTIF_WM_HINTS", False); MotifWmHints_X11 hints{}; hints.flags = 1UL << 1; hints.decorations = 0; XChangeProperty(display, window, property, property, 32, PropModeReplace, reinterpret_cast<unsigned char*>(&hints), 5); }
static ui::UiRect GetWorkArea_X11(Display* display)
{
    const int screen = DefaultScreen(display);
    ui::UiRect workArea(0, 0, DisplayWidth(display, screen), DisplayHeight(display, screen));
    Atom property = XInternAtom(display, "_NET_WORKAREA", False);
    Atom actualType = None;
    int actualFormat = 0;
    unsigned long itemCount = 0;
    unsigned long bytesAfter = 0;
    unsigned char* data = nullptr;
    if (XGetWindowProperty(display, DefaultRootWindow(display), property, 0, 4, False,
                           XA_CARDINAL, &actualType, &actualFormat, &itemCount,
                           &bytesAfter, &data) == Success && data != nullptr && itemCount >= 4) {
        const unsigned long* values = reinterpret_cast<const unsigned long*>(data);
        workArea = ui::UiRect(static_cast<int32_t>(values[0]), static_cast<int32_t>(values[1]),
                              static_cast<int32_t>(values[0] + values[2]),
                              static_cast<int32_t>(values[1] + values[3]));
    }
    if (data != nullptr) XFree(data);
    return workArea;
}
static ui::VirtualKeyCode GetVirtualKeyCode_X11(KeySym sym)
{
    if (sym >= XK_0 && sym <= XK_9) return static_cast<ui::VirtualKeyCode>(ui::kVK_0 + sym - XK_0);
    if (sym >= XK_a && sym <= XK_z) return static_cast<ui::VirtualKeyCode>(ui::kVK_A + sym - XK_a);
    if (sym >= XK_A && sym <= XK_Z) return static_cast<ui::VirtualKeyCode>(ui::kVK_A + sym - XK_A);
    if (sym >= XK_F1 && sym <= XK_F24) return static_cast<ui::VirtualKeyCode>(ui::kVK_F1 + sym - XK_F1);
    switch (sym) {
    case XK_BackSpace: return ui::kVK_BACK; case XK_Tab: return ui::kVK_TAB; case XK_Return: return ui::kVK_RETURN; case XK_Escape: return ui::kVK_ESCAPE; case XK_space: return ui::kVK_SPACE;
    case XK_Left: return ui::kVK_LEFT; case XK_Right: return ui::kVK_RIGHT; case XK_Up: return ui::kVK_UP; case XK_Down: return ui::kVK_DOWN; case XK_Home: return ui::kVK_HOME; case XK_End: return ui::kVK_END; case XK_Page_Up: return ui::kVK_PRIOR; case XK_Page_Down: return ui::kVK_NEXT; case XK_Insert: return ui::kVK_INSERT; case XK_Delete: return ui::kVK_DELETE;
    case XK_Shift_L: case XK_Shift_R: return ui::kVK_SHIFT; case XK_Control_L: case XK_Control_R: return ui::kVK_CONTROL; case XK_Alt_L: case XK_Alt_R: return ui::kVK_MENU; case XK_Caps_Lock: return ui::kVK_CAPITAL; case XK_Num_Lock: return ui::kVK_NUMLOCK; case XK_Scroll_Lock: return ui::kVK_SCROLL;
    case XK_comma: return ui::kVK_OEM_COMMA; case XK_period: return ui::kVK_OEM_PERIOD; case XK_slash: return ui::kVK_OEM_2; case XK_semicolon: return ui::kVK_OEM_1; case XK_apostrophe: return ui::kVK_OEM_7; case XK_bracketleft: return ui::kVK_OEM_4; case XK_bracketright: return ui::kVK_OEM_6; case XK_backslash: return ui::kVK_OEM_5; case XK_minus: return ui::kVK_OEM_MINUS; case XK_equal: return ui::kVK_OEM_PLUS; case XK_grave: return ui::kVK_OEM_3; default: return ui::kVK_None;
    }
}

static bool GetWindowRect_X11(Display* display, ::Window window, ui::UiRect& rc)
{
    if (display == nullptr || window == None) {
        rc.Clear();
        return false;
    }
    XWindowAttributes attributes{};
    if (!XGetWindowAttributes(display, window, &attributes)) {
        rc.Clear();
        return false;
    }
    // XGetWindowAttributes returns coordinates relative to the parent window.
    // With a reparenting window manager the top-level window is a child of the
    // frame, so attributes.x/y are (0,0) even when the window is elsewhere on
    // screen. Translate to the root to save/restore the real geometry.
    ::Window root = DefaultRootWindow(display);
    ::Window child = None;
    int rootX = 0;
    int rootY = 0;
    if (XTranslateCoordinates(display, window, root, 0, 0, &rootX, &rootY, &child)) {
        rc = ui::UiRect(rootX, rootY, rootX + attributes.width, rootY + attributes.height);
    } else {
        rc = ui::UiRect(attributes.x, attributes.y,
                        attributes.x + attributes.width, attributes.y + attributes.height);
    }
    return true;
}
namespace ui {

std::unordered_map<X11_WindowID, NativeWindow_X11*> NativeWindow_X11::s_windowIDMap;

NativeWindow_X11::NativeWindow_X11(INativeWindow* owner)
    : m_pOwner(owner), m_x11Display(nullptr), m_x11Colormap(0), m_x11Window(0), m_x11Renderer(nullptr),
      m_bChildWindow(false), m_bCloseing(false), m_closeParam(kWindowCloseNormal),
      m_lastWindowFlags(0), m_bFullscreen(false), m_bFullscreenExiting(false),
      m_bFullscreenMaximized(false), m_bInitWindowPosFlag(false), m_bWindowMaximized(false), m_bWindowMinimized(false), m_bUseSystemCaption(false),
      m_bMouseCapture(false), m_lastButtonPressTime(0), m_lastButtonPressPos(), m_bWindowMoving(false), m_nMoveStartRootX(0), m_nMoveStartRootY(0),
      m_nMoveStartWindowX(0), m_nMoveStartWindowY(0), m_bWindowPositionKnown(false),
      m_nWindowPositionX(0), m_nWindowPositionY(0),
      m_nLayeredWindowAlpha(255), m_nLayeredWindowOpacity(255),
      m_bEnableDragDrop(false), m_bFakeModal(false), m_bDoModal(false),
      m_bIsLayeredWindow(false)
{
}

NativeWindow_X11::~NativeWindow_X11() { Close(); }

bool NativeWindow_X11::CreateWnd(NativeWindow_X11* pParent, const WindowCreateParam& param, const WindowCreateAttributes& attrs)
{
    if (!MessageLoop_X11::CheckInitX11()) return false;
    m_createParam = param;
    m_x11Display = MessageLoop_X11::GetDisplay();
    // CreateWnd(pParent) is also used for top-level popup windows such as
    // Combo's drop-down. Only CreateChildWnd marks a true X11 child.
    const ::Window parentWindow = m_bChildWindow && pParent != nullptr
        ? pParent->m_x11Window : DefaultRootWindow(m_x11Display);
    const int width = param.m_nWidth > 0 ? param.m_nWidth : (attrs.m_szInitSize.cx > 0 ? attrs.m_szInitSize.cx : 800);
    const int height = param.m_nHeight > 0 ? param.m_nHeight : (attrs.m_szInitSize.cy > 0 ? attrs.m_szInitSize.cy : 600);
    XVisualInfo visualInfo{};
    bool argbVisualCreated = false;
    // A self-drawn shadow needs an ARGB top-level window even when the XML
    // explicitly disables layered mode. Select the visual at creation time.
    const bool useArgbVisual = !m_bChildWindow &&
        (attrs.m_bIsLayeredWindow ||
         (attrs.m_bShadowAttachedDefined && attrs.m_bShadowAttached));
    if (useArgbVisual && XMatchVisualInfo(m_x11Display, DefaultScreen(m_x11Display), 32, TrueColor, &visualInfo)) {
        m_x11Colormap = XCreateColormap(m_x11Display, DefaultRootWindow(m_x11Display), visualInfo.visual, AllocNone);
        XSetWindowAttributes windowAttrs{};
        windowAttrs.colormap = m_x11Colormap;
        windowAttrs.border_pixel = 0;
        windowAttrs.background_pixmap = None;
        m_x11Window = XCreateWindow(m_x11Display, parentWindow,
            param.m_nX == kCW_USEDEFAULT ? 0 : param.m_nX,
            param.m_nY == kCW_USEDEFAULT ? 0 : param.m_nY, width, height, 0,
            visualInfo.depth, InputOutput, visualInfo.visual,
            CWColormap | CWBorderPixel | CWBackPixmap, &windowAttrs);
        argbVisualCreated = true;
    } else {
        m_x11Window = XCreateSimpleWindow(m_x11Display, parentWindow,
            param.m_nX == kCW_USEDEFAULT ? 0 : param.m_nX,
            param.m_nY == kCW_USEDEFAULT ? 0 : param.m_nY, width, height, 0,
            BlackPixel(m_x11Display, DefaultScreen(m_x11Display)),
            WhitePixel(m_x11Display, DefaultScreen(m_x11Display)));
    }
    if (!m_x11Window) return false;
    m_bIsLayeredWindow = argbVisualCreated;
    s_windowIDMap[m_x11Window] = this;
    MessageLoop_X11::RegisterPaintWindow(this);
    XSelectInput(m_x11Display, m_x11Window, ExposureMask | StructureNotifyMask |
        KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | EnterWindowMask | LeaveWindowMask | FocusChangeMask);
    Atom wmDelete = XInternAtom(m_x11Display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(m_x11Display, m_x11Window, &wmDelete, 1);
    XClassHint classHint{};
    classHint.res_name = const_cast<char*>("dui");
    classHint.res_class = const_cast<char*>("dui");
    XSetClassHint(m_x11Display, m_x11Window, &classHint);
    SetUndecorated_X11(m_x11Display, m_x11Window);
    SetText(param.m_windowTitle);
    m_szInitWindow = UiSize(width, height);
    m_nLayeredWindowAlpha = attrs.m_nLayeredWindowAlpha;
    m_nLayeredWindowOpacity = attrs.m_nLayeredWindowOpacity;
    m_rcUpdateRect = UiRect(0, 0, width, height);
    if (pParent == nullptr) {
        const UiRect workArea = GetWorkArea_X11(m_x11Display);
        const int x = workArea.left + (workArea.Width() - width) / 2;
        const int y = workArea.top + (workArea.Height() - height) / 2;
        XMoveWindow(m_x11Display, m_x11Window, x, y);
    }

    if (m_pOwner != nullptr) {
        bool bHandled = false;
        m_pOwner->OnNativeCreateWndMsg(false, NativeMsg(0, 0, 0), bHandled);
    }
    return true;
}

bool NativeWindow_X11::CreateChildWnd(NativeWindow_X11* p, int32_t x, int32_t y, int32_t w, int32_t h)
{
    WindowCreateParam param;
    param.m_nX = x; param.m_nY = y; param.m_nWidth = w; param.m_nHeight = h;
    m_bChildWindow = true;
    return CreateWnd(p, param, WindowCreateAttributes());
}
bool NativeWindow_X11::ShowWindow(ShowWindowCommands command)
{
    if (!m_x11Display || !m_x11Window) return false;
    if (command == kSW_HIDE) {
        XUnmapWindow(m_x11Display, m_x11Window);
    }
    else if (command == kSW_MINIMIZE || command == kSW_SHOW_MINIMIZED || command == kSW_SHOW_MIN_NOACTIVE) {
        m_bWindowMinimized = true;
        XIconifyWindow(m_x11Display, m_x11Window, DefaultScreen(m_x11Display));
        if (m_pOwner != nullptr) {
            bool handled = false;
            m_pOwner->OnNativeSizeMsg(WindowSizeType::kSIZE_MINIMIZED, m_szInitWindow,
                                      NativeMsg(0, 0, 0), handled);
        }
    }
    else {
        XMapWindow(m_x11Display, m_x11Window);
        XFlush(m_x11Display);
        XSync(m_x11Display, False);
        if (command == kSW_SHOW_MAXIMIZED) {
            if (!m_bWindowMaximized) {
                UiRect rcRestore;
                if (GetWindowRect_X11(m_x11Display, m_x11Window, rcRestore)) {
                    m_rcRestoreWindow = rcRestore;
                }
            }
            m_bWindowMaximized = true;
            const UiRect workArea = GetWorkArea_X11(m_x11Display);
            XMoveResizeWindow(m_x11Display, m_x11Window, workArea.left, workArea.top,
                              workArea.Width(), workArea.Height());
            m_szInitWindow = UiSize(workArea.Width(), workArea.Height());
            m_bWindowMinimized = false;
            if (m_pOwner != nullptr) {
                bool handled = false;
                m_pOwner->OnNativeSizeMsg(WindowSizeType::kSIZE_MAXIMIZED, m_szInitWindow,
                                          NativeMsg(0, 0, 0), handled);
            }
        }
        else if (command == kSW_RESTORE) {
            m_bWindowMinimized = false;
            if (m_bWindowMaximized && !m_rcRestoreWindow.IsEmpty()) {
                m_bWindowMaximized = false;
                XMoveResizeWindow(m_x11Display, m_x11Window,
                                  m_rcRestoreWindow.left, m_rcRestoreWindow.top,
                                  m_rcRestoreWindow.Width(), m_rcRestoreWindow.Height());
                m_szInitWindow = UiSize(m_rcRestoreWindow.Width(), m_rcRestoreWindow.Height());
                m_rcRestoreWindow.Clear();
            }
            if (m_pOwner != nullptr) {
                bool handled = false;
                m_pOwner->OnNativeSizeMsg(WindowSizeType::kSIZE_RESTORED, m_szInitWindow,
                                          NativeMsg(0, 0, 0), handled);
            }
        }
        if (command == kSW_SHOW_MAXIMIZED || command == kSW_RESTORE) {
            XFlush(m_x11Display);
        }
        PaintWindow(true);
    }
    XFlush(m_x11Display);
    return true;
}
void* NativeWindow_X11::GetWindowHandle() const { return reinterpret_cast<void*>(static_cast<uintptr_t>(m_x11Window)); }
bool NativeWindow_X11::IsWindow() const { return m_x11Window != 0; }
bool NativeWindow_X11::IsChildWindow() const { return m_bChildWindow; }
bool NativeWindow_X11::SetParentWindow(NativeWindow_X11*) { return false; }
void NativeWindow_X11::CloseWnd(int32_t ret) { if (m_bCloseing) return; m_closeParam = ret; m_bCloseing = true; if (m_pOwner != nullptr) m_pOwner->OnNativePreCloseWindow(); Close(); if (m_pOwner != nullptr) m_pOwner->OnNativePostCloseWindow(); }
void NativeWindow_X11::Close() { if (m_x11Window) s_windowIDMap.erase(m_x11Window); if (m_x11Display && m_x11Window) XDestroyWindow(m_x11Display, m_x11Window); if (m_x11Display && m_x11Colormap) { XFreeColormap(m_x11Display, m_x11Colormap); m_x11Colormap = 0; } MessageLoop_X11::UnregisterPaintWindow(this); m_x11Window = 0; m_bCloseing = true; }
bool NativeWindow_X11::IsClosingWnd() const { return m_bCloseing; }
int32_t NativeWindow_X11::GetCloseParam() const { return m_closeParam; }
bool NativeWindow_X11::SetLayeredWindow(bool b, bool) { m_bIsLayeredWindow = b; return true; }
bool NativeWindow_X11::IsLayeredWindow() const { return m_bIsLayeredWindow; }
bool NativeWindow_X11::IsSystemShadowSupported() const { return false; }
bool NativeWindow_X11::SetSystemShadowType(NativeWindowShadowType t) { m_systemShadowType = t; return false; }
NativeWindowShadowType NativeWindow_X11::GetSystemShadowType() const { return m_systemShadowType; }
void NativeWindow_X11::RefreshSystemShadow() {}
void NativeWindow_X11::ClearWindowRgnForSystemShadow() {}
void NativeWindow_X11::SetLayeredWindowAlpha(int32_t n) { m_nLayeredWindowAlpha = static_cast<uint8_t>(n); }
uint8_t NativeWindow_X11::GetLayeredWindowAlpha() const { return m_nLayeredWindowAlpha; }
void NativeWindow_X11::SetLayeredWindowOpacity(int32_t n) { m_nLayeredWindowOpacity = static_cast<uint8_t>(n); }
uint8_t NativeWindow_X11::GetLayeredWindowOpacity() const { return m_nLayeredWindowOpacity; }
void NativeWindow_X11::SetUseSystemCaption(bool b) { m_bUseSystemCaption = b; }
bool NativeWindow_X11::IsUseSystemCaption() const { return m_bUseSystemCaption; }
bool NativeWindow_X11::IsWindowVisible() const { if (!m_x11Display || !m_x11Window) return false; XWindowAttributes a{}; return XGetWindowAttributes(m_x11Display, m_x11Window, &a) && a.map_state == IsViewable; }
bool NativeWindow_X11::IsWindowMaximized() const { return m_bWindowMaximized; }
bool NativeWindow_X11::IsWindowMinimized() const { return m_bWindowMinimized; }
bool NativeWindow_X11::IsWindowFullscreen() const { return m_bFullscreen; }
bool NativeWindow_X11::EnableWindow(bool) { return true; }
bool NativeWindow_X11::IsWindowEnabled() const { return true; }
bool NativeWindow_X11::SetWindowForeground() { return SetWindowFocus(); }
bool NativeWindow_X11::SetWindowFocus() { if (!m_x11Display || !m_x11Window) return false; XWindowAttributes attributes{}; if (!XGetWindowAttributes(m_x11Display, m_x11Window, &attributes) || attributes.map_state != IsViewable) return false; XSetInputFocus(m_x11Display, m_x11Window, RevertToParent, CurrentTime); XFlush(m_x11Display); return true; }
bool NativeWindow_X11::KillWindowFocus() { if (!m_x11Display) return false; XSetInputFocus(m_x11Display, PointerRoot, RevertToParent, CurrentTime); XFlush(m_x11Display); return true; }
bool NativeWindow_X11::IsWindowFocused() const { if (!m_x11Display || !m_x11Window) return false; ::Window focus = None; int revert = RevertToParent; XGetInputFocus(m_x11Display, &focus, &revert); return focus == m_x11Window; }
void NativeWindow_X11::CheckSetWindowFocus() {}
LRESULT NativeWindow_X11::PostMsg(UINT u, WPARAM w, LPARAM l) { return MessageLoop_X11::PostUserEvent(u, w, l) ? 0 : -1; }
void NativeWindow_X11::PostQuitMsg(int32_t) { MessageLoop_X11::PostQuitEvent(); }
bool NativeWindow_X11::EnterFullscreen() { m_bFullscreen = true; return true; }
bool NativeWindow_X11::ExitFullscreen() { m_bFullscreen = false; return true; }
bool NativeWindow_X11::MoveWindow(int32_t x, int32_t y, int32_t w, int32_t h, bool) { if (!m_x11Display || !m_x11Window) return false; XMoveResizeWindow(m_x11Display, m_x11Window, x, y, w, h); if (w > 0 && h > 0) m_szInitWindow = UiSize(w, h); XFlush(m_x11Display); return true; }
void NativeWindow_X11::SetText(const std::string& text) { if (!m_x11Display || !m_x11Window) return; std::string s = StringConvert::TToUTF8(text); XStoreName(m_x11Display, m_x11Window, s.c_str()); }
std::string NativeWindow_X11::GetText() const { return m_createParam.m_windowTitle; }
void NativeWindow_X11::SetWindowMaximumSize(const UiSize& s) { m_szMaxWindow = s; }
const UiSize& NativeWindow_X11::GetWindowMaximumSize() const { return m_szMaxWindow; }
void NativeWindow_X11::SetWindowMinimumSize(const UiSize& s) { m_szMinWindow = s; }
const UiSize& NativeWindow_X11::GetWindowMinimumSize() const { return m_szMinWindow; }
void NativeWindow_X11::SetCapture() { if (!m_x11Display || !m_x11Window) return; m_bMouseCapture = true; XGrabPointer(m_x11Display, m_x11Window, False, ButtonPressMask | ButtonReleaseMask | PointerMotionMask | EnterWindowMask | LeaveWindowMask, GrabModeAsync, GrabModeAsync, None, None, CurrentTime); }
void NativeWindow_X11::ReleaseCapture() { if (m_x11Display && m_bMouseCapture) XUngrabPointer(m_x11Display, CurrentTime); m_bMouseCapture = false; }
bool NativeWindow_X11::IsCaptured() const { return m_bMouseCapture; }
bool NativeWindow_X11::SetWindowRoundRectRgn(const UiRect&, float, float, bool) { return false; }
bool NativeWindow_X11::SetWindowRectRgn(const UiRect&, bool) { return false; }
void NativeWindow_X11::ClearWindowRgn(bool) {}
void NativeWindow_X11::Invalidate(const UiRect& r) { if (r.IsEmpty()) m_rcUpdateRect = UiRect(0,0,m_szInitWindow.cx,m_szInitWindow.cy); else m_rcUpdateRect.Union(r); MessageLoop_X11::PostNoneEvent(); }
bool NativeWindow_X11::UpdateWindow() const { if (m_x11Display) XFlush(m_x11Display); return true; }
void NativeWindow_X11::GetClientRect(UiRect& r) const { r = UiRect(0,0,m_szInitWindow.cx,m_szInitWindow.cy); }
void NativeWindow_X11::GetWindowRect(UiRect& r) const { if (!m_x11Display || !m_x11Window) { r.Clear(); return; } GetWindowRect_X11(m_x11Display, m_x11Window, r); }
void NativeWindow_X11::ScreenToClient(UiPoint& pt) const
{
    UiRect rcWindow;
    GetWindowRect(rcWindow);
    pt.x -= rcWindow.left;
    pt.y -= rcWindow.top;
}
void NativeWindow_X11::ClientToScreen(UiPoint& pt) const
{
    UiRect rcWindow;
    GetWindowRect(rcWindow);
    pt.x += rcWindow.left;
    pt.y += rcWindow.top;
}
void NativeWindow_X11::GetCursorPos(UiPoint& p) const { p = m_ptLastMousePos; }
bool NativeWindow_X11::GetMonitorRect(UiRect& r) const { return GetPrimaryMonitorWorkRect(r); }
bool NativeWindow_X11::GetPrimaryMonitorWorkRect(UiRect& r) { if (!MessageLoop_X11::CheckInitX11()) return false; Display* d=MessageLoop_X11::GetDisplay(); r=UiRect(0,0,DisplayWidth(d,DefaultScreen(d)),DisplayHeight(d,DefaultScreen(d))); return true; }
bool NativeWindow_X11::GetMonitorWorkRect(UiRect& r) const { return GetMonitorRect(r); }
bool NativeWindow_X11::GetMonitorWorkRect(const UiPoint&, UiRect& r) const { return GetMonitorRect(r); }
const UiPoint& NativeWindow_X11::GetLastMousePos() const { return m_ptLastMousePos; }
void NativeWindow_X11::SetLastMousePos(const UiPoint& p) { m_ptLastMousePos = p; }
INativeWindow* NativeWindow_X11::WindowBaseFromPoint(const UiPoint&, bool) { return m_pOwner; }
class NativeWindowRenderPaint_X11 : public IRenderPaint { public: NativeWindow_X11* window = nullptr; INativeWindow* owner = nullptr; NativeMsg nativeMsg; bool handled = false; bool DoPaint(const UiRect& rcPaint) override { if (owner) owner->OnNativePaintMsg(rcPaint, nativeMsg, handled); return true; } uint8_t GetLayeredWindowAlpha() override { return window ? window->GetLayeredWindowAlpha() : 255; } bool GetUpdateRect(UiRect& rcUpdate) const override { if (!window) return false; rcUpdate = window->GetUpdateRect(); return !rcUpdate.IsEmpty(); } }; void NativeWindow_X11::PaintWindow(bool bPaintAll) { if (!m_pOwner) return; UiRect paintedRect = m_rcUpdateRect; m_rcUpdateRect.Clear(); if (bPaintAll) paintedRect.Clear(); if (!m_pOwner->OnNativePreparePaint()) return; IRender* render = m_pOwner->OnNativeGetRender(); if (!render) return; NativeWindowRenderPaint_X11 paint; paint.window = this; paint.owner = m_pOwner; paint.nativeMsg = NativeMsg(0, 0, 0); render->PaintAndSwapBuffers(&paint); }
const UiRect& NativeWindow_X11::GetUpdateRect() const { return m_rcUpdateRect; }
bool NativeWindow_X11::GetWindowSize(int32_t* w, int32_t* h) const { if(w)*w=m_szInitWindow.cx; if(h)*h=m_szInitWindow.cy; return true; }
bool NativeWindow_X11::GetWindowSizeInPixels(int32_t* w, int32_t* h) const { return GetWindowSize(w,h); }
float NativeWindow_X11::GetDisplayContentScale() const { return 1.0f; }
float NativeWindow_X11::GetWindowDisplayScale() const { return 1.0f; }
float NativeWindow_X11::GetWindowPixelDensity() const { return 1.0f; }
uint64_t NativeWindow_X11::GetX11WindowNumber() const { return m_x11Window; }
size_t NativeWindow_X11::GetX11DisplayPointer() const { return reinterpret_cast<size_t>(m_x11Display); }
size_t NativeWindow_X11::GetWaylandDisplayPointer() const { return 0; }
bool NativeWindow_X11::IsVideoDriverX11() const { return true; }
bool NativeWindow_X11::IsVideoDriverWayland() const { return false; }
std::string NativeWindow_X11::GetVideoDriverName() const { return "x11"; }
std::string NativeWindow_X11::GetWindowRenderName() const { return "software"; }
// Top-level windows are centered during CreateWnd. Popup windows provide
// their own screen coordinates, so Window::AutoResizeWindow must not recenter
// them after the popup content is attached.
bool NativeWindow_X11::NeedCenterWindowAfterCreated() const { return false; }
void NativeWindow_X11::SetEnableDragDrop(bool b) { m_bEnableDragDrop=b; }
bool NativeWindow_X11::IsEnableDragDrop() const { return m_bEnableDragDrop; }
bool NativeWindow_X11::IsFakeModal() const { return m_bFakeModal; }
bool NativeWindow_X11::IsDoModal() const { return m_bDoModal; }
void NativeWindow_X11::ShowModalFake(NativeWindow_X11* p) { m_bFakeModal=true; if(p) SetWindowFocus(); }
void NativeWindow_X11::OnCloseModalFake(NativeWindow_X11*) { m_bFakeModal=false; }
int32_t NativeWindow_X11::DoModal(NativeWindow_X11* p, const WindowCreateParam& cp, const WindowCreateAttributes& ca, bool, bool) { m_bDoModal = true; if (!CreateWnd(p, cp, ca)) { m_bDoModal = false; return -1; } ShowWindow(kSW_SHOW); MessageLoop_X11 loop; loop.RunDoModal(*this); m_bDoModal = false; return m_closeParam; }
bool NativeWindow_X11::OnX11WindowEvent(const X11_Event& event) { if (event.xany.window != m_x11Window || !m_pOwner) return false; bool handled = false; uint32_t mod = kNone; unsigned state = (event.type == MotionNotify ? event.xmotion.state : (event.type == ButtonPress || event.type == ButtonRelease ? event.xbutton.state : 0)); if (state & ShiftMask) mod |= kShift; if (state & ControlMask) mod |= kControl; if (state & Mod1Mask) mod |= kAlt; UiPoint pt; if (event.type == KeyPress || event.type == KeyRelease) {
        KeySym sym = XLookupKeysym(const_cast<XKeyEvent*>(&event.xkey), 0);
        const ui::VirtualKeyCode vkCode = GetVirtualKeyCode_X11(sym);
        if (event.type == KeyPress) {
            m_pOwner->OnNativeKeyDownMsg(vkCode, mod, NativeMsg(event.type, 0, 0), handled);
            char text[32] = {};
            XKeyEvent keyEvent = event.xkey;
            const int length = XLookupString(&keyEvent, text, sizeof(text) - 1, nullptr, nullptr);
            for (int i = 0; i < length; ++i) {
                if (static_cast<unsigned char>(text[i]) >= 0x20 || text[i] == '	') {
                    wchar_t character = static_cast<unsigned char>(text[i]);
                    m_pOwner->OnNativeCharMsg(ui::kVK_None, mod,
                                              NativeMsg(event.type,
                                                        reinterpret_cast<WPARAM>(&character), 1), handled);
                }
            }
        } else {
            m_pOwner->OnNativeKeyUpMsg(vkCode, mod, NativeMsg(event.type, 0, 0), handled);
        }
        return true;
    } if (event.type == MotionNotify) { pt.x = event.xmotion.x; pt.y = event.xmotion.y; m_ptLastMousePos = pt; if (m_bWindowMoving) { m_nWindowPositionX = m_nMoveStartWindowX + event.xmotion.x_root - m_nMoveStartRootX; m_nWindowPositionY = m_nMoveStartWindowY + event.xmotion.y_root - m_nMoveStartRootY; XMoveWindow(m_x11Display, m_x11Window, m_nWindowPositionX, m_nWindowPositionY); XFlush(m_x11Display); return true; } m_pOwner->OnNativeMouseMoveMsg(pt, mod, false, NativeMsg(event.type, 0, 0), handled); return true; } if (event.type == EnterNotify) { pt.x = event.xcrossing.x; pt.y = event.xcrossing.y; m_pOwner->OnNativeMouseHoverMsg(pt, mod, NativeMsg(event.type, 0, 0), handled); return true; } if (event.type == LeaveNotify) { m_pOwner->OnNativeMouseLeaveMsg(NativeMsg(event.type, 0, 0), handled); return true; } if (event.type == FocusIn) { m_pOwner->OnNativeSetFocusMsg(nullptr, NativeMsg(event.type, 0, 0), handled); return true; } if (event.type == FocusOut) { m_pOwner->OnNativeKillFocusMsg(nullptr, NativeMsg(event.type, 0, 0), handled); return true; } if (event.type == MapNotify) { if (m_bWindowMinimized) { m_bWindowMinimized = false; XRaiseWindow(m_x11Display, m_x11Window); XSetInputFocus(m_x11Display, m_x11Window, RevertToParent, CurrentTime); m_pOwner->OnNativeSizeMsg(WindowSizeType::kSIZE_RESTORED, m_szInitWindow, NativeMsg(event.type, 0, 0), handled); Invalidate(UiRect()); PaintWindow(true); XFlush(m_x11Display); } return true; } if (event.type == UnmapNotify) { if (!m_bWindowMinimized) { m_bWindowMinimized = true; m_pOwner->OnNativeSizeMsg(WindowSizeType::kSIZE_MINIMIZED, m_szInitWindow, NativeMsg(event.type, 0, 0), handled); } return true; } if (event.type == ButtonPress || event.type == ButtonRelease) { pt.x = event.xbutton.x; pt.y = event.xbutton.y; m_ptLastMousePos = pt; if (event.type == ButtonPress && event.xbutton.button == Button1 && !m_bWindowMaximized) { UiRect caption; m_pOwner->OnNativeGetCaptionRect(caption); UiPadding shadow; m_pOwner->OnNativeGetShadowCorner(shadow); UiRect captionRect(shadow.left + caption.left, shadow.top + caption.top, m_szInitWindow.cx - shadow.right - caption.right, shadow.top + caption.top + caption.bottom); if (!caption.IsEmpty() && captionRect.ContainsPt(pt) && !m_pOwner->OnNativeIsPtInCaptionBarControl(pt)) { XWindowAttributes attributes{}; if (XGetWindowAttributes(m_x11Display, m_x11Window, &attributes)) { if (!m_bWindowPositionKnown) { UiRect rcWindow; if (GetWindowRect_X11(m_x11Display, m_x11Window, rcWindow)) { m_nWindowPositionX = rcWindow.left; m_nWindowPositionY = rcWindow.top; } m_bWindowPositionKnown = true; } Atom moveresize = XInternAtom(m_x11Display, "_NET_WM_MOVERESIZE", False); XEvent moveEvent{}; moveEvent.xclient.type = ClientMessage; moveEvent.xclient.window = m_x11Window; moveEvent.xclient.message_type = moveresize; moveEvent.xclient.format = 32; moveEvent.xclient.data.l[0] = event.xbutton.x_root; moveEvent.xclient.data.l[1] = event.xbutton.y_root; moveEvent.xclient.data.l[2] = 8; moveEvent.xclient.data.l[3] = Button1; moveEvent.xclient.data.l[4] = 1; XSendEvent(m_x11Display, DefaultRootWindow(m_x11Display), False, SubstructureRedirectMask | SubstructureNotifyMask, &moveEvent); XFlush(m_x11Display); } return true; } } if (event.type == ButtonRelease && event.xbutton.button == Button1 && m_bWindowMoving) { m_bWindowMoving = false; XUngrabPointer(m_x11Display, CurrentTime); XFlush(m_x11Display); return true; } if (event.xbutton.button == Button4 || event.xbutton.button == Button5) { if (event.type == ButtonPress) m_pOwner->OnNativeMouseWheelMsg(event.xbutton.button == Button4 ? 120 : -120, pt, mod, NativeMsg(event.type, 0, 0), handled); return true; } if (event.xbutton.button == Button1) { if (event.type == ButtonPress) { const bool doubleClick = m_lastButtonPressTime != 0 && event.xbutton.time - m_lastButtonPressTime <= 500 && std::abs(pt.x - m_lastButtonPressPos.x) <= 4 && std::abs(pt.y - m_lastButtonPressPos.y) <= 4; m_lastButtonPressTime = event.xbutton.time; m_lastButtonPressPos = pt; if (doubleClick) m_pOwner->OnNativeMouseLButtonDbClickMsg(pt, mod, NativeMsg(event.type, 0, 0), handled); else m_pOwner->OnNativeMouseLButtonDownMsg(pt, mod, NativeMsg(event.type, 0, 0), handled); } else m_pOwner->OnNativeMouseLButtonUpMsg(pt, mod, NativeMsg(event.type, 0, 0), handled); return true; } if (event.xbutton.button == Button3) { if (event.type == ButtonPress) m_pOwner->OnNativeMouseRButtonDownMsg(pt, mod, NativeMsg(event.type, 0, 0), handled); else m_pOwner->OnNativeMouseRButtonUpMsg(pt, mod, NativeMsg(event.type, 0, 0), handled); return true; } if (event.xbutton.button == Button2) { if (event.type == ButtonPress) m_pOwner->OnNativeMouseMButtonDownMsg(pt, mod, NativeMsg(event.type, 0, 0), handled); else m_pOwner->OnNativeMouseMButtonUpMsg(pt, mod, NativeMsg(event.type, 0, 0), handled); return true; } } if (event.type == ClientMessage) { Atom wmDelete = XInternAtom(m_x11Display, "WM_DELETE_WINDOW", False); if (static_cast<Atom>(event.xclient.data.l[0]) == wmDelete) { CloseWnd(kWindowCloseNormal); return true; } } if (event.type == Expose) { Invalidate(UiRect()); return true; } if (event.type == ConfigureNotify) { m_szInitWindow = UiSize(event.xconfigure.width, event.xconfigure.height); Invalidate(UiRect()); return true; } return false; }
void NativeWindow_X11::CenterWindow() {
    if (!m_x11Display || !m_x11Window || m_bWindowMinimized) return;
    XWindowAttributes attributes{};
    if (XGetWindowAttributes(m_x11Display, m_x11Window, &attributes) &&
        attributes.width > 0 && attributes.height > 0) {
        m_szInitWindow = UiSize(attributes.width, attributes.height);
    }
    const UiRect workArea = GetWorkArea_X11(m_x11Display);
    const int width = m_szInitWindow.cx;
    const int height = m_szInitWindow.cy;
    XMoveWindow(m_x11Display, m_x11Window,
                workArea.left + (workArea.Width() - width) / 2,
                workArea.top + (workArea.Height() - height) / 2);
    XFlush(m_x11Display);
}
void NativeWindow_X11::SetWindowAlwaysOnTop(bool) {}
bool NativeWindow_X11::IsWindowAlwaysOnTop() const { return false; }
bool NativeWindow_X11::SetWindowPos(const NativeWindow_X11*, InsertAfterFlag, int32_t x, int32_t y, int32_t w, int32_t h, uint32_t flags) {
    if (!MoveWindow(x, y, w, h, true)) return false;
    if (flags & kSWP_HIDEWINDOW) {
        XUnmapWindow(m_x11Display, m_x11Window);
    }
    if (flags & kSWP_SHOWWINDOW) {
        XMapRaised(m_x11Display, m_x11Window);
        // X11 does not generate a paint callback for a newly mapped window
        // until the first expose event, so render the initial menu frame now.
        PaintWindow(true);
    }
    XFlush(m_x11Display);
    return true;
}
void NativeWindow_X11::SetImeOpenStatus(bool) {}
void NativeWindow_X11::SetTextInputArea(const UiRect*,int32_t) {}
void NativeWindow_X11::ClearNativeWindow() { Close(); }
void NativeWindow_X11::OnFinalMessage() {}
bool NativeWindow_X11::CreateWindowAndRender(NativeWindow_X11*,const WindowCreateAttributes&) { return true; }
void NativeWindow_X11::InitNativeWindow() {}
void NativeWindow_X11::SyncCreateWindowAttributes(const WindowCreateAttributes&,bool) {}
void NativeWindow_X11::GetRenderNameList(const std::string&,std::vector<std::string>& names) const { names.push_back("software"); }
void NativeWindow_X11::QueryRenderProperties(const std::string&,bool& gl,bool& es,bool& transparent) const { gl=false; es=false; transparent=true; }
bool NativeWindow_X11::IsRenderSupportTransparent(const std::string&) const { return true; }
NativeWindow_X11* NativeWindow_X11::GetWindowFromID(X11_WindowID id) { auto i=s_windowIDMap.find(id); return i==s_windowIDMap.end()?nullptr:i->second; }
X11_WindowID NativeWindow_X11::GetWindowIdFromEvent(const X11_Event& event) { return event.xany.window; }
uint32_t NativeWindow_X11::GetHoverMsgId() { return kWM_USER+4; }
uint32_t NativeWindow_X11::GetModifiers(X11_Keymod) { return 0; }
void NativeWindow_X11::SetEnableSnapLayoutMenu(bool) {}
bool NativeWindow_X11::IsEnableSnapLayoutMenu() const { return false; }
void NativeWindow_X11::SetEnableSysMenu(bool) {}
bool NativeWindow_X11::IsEnableSysMenu() const { return true; }
int32_t NativeWindow_X11::SetWindowHotKey(uint8_t,uint8_t) { return -1; }
bool NativeWindow_X11::GetWindowHotKey(uint8_t&,uint8_t&) const { return false; }
bool NativeWindow_X11::RegisterHotKey(uint8_t,uint8_t,int32_t) { return false; }
bool NativeWindow_X11::UnregisterHotKey(int32_t) { return false; }
LRESULT NativeWindow_X11::CallDefaultWindowProc(UINT,WPARAM,LPARAM) { return 0; }
int32_t NativeWindow_X11::X11_HitTest(X11_Window*,const X11_Point*,void*) { return 0; }
bool NativeWindow_X11::SetWindowIcon(const FilePath&) { return false; }
bool NativeWindow_X11::SetWindowIcon(const std::vector<uint8_t>&,const std::string&) { return false; }
}
#endif
