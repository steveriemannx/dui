#include "dui/Core/TimerManager.h"
#include "dui/Core/GlobalManager.h"
#include "dui/Utils/LogUtil.h"
#include "dui/Utils/StringUtil.h"
#include "dui/Core/WindowMessage.h"

#if defined (DUI_BUILD_FOR_SDL)
    #include <SDL3/SDL.h>
#elif defined (DUI_BUILD_FOR_WAYLAND)
    #include "dui/Core/WindowMessage.h"
#endif

/** Custom message
*/
#if defined (DUI_BUILD_FOR_SDL)
    #define WM_USER_DEFINED_TIMER   (SDL_EVENT_USER + 2)
#elif defined (DUI_BUILD_FOR_WAYLAND)
    #define WM_USER_DEFINED_TIMER   (kWM_USER + 2)
#else
    #define WM_USER_DEFINED_TIMER   (kWM_USER + 567)
#endif

namespace ui 
{

/** Timer data
*/
class TimerInfo
{
public:
    TimerInfo(): 
        timerCallback(nullptr),
        uElapseMs(0),
        uRepeatTime(0),
        m_nTimerId(0)
    {
    }

    bool operator < (const TimerInfo& r) const {
        // Sort condition: the earliest triggered timer comes first
        return trigerTime > r.trigerTime;
    }

    // Timer ID
    size_t m_nTimerId;

    // Timer callback function
    TimerCallback timerCallback;

    // Timer cancellation synchronization mechanism
    std::weak_ptr<WeakFlag> weakFlag;

    // Timer interval: (unit: milliseconds)
    uint32_t uElapseMs;

    // Number of repetitions
    uint32_t uRepeatTime;

