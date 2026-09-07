#ifndef UI_CORE_MESSAGE_LOOP_X11_H_
#define UI_CORE_MESSAGE_LOOP_X11_H_
#include "dui/dui_defs.h"
#include <functional>
#include <unordered_map>
#include <vector>
#include <mutex>
#include <atomic>
#ifdef DUI_BUILD_FOR_X11
struct _XDisplay;
typedef _XDisplay Display;
namespace ui {
class NativeWindow_X11;
using X11UserMessageCallback = std::function<void(uint32_t, WPARAM, LPARAM)>;
class DUI_API MessageLoop_X11 {
public:
    using MessageLoopIdleCallback = std::function<void()>;
    MessageLoop_X11() = default;
    ~MessageLoop_X11() = default;
    MessageLoop_X11(const MessageLoop_X11&) = delete;
    MessageLoop_X11& operator=(const MessageLoop_X11&) = delete;
    int32_t Run(MessageLoopIdleCallback idleCallback);
    void RunDoModal(NativeWindow_X11&, bool = true, bool = false);
    void RunUserLoop(bool& bTerminate);
    static bool CheckInitX11();
    static void RemoveDuplicateMsg(uint32_t);
    static bool PostUserEvent(uint32_t, WPARAM, LPARAM);
    static void AddUserMessageCallback(uint32_t, const X11UserMessageCallback&);
    static void RemoveUserMessageCallback(uint32_t);
    static void PostNoneEvent();
    static void PostQuitEvent();
    static bool IsQuitEventReceived();
    static DString GetCurrentVideoDriverName();
    static float GetPrimaryDisplayContentScale();
    static Display* GetDisplay();
    static void RegisterPaintWindow(NativeWindow_X11*);
    static void UnregisterPaintWindow(NativeWindow_X11*);
    static void PaintAllWindows();
    static void Flush();
private:
    struct UserEvent { uint32_t msgId; WPARAM wParam; LPARAM lParam; };
    static void DispatchX11Events();
    static void DispatchUserEvents();
    static std::unordered_map<uint32_t, X11UserMessageCallback> s_callbacks;
    static std::vector<UserEvent> s_events;
    static std::mutex s_eventMutex;
    static std::vector<NativeWindow_X11*> s_windows;
    static std::mutex s_windowMutex;
    static Display* s_display;
    static std::atomic<bool> s_initialized;
    static std::atomic<bool> s_quit;
};
}
#endif
#endif
