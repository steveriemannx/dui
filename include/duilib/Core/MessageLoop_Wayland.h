#ifndef UI_CORE_MESSAGE_LOOP_WAYLAND_H_
#define UI_CORE_MESSAGE_LOOP_WAYLAND_H_

#include "duilib/duilib_defs.h"
#include <functional>
#include <unordered_map>
#include <atomic>

#ifdef DUILIB_BUILD_FOR_WAYLAND

// Forward declarations
struct wl_display;
struct wl_registry;
struct wl_compositor;
struct wl_shm;
struct wl_seat;
struct wl_output;
struct xdg_wm_base;

namespace ui {

class NativeWindow_SDL;

/** Custom message callback prototype: void FunctionName(uint32_t msgID, WPARAM wParam, LPARAM lParam);
*/
typedef std::function<void(uint32_t msgID, WPARAM wParam, LPARAM lParam)> WaylandUserMessageCallback;

/** Main thread message loop using Wayland
*/
class DUILIB_API MessageLoop_Wayland
{
public:
    MessageLoop_Wayland();
    MessageLoop_Wayland(const MessageLoop_Wayland& r) = delete;
    MessageLoop_Wayland& operator = (const MessageLoop_Wayland& r) = delete;
    ~MessageLoop_Wayland();

public:
    /** Idle callback type
    * @return void
    */
    using MessageLoopIdleCallback = std::function<void()>;

public:
    /** Run the message loop
    * @param [in] idleCallback Idle callback, can be nullptr
    */
    int32_t Run(MessageLoopIdleCallback idleCallback);

public:
    /** Run modal window message loop until the window exits
    * @param [in] nativeWindow The window to wait for
    * @param [in] bCloseByEsc Close window on ESC key
    * @param [in] bCloseByEnter Close window on ENTER key
    */
    void RunDoModal(NativeWindow_SDL& nativeWindow, bool bCloseByEsc = true, bool bCloseByEnter = false);

    /** Run a user message loop until exit condition is met
    * @param [in,out] bTerminate Set to true to exit the loop
    */
    void RunUserLoop(bool& bTerminate);

public:
    /** Remove duplicate messages from the queue
    * @param [in] msgId Message ID
    */
    static void RemoveDuplicateMsg(uint32_t msgId);

    /** Post a message to the queue
    * @param [in] msgId Message ID
    * @param [in] wParam Message parameter 1
    * @param [in] lParam Message parameter 2
    */
    static bool PostUserEvent(uint32_t msgId, WPARAM wParam, LPARAM lParam);

    /** Set custom message callback
    * @param [in] msgId Message ID
    * @param [in] callback Callback function
    */
    static void AddUserMessageCallback(uint32_t msgId, const WaylandUserMessageCallback& callback);

    /** Remove custom message callback
    * @param [in] msgId Message ID
    */
    static void RemoveUserMessageCallback(uint32_t msgId);

    /** Message ID for paint events
    */
    static constexpr uint32_t WM_PAINT_EVENT = 0x8000 + 2;

    /** Post an empty event to wake up the message loop
    */
    static void PostNoneEvent();

    /** Post a quit event to stop the message loop
    */
    static void PostQuitEvent();

    /** Check if the quit event has been received
    */
    static bool IsQuitEventReceived();

    /** Initialize Wayland connection and input
    * @return true on success
    */
    static bool CheckInitWayland();

    /** Get the current video driver name
    */
    static DString GetCurrentVideoDriverName();

    /** Get primary display content scale
    */
    static float GetPrimaryDisplayContentScale();

public:
    /** Get the wl_display (for use by other components)
    */
    static wl_display* GetDisplay();

    /** Get the wl_compositor (for use by other components)
    */
    static wl_compositor* GetCompositor();

    /** Get the wl_shm (for use by other components)
    */
    static wl_shm* GetShm();

    /** Get the wl_seat (for use by other components)
    */
    static wl_seat* GetSeat();

    /** Get the xdg_wm_base (for use by other components)
    */
    static xdg_wm_base* GetXdgWmBase();

    /** Get the wl_output (for use by other components)
    */
    static wl_output* GetOutput();

    /** Flush display events
    */
    static void Flush();

    /** Register/unregister a window for idle painting
    */
    static void RegisterPaintWindow(NativeWindow_SDL* window);
    static void UnregisterPaintWindow(NativeWindow_SDL* window);

    /** Paint all registered windows (called during idle)
    */
    static void PaintAllWindows();

    /** Initialize Wayland input (pointer, keyboard, seat listeners)
    */
    static void InitWaylandInput();

    /** Shutdown Wayland input
    */
    static void ShutdownWaylandInput();

    /** Handle registry global event (public for C callback access)
    */
    static void OnRegistryGlobal(void* data, wl_registry* registry, uint32_t name, const char* interface, uint32_t version);

    /** Handle registry global remove event (public for C callback access)
    */
    static void OnRegistryGlobalRemove(void* data, wl_registry* registry, uint32_t name);

private:
    /** Process pending Wayland events
    */
    static void ProcessWaylandEvents();

    /** Dispatch user-defined events from our internal queue
    */
    static void DispatchUserEvents();

public:
    // Static state (public for C callback access)
    static float s_fDisplayScale;

private:
    /** Custom message mapping
    */
    static std::unordered_map<uint32_t, WaylandUserMessageCallback> s_userMsgCallbacks;

    /** Global Wayland state
    */
    static wl_display* s_pDisplay;
    static wl_registry* s_pRegistry;
    static wl_compositor* s_pCompositor;
    static wl_shm* s_pShm;
    static wl_seat* s_pSeat;
    static wl_output* s_pOutput;
    static xdg_wm_base* s_pXdgWmBase;
    
    /** Event fd for waking up the event loop with custom messages
    */
    static int s_eventFd;
    
    /** Whether Wayland has been initialized
    */
    static std::atomic<bool> s_bInitialized;

    /** User event queue for custom messages
    */
    struct UserEvent {
        uint32_t msgId;
        WPARAM wParam;
        LPARAM lParam;
    };
    static std::vector<UserEvent> s_userEvents;
    static std::mutex s_userEventMutex;


};

} // namespace ui

// Input-related functions (implemented in Input_Wayland.cpp)
namespace ui {
    void RegisterWaylandSurface(struct wl_surface* surface, class NativeWindow_SDL* window);
    void UnregisterWaylandSurface(struct wl_surface* surface);
}

#endif // DUILIB_BUILD_FOR_WAYLAND

#endif // UI_CORE_MESSAGE_LOOP_WAYLAND_H_
