#include "dui/Core/MessageLoop_Wayland.h"
#include "dui/Core/NativeWindow_SDL.h"
#include "dui/Utils/StringConvert.h"
#include <algorithm>

#ifdef DUI_BUILD_FOR_WAYLAND

#include <wayland-client.h>
#include <wayland-egl.h>
#include <xdg-shell-client-protocol.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <unistd.h>
#include <cstring>
#include <mutex>
#include <vector>
#include <algorithm>

namespace ui
{

// Static member initialization
std::unordered_map<uint32_t, WaylandUserMessageCallback> MessageLoop_Wayland::s_userMsgCallbacks;
wl_display* MessageLoop_Wayland::s_pDisplay = nullptr;
wl_registry* MessageLoop_Wayland::s_pRegistry = nullptr;
wl_compositor* MessageLoop_Wayland::s_pCompositor = nullptr;
wl_shm* MessageLoop_Wayland::s_pShm = nullptr;
wl_seat* MessageLoop_Wayland::s_pSeat = nullptr;
wl_output* MessageLoop_Wayland::s_pOutput = nullptr;
xdg_wm_base* MessageLoop_Wayland::s_pXdgWmBase = nullptr;
int MessageLoop_Wayland::s_eventFd = -1;
std::atomic<bool> MessageLoop_Wayland::s_bInitialized(false);
std::vector<MessageLoop_Wayland::UserEvent> MessageLoop_Wayland::s_userEvents;
std::mutex MessageLoop_Wayland::s_userEventMutex;
float MessageLoop_Wayland::s_fDisplayScale = 1.0f;
static bool s_bQuitEventReceived = false;
static std::vector<NativeWindow_SDL*> s_paintWindows;
static std::mutex s_paintWindowsMutex;

// Registry listener
static void registry_global_handler(void* data, wl_registry* registry, uint32_t name,
                                     const char* interface, uint32_t version)
{
    MessageLoop_Wayland::OnRegistryGlobal(data, registry, name, interface, version);
}

static void registry_global_remove_handler(void* data, wl_registry* registry, uint32_t name)
{
    MessageLoop_Wayland::OnRegistryGlobalRemove(data, registry, name);
}

static const wl_registry_listener registry_listener = {
    registry_global_handler,
    registry_global_remove_handler
};

// Output listener for scale
static void output_geometry_handler(void* data, wl_output* output, int32_t x, int32_t y,
                                     int32_t physical_width, int32_t physical_height,
                                     int32_t subpixel, const char* make, const char* model,
                                     int32_t transform)
{
    // Not used
    (void)data; (void)output; (void)x; (void)y;
    (void)physical_width; (void)physical_height; (void)subpixel;
    (void)make; (void)model; (void)transform;
}

static void output_mode_handler(void* data, wl_output* output, uint32_t flags,
                                 int32_t width, int32_t height, int32_t refresh)
{
    (void)data; (void)output; (void)flags; (void)width; (void)height; (void)refresh;
}

static void output_done_handler(void* data, wl_output* output)
{
    (void)data; (void)output;
}

static void output_scale_handler(void* data, wl_output* output, int32_t factor)
{
    MessageLoop_Wayland::s_fDisplayScale = (float)factor;
}

static const wl_output_listener output_listener = {
    output_geometry_handler,
    output_mode_handler,
    output_done_handler,
    output_scale_handler
};

MessageLoop_Wayland::MessageLoop_Wayland()
{
}

MessageLoop_Wayland::~MessageLoop_Wayland()
{
}

// xdg_wm_base ping handler (required by GNOME/mutter)
static void xdg_wm_base_ping_handler(void* data, struct xdg_wm_base* wm_base, uint32_t serial)
{
    xdg_wm_base_pong(wm_base, serial);
    (void)data;
}
static const struct xdg_wm_base_listener xdg_wm_base_listener_impl = {
    .ping = xdg_wm_base_ping_handler,
};

void MessageLoop_Wayland::OnRegistryGlobal(void* data, wl_registry* registry, uint32_t name,
                                            const char* interface, uint32_t version)
{
    (void)data;
    if (strcmp(interface, wl_compositor_interface.name) == 0) {
        s_pCompositor = (wl_compositor*)wl_registry_bind(registry, name,
            &wl_compositor_interface, 4);
    }
    else if (strcmp(interface, wl_shm_interface.name) == 0) {
        s_pShm = (wl_shm*)wl_registry_bind(registry, name,
            &wl_shm_interface, 1);
    }
    else if (strcmp(interface, wl_seat_interface.name) == 0) {
        s_pSeat = (wl_seat*)wl_registry_bind(registry, name,
            &wl_seat_interface, 5);
    }
    else if (strcmp(interface, wl_output_interface.name) == 0) {
        s_pOutput = (wl_output*)wl_registry_bind(registry, name,
            &wl_output_interface, 3);
        wl_output_add_listener(s_pOutput, &output_listener, nullptr);
    }
    else if (strcmp(interface, xdg_wm_base_interface.name) == 0) {
        s_pXdgWmBase = (xdg_wm_base*)wl_registry_bind(registry, name,
            &xdg_wm_base_interface, 1);
        if (s_pXdgWmBase) {
            xdg_wm_base_add_listener(s_pXdgWmBase, &xdg_wm_base_listener_impl, nullptr);
        }
    }
}

void MessageLoop_Wayland::OnRegistryGlobalRemove(void* data, wl_registry* registry, uint32_t name)
{
    (void)data;
    (void)registry;
    (void)name;
}
bool MessageLoop_Wayland::CheckInitWayland()
{
    if (s_bInitialized) {
        return true;
    }

    // Connect to the Wayland display
    s_pDisplay = wl_display_connect(nullptr);
    if (s_pDisplay == nullptr) {
        return false;
    }

    // Get the registry and bind globals
    s_pRegistry = wl_display_get_registry(s_pDisplay);
    if (s_pRegistry == nullptr) {
        wl_display_disconnect(s_pDisplay);
        s_pDisplay = nullptr;
        return false;
    }
    wl_registry_add_listener(s_pRegistry, &registry_listener, nullptr);
    
    // Roundtrip to get all globals
    wl_display_roundtrip(s_pDisplay);

    // Verify required globals are available
    if (s_pCompositor == nullptr || s_pShm == nullptr || s_pXdgWmBase == nullptr) {
        if (s_pCompositor) wl_compositor_destroy(s_pCompositor);
        if (s_pShm) wl_shm_destroy(s_pShm);
        if (s_pSeat) wl_seat_destroy(s_pSeat);
        if (s_pOutput) wl_output_destroy(s_pOutput);
        if (s_pXdgWmBase) xdg_wm_base_destroy(s_pXdgWmBase);
        wl_registry_destroy(s_pRegistry);
        wl_display_disconnect(s_pDisplay);
        s_pDisplay = nullptr;
        s_pRegistry = nullptr;
        s_pCompositor = nullptr;
        s_pShm = nullptr;
        s_pSeat = nullptr;
        s_pOutput = nullptr;
        s_pXdgWmBase = nullptr;
        return false;
    }

    // Create eventfd for custom messages
    s_eventFd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (s_eventFd < 0) {
        return false;
    }

    // Initialize input (seat listeners for pointer and keyboard)
    InitWaylandInput();

    s_bInitialized = true;
    return true;
}

DString MessageLoop_Wayland::GetCurrentVideoDriverName()
{
    return _T("wayland");
}

float MessageLoop_Wayland::GetPrimaryDisplayContentScale()
{
    CheckInitWayland();
    // Do another roundtrip to get output scale if needed
    if (s_pDisplay && s_fDisplayScale == 1.0f) {
        wl_display_roundtrip(s_pDisplay);
    }
    return s_fDisplayScale > 0.0f ? s_fDisplayScale : 1.0f;
}

int32_t MessageLoop_Wayland::Run(MessageLoopIdleCallback idleCallback)
{
    if (!CheckInitWayland()) {
        return -1;
    }

    int displayFd = wl_display_get_fd(s_pDisplay);
    
    // Set up epoll
    int epollFd = epoll_create1(EPOLL_CLOEXEC);
    if (epollFd < 0) {
        return -1;
    }

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = displayFd;
    epoll_ctl(epollFd, EPOLL_CTL_ADD, displayFd, &ev);

    ev.events = EPOLLIN;
    ev.data.fd = s_eventFd;
    epoll_ctl(epollFd, EPOLL_CTL_ADD, s_eventFd, &ev);

    s_bQuitEventReceived = false;

    if (idleCallback == nullptr) {
        // Simple message loop without idle
        while (!s_bQuitEventReceived) {
            // Prepare to read events
            while (wl_display_prepare_read(s_pDisplay) != 0) {
                // Dispatch pending events
                wl_display_dispatch_pending(s_pDisplay);
            }
            wl_display_flush(s_pDisplay);

            // Handle custom user events
            DispatchUserEvents();

            // Check for quit after dispatching user events
            if (s_bQuitEventReceived) {
                wl_display_cancel_read(s_pDisplay);
                break;
            }

            // Wait for events
            struct epoll_event events[2];
            int nfds = epoll_wait(epollFd, events, 2, -1);

            if (nfds < 0) {
                wl_display_cancel_read(s_pDisplay);
                if (errno == EINTR) continue;
                break;
            }

            // Check which fd is ready
            bool displayReady = false;
            bool eventFdReady = false;
            for (int i = 0; i < nfds; i++) {
                if (events[i].data.fd == displayFd) displayReady = true;
                if (events[i].data.fd == s_eventFd) eventFdReady = true;
            }

            if (displayReady) {
                wl_display_read_events(s_pDisplay);
                wl_display_dispatch_pending(s_pDisplay);
            }
            else {
                wl_display_cancel_read(s_pDisplay);
            }

            if (eventFdReady) {
                // Consume the eventfd
                uint64_t val;
                ssize_t ret = read(s_eventFd, &val, sizeof(val));
                (void)ret;
                DispatchUserEvents();
            }

            ProcessWaylandEvents();
            PaintAllWindows();
        }
    }
    else {
        // Message loop with idle callback
        while (!s_bQuitEventReceived) {
            // Process pending Wayland events
            while (wl_display_prepare_read(s_pDisplay) != 0) {
                wl_display_dispatch_pending(s_pDisplay);
            }
            wl_display_flush(s_pDisplay);

            // Handle custom user events
            DispatchUserEvents();

            // Check for quit after dispatching user events
            if (s_bQuitEventReceived) {
                wl_display_cancel_read(s_pDisplay);
                break;
            }

            // Poll with timeout for idle support
            struct epoll_event events[2];
            int nfds = epoll_wait(epollFd, events, 2, 1); // 1ms timeout

            if (nfds < 0) {
                wl_display_cancel_read(s_pDisplay);
                if (errno == EINTR) continue;
                break;
            }

            if (nfds == 0) {
                // Timeout - no events, call idle
                wl_display_cancel_read(s_pDisplay);
                if (!s_bQuitEventReceived) {
                    PaintAllWindows();
                    idleCallback();
                }
                continue;
            }

            bool displayReady = false;
            bool eventFdReady = false;
            for (int i = 0; i < nfds; i++) {
                if (events[i].data.fd == displayFd) displayReady = true;
                if (events[i].data.fd == s_eventFd) eventFdReady = true;
            }

            if (displayReady) {
                wl_display_read_events(s_pDisplay);
                wl_display_dispatch_pending(s_pDisplay);
            }
            else {
                wl_display_cancel_read(s_pDisplay);
            }

            if (eventFdReady) {
                uint64_t val;
                read(s_eventFd, &val, sizeof(val));
                DispatchUserEvents();
            }

            if (!s_bQuitEventReceived) {
                PaintAllWindows();
                idleCallback();
            }
        }
    }

    close(epollFd);
    
    // Cleanup Wayland resources
    if (s_pXdgWmBase) { xdg_wm_base_destroy(s_pXdgWmBase); s_pXdgWmBase = nullptr; }
    if (s_pOutput) { wl_output_destroy(s_pOutput); s_pOutput = nullptr; }
    if (s_pSeat) { wl_seat_destroy(s_pSeat); s_pSeat = nullptr; }
    if (s_pShm) { wl_shm_destroy(s_pShm); s_pShm = nullptr; }
    if (s_pCompositor) { wl_compositor_destroy(s_pCompositor); s_pCompositor = nullptr; }
    if (s_pRegistry) { wl_registry_destroy(s_pRegistry); s_pRegistry = nullptr; }
    if (s_pDisplay) { wl_display_disconnect(s_pDisplay); s_pDisplay = nullptr; }
    if (s_eventFd >= 0) { close(s_eventFd); s_eventFd = -1; }

    ShutdownWaylandInput();
    s_bInitialized = false;
    return 0;
}

void MessageLoop_Wayland::ProcessWaylandEvents()
{
    // Check for quit condition - currently handled by the run loop directly
}

void MessageLoop_Wayland::RunDoModal(NativeWindow_SDL& nativeWindow, bool bCloseByEsc, bool bCloseByEnter)
{
    // Will be implemented when NativeWindow_SDL is available
    (void)nativeWindow;
    (void)bCloseByEsc;
    (void)bCloseByEnter;
}

void MessageLoop_Wayland::RunUserLoop(bool& bTerminate)
{
    if (bTerminate) {
        return;
    }
    // Process events until terminate is set
    while (!bTerminate) {
        if (s_pDisplay) {
            wl_display_dispatch_pending(s_pDisplay);
            wl_display_flush(s_pDisplay);
        }
        DispatchUserEvents();
        
        // Small sleep to avoid busy-waiting
        if (!bTerminate) {
            struct timespec ts = {0, 1000000}; // 1ms
            nanosleep(&ts, nullptr);
        }
    }
}

void MessageLoop_Wayland::RemoveDuplicateMsg(uint32_t msgId)
{
    std::lock_guard<std::mutex> lock(s_userEventMutex);
    s_userEvents.erase(
        std::remove_if(s_userEvents.begin(), s_userEvents.end(),
            [msgId](const UserEvent& e) { return e.msgId == msgId; }),
        s_userEvents.end()
    );
}

bool MessageLoop_Wayland::PostUserEvent(uint32_t msgId, WPARAM wParam, LPARAM lParam)
{
    {
        std::lock_guard<std::mutex> lock(s_userEventMutex);
        UserEvent ev;
        ev.msgId = msgId;
        ev.wParam = wParam;
        ev.lParam = lParam;
        s_userEvents.push_back(ev);
    }
    
    // Wake up the event loop
    if (s_eventFd >= 0) {
        uint64_t val = 1;
        write(s_eventFd, &val, sizeof(val));
    }
    return true;
}

void MessageLoop_Wayland::AddUserMessageCallback(uint32_t msgId, const WaylandUserMessageCallback& callback)
{
    s_userMsgCallbacks[msgId] = callback;
}

void MessageLoop_Wayland::RemoveUserMessageCallback(uint32_t msgId)
{
    auto iter = s_userMsgCallbacks.find(msgId);
    if (iter != s_userMsgCallbacks.end()) {
        s_userMsgCallbacks.erase(iter);
    }
}

void MessageLoop_Wayland::PostNoneEvent()
{
    PostUserEvent(0x8000 + 0, 0, 0);
}

#define WM_QUIT_EVENT_ID (0x8000 + 9999)

void MessageLoop_Wayland::PostQuitEvent()
{
    s_bQuitEventReceived = true;
    PostUserEvent(WM_QUIT_EVENT_ID, 0, 0);
}

bool MessageLoop_Wayland::IsQuitEventReceived()
{
    return s_bQuitEventReceived;
}

void MessageLoop_Wayland::DispatchUserEvents()
{
    std::vector<UserEvent> events;
    {
        std::lock_guard<std::mutex> lock(s_userEventMutex);
        events.swap(s_userEvents);
    }
    
    for (const auto& ev : events) {
        auto iter = s_userMsgCallbacks.find(ev.msgId);
        if (iter != s_userMsgCallbacks.end() && iter->second) {
            iter->second(ev.msgId, ev.wParam, ev.lParam);
        }
    }
}

wl_display* MessageLoop_Wayland::GetDisplay()
{
    return s_pDisplay;
}

wl_compositor* MessageLoop_Wayland::GetCompositor()
{
    return s_pCompositor;
}

wl_shm* MessageLoop_Wayland::GetShm()
{
    return s_pShm;
}

wl_seat* MessageLoop_Wayland::GetSeat()
{
    return s_pSeat;
}

xdg_wm_base* MessageLoop_Wayland::GetXdgWmBase()
{
    return s_pXdgWmBase;
}

wl_output* MessageLoop_Wayland::GetOutput()
{
    return s_pOutput;
}

void MessageLoop_Wayland::Flush()
{
    if (s_pDisplay) {
        wl_display_flush(s_pDisplay);
    }
}

void MessageLoop_Wayland::RegisterPaintWindow(NativeWindow_SDL* window)
{
    std::lock_guard<std::mutex> lock(s_paintWindowsMutex);
    for (auto* w : s_paintWindows) {
        if (w == window) return;
    }
    s_paintWindows.push_back(window);
}

void MessageLoop_Wayland::UnregisterPaintWindow(NativeWindow_SDL* window)
{
    std::lock_guard<std::mutex> lock(s_paintWindowsMutex);
    s_paintWindows.erase(
        std::remove(s_paintWindows.begin(), s_paintWindows.end(), window),
        s_paintWindows.end()
    );
}

void MessageLoop_Wayland::PaintAllWindows()
{
    std::lock_guard<std::mutex> lock(s_paintWindowsMutex);
    for (auto* window : s_paintWindows) {
        if (window && window->IsWindowVisible() && !window->IsClosingWnd()) {
            window->PaintWindow(false);
        }
    }
}

} // namespace ui

#endif // DUI_BUILD_FOR_WAYLAND
