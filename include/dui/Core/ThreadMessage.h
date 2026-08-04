#ifndef UI_CORE_THREAD_MESSAGE_H_
#define UI_CORE_THREAD_MESSAGE_H_

#include "dui/Core/Callback.h"

namespace ui {

/** Message callback function prototype: void FunctionName(uint32_t msgID, WPARAM wParam, LPARAM lParam);
*/
typedef std::function<void(uint32_t msgID, WPARAM wParam, LPARAM lParam)> ThreadMessageCallback;

/** Helper class for communicating with the main thread
*/
class DUI_API ThreadMessage
{
public:
    ThreadMessage();
    ThreadMessage(const ThreadMessage& r) = delete;
    ThreadMessage& operator = (const ThreadMessage& r) = delete;
    ~ThreadMessage();

public:
    /** Initialize
    * @param [in] platformData Platform-related data (optional parameter; if not provided, the default value nullptr is used)
    * Windows platform: the handle of the module where the resource resides (HMODULE); if nullptr, the handle of the exe is used (optional parameter)
    */
    void Initialize(void* platformData);

    /** Set the message callback function
    * @param [in] msgId Message ID
    * @param [in] callback Callback function
    */
    void SetMessageCallback(uint32_t msgId, const ThreadMessageCallback& callback);

    /** Post a message
    * @param [in] msgId Message ID
    * @param [in] wParam The 1st parameter of the message
    * @param [in] lParam The 2nd parameter of the message
    * @param [out] nErrorCode Returns the error code when an error occurs
    */
    bool PostMsg(uint32_t msgId, WPARAM wParam, LPARAM lParam, uint32_t* nErrorCode);

    /** Remove redundant messages from the message queue
    * @param [in] msgId Message ID
    */
    void RemoveDuplicateMsg(uint32_t msgId);

    /** Clean up resources
    */
    void Clear();

private:
    /** Message window function
    */
    void OnUserMessage(uint32_t msgId, WPARAM wParam, LPARAM lParam);

private:
    /** Internal implementation
    */
    class TImpl;
    TImpl* m_impl;
};

} // namespace ui

#endif // UI_CORE_THREAD_MESSAGE_H_
