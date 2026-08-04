#include "dui/Core/ThreadMessage.h"

#ifdef DUI_BUILD_FOR_SDL

#include "dui/Core/MessageLoop_SDL.h"

namespace ui
{
class ThreadMessage::TImpl
{
public:
    /** Message ID
    */
    uint32_t m_msgId = 0;

    /** Whether it has been terminated
    */
    bool m_bTerm = false;
};

ThreadMessage::ThreadMessage()
{
    m_impl = new TImpl;
}

ThreadMessage::~ThreadMessage()
{
    Clear();
    if (m_impl != nullptr) {
        delete m_impl;
        m_impl = nullptr;
    }
}

void ThreadMessage::Initialize(void* /*platformData*/)
{
}

bool ThreadMessage::PostMsg(uint32_t msgId, WPARAM wParam, LPARAM lParam, uint32_t* nErrorCode)
{
    if (nErrorCode) {
        *nErrorCode = 0;
    }
    if (m_impl->m_bTerm) {
        // Already terminated
        return false;
    }
    bool bRet = false;
    ASSERT(msgId == m_impl->m_msgId);
    if (msgId == m_impl->m_msgId) {
        bRet = MessageLoop_SDL::PostUserEvent(msgId, wParam, lParam);
    }
    return bRet;
}

void ThreadMessage::RemoveDuplicateMsg(uint32_t msgId)
{
    // Remove redundant messages from the queue, to avoid a large number of useless duplicate messages in the queue that would prevent mouse and keyboard messages from being processed
    ASSERT(msgId == m_impl->m_msgId);
    if (msgId == m_impl->m_msgId) {
        MessageLoop_SDL::RemoveDuplicateMsg(msgId);
    }
}

void ThreadMessage::SetMessageCallback(uint32_t msgId, const ThreadMessageCallback& callback)
{
    if (m_impl->m_msgId != 0) {
        MessageLoop_SDL::RemoveUserMessageCallback(m_impl->m_msgId);
    }
    m_impl->m_msgId = msgId;
    if (m_impl->m_msgId != 0) {
        MessageLoop_SDL::AddUserMessageCallback(m_impl->m_msgId, callback);
    }
}

void ThreadMessage::Clear()
{
    if (m_impl->m_msgId != 0) {
        MessageLoop_SDL::RemoveUserMessageCallback(m_impl->m_msgId);
    }    
    m_impl->m_bTerm = true;
    m_impl->m_msgId = 0;
}

void ThreadMessage::OnUserMessage(uint32_t /*msgId*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
}

} // namespace ui

#endif // DUI_BUILD_FOR_SDL
