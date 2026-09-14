#ifndef UI_CORE_MESSAGE_LOOP_MACOS_H_
#define UI_CORE_MESSAGE_LOOP_MACOS_H_

#include "dui/dui_defs.h"
#include <functional>
#include <unordered_map>

#if defined(DUI_BUILD_FOR_MACOS)

namespace ui {

class NativeWindow_MacOS;

/** Prototype of the custom message callback function: void FunctionName(uint32_t msgID, WPARAM wParam, LPARAM lParam);
*/
typedef std::function<void(uint32_t msgID, WPARAM wParam, LPARAM lParam)> MacOSUserMessageCallback;

/** Message loop of the main thread (macOS native, AppKit event loop)
*/
class DUI_API MessageLoop_MacOS
{
public:
    MessageLoop_MacOS();
    MessageLoop_MacOS(const MessageLoop_MacOS& r) = delete;
    MessageLoop_MacOS& operator = (const MessageLoop_MacOS& r) = delete;
    ~MessageLoop_MacOS();

public:
    /** Declaration of the Idle function
    */
    using MessageLoopIdleCallback = std::function<void()>;

public:
    /** Run the message loop
    * @param [in] idleCallback Callback function for the Idle state, can be nullptr
    */
    int32_t Run(MessageLoopIdleCallback idleCallback);

public:
    /** Run the message loop of a modal window until the window exits
    * @param [in] nativeWindow The window to wait for exit
    * @param [in] bCloseByEsc Whether to close the window when the ESC key is pressed
    * @param [in] bCloseByEnter Whether to close the window when the Enter key is pressed
    */
    void RunDoModal(NativeWindow_MacOS& nativeWindow, bool bCloseByEsc = true, bool bCloseByEnter = false);

    /** Run a user message loop until the exit condition is reached
    * @param [in] bTerminate true means to exit the message loop
    */
    void RunUserLoop(bool& bTerminate);

public:
    /** Remove duplicate messages from the message queue (no-op on macOS)
    * @param [in] msgId The message ID
    */
    static void RemoveDuplicateMsg(uint32_t msgId);

    /** Send a message to the message queue
    * @param [in] msgId The message ID
    * @param [in] wParam The first parameter of the message
    * @param [in] lParam The second parameter of the message
    */
    static bool PostUserEvent(uint32_t msgId, WPARAM wParam, LPARAM lParam);

    /** Set the callback function of a custom message
    * @param [in] msgId The message ID
    * @param [in] callback The callback function
    */
    static void AddUserMessageCallback(uint32_t msgId, const MacOSUserMessageCallback& callback);

    /** Delete the callback function of a custom message
    * @param [in] msgId The message ID
    */
    static void RemoveUserMessageCallback(uint32_t msgId);

    /** Put an empty message into the queue (wakes a pending run loop iteration)
    */
    static void PostNoneEvent();

    /** Request the message loop to exit
    * @param [in] nExitCode The exit code
    */
    static void PostQuitMsg(int32_t nExitCode);

    /** Initialize NSApplication (called once on the main thread)
    */
    static bool CheckInitMacOS();

    /** Return the current video driver name
    */
    static std::string GetCurrentVideoDriverName();

    /** Get the content display scale of the primary display
    */
    static float GetPrimaryDisplayContentScale();

    /** Dispatch one queued user message (called by the run loop)
    */
    static void ProcessUserMessages();

private:
    /** Handle user-defined messages
    */
    static void OnUserEvent(uint32_t msgId, WPARAM wParam, LPARAM lParam);

    /** Run one iteration of the AppKit event loop (default mode)
    * @return true if an event was processed
    */
    static bool RunOneEventLoopIteration();

private:
    /** Run one iteration of the AppKit event loop, blocking at most
    * waitSeconds for an event to arrive (run-loop timers stay serviced while
    * it waits)
    * @param [in] waitSeconds Maximum time to wait for an event
    * @return true if an event was processed
    */
    static bool RunOneEventLoopIterationWithWait(double waitSeconds);

private:
    /** Mapping of custom messages
    */
    static std::unordered_map<uint32_t, MacOSUserMessageCallback> s_userMsgCallbacks;

    /** Whether the message loop has been requested to quit
    */
    static bool s_bQuit;

    /** The message loop exit code
    */
    static int32_t s_nExitCode;
};

} // namespace ui

#endif // DUI_BUILD_FOR_MACOS

#endif // UI_CORE_MESSAGE_LOOP_MACOS_H_
