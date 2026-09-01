#include "dui/Core/MessageLoop_MacOS.h"
#include "dui/Core/NativeWindow_MacOS.h"

#if defined(DUI_BUILD_FOR_MACOS)

#include "dui/Control/Menu.h"
#include "dui/Core/ScopedLock.h"

#import <Cocoa/Cocoa.h>

#include <mutex>
#include <queue>

namespace ui {

/** A queued user message (posted from any thread)
*/
struct MacOSUserMsg
{
    uint32_t msgId = 0;
    WPARAM wParam = 0;
    LPARAM lParam = 0;
};

static std::mutex s_userMsgMutex;
static std::queue<MacOSUserMsg> s_userMsgQueue;

std::unordered_map<uint32_t, MacOSUserMessageCallback> MessageLoop_MacOS::s_userMsgCallbacks;
bool MessageLoop_MacOS::s_bQuit = false;
int32_t MessageLoop_MacOS::s_nExitCode = 0;

MessageLoop_MacOS::MessageLoop_MacOS()
{
    //Do NOT reset the static s_bQuit here: a modal loop constructs a fresh
    //MessageLoop_MacOS (DoModal) and must not clear a quit already posted for
    //the main loop. The quit flag is reset at the top of Run() only.
}

MessageLoop_MacOS::~MessageLoop_MacOS()
{
}

int32_t MessageLoop_MacOS::Run(MessageLoopIdleCallback idleCallback)
{
    CheckInitMacOS();
    s_bQuit = false;

    while (!s_bQuit) {
        bool bProcessedEvent = false;
        if (idleCallback != nullptr) {
            //The app's idle callback drives continuous drawing (e.g. the
            //child-window paint demo); a long blocking wait here throttles
            //every child window's FPS, so don't wait for an event.
            //Timers are still serviced because nextEventMatchingMask spins
            //the run loop while it waits - that keeps the window display
            //timers working with this custom pump.
            bProcessedEvent = RunOneEventLoopIterationWithWait(0.0);
            ProcessUserMessages();
            //Always service continuous idle work; the continuous paint demo
            //needs to run even when native/timer events are being delivered.
            idleCallback();
        }
        else {
            //No idle work: block longer so nothing is done burns no CPU
            bProcessedEvent = RunOneEventLoopIterationWithWait(0.01);
            ProcessUserMessages();
        }
    }
    return s_nExitCode;
}

void MessageLoop_MacOS::RunDoModal(NativeWindow_MacOS& nativeWindow, bool /*bCloseByEsc*/, bool /*bCloseByEnter*/)
{
    CheckInitMacOS();
    while (!s_bQuit && nativeWindow.IsWindow() && !nativeWindow.IsClosingWnd()) {
        RunOneEventLoopIteration();
        ProcessUserMessages();
    }
}

void MessageLoop_MacOS::RunUserLoop(bool& bTerminate)
{
    CheckInitMacOS();
    while (!s_bQuit && !bTerminate) {
        RunOneEventLoopIteration();
        ProcessUserMessages();
    }
}

bool MessageLoop_MacOS::RunOneEventLoopIteration()
{
    return RunOneEventLoopIterationWithWait(0.01);
}

bool MessageLoop_MacOS::RunOneEventLoopIterationWithWait(double waitSeconds)
{
    //Do NOT service the run loop via runMode:beforeDate: here - in a custom
    //(non-[NSApp run]) loop that call dequeues NSEvents without dispatching them
    //(they never reach the windows). Block briefly on nextEventMatchingMask and
    //dispatch explicitly, exactly like SDL's macOS event pump / [NSApp run] do.
    NSEvent* event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                        untilDate:[NSDate dateWithTimeIntervalSinceNow:waitSeconds]
                                           inMode:NSDefaultRunLoopMode
                                          dequeue:YES];
    if (event != nil) {
        [NSApp sendEvent:event];
        return true;
    }
    return false;
}

void MessageLoop_MacOS::ProcessUserMessages()
{
    MacOSUserMsg msg;
    bool bHasMsg = false;
    {
        ScopedLock threadGuard(s_userMsgMutex);
        if (!s_userMsgQueue.empty()) {
            msg = s_userMsgQueue.front();
            s_userMsgQueue.pop();
            bHasMsg = true;
        }
    }
    if (bHasMsg) {
        OnUserEvent(msg.msgId, msg.wParam, msg.lParam);
    }
}

