#ifndef UI_CORE_MESSAGE_LOOP_WINDOWS_H_
#define UI_CORE_MESSAGE_LOOP_WINDOWS_H_

#include "duilib/duilib_defs.h"

#if defined (DUILIB_BUILD_FOR_WIN)
#include <functional>

namespace ui {

/** Message loop of the main thread
*/
class DUILIB_API MessageLoop_Windows
{
public:
    MessageLoop_Windows();
    MessageLoop_Windows(const MessageLoop_Windows& r) = delete;
    MessageLoop_Windows& operator = (const MessageLoop_Windows& r) = delete;
    ~MessageLoop_Windows();

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

private:
    /** Message loop that supports the Idle function
    */
    int32_t RunWithIdle(MessageLoopIdleCallback idleCallback);
};

} // namespace ui

#endif // DUILIB_BUILD_FOR_WIN

#endif // UI_CORE_MESSAGE_LOOP_WINDOWS_H_
