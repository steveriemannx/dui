#include "duilib/Core/MessageLoop_Windows.h"

#if defined (DUILIB_BUILD_FOR_WIN)

namespace ui
{
MessageLoop_Windows::MessageLoop_Windows()
{
}

MessageLoop_Windows::~MessageLoop_Windows()
{
}

int32_t MessageLoop_Windows::Run(MessageLoopIdleCallback idleCallback)
{
    if (idleCallback == nullptr) {
        //Normal message loop, the Idle function is not supported
        MSG msg = { 0, };
        BOOL bRet = FALSE;
        while ((bRet = ::GetMessage(&msg, 0, 0, 0)) != 0) {
            if (bRet == -1) {
                // handle the error and possibly exit
                // Ignore this error
            }
            else {
                ::TranslateMessage(&msg);
                ::DispatchMessage(&msg);
            }
        }
        return (int32_t)msg.wParam;
    }
    else {
        //The Idle function needs to be supported
        return RunWithIdle(idleCallback);
    }
}

int32_t MessageLoop_Windows::RunWithIdle(MessageLoopIdleCallback idleCallback)
{
    MSG msg = { 0, };
    while (1) {
        BOOL bHasMsg = ::PeekMessage(&msg, 0, 0, 0, PM_REMOVE);
        if (bHasMsg) {
            if (msg.message == WM_QUIT) {
                break;
            }
            // Standard message handling flow
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
        else {
            // Perform Idle handling when there is no message
            idleCallback();
            if (!::PeekMessage(&msg, 0, 0, 0, PM_NOREMOVE)) {
                // No message in the queue, wait for a message (to avoid CPU idling)
                ::MsgWaitForMultipleObjects(0, nullptr, FALSE, INFINITE, QS_ALLINPUT);
            }
        }
    }
    return (int32_t)msg.wParam;
}

} // namespace ui

#endif // DUILIB_BUILD_FOR_WIN