void MessageLoop_MacOS::OnUserEvent(uint32_t msgId, WPARAM wParam, LPARAM lParam)
{
    MacOSUserMessageCallback callback;
    {
        ScopedLock threadGuard(s_userMsgMutex);
        auto iter = s_userMsgCallbacks.find(msgId);
        if (iter != s_userMsgCallbacks.end()) {
            callback = iter->second;
        }
    }
    if (callback) {
        callback(msgId, wParam, lParam);
    }
}

void MessageLoop_MacOS::RemoveDuplicateMsg(uint32_t /*msgId*/)
{
    //macOS has no message queue deduplication; no-op
}

bool MessageLoop_MacOS::PostUserEvent(uint32_t msgId, WPARAM wParam, LPARAM lParam)
{
    MacOSUserMsg msg;
    msg.msgId = msgId;
    msg.wParam = wParam;
    msg.lParam = lParam;
    {
        ScopedLock threadGuard(s_userMsgMutex);
        s_userMsgQueue.push(msg);
    }
    //Wake the run loop so the message is processed promptly
    PostNoneEvent();
    return true;
}

void MessageLoop_MacOS::AddUserMessageCallback(uint32_t msgId, const MacOSUserMessageCallback& callback)
{
    ScopedLock threadGuard(s_userMsgMutex);
    s_userMsgCallbacks[msgId] = callback;
}

void MessageLoop_MacOS::RemoveUserMessageCallback(uint32_t msgId)
{
    ScopedLock threadGuard(s_userMsgMutex);
    auto iter = s_userMsgCallbacks.find(msgId);
    if (iter != s_userMsgCallbacks.end()) {
        s_userMsgCallbacks.erase(iter);
    }
}

void MessageLoop_MacOS::PostNoneEvent()
{
    //Wake the run loop from a blocking wait (runMode with distantPast never blocks,
    //so this is only needed to break out of other modal run loops)
    CFRunLoopWakeUp(CFRunLoopGetMain());
}

void MessageLoop_MacOS::PostQuitMsg(int32_t nExitCode)
{
    s_nExitCode = nExitCode;
    s_bQuit = true;
}

bool MessageLoop_MacOS::CheckInitMacOS()
{
    static bool s_bInitialized = false;
    if (s_bInitialized) {
        return true;
    }
    @autoreleasepool {
        NSApplication* app = [NSApplication sharedApplication];
        [app setActivationPolicy:NSApplicationActivationPolicyRegular];
        [app finishLaunching];
        //Bring the app to the front so it receives events when launched from a terminal
        [app activateIgnoringOtherApps:YES];
        //Close dui popup menus when the user switches to another application.
        [[NSNotificationCenter defaultCenter] addObserverForName:NSApplicationDidResignActiveNotification
                                                         object:nil
                                                          queue:[NSOperationQueue mainQueue]
                                                     usingBlock:^(NSNotification* /*note*/) {
            ui::Menu::CloseAllMenus();
        }];
        //Close submenus as soon as the cursor leaves their window.
        static id s_menuMouseMonitor = nil;
        s_menuMouseMonitor = [NSEvent addLocalMonitorForEventsMatchingMask:NSEventMaskMouseMoved handler:^NSEvent* (NSEvent* event) {
            ui::Menu::CloseSubmenusOutsidePointer();
            return event;
        }];
        static id s_menuClickMonitor = nil;
        s_menuClickMonitor = [NSEvent addLocalMonitorForEventsMatchingMask:(NSEventMaskLeftMouseDown |
                                                                              NSEventMaskRightMouseDown |
                                                                              NSEventMaskOtherMouseDown)
                                                                  handler:^NSEvent* (NSEvent* event) {
            //A click outside a menu must dismiss the complete menu chain
            //before the target control processes the click.
            if (!ui::Menu::IsOpenMenuWindow((__bridge void*)event.window)) {
                ui::Menu::CloseAllMenus();
            }
            return event;
        }];
    }
    s_bInitialized = true;
    return true;
}

DString MessageLoop_MacOS::GetCurrentVideoDriverName()
{
    return _T("cocoa");
}

float MessageLoop_MacOS::GetPrimaryDisplayContentScale()
{
    NSScreen* screen = [NSScreen mainScreen];
    if (screen == nullptr) {
        return 1.0f;
    }
    return (float)screen.backingScaleFactor;
}

} // namespace ui

#endif // DUI_BUILD_FOR_MACOS
