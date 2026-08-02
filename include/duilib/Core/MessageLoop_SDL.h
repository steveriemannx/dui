#ifndef UI_CORE_MESSAGE_LOOP_SDL_H_
#define UI_CORE_MESSAGE_LOOP_SDL_H_

#include "duilib/duilib_defs.h"
#include <functional>
#include <unordered_map>

#if defined(DUILIB_BUILD_FOR_SDL)

union SDL_Event;

namespace ui {

class NativeWindow_SDL;

/** Prototype of the custom message callback function: void FunctionName(uint32_t msgID, WPARAM wParam, LPARAM lParam);
*/
typedef std::function<void(uint32_t msgID, WPARAM wParam, LPARAM lParam)> SDLUserMessageCallback;

/** Message loop of the main thread
*/
class DUILIB_API MessageLoop_SDL
{
public:
    MessageLoop_SDL();
    MessageLoop_SDL(const MessageLoop_SDL& r) = delete;
    MessageLoop_SDL& operator = (const MessageLoop_SDL& r) = delete;
    ~MessageLoop_SDL();

public:
    /** Declaration of the Idle function
    * @return Returns true if a message was sent to the message queue and needs to be processed immediately; returns false if no message was sent to the message queue
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
    void RunDoModal(NativeWindow_SDL& nativeWindow, bool bCloseByEsc = true, bool bCloseByEnter = false);

    /** Run a user message loop until the exit condition is reached
    * @param [in] bTerminate true means to exit the message loop, false means to keep running the message loop; the initial value should be false
    */
    void RunUserLoop(bool& bTerminate);

public:
    /** Remove duplicate messages from the message queue
    * @param [in] msgId The message ID
    */
    static void RemoveDuplicateMsg(uint32_t msgId);

    /** Send a message to the message queue
    * @param [in] msgId The message ID; the ID must be between SDL_EVENT_USER and SDL_EVENT_LAST
    * @param [in] wParam The first parameter of the message
    * @param [in] lParam The second parameter of the message
    */
    static bool PostUserEvent(uint32_t msgId, WPARAM wParam, LPARAM lParam);

    /** Set the callback function of a custom message
    * @param [in] msgId The message ID
    * @param [in] callback The callback function
    */
    static void AddUserMessageCallback(uint32_t msgId, const SDLUserMessageCallback& callback);

    /** Delete the callback function of a custom message
    * @param [in] msgId The message ID
    */
    static void RemoveUserMessageCallback(uint32_t msgId);

    /** Put an empty message into the queue, returning to the handler that waits on the queue
    */
    static void PostNoneEvent();

    /** Initialize SDL
    * @param [in] videoDriverName The name of the display driver, valid values are:
      Windows platform: "windows"
      Linux platform: "X11" or "wayland" or "wayland,X11" or "X11,wayland"
    */
    static bool CheckInitSDL(const DString& videoDriverName = _T(""));

    /** Return the current VideoDriver name
    */
    static DString GetCurrentVideoDriverName();

    /** Get the content display scale of the primary display
    */
    static float GetPrimaryDisplayContentScale();

private:
    /** Handle one SDL event in the queue (a sub-function of the message loop)
    */
    static void ProcessSDLEvent(const SDL_Event& sdlEvent, bool& bKeepGoing);

    /** Dispatch an SDL event
    */
    static void DispatchSDLEvent(const SDL_Event& sdlEvent);

    /** Handle user-defined messages
    */
    static void OnUserEvent(const SDL_Event& sdlEvent);

private:
    /** Mapping of custom messages
    */
    static std::unordered_map<uint32_t, SDLUserMessageCallback> s_userMsgCallbacks;
};

} // namespace ui

#endif // DUILIB_BUILD_FOR_SDL

#endif // UI_CORE_MESSAGE_LOOP_SDL_H_
