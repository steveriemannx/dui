#include "dui/Core/WindowDropTarget_Wayland.h"
#include "dui/Core/NativeWindow_Wayland.h"
#include "dui/Core/MessageLoop_Wayland.h"
#include "dui/Render/IRender.h"
#include "dui/Utils/MonitorUtil.h"
#include "dui/Utils/StringConvert.h"

#ifdef DUI_BUILD_FOR_WAYLAND

#include <wayland-client.h>
#include <wayland-egl.h>
#include <xdg-shell-client-protocol.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>

namespace ui {

// Helper class for paint context (same as NativeWindowRenderPaint in NativeWindow_Wayland.cpp)
class NativeWindowRenderPaint :
    public IRenderPaint
{
public:
    NativeWindow_Wayland* m_pNativeWindow = nullptr;
    INativeWindow* m_pOwner = nullptr;
    NativeMsg m_nativeMsg;
    bool m_bHandled = false;

public:
    virtual bool GetUpdateRect(UiRect& rcUpdate) const override {
        if (m_pNativeWindow) {
            rcUpdate = m_pNativeWindow->GetUpdateRect();
            return true;
        }
        return false;
    }
    virtual uint8_t GetLayeredWindowAlpha() override {
        if (m_pNativeWindow) return m_pNativeWindow->GetLayeredWindowAlpha();
        return 255;
    }
    virtual bool DoPaint(const UiRect& rcPaint) override {
        if (m_pOwner != nullptr) {
            m_pOwner->OnNativePaintMsg(rcPaint, m_nativeMsg, m_bHandled);
            return true;
        }
        return false;
    }
};

// xdg_surface configure
static void xdg_surface_configure_handler(void* data, struct xdg_surface* s, uint32_t serial) {
    xdg_surface_ack_configure(s, serial);
    (void)data;
}
static const struct xdg_surface_listener xdg_surface_listener_impl = {
    .configure = xdg_surface_configure_handler,
};

// xdg_toplevel configure
static void xdg_toplevel_configure_handler(void* data, struct xdg_toplevel* t,
    int32_t width, int32_t height, struct wl_array* states) {
    NativeWindow_Wayland* pWindow = (NativeWindow_Wayland*)data;
    (void)t;
    if (!pWindow) return;

    // Parse states to detect maximized/fullscreen
    bool isMaximized = false;
    bool isFullscreen = false;
    uint32_t* sdata = (uint32_t*)states->data;
    size_t scount = states->size / sizeof(uint32_t);
    for (size_t si = 0; si < scount; si++) {
        uint32_t state = sdata[si];
        if (state == XDG_TOPLEVEL_STATE_MAXIMIZED) isMaximized = true;
        if (state == XDG_TOPLEVEL_STATE_FULLSCREEN) isFullscreen = true;
    }

    // Detect state changes
    bool wasMaximized = pWindow->m_bWaylandMaximized;
    bool wasFullscreen = pWindow->m_bWaylandFullscreen;
    pWindow->m_bWaylandMaximized = isMaximized;
    pWindow->m_bWaylandFullscreen = isFullscreen;

    // Notify framework of state changes
    INativeWindow* pOwner = pWindow->GetOwner();
    if (pOwner) {
        if (isMaximized && !wasMaximized) {
            bool bHandled = false;
            UiSize newSize(width, height);
            pOwner->OnNativeSizeMsg(WindowSizeType::kSIZE_MAXIMIZED, newSize, NativeMsg(0, 0, 0), bHandled);
        } else if (!isMaximized && wasMaximized && !isFullscreen) {
            bool bHandled = false;
            UiSize newSize(width, height);
            pOwner->OnNativeSizeMsg(WindowSizeType::kSIZE_RESTORED, newSize, NativeMsg(0, 0, 0), bHandled);
        }
    }

    if (width > 0 && height > 0) {
        pWindow->m_nWaylandPendingWidth = width;
        pWindow->m_nWaylandPendingHeight = height;
        pWindow->m_bWaylandConfigured = true;
        if (pOwner) {
            pWindow->Invalidate(UiRect());
        }
    }
}
static void xdg_toplevel_close_handler(void* data, struct xdg_toplevel* t) {
    NativeWindow_Wayland* pWindow = (NativeWindow_Wayland*)data;
    (void)t;
    if (pWindow) {
        pWindow->m_bWaylandVisible = false;
        pWindow->CloseWnd(0); // CloseWnd now calls OnNativePostCloseWindow internally
    }
}
static const struct xdg_toplevel_listener xdg_toplevel_listener_impl = {
    .configure = xdg_toplevel_configure_handler,
    .close = xdg_toplevel_close_handler,
};

// wl_buffer release listener
static void buffer_release_handler(void* data, struct wl_buffer* buffer) {
    NativeWindow_Wayland* pWindow = (NativeWindow_Wayland*)data;
    if (pWindow) {
        pWindow->m_bWaylandBufferBusy = false;
    }
    (void)buffer;
}
static const struct wl_buffer_listener buffer_listener_impl = {
    .release = buffer_release_handler,
};

// Constructor
NativeWindow_Wayland::NativeWindow_Wayland(INativeWindow* pOwner) {
    m_pOwner = pOwner;
    m_nativeWindow = nullptr;
    m_pWaylandSurface = nullptr;
    m_pXdgSurface = nullptr;
    m_pXdgToplevel = nullptr;
    m_pEglWindow = nullptr;
    m_bWaylandConfigured = false;
    m_nWaylandPendingWidth = 0;
    m_nWaylandPendingHeight = 0;
    m_bWaylandVisible = false;
    m_bWaylandBufferBusy = false;
    m_bWaylandMaximized = false;
    m_bWaylandFullscreen = false;
    m_bCloseing = false;
    m_closeParam = 0;
    m_bChildWindow = false;
    m_bFullscreen = false;
    m_bMouseCapture = false;
    m_bEnableDragDrop = false;
    m_bFakeModal = false;
    m_bDoModal = false;
    m_bIsLayeredWindow = false;
    m_nLayeredWindowAlpha = 255;
    m_nLayeredWindowOpacity = 255;
    m_bUseSystemCaption = false;
    m_ptLastMousePos = UiPoint(-1, -1);
    m_bInitWindowPosFlag = false;
}

// Destructor
NativeWindow_Wayland::~NativeWindow_Wayland() {
    if (m_pEglWindow) { wl_egl_window_destroy(m_pEglWindow); m_pEglWindow = nullptr; }
    if (m_pXdgToplevel) { xdg_toplevel_destroy(m_pXdgToplevel); m_pXdgToplevel = nullptr; }
    if (m_pXdgSurface) { xdg_surface_destroy(m_pXdgSurface); m_pXdgSurface = nullptr; }
    if (m_pWaylandSurface) { wl_surface_destroy(m_pWaylandSurface); m_pWaylandSurface = nullptr; }
}

// Buffer helper - creates a wl_buffer, attaches it, sets up release listener
static wl_buffer* wayland_create_and_attach(wl_surface* surface, wl_shm* shm,
    int width, int height, void* listenerData)
{
    if (!surface || !shm || width <= 0 || height <= 0) return nullptr;

    int stride = width * 4;
    int size = stride * height;

    int fd = memfd_create("dui-wl", MFD_CLOEXEC | MFD_ALLOW_SEALING);
    if (fd < 0) return nullptr;
    if (ftruncate(fd, size) < 0) { close(fd); return nullptr; }

    void* data = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (data == MAP_FAILED) { close(fd); return nullptr; }

    // Fill with background color
    uint32_t* pixels = (uint32_t*)data;
    for (int i = 0; i < width * height; i++) pixels[i] = 0xFF2D2D30;
    munmap(data, size);

    wl_shm_pool* pool = wl_shm_create_pool(shm, fd, size);
    wl_buffer* buffer = wl_shm_pool_create_buffer(pool, 0, width, height, stride, WL_SHM_FORMAT_ARGB8888);
    wl_shm_pool_destroy(pool);
    close(fd);

    if (!buffer) return nullptr;

    wl_buffer_add_listener(buffer, &buffer_listener_impl, listenerData);
    wl_surface_attach(surface, buffer, 0, 0);
    wl_surface_damage_buffer(surface, 0, 0, width, height);

    return buffer;
}

bool NativeWindow_Wayland::CreateWnd(NativeWindow_Wayland* pParentWindow,
              const WindowCreateParam& createParam,
              const WindowCreateAttributes& createAttributes) {
    (void)pParentWindow;
    if (!MessageLoop_Wayland::CheckInitWayland()) return false;

    wl_display* display = MessageLoop_Wayland::GetDisplay();
    wl_compositor* compositor = MessageLoop_Wayland::GetCompositor();
    xdg_wm_base* wmBase = MessageLoop_Wayland::GetXdgWmBase();
    if (!display || !compositor || !wmBase) return false;

    m_pWaylandSurface = wl_compositor_create_surface(compositor);
    if (!m_pWaylandSurface) return false;

    m_pXdgSurface = xdg_wm_base_get_xdg_surface(wmBase, m_pWaylandSurface);
    if (!m_pXdgSurface) return false;
    xdg_surface_add_listener(m_pXdgSurface, &xdg_surface_listener_impl, this);

    m_pXdgToplevel = xdg_surface_get_toplevel(m_pXdgSurface);
    if (!m_pXdgToplevel) return false;
    xdg_toplevel_add_listener(m_pXdgToplevel, &xdg_toplevel_listener_impl, this);

    xdg_toplevel_set_app_id(m_pXdgToplevel, "dui.basic");

    std::string title = "dui Demo";
    if (!createParam.m_windowTitle.empty()) {
        title = StringConvert::TToUTF8(createParam.m_windowTitle);
    }
    xdg_toplevel_set_title(m_pXdgToplevel, title.c_str());

    if (createAttributes.m_bInitSizeDefined &&
        createAttributes.m_szInitSize.cx > 0 && createAttributes.m_szInitSize.cy > 0) {
        m_nWaylandPendingWidth = createAttributes.m_szInitSize.cx;
        m_nWaylandPendingHeight = createAttributes.m_szInitSize.cy;
    } else {
        m_nWaylandPendingWidth = 800;
        m_nWaylandPendingHeight = 600;
    }

    // Commit to confirm role and trigger configure
    wl_surface_commit(m_pWaylandSurface);
    wl_display_roundtrip(display);
    wl_display_flush(display);

    // Register for idle painting and input events
    MessageLoop_Wayland::RegisterPaintWindow(this);
    RegisterWaylandSurface(m_pWaylandSurface, this);

    // Call OnNativeCreateWndMsg to initialize the window (creates render, etc.)
    if (m_pOwner != nullptr) {
        bool bHandled = false;
        m_pOwner->OnNativeCreateWndMsg(false, NativeMsg(0, 0, 0), bHandled);
    }

    return true;
}

bool NativeWindow_Wayland::ShowWindow(ShowWindowCommands nCmdShow) {
    if (!m_pWaylandSurface) return false;

    switch (nCmdShow) {
    case kSW_HIDE:
        m_bWaylandVisible = false;
        return true;
    case kSW_SHOW_MAXIMIZED:
        if (m_pXdgToplevel) xdg_toplevel_set_maximized(m_pXdgToplevel);
        break;
    case kSW_RESTORE:
        if (m_pXdgToplevel) xdg_toplevel_unset_maximized(m_pXdgToplevel);
        break;
    case kSW_MINIMIZE: case kSW_SHOW_MINIMIZED: case kSW_SHOW_MIN_NOACTIVE:
        if (m_pXdgToplevel) xdg_toplevel_set_minimized(m_pXdgToplevel);
        break;
    default: break;
    }

    m_bWaylandVisible = true;

    // Attach the initial buffer to map the window (will be replaced by first paint)
    wl_shm* shm = MessageLoop_Wayland::GetShm();
    if (shm && m_nWaylandPendingWidth > 0 && m_nWaylandPendingHeight > 0) {
        wl_buffer* buffer = wayland_create_and_attach(m_pWaylandSurface, shm,
            m_nWaylandPendingWidth, m_nWaylandPendingHeight, this);
        if (buffer) {
            m_bWaylandBufferBusy = true;
            wl_surface_commit(m_pWaylandSurface);
            wl_display_flush(MessageLoop_Wayland::GetDisplay());
        }
    }

    // Dispatch to get configure events, then do initial paint
    wl_display* display = MessageLoop_Wayland::GetDisplay();
    if (display) {
        wl_display_roundtrip(display);
    }
    PaintWindow(true);

    return true;
}

void* NativeWindow_Wayland::GetWindowHandle() const {
    return (void*)m_pWaylandSurface;
}

bool NativeWindow_Wayland::IsWindow() const {
    return m_pWaylandSurface != nullptr;
}

bool NativeWindow_Wayland::IsWindowVisible() const {
    return m_bWaylandVisible;
}

DString NativeWindow_Wayland::GetVideoDriverName() const {
    return DUI_T("wayland");
}

DString NativeWindow_Wayland::GetWindowRenderName() const {
    return DUI_T("software");
}

bool NativeWindow_Wayland::IsVideoDriverX11() const { return false; }

bool NativeWindow_Wayland::IsVideoDriverWayland() const { return true; }

void NativeWindow_Wayland::CloseWnd(int32_t nRet) {
    if (m_bCloseing) return; // Already closing
    m_bCloseing = true;
    m_closeParam = nRet;
    // Trigger the framework's close sequence (PostCloseWindow -> PostQuitMsg)
    if (m_pOwner) {
        m_pOwner->OnNativePostCloseWindow();
    }
}

void NativeWindow_Wayland::Close() {
    MessageLoop_Wayland::UnregisterPaintWindow(this);
    if (m_pWaylandSurface) UnregisterWaylandSurface(m_pWaylandSurface);
    if (m_pEglWindow) { wl_egl_window_destroy(m_pEglWindow); m_pEglWindow = nullptr; }
    if (m_pXdgToplevel) { xdg_toplevel_destroy(m_pXdgToplevel); m_pXdgToplevel = nullptr; }
    if (m_pXdgSurface) { xdg_surface_destroy(m_pXdgSurface); m_pXdgSurface = nullptr; }
    if (m_pWaylandSurface) { wl_surface_destroy(m_pWaylandSurface); m_pWaylandSurface = nullptr; }
    m_bCloseing = true;
    m_bWaylandVisible = false;
}

bool NativeWindow_Wayland::IsClosingWnd() const { return m_bCloseing; }

int32_t NativeWindow_Wayland::GetCloseParam() const { return m_closeParam; }

void NativeWindow_Wayland::GetClientRect(UiRect& rc) const {
    rc.Clear();
    rc.right = m_nWaylandPendingWidth;
    rc.bottom = m_nWaylandPendingHeight;
}

void NativeWindow_Wayland::GetWindowRect(UiRect& rc) const {
    GetClientRect(rc);
}

void NativeWindow_Wayland::Invalidate(const UiRect& rc) {
    if (!rc.IsEmpty()) {
        m_rcUpdateRect.Union(rc);
    } else {
        // Full repaint - set to entire client area
        GetClientRect(m_rcUpdateRect);
    }
    m_bWaylandBufferBusy = true; // Flag that we need to repaint
    MessageLoop_Wayland::PostUserEvent(MessageLoop_Wayland::WM_PAINT_EVENT, 0, 0);
}

bool NativeWindow_Wayland::UpdateWindow() const {
    if (m_pWaylandSurface) {
        wl_display_flush(MessageLoop_Wayland::GetDisplay());
        return true;
    }
    return false;
}

bool NativeWindow_Wayland::GetMonitorRect(UiRect& rc) const {
    rc.Clear();
    rc.right = 1920;
    rc.bottom = 1080;
    return true;
}

bool NativeWindow_Wayland::GetWindowSize(int32_t* w, int32_t* h) const {
    if (w) *w = m_nWaylandPendingWidth;
    if (h) *h = m_nWaylandPendingHeight;
    return true;
}

int32_t NativeWindow_Wayland::DoModal(NativeWindow_Wayland* p, const WindowCreateParam& cp,
    const WindowCreateAttributes& ca, bool esc, bool enter) {
    bool b = CreateWnd(p, cp, ca);
    if (b) ShowWindow(kSW_SHOW);
    (void)esc; (void)enter;
    return b ? m_closeParam : -1;
}

bool NativeWindow_Wayland::CreateChildWnd(NativeWindow_Wayland* p, int32_t x, int32_t y, int32_t w, int32_t h) {
    (void)p; (void)x; (void)y;
    m_nWaylandPendingWidth = w;
    m_nWaylandPendingHeight = h;
    return true;
}

bool NativeWindow_Wayland::SetParentWindow(NativeWindow_Wayland* p) { (void)p; return true; }
bool NativeWindow_Wayland::IsChildWindow() const { return false; }
bool NativeWindow_Wayland::IsUseSystemCaption() const { return m_bUseSystemCaption; }
void NativeWindow_Wayland::SetUseSystemCaption(bool b) { m_bUseSystemCaption = b; }
bool NativeWindow_Wayland::IsLayeredWindow() const { return false; }
bool NativeWindow_Wayland::SetLayeredWindow(bool b, bool r) { (void)b; (void)r; return true; }
void NativeWindow_Wayland::SetLayeredWindowAlpha(int32_t n) { (void)n; }
uint8_t NativeWindow_Wayland::GetLayeredWindowAlpha() const { return 255; }
void NativeWindow_Wayland::SetLayeredWindowOpacity(int32_t n) { (void)n; }
uint8_t NativeWindow_Wayland::GetLayeredWindowOpacity() const { return 255; }
void NativeWindow_Wayland::CenterWindow() { }
void NativeWindow_Wayland::SetWindowAlwaysOnTop(bool b) { (void)b; }
bool NativeWindow_Wayland::IsWindowAlwaysOnTop() const { return false; }
bool NativeWindow_Wayland::SetWindowForeground() { return false; }
bool NativeWindow_Wayland::IsWindowForeground() const { return m_bWaylandVisible; }
bool NativeWindow_Wayland::SetWindowFocus() { return false; }
bool NativeWindow_Wayland::KillWindowFocus() { return false; }
bool NativeWindow_Wayland::IsWindowFocused() const { return false; }
void NativeWindow_Wayland::CheckSetWindowFocus() { }
LRESULT NativeWindow_Wayland::PostMsg(UINT u, WPARAM w, LPARAM l) {
    MessageLoop_Wayland::PostUserEvent(u, w, l);
    return 0;
}
void NativeWindow_Wayland::PostQuitMsg(int32_t n) {
    MessageLoop_Wayland::PostQuitEvent();
    (void)n;
}
bool NativeWindow_Wayland::EnterFullscreen() {
    if (m_pXdgToplevel) {
        xdg_toplevel_set_fullscreen(m_pXdgToplevel, nullptr);
        m_bWaylandFullscreen = true;
        return true;
    }
    return false;
}
bool NativeWindow_Wayland::ExitFullscreen() {
    if (m_pXdgToplevel) {
        xdg_toplevel_unset_fullscreen(m_pXdgToplevel);
        m_bWaylandFullscreen = false;
        return true;
    }
    return false;
}
bool NativeWindow_Wayland::IsWindowMaximized() const { return m_bWaylandMaximized; }
bool NativeWindow_Wayland::IsWindowMinimized() const { return !m_bWaylandVisible; }
bool NativeWindow_Wayland::IsWindowFullscreen() const { return m_bWaylandFullscreen; }
bool NativeWindow_Wayland::EnableWindow(bool b) { (void)b; return true; }
bool NativeWindow_Wayland::IsWindowEnabled() const { return true; }
void NativeWindow_Wayland::ShowModalFake(NativeWindow_Wayland* p) { (void)p; }
void NativeWindow_Wayland::OnCloseModalFake(NativeWindow_Wayland* p) { (void)p; }
bool NativeWindow_Wayland::IsFakeModal() const { return false; }
bool NativeWindow_Wayland::IsDoModal() const { return false; }
bool NativeWindow_Wayland::SetWindowPos(const NativeWindow_Wayland* p, InsertAfterFlag f, int32_t X, int32_t Y, int32_t cx, int32_t cy, uint32_t u) { (void)p;(void)f;(void)X;(void)Y;(void)cx;(void)cy;(void)u; return true; }
bool NativeWindow_Wayland::MoveWindow(int32_t X, int32_t Y, int32_t nW, int32_t nH, bool bR) { (void)X;(void)Y;(void)nW;(void)nH;(void)bR; return true; }
bool NativeWindow_Wayland::SetWindowIcon(const FilePath& f) { (void)f; return false; }
bool NativeWindow_Wayland::SetWindowIcon(const std::vector<uint8_t>& d, const DString& n) { (void)d;(void)n; return false; }
void NativeWindow_Wayland::SetText(const DString& s) {
    if (m_pXdgToplevel && !s.empty()) {
        std::string title = StringConvert::TToUTF8(s);
        xdg_toplevel_set_title(m_pXdgToplevel, title.c_str());
    }
}
DString NativeWindow_Wayland::GetText() const { return DUI_T(""); }
void NativeWindow_Wayland::SetWindowMaximumSize(const UiSize& s) { (void)s; }
const UiSize& NativeWindow_Wayland::GetWindowMaximumSize() const { static UiSize s; return s; }
void NativeWindow_Wayland::SetWindowMinimumSize(const UiSize& s) { (void)s; }
const UiSize& NativeWindow_Wayland::GetWindowMinimumSize() const { static UiSize s; return s; }
void NativeWindow_Wayland::SetCapture() { }
void NativeWindow_Wayland::ReleaseCapture() { }
bool NativeWindow_Wayland::IsCaptured() const { return false; }
bool NativeWindow_Wayland::SetWindowRoundRectRgn(const UiRect& r, float rx, float ry, bool b) { (void)r;(void)rx;(void)ry;(void)b; return false; }
bool NativeWindow_Wayland::SetWindowRectRgn(const UiRect& r, bool b) { (void)r;(void)b; return false; }
void NativeWindow_Wayland::ClearWindowRgn(bool b) { (void)b; }
bool NativeWindow_Wayland::IsSystemShadowSupported() const {
    // xdg-shell has no system-shadow request for borderless toplevels. Keep
    // this false so an explicit system shadow request safely falls back to
    // the framework-drawn shadow instead of removing the shadow entirely.
    return false;
}
bool NativeWindow_Wayland::SetSystemShadowType(NativeWindowShadowType t) {
    (void)t;
    m_systemShadowType = NativeWindowShadowType::kShadowSystemDisabled;
    return false;
}
NativeWindowShadowType NativeWindow_Wayland::GetSystemShadowType() const { return m_systemShadowType; }
void NativeWindow_Wayland::RefreshSystemShadow() { }
void NativeWindow_Wayland::ClearWindowRgnForSystemShadow() { }
void NativeWindow_Wayland::KeepParentActive() { }
void NativeWindow_Wayland::ScreenToClient(UiPoint& p) const { (void)p; }
void NativeWindow_Wayland::ClientToScreen(UiPoint& p) const { (void)p; }
void NativeWindow_Wayland::GetCursorPos(UiPoint& p) const { (void)p; }
const UiPoint& NativeWindow_Wayland::GetLastMousePos() const { static UiPoint p; return p; }
void NativeWindow_Wayland::SetLastMousePos(const UiPoint& p) { (void)p; }
INativeWindow* NativeWindow_Wayland::WindowBaseFromPoint(const UiPoint& p, bool b) { (void)p;(void)b; return m_pOwner; }
void NativeWindow_Wayland::PaintWindow(bool bPaintAll) {
    INativeWindow* pOwner = m_pOwner;
    if (pOwner == nullptr) return;
    std::weak_ptr<WeakFlag> ownerFlag = pOwner->GetWeakFlag();

    UiRect paintedRect = m_rcUpdateRect;
    m_rcUpdateRect.Clear();
    if (bPaintAll) paintedRect.Clear();

    bool bPaint = pOwner->OnNativePreparePaint();
    // The shadow is part of the buffer, but it must not be part of the
    // window's input region. Otherwise it blocks clicks on windows behind it.
    UpdateWaylandInputRegion();
    if (bPaint && !ownerFlag.expired()) {
        IRender* pRender = pOwner->OnNativeGetRender();
        if (pRender != nullptr && !ownerFlag.expired()) {
            NativeWindowRenderPaint renderPaint;
            renderPaint.m_pNativeWindow = this;
            renderPaint.m_pOwner = pOwner;
            renderPaint.m_nativeMsg = NativeMsg(0, 0, 0);
            renderPaint.m_bHandled = false;
            pRender->PaintAndSwapBuffers(&renderPaint);
        }
    }
}

void NativeWindow_Wayland::UpdateWaylandInputRegion() {
    if (m_pWaylandSurface == nullptr) {
        return;
    }

    wl_compositor* compositor = MessageLoop_Wayland::GetCompositor();
    if (compositor == nullptr) {
        return;
    }

    UiRect inputRect;
    GetClientRect(inputRect);
    if (m_pOwner != nullptr && !IsWindowMaximized() && !IsWindowFullscreen()) {
        UiPadding shadow;
        m_pOwner->OnNativeGetShadowCorner(shadow);
        inputRect.Deflate(shadow);
    }

    wl_region* region = wl_compositor_create_region(compositor);
    if (region == nullptr) {
        return;
    }
    if (!inputRect.IsEmpty()) {
        wl_region_add(region, inputRect.left, inputRect.top,
                      inputRect.Width(), inputRect.Height());
    }
    wl_surface_set_input_region(m_pWaylandSurface, region);
    wl_region_destroy(region);
}
LRESULT NativeWindow_Wayland::CallDefaultWindowProc(UINT u, WPARAM w, LPARAM l) { (void)u;(void)w;(void)l; return 0; }
bool NativeWindow_Wayland::CalculateCenterWindowPos(Wayland_Window* p, int32_t& x, int32_t& y) const { (void)p; x=0; y=0; return false; }
bool NativeWindow_Wayland::GetMonitorWorkRect(UiRect& m) const { return GetMonitorRect(m); }
bool NativeWindow_Wayland::GetMonitorWorkRect(const UiPoint& pt, UiRect& m) const { (void)pt; return GetMonitorRect(m); }
bool NativeWindow_Wayland::GetPrimaryMonitorWorkRect(UiRect& m) { m.Clear(); m.right=1920; m.bottom=1080; return true; }
bool NativeWindow_Wayland::GetWindowSizeInPixels(int32_t* w, int32_t* h) const { return GetWindowSize(w, h); }
float NativeWindow_Wayland::GetDisplayContentScale() const { return 1.0f; }
float NativeWindow_Wayland::GetWindowDisplayScale() const { return 1.0f; }
float NativeWindow_Wayland::GetWindowPixelDensity() const { return 1.0f; }
bool NativeWindow_Wayland::OnWaylandWindowEvent(const Wayland_Event& e) { (void)e; return false; }
Wayland_WindowID NativeWindow_Wayland::GetWindowIdFromEvent(const Wayland_Event& e) { (void)e; return 0; }
NativeWindow_Wayland* NativeWindow_Wayland::GetWindowFromID(Wayland_WindowID id) { (void)id; return nullptr; }
uint32_t NativeWindow_Wayland::GetModifiers(Wayland_Keymod k) { (void)k; return 0; }
uint32_t NativeWindow_Wayland::GetHoverMsgId() { return 4; }
uint64_t NativeWindow_Wayland::GetX11WindowNumber() const { return 0; }
size_t NativeWindow_Wayland::GetX11DisplayPointer() const { return 0; }
size_t NativeWindow_Wayland::GetWaylandDisplayPointer() const { return (size_t)MessageLoop_Wayland::GetDisplay(); }
void NativeWindow_Wayland::SetEnableSnapLayoutMenu(bool b) { (void)b; }
bool NativeWindow_Wayland::IsEnableSnapLayoutMenu() const { return false; }
void NativeWindow_Wayland::SetEnableSysMenu(bool b) { (void)b; }
bool NativeWindow_Wayland::IsEnableSysMenu() const { return true; }
int32_t NativeWindow_Wayland::SetWindowHotKey(uint8_t k, uint8_t m) { (void)k;(void)m; return -1; }
bool NativeWindow_Wayland::GetWindowHotKey(uint8_t& k, uint8_t& m) const { (void)k;(void)m; return false; }
bool NativeWindow_Wayland::RegisterHotKey(uint8_t k, uint8_t m, int32_t id) { (void)k;(void)m;(void)id; return false; }
bool NativeWindow_Wayland::UnregisterHotKey(int32_t id) { (void)id; return false; }
void NativeWindow_Wayland::SetEnableDragDrop(bool b) { (void)b; }
bool NativeWindow_Wayland::IsEnableDragDrop() const { return false; }
bool NativeWindow_Wayland::NeedCenterWindowAfterCreated() const { return false; }
void NativeWindow_Wayland::SetImeOpenStatus(bool bOpen) { (void)bOpen; }
void NativeWindow_Wayland::SetTextInputArea(const UiRect* rect, int32_t nCursor) { (void)rect; (void)nCursor; }
void NativeWindow_Wayland::ClearNativeWindow() { Close(); }
void NativeWindow_Wayland::OnFinalMessage() { }
void NativeWindow_Wayland::InitNativeWindow() { }
bool NativeWindow_Wayland::CreateWindowAndRender(NativeWindow_Wayland* p, const WindowCreateAttributes& a) { (void)p;(void)a; return true; }
void NativeWindow_Wayland::SyncCreateWindowAttributes(const WindowCreateAttributes& a, bool b) { (void)a;(void)b; }
void NativeWindow_Wayland::SetCreateWindowProperties(Wayland_PropertiesID props, NativeWindow_Wayland* p, const WindowCreateAttributes& a, bool gl) { (void)props;(void)p;(void)a;(void)gl; }
int32_t NativeWindow_Wayland::Wayland_HitTest(Wayland_Window* win, const Wayland_Point* area, void* data) { (void)win;(void)area;(void)data; return 0; }
void NativeWindow_Wayland::CheckWindowSnap(Wayland_Window* window) { (void)window; }
void NativeWindow_Wayland::OnDropBegin() { }
void NativeWindow_Wayland::OnDropPosition(const UiPoint& pt, bool& h) { (void)pt; h=false; }
void NativeWindow_Wayland::OnDropTexts(const std::vector<DString>& t, const UiPoint& pt, bool& h) { (void)t;(void)pt; h=false; }
void NativeWindow_Wayland::OnDropFiles(const DString& s, const std::vector<DString>& f, const UiPoint& pt, bool& h) { (void)s;(void)f;(void)pt; h=false; }
void NativeWindow_Wayland::OnDropLeave() { }
void NativeWindow_Wayland::GetRenderNameList(const DString& n, std::vector<DString>& l) const { (void)n; l.clear(); }
void NativeWindow_Wayland::QueryRenderProperties(const DString& n, bool& gl, bool& es, bool& t) const { (void)n; gl=false; es=false; t=false; }
bool NativeWindow_Wayland::IsRenderSupportTransparent(const DString& n) const { (void)n; return false; }
float MonitorUtil::GetPrimaryMonitorDisplayScale() { return 1.0f; }
float MonitorUtil::GetWindowDisplayScale(const WindowBase* b, float& d) { (void)b; d=1.0f; return 1.0f; }

// ProcessWaylandMoveResize - start interactive move or resize
void NativeWindow_Wayland::ProcessWaylandMoveResize(const UiPoint& pt, uint32_t serial) {
    if (!m_pXdgToplevel || !m_pOwner) return;
    if (IsUseSystemCaption() || IsWindowFullscreen()) return;

    wl_seat* seat = MessageLoop_Wayland::GetSeat();
    if (!seat) return;

    UiRect rcClient;
    GetClientRect(rcClient);

    // Hit testing is done against the visible window, not the self-drawn
    // shadow margin. This also keeps resize handles aligned with the frame.
    if (!IsWindowMaximized()) {
        UiPadding rcShadow;
        m_pOwner->OnNativeGetShadowCorner(rcShadow);
        rcClient.Deflate(rcShadow);
    }

    // Check resize borders (only when not maximized)
    if (!IsWindowMaximized()) {
        UiRect rcSizeBox = m_pOwner->OnNativeGetSizeBox();
        const int L = rcSizeBox.left;
        const int R = rcSizeBox.right;
        const int T = rcSizeBox.top;
        const int B = rcSizeBox.bottom;

        if (pt.y < rcClient.top + T && pt.y >= rcClient.top) {
            if (pt.x < rcClient.left + L && pt.x >= rcClient.left) {
                xdg_toplevel_resize(m_pXdgToplevel, seat, serial, XDG_TOPLEVEL_RESIZE_EDGE_TOP_LEFT);
                return;
            }
            if (pt.x > rcClient.right - R && pt.x <= rcClient.right) {
                xdg_toplevel_resize(m_pXdgToplevel, seat, serial, XDG_TOPLEVEL_RESIZE_EDGE_TOP_RIGHT);
                return;
            }
            xdg_toplevel_resize(m_pXdgToplevel, seat, serial, XDG_TOPLEVEL_RESIZE_EDGE_TOP);
            return;
        }
        if (pt.y > rcClient.bottom - B && pt.y <= rcClient.bottom) {
            if (pt.x < rcClient.left + L && pt.x >= rcClient.left) {
                xdg_toplevel_resize(m_pXdgToplevel, seat, serial, XDG_TOPLEVEL_RESIZE_EDGE_BOTTOM_LEFT);
                return;
            }
            if (pt.x > rcClient.right - R && pt.x <= rcClient.right) {
                xdg_toplevel_resize(m_pXdgToplevel, seat, serial, XDG_TOPLEVEL_RESIZE_EDGE_BOTTOM_RIGHT);
                return;
            }
            xdg_toplevel_resize(m_pXdgToplevel, seat, serial, XDG_TOPLEVEL_RESIZE_EDGE_BOTTOM);
            return;
        }
        if (pt.x < rcClient.left + L && pt.x >= rcClient.left) {
            xdg_toplevel_resize(m_pXdgToplevel, seat, serial, XDG_TOPLEVEL_RESIZE_EDGE_LEFT);
            return;
        }
        if (pt.x > rcClient.right - R && pt.x <= rcClient.right) {
            xdg_toplevel_resize(m_pXdgToplevel, seat, serial, XDG_TOPLEVEL_RESIZE_EDGE_RIGHT);
            return;
        }
    }

    // Check caption area for window move
    UiRect rcCaption;
    m_pOwner->OnNativeGetCaptionRect(rcCaption);
    UiRect rcCaptionRect;
    rcCaptionRect.left = rcClient.left + rcCaption.left;
    rcCaptionRect.right = rcClient.right - rcCaption.right;
    rcCaptionRect.top = rcClient.top + rcCaption.top;
    rcCaptionRect.bottom = rcClient.top + rcCaption.bottom;

    if (rcCaptionRect.ContainsPt(pt)) {
        if (m_pOwner->OnNativeIsPtInCaptionBarControl(pt)) {
            return;
        }
        xdg_toplevel_move(m_pXdgToplevel, seat, serial);
    }
}

// GetUpdateRect - required by NativeWindowRenderPaint
const UiRect& NativeWindow_Wayland::GetUpdateRect() const {
    return m_rcUpdateRect;
}

} // namespace ui
#endif // DUI_BUILD_FOR_WAYLAND
