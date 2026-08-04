#include "dui/Core/ThreadMessage.h"

#if defined (DUI_BUILD_FOR_WIN) && !defined (DUI_BUILD_FOR_SDL)

#include "dui/Core/GlobalManager.h"

namespace ui
{
class ThreadMessage::TImpl
{
public:
    /** Message window function
    */
    static LRESULT CALLBACK WndProcThunk(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

public:
    /** Message window handle, used to dispatch timer events in the UI thread
    */
    HWND m_hMessageWnd = nullptr;

    /** Message callback function
    */
    ThreadMessageCallback m_callback = nullptr;

    /** Message ID
    */
    uint32_t m_msgId = 0;

    /** Whether it has been terminated
    */
    bool m_bTerm = false;
};

LRESULT ThreadMessage::TImpl::WndProcThunk(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
    if (message > WM_USER) {
        ThreadMessage* pThis = reinterpret_cast<ThreadMessage*>(::GetWindowLongPtr(hwnd, GWLP_USERDATA));
        if (pThis != nullptr) {
            pThis->OnUserMessage(message, wparam, lparam);
        }
        return 1;
    }
    return ::DefWindowProcW(hwnd, message, wparam, lparam);
}

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

// Name of the window class
#define DUI_MESSAGING_WINDOW_CLASS L"dui_messaging_window"

void ThreadMessage::Initialize(void* platformData)
{
    ASSERT(m_impl->m_hMessageWnd == nullptr);
    if (m_impl->m_hMessageWnd != nullptr) {
        return;
    }
    auto hInstance = platformData != nullptr ? (HMODULE)platformData : ::GetModuleHandle(nullptr);
    WNDCLASSEXW wc = { 0 };
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = ThreadMessage::TImpl::WndProcThunk;
    wc.hInstance = hInstance;
    wc.lpszClassName = DUI_MESSAGING_WINDOW_CLASS;
    ATOM ret = ::RegisterClassExW(&wc);
    ASSERT_UNUSED_VARIABLE(ret != 0 || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS);
    m_impl->m_hMessageWnd = ::CreateWindowW(wc.lpszClassName, 0, 0, 0, 0, 0, 0, HWND_MESSAGE, 0, hInstance, 0);
    if (::IsWindow(m_impl->m_hMessageWnd)) {
        ::SetWindowLongPtr(m_impl->m_hMessageWnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(this));
    }

    // When the module exits, unregister the ATOM
    GlobalManager::Instance().AddAtExitFunction([hInstance]() {
            ::UnregisterClassW(DUI_MESSAGING_WINDOW_CLASS, hInstance);
        });
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
    ASSERT(m_impl->m_hMessageWnd != nullptr);
    if ((m_impl->m_hMessageWnd != nullptr) && (msgId == m_impl->m_msgId)) {
        bRet = ::PostMessage(m_impl->m_hMessageWnd, m_impl->m_msgId, wParam, lParam) != FALSE;
        if (!bRet) {
            if (nErrorCode) {
                *nErrorCode = (uint32_t)::GetLastError();
            }
        }
    }
    return bRet;
}

void ThreadMessage::RemoveDuplicateMsg(uint32_t msgId)
{
    // Remove redundant messages from the queue, to avoid a large number of useless duplicate messages in the queue that would prevent mouse and keyboard messages from being processed
    ASSERT(msgId == m_impl->m_msgId);
    ASSERT(m_impl->m_hMessageWnd != nullptr);
    if ((m_impl->m_hMessageWnd != nullptr) && (msgId == m_impl->m_msgId)) {
        MSG msg;
        while (::PeekMessage(&msg, m_impl->m_hMessageWnd, m_impl->m_msgId, m_impl->m_msgId, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                // Detected the quit message; put it back into the message queue to avoid the process being unable to exit
                ::PostQuitMessage(static_cast<int>(msg.wParam));
                break;
            }
            else {
                ASSERT(msg.message == m_impl->m_msgId);
            }
        }
    }
}

void ThreadMessage::SetMessageCallback(uint32_t msgId, const ThreadMessageCallback& callback)
{
    m_impl->m_msgId = msgId;
    m_impl->m_callback = callback;
}

void ThreadMessage::Clear()
{
    m_impl->m_bTerm = true;
    m_impl->m_msgId = 0;
    m_impl->m_callback = nullptr;
    if (m_impl->m_hMessageWnd != nullptr) {
        ::DestroyWindow(m_impl->m_hMessageWnd);
        m_impl->m_hMessageWnd = nullptr;
    }
}

void ThreadMessage::OnUserMessage(uint32_t msgId, WPARAM wParam, LPARAM lParam)
{
    ASSERT(m_impl->m_msgId == msgId);
    if ((m_impl->m_msgId == msgId) && (m_impl->m_callback != nullptr)) {
        m_impl->m_callback(msgId, wParam, lParam);
    }
}

} // namespace ui

#endif // DUI_BUILD_FOR_WIN
