#include "dui/Core/MessageLoop_X11.h"
#include "dui/Core/NativeWindow_X11.h"
#ifdef DUI_BUILD_FOR_X11
#include <X11/Xlib.h>
#include <algorithm>
#include <chrono>
#include <thread>
namespace ui {
std::unordered_map<uint32_t, X11UserMessageCallback> MessageLoop_X11::s_callbacks;
std::vector<MessageLoop_X11::UserEvent> MessageLoop_X11::s_events;
std::mutex MessageLoop_X11::s_eventMutex;
std::vector<NativeWindow_X11*> MessageLoop_X11::s_windows;
std::mutex MessageLoop_X11::s_windowMutex;
Display* MessageLoop_X11::s_display = nullptr;
std::atomic<bool> MessageLoop_X11::s_initialized{false};
std::atomic<bool> MessageLoop_X11::s_quit{false};
bool MessageLoop_X11::CheckInitX11() { if (s_initialized) return true; s_display = XOpenDisplay(nullptr); if (!s_display) return false; s_quit = false; s_initialized = true; return true; }
Display* MessageLoop_X11::GetDisplay() { return s_display; }
std::string MessageLoop_X11::GetCurrentVideoDriverName() { return "x11"; }
float MessageLoop_X11::GetPrimaryDisplayContentScale() { return 1.0f; }
int32_t MessageLoop_X11::Run(MessageLoopIdleCallback idle) { if (!CheckInitX11()) return -1; s_quit = false; while (!s_quit) { DispatchX11Events(); DispatchUserEvents(); PaintAllWindows(); if (idle) idle(); std::this_thread::sleep_for(std::chrono::milliseconds(8)); } return 0; }
void MessageLoop_X11::RunDoModal(NativeWindow_X11& w, bool, bool) { while (!w.IsClosingWnd() && !s_quit) { bool terminate = s_quit; RunUserLoop(terminate); } }
void MessageLoop_X11::RunUserLoop(bool& terminate) { DispatchX11Events(); DispatchUserEvents(); PaintAllWindows(); Flush(); if (terminate) s_quit = true; std::this_thread::sleep_for(std::chrono::milliseconds(8)); }
bool MessageLoop_X11::PostUserEvent(uint32_t id, WPARAM w, LPARAM l) { std::lock_guard<std::mutex> lock(s_eventMutex); s_events.push_back({id,w,l}); return true; }
void MessageLoop_X11::AddUserMessageCallback(uint32_t id, const X11UserMessageCallback& cb) { s_callbacks[id] = cb; }
void MessageLoop_X11::RemoveUserMessageCallback(uint32_t id) { s_callbacks.erase(id); }
void MessageLoop_X11::DispatchX11Events() { if (!s_display) return; while (XPending(s_display) > 0) { XEvent event; XNextEvent(s_display, &event); NativeWindow_X11* window = NativeWindow_X11::GetWindowFromID(event.xany.window); if (window != nullptr) window->OnX11WindowEvent(event); } }
void MessageLoop_X11::RemoveDuplicateMsg(uint32_t id) { std::lock_guard<std::mutex> lock(s_eventMutex); s_events.erase(std::remove_if(s_events.begin(),s_events.end(),[id](const UserEvent& e){return e.msgId==id;}),s_events.end()); }
void MessageLoop_X11::DispatchUserEvents() { std::vector<UserEvent> events; { std::lock_guard<std::mutex> lock(s_eventMutex); events.swap(s_events); } for (const auto& e: events) { auto i=s_callbacks.find(e.msgId); if(i!=s_callbacks.end()) i->second(e.msgId,e.wParam,e.lParam); } }
void MessageLoop_X11::PostNoneEvent() { PostUserEvent(kWM_USER,0,0); }
void MessageLoop_X11::PostQuitEvent() { s_quit = true; }
bool MessageLoop_X11::IsQuitEventReceived() { return s_quit; }
void MessageLoop_X11::Flush() { if(s_display) XFlush(s_display); }
void MessageLoop_X11::RegisterPaintWindow(NativeWindow_X11* w) { std::lock_guard<std::mutex> l(s_windowMutex); s_windows.push_back(w); }
void MessageLoop_X11::UnregisterPaintWindow(NativeWindow_X11* w) { std::lock_guard<std::mutex> l(s_windowMutex); s_windows.erase(std::remove(s_windows.begin(),s_windows.end(),w),s_windows.end()); }
void MessageLoop_X11::PaintAllWindows() { std::lock_guard<std::mutex> l(s_windowMutex); for(auto* w:s_windows) if(w&& !w->IsClosingWnd() && w->IsWindowVisible()) w->PaintWindow(false); }
}
#endif
