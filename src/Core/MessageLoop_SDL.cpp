#include "duilib/Core/MessageLoop_SDL.h"
#include "duilib/Utils/StringConvert.h"

#if defined(DUILIB_BUILD_FOR_SDL)

#include "duilib/Core/NativeWindow_SDL.h"
#include <SDL3/SDL.h>

namespace ui
{
std::unordered_map<uint32_t, SDLUserMessageCallback> MessageLoop_SDL::s_userMsgCallbacks;

MessageLoop_SDL::MessageLoop_SDL()
{
}

MessageLoop_SDL::~MessageLoop_SDL()
{
}

bool MessageLoop_SDL::CheckInitSDL(const DString& videoDriverName)
{
    //Support DPI adaptation under Linux Wayland
    SDL_SetHint(SDL_HINT_VIDEO_WAYLAND_SCALE_TO_DISPLAY, "true");

    //Initialize SDL
    bool bRet = true;
    if (!SDL_WasInit(SDL_INIT_VIDEO)) {
        if (!videoDriverName.empty()) {
            DStringA videoDriverNameA = StringConvert::TToUTF8(videoDriverName);
            if (!videoDriverNameA.empty()) {
                SDL_SetHint(SDL_HINT_VIDEO_DRIVER, videoDriverNameA.c_str());
            }            
        }
        bRet = SDL_Init(SDL_INIT_VIDEO);
        ASSERT_UNUSED_VARIABLE(bRet);

        //The range of custom messages to handle (32 IDs are occupied internally, for backup)
        uint32_t nBaseId = SDL_RegisterEvents(32);
        ASSERT_UNUSED_VARIABLE(nBaseId == SDL_EVENT_USER);

        //Usage of custom messages:
        //1. SDL_EVENT_USER + 0:  occupied by the MessageLoop_SDL::PostNoneEvent function
        //2. SDL_EVENT_USER + 1:  occupied by the WM_USER_DEFINED_MSG message in duilib\Core\FrameworkThread.cpp
        //3. SDL_EVENT_USER + 2:  occupied by WM_USER_DEFINED_TIMER in duilib\Core\TimerManager.cpp
        //4. SDL_EVENT_USER + 3:  occupied by WM_USER_PAINT_MSG in duilib\Core\NativeWindow_SDL.cpp
        //5. SDL_EVENT_USER + 4:  occupied by WM_USER_HOVER_MSG in duilib\Core\NativeWindow_SDL.cpp
    }
    return bRet;
}

DString MessageLoop_SDL::GetCurrentVideoDriverName()
{
    CheckInitSDL();
    DString videoDriverName;
    const char* videoDriver = SDL_GetCurrentVideoDriver();
    if (videoDriver != nullptr) {
        videoDriverName = StringConvert::UTF8ToT(videoDriver);
    }
    return videoDriverName;
}

float MessageLoop_SDL::GetPrimaryDisplayContentScale()
{
    CheckInitSDL();
    SDL_DisplayID displayID = SDL_GetPrimaryDisplay();
    float scale = 1.0f;
    if (displayID != 0) {
        scale = SDL_GetDisplayContentScale(displayID);
    }
    return scale;
}

int32_t MessageLoop_SDL::Run(MessageLoopIdleCallback idleCallback)
{
    //Initialize SDL
    if (!MessageLoop_SDL::CheckInitSDL()) {
        return -1;
    }

    bool bKeepGoing = true;
    SDL_Event sdlEvent;
    memset(&sdlEvent, 0, sizeof(sdlEvent));

    if (idleCallback == nullptr) {
        //Normal message loop, the Idle function is not supported
        while (bKeepGoing) {
            while (bKeepGoing && SDL_WaitEvent(&sdlEvent)) {
                ProcessSDLEvent(sdlEvent, bKeepGoing);
            }
        }
    }
    else {
        //The Idle function needs to be supported
        while (bKeepGoing) {
            while (bKeepGoing && SDL_PollEvent(&sdlEvent)) {
                ProcessSDLEvent(sdlEvent, bKeepGoing);
            }
            if (bKeepGoing) {
                idleCallback();

                //Wait for a message in the queue to avoid a busy loop causing high CPU usage
                if (SDL_WaitEvent(&sdlEvent)) {
                    ProcessSDLEvent(sdlEvent, bKeepGoing);
                }
            }
        }
    }

    //Quit SDL
    SDL_Quit();
    return 0;
}

void MessageLoop_SDL::ProcessSDLEvent(const SDL_Event& sdlEvent, bool& bKeepGoing)
{
    switch (sdlEvent.type) {
    case SDL_EVENT_QUIT:  /* triggers on last window close and other things. End the program. */
        bKeepGoing = false;
        break;
    default:
        //Dispatch the event to the window
        DispatchSDLEvent(sdlEvent);
        break;
    }
}

void MessageLoop_SDL::RunDoModal(NativeWindow_SDL& nativeWindow, bool bCloseByEsc, bool bCloseByEnter)
{
    ASSERT(nativeWindow.IsWindow());
    if (!nativeWindow.IsWindow()) {
        return;
    }
    const SDL_WindowID currentWindowId = SDL_GetWindowID((SDL_Window*)nativeWindow.GetWindowHandle());
    ASSERT(currentWindowId != 0);
    if (currentWindowId == 0) {
        return;
    }

    bool bKeepGoing = true;
    SDL_Event sdlEvent;
    memset(&sdlEvent, 0, sizeof(sdlEvent));
    /* run the program until told to stop. */
    while (bKeepGoing) {

        /* run through all pending events until we run out. */
        while (bKeepGoing && SDL_WaitEvent(&sdlEvent)) {
            switch (sdlEvent.type) {
            case SDL_EVENT_QUIT:  /* triggers on last window close and other things. End the program. */
                bKeepGoing = false;
                //Push a Quit message back so that the main message loop also exits, to avoid losing this event
                nativeWindow.PostQuitMsg(0);
                break;
            default:
                {
                    //Dispatch the event to the window
                    DispatchSDLEvent(sdlEvent);

                    SDL_WindowID windowID = NativeWindow_SDL::GetWindowIdFromEvent(sdlEvent);
                    if ((sdlEvent.type == SDL_EVENT_WINDOW_DESTROYED) && (windowID == currentWindowId)) {
                        //The window has exited, quit the message loop
                        bKeepGoing = false;
                    }
                    else if ((bCloseByEsc || bCloseByEnter) && (sdlEvent.type == SDL_EVENT_KEY_DOWN) && (windowID == currentWindowId)) {
                        VirtualKeyCode vkCode = Keycode::GetVirtualKeyCode(sdlEvent.key.key);
                        if (bCloseByEsc && (vkCode == VirtualKeyCode::kVK_ESCAPE)) {
                            //Modal dialog: close when the ESC key is pressed
                            if (!nativeWindow.IsClosingWnd()) {
                                nativeWindow.CloseWnd(kWindowCloseCancel);
                            }
                        }
                        else if (bCloseByEnter && (vkCode == VirtualKeyCode::kVK_RETURN)) {
                            //Modal dialog: close when the Enter key is pressed
                            nativeWindow.CloseWnd(kWindowCloseOK);
                        }
                    }
                }
                break;
            }
        }
    }
}

void MessageLoop_SDL::RunUserLoop(bool& bTerminate)
{
    ASSERT(!bTerminate);
    if (bTerminate) {
        return;
    }
    bool bKeepGoing = true;
    SDL_Event sdlEvent;
    memset(&sdlEvent, 0, sizeof(sdlEvent));
    /* run the program until told to stop. */
    while (bKeepGoing) {

        /* run through all pending events until we run out. */
        while (bKeepGoing && SDL_WaitEvent(&sdlEvent)) {
            switch (sdlEvent.type) {
            case SDL_EVENT_QUIT:  /* triggers on last window close and other things. End the program. */
                bKeepGoing = false;
                //Push a Quit message back so that the main message loop also exits, to avoid losing this event
                SDL_Event quitEvent;
                quitEvent.type = SDL_EVENT_QUIT;
                quitEvent.common.timestamp = 0;
                SDL_PushEvent(&quitEvent);
                break;
            default:
                {
                    //Dispatch the event to the window
                    DispatchSDLEvent(sdlEvent);

                    if (bTerminate) {
                        //Exit has been marked, quit this message loop
                        bKeepGoing = false;
                    }
                }
                break;
            }
        }
    }
}

void MessageLoop_SDL::RemoveDuplicateMsg(uint32_t msgId)
{
    SDL_FlushEvent(msgId);
}

bool MessageLoop_SDL::PostUserEvent(uint32_t msgId, WPARAM wParam, LPARAM lParam)
{
    ASSERT((msgId > SDL_EVENT_USER) && (msgId < SDL_EVENT_LAST));
    if ((msgId <= SDL_EVENT_USER) || (msgId >= SDL_EVENT_LAST)) {
        return false;
    }
    SDL_Event sdlEvent;
    sdlEvent.type = msgId;
    sdlEvent.common.timestamp = 0;
    sdlEvent.user.reserved = 0;
    sdlEvent.user.timestamp = 0;
    sdlEvent.user.type = msgId;
    sdlEvent.user.code = msgId;
    sdlEvent.user.data1 = (void*)wParam;
    sdlEvent.user.data2 = (void*)lParam;
    sdlEvent.user.windowID = 0;
    bool nRet = SDL_PushEvent(&sdlEvent);
    ASSERT(nRet);
    return nRet;
}

void MessageLoop_SDL::PostNoneEvent()
{
    SDL_Event sdlEvent;
    sdlEvent.type = SDL_EVENT_USER;
    sdlEvent.common.timestamp = 0;
    sdlEvent.user.reserved = 0;
    sdlEvent.user.timestamp = 0;
    sdlEvent.user.type = SDL_EVENT_USER;
    sdlEvent.user.code = SDL_EVENT_USER;
    sdlEvent.user.data1 = 0;
    sdlEvent.user.data2 = 0;
    sdlEvent.user.windowID = 0;
    bool nRet = SDL_PushEvent(&sdlEvent);
    ASSERT_UNUSED_VARIABLE(nRet);
}

void MessageLoop_SDL::AddUserMessageCallback(uint32_t msgId, const SDLUserMessageCallback& callback)
{
    ASSERT((msgId > SDL_EVENT_USER) && (msgId < SDL_EVENT_LAST));
    if ((msgId <= SDL_EVENT_USER) || (msgId >= SDL_EVENT_LAST)) {
        return;
    }
    ASSERT(callback != nullptr);
    if (callback == nullptr) {
        return;
    }
    s_userMsgCallbacks[msgId] = callback;
}

void MessageLoop_SDL::RemoveUserMessageCallback(uint32_t msgId)
{
    auto iter = s_userMsgCallbacks.find(msgId);
    if (iter != s_userMsgCallbacks.end()) {
        s_userMsgCallbacks.erase(iter);
    }
}

void MessageLoop_SDL::DispatchSDLEvent(const SDL_Event& sdlEvent)
{
    NativeWindow_SDL* pWindow = nullptr;
    SDL_WindowID windowID = NativeWindow_SDL::GetWindowIdFromEvent(sdlEvent);
    if (windowID != 0) {
        pWindow = NativeWindow_SDL::GetWindowFromID(windowID);
    }
    if (pWindow != nullptr) {
        pWindow->OnSDLWindowEvent(sdlEvent);
    }
    else {
        //Other messages are not handled, except the registered custom messages
        if ((sdlEvent.type > SDL_EVENT_USER) && (sdlEvent.type < SDL_EVENT_LAST)) {
            //User-defined message
            OnUserEvent(sdlEvent);
        }
    }
}

void MessageLoop_SDL::OnUserEvent(const SDL_Event& sdlEvent)
{
    if ((sdlEvent.type <= SDL_EVENT_USER) || (sdlEvent.type >= SDL_EVENT_LAST)) {
        return;
    }
    if (sdlEvent.type != sdlEvent.user.type) {
        return;
    }
    if (sdlEvent.type != (uint32_t)sdlEvent.user.code) {
        return;
    }
    if (sdlEvent.user.windowID != 0) {
        return;
    }
    uint32_t msgId = sdlEvent.user.type;
    WPARAM wParam = (WPARAM)sdlEvent.user.data1;
    LPARAM lParam = (LPARAM)sdlEvent.user.data2;

    auto iter = s_userMsgCallbacks.find(msgId);
    if (iter != s_userMsgCallbacks.end()) {
        SDLUserMessageCallback callback = iter->second;
        ASSERT(callback != nullptr);
        if (callback != nullptr) {
            callback(msgId, wParam, lParam);
        }
    }
}

} // namespace ui

#endif // DUILIB_BUILD_FOR_SDL
