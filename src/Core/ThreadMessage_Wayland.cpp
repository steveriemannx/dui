#include "dui/Core/ThreadMessage.h"

#ifdef DUI_BUILD_FOR_WAYLAND

#include "dui/Core/MessageLoop_Wayland.h"

namespace ui
{
class ThreadMessage::TImpl
{
public:
    uint32_t m_msgId = 0;
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
        return false;
    }
    bool bRet = false;
    if (msgId == m_impl->m_msgId) {
        bRet = MessageLoop_Wayland::PostUserEvent(msgId, wParam, lParam);
    }
    return bRet;
}

void ThreadMessage::RemoveDuplicateMsg(uint32_t msgId)
{
    if (msgId == m_impl->m_msgId) {
        MessageLoop_Wayland::RemoveDuplicateMsg(msgId);
    }
}

void ThreadMessage::SetMessageCallback(uint32_t msgId, const ThreadMessageCallback& callback)
{
    if (m_impl->m_msgId != 0) {
        MessageLoop_Wayland::RemoveUserMessageCallback(m_impl->m_msgId);
    }
    m_impl->m_msgId = msgId;
    if (m_impl->m_msgId != 0) {
        MessageLoop_Wayland::AddUserMessageCallback(m_impl->m_msgId, callback);
    }
}

void ThreadMessage::Clear()
{
    if (m_impl->m_msgId != 0) {
        MessageLoop_Wayland::RemoveUserMessageCallback(m_impl->m_msgId);
    }
    m_impl->m_bTerm = true;
    m_impl->m_msgId = 0;
}

void ThreadMessage::OnUserMessage(uint32_t /*msgId*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
}

} // namespace ui

#endif // DUI_BUILD_FOR_WAYLAND