    // Trigger time of the timer
    std::chrono::steady_clock::time_point trigerTime;
};

TimerManager::TimerManager():
    m_nNextTimerId(1),
    m_bRunning(false),
    m_bHasPenddingPoll(false)
{
}

TimerManager::~TimerManager()
{
    Clear();
}

void TimerManager::Initialize(void* platformData)
{
    m_threadMsg.Initialize(platformData);
    m_threadMsg.SetMessageCallback(WM_USER_DEFINED_TIMER, UiBind(&TimerManager::OnTimerMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

void TimerManager::Clear()
{
    std::unique_lock<std::mutex> guard(m_taskMutex);
    m_threadMsg.Clear();
    while (!m_aTimers.empty()) {
        m_aTimers.pop();
    }
    m_removedTimerIds.clear();
    m_bRunning = false;
    if (m_pWorkerThread != nullptr) {
        m_cv.notify_one();
        guard.unlock();
        m_pWorkerThread->join();
        m_pWorkerThread = nullptr;
    }
}

size_t TimerManager::AddTimer(const std::weak_ptr<WeakFlag>& weakFlag, const TimerCallback& callback,
                              uint32_t uElapseMs, int32_t iRepeatTime)
{
    ASSERT((callback != nullptr) && (uElapseMs > 0) && (iRepeatTime != 0));
    if ((callback == nullptr) || (uElapseMs == 0) || (iRepeatTime == 0)) {
        return 0;
    }
    if (iRepeatTime < 0) {
        iRepeatTime = -1;
    }
    size_t nTimerId = m_nNextTimerId++;
    TimerInfo pTimer;
    pTimer.m_nTimerId = nTimerId;

    pTimer.timerCallback = callback;
    pTimer.uElapseMs = uElapseMs;
    pTimer.trigerTime = std::chrono::steady_clock::now();
    pTimer.trigerTime += std::chrono::milliseconds(uElapseMs); // Calculate the next trigger time (current time + interval in milliseconds)
    pTimer.uRepeatTime = static_cast<uint32_t>(iRepeatTime);
    pTimer.weakFlag = weakFlag;

    std::lock_guard<std::mutex> threadGuard(m_taskMutex);
    m_aTimers.push(pTimer);
    if (m_pWorkerThread == nullptr) {
        // Start the thread
        m_bRunning = true;
        m_pWorkerThread = std::make_unique<std::thread>(&TimerManager::WorkerThreadProc, this);
    }
    ASSERT(m_bRunning);
    // Wake up the worker thread to check the task status
    m_cv.notify_one();
    return nTimerId;
}

void TimerManager::RemoveTimer(size_t nTimerId)
{
    std::lock_guard<std::mutex> threadGuard(m_taskMutex);
    m_removedTimerIds.insert(nTimerId);
}

bool TimerManager::IsTimerRemoved(size_t nTimerId) const
{
    if (!m_removedTimerIds.empty()) {
        return m_removedTimerIds.find(nTimerId) != m_removedTimerIds.end();
    }
    else {
        return false;
    }
}

void TimerManager::ClearRemovedTimerId(size_t nTimerId)
{
    m_removedTimerIds.erase(nTimerId);
}

void TimerManager::OnTimerMessage(uint32_t msgId, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    ASSERT(msgId == WM_USER_DEFINED_TIMER);
    if (msgId == WM_USER_DEFINED_TIMER) {
        //LogUtil::OutputLine(StringUtil::Printf(_T("TimerManager::OnTimerMessage: received timer event")));
        m_threadMsg.RemoveDuplicateMsg(WM_USER_DEFINED_TIMER);
        Poll();
    }    
}

void TimerManager::Poll()
{
    // This function is called in the UI thread
    std::unique_lock<std::mutex> taskGuard(m_taskMutex);
    while (!m_aTimers.empty()) {
        if (m_aTimers.top().weakFlag.expired() || IsTimerRemoved(m_aTimers.top().m_nTimerId)) {
            // Remove timers that are invalidated or canceled
            ClearRemovedTimerId(m_aTimers.top().m_nTimerId);
            m_aTimers.pop();
        }
        else if (std::chrono::steady_clock::now() >= m_aTimers.top().trigerTime) {
            // The timer at the top of the queue: the timer trigger condition has been reached
            TimerInfo timerTask = m_aTimers.top();
            m_aTimers.pop();

            if (!timerTask.weakFlag.expired()) {
                // Call the callback function of the timer
                taskGuard.unlock();
                timerTask.timerCallback();
                //LogUtil::OutputLine(StringUtil::Printf(_T("timerTask.timerCallback(): exec. TimerId: %u, ElapseMs: %u"), timerTask.m_nTimerId, timerTask.uElapseMs));
                taskGuard.lock();
            }
            if (timerTask.uRepeatTime > 0) {
                timerTask.uRepeatTime--;
            }
            if ((timerTask.uRepeatTime > 0) &&
                !timerTask.weakFlag.expired() &&
                !IsTimerRemoved(timerTask.m_nTimerId)) {
                // If the trigger count limit has not been reached, reset the next trigger time
                timerTask.trigerTime = std::chrono::steady_clock::now();
                timerTask.trigerTime += std::chrono::milliseconds(timerTask.uElapseMs); // Calculate the next trigger time (current time + interval in milliseconds)
                m_aTimers.push(timerTask);
            }
            else {
                // The execution is complete or has been invalidated
                ClearRemovedTimerId(timerTask.m_nTimerId);
            }
        }
        else {
            // There are no more timer tasks to trigger
            break;
        }
    }
    // Wake up the worker thread to check the task status
    m_cv.notify_one();
    m_bHasPenddingPoll = false;
}

void TimerManager::WorkerThreadProc()
{
    while (m_bRunning) {
        std::unique_lock taskGuard(m_taskMutex);
        if (!m_bRunning) {
            break;
        }
        if (m_aTimers.empty()) {
            // Empty, wait for tasks
            m_cv.wait(taskGuard);
            if (!m_bRunning) {
                break;
            }
        }
        else {
            // Calculate the earliest task and wait for the timeout
            uint32_t nDetaTimeMs = 0;
            auto currentTime = std::chrono::steady_clock::now();
            if (m_aTimers.top().weakFlag.expired() || IsTimerRemoved(m_aTimers.top().m_nTimerId)) {
                // Remove timers that are invalidated or canceled
                nDetaTimeMs = 0;
            }
            else if (currentTime >= m_aTimers.top().trigerTime) {
                // The timer at the top of the queue: the timer trigger condition has been reached
                nDetaTimeMs = 0;
            }
            else {
                // There are no more timer tasks to trigger
                auto nDiffTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(m_aTimers.top().trigerTime - currentTime);
                nDetaTimeMs = (uint32_t)nDiffTimeMs.count();
            }

            if (nDetaTimeMs > 0) {
                // Wait for the timeout with a delay
                //LogUtil::OutputLine(StringUtil::Printf(_T("condition_variable: wait_for timer event(%u ms)"), nDetaTimeMs));
                // The accuracy of this function is about 10ms
                // Note: it was found that both the gcc version and the glibc version have problems with wait_for (they use system time); only gcc >= 10 and glibc >= 2.30 have no impact on program behavior.
                m_cv.wait_for(taskGuard, std::chrono::milliseconds(nDetaTimeMs));
            }
            // Notify for processing (sent to the main thread for execution; the lock must not be held at this time to avoid deadlock issues)
            m_bHasPenddingPoll = true;
            taskGuard.unlock();

            uint32_t nErrorCode = 0;
            bool bRet = m_threadMsg.PostMsg(WM_USER_DEFINED_TIMER, 0, 0, &nErrorCode);
#if defined (DUI_BUILD_FOR_WIN) && !defined (DUI_BUILD_FOR_SDL)
            if (!bRet) {
                if ((nErrorCode == ERROR_NOT_ENOUGH_QUOTA) && !GlobalManager::Instance().IsInUIThread()) {
                    // When the program starts, posting a message to the main thread from a child thread will encounter this error
                    for (int32_t i = 0; i < 200; ++i) {
                        ::Sleep(50);
                        if (!m_bRunning) {
                            break;
                        }
                        bRet = m_threadMsg.PostMsg(WM_USER_DEFINED_TIMER, 0, 0, &nErrorCode);
                        if (bRet || (nErrorCode != ERROR_NOT_ENOUGH_QUOTA)) {
                            break;
                        }
                    }
                }
                if (m_bRunning) {
                    ASSERT_UNUSED_VARIABLE(bRet);
                }                
            }
#else
            if (m_bRunning) {
                ASSERT_UNUSED_VARIABLE(bRet);
            }
#endif
            taskGuard.lock();
            if (m_bRunning) {
                ASSERT_UNUSED_VARIABLE(bRet);
            }            
            //LogUtil::OutputLine(StringUtil::Printf(_T("PostMessage: send timer event")));

            if (m_bRunning && m_bHasPenddingPoll) {
                m_cv.wait(taskGuard);
            }
        }        
    }
    m_bRunning = false;
}

}


