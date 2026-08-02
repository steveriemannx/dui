#ifndef UI_CORE_TIMER_MANAGER_H_
#define UI_CORE_TIMER_MANAGER_H_

#include "duilib/Core/Callback.h"
#include "duilib/Core/ThreadMessage.h"
#include <queue>
#include <set>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace ui 
{

/** Timer callback function prototype: void FunctionName();
*/
typedef std::function<void()> TimerCallback;
class TimerInfo;

/** Timer manager
*/
class DUILIB_API TimerManager: public SupportWeakCallback
{
public:
    TimerManager();
    virtual ~TimerManager() override;
    TimerManager(const TimerManager&) = delete;
    TimerManager& operator = (const TimerManager&) = delete;

public:
    /** Initialize
    * @param [in] platformData Platform-related data (optional parameter; if not provided, the default value nullptr is used)
    * Windows platform: the handle of the module where the resource resides (HMODULE); if nullptr, the handle of the exe is used (optional parameter)
    */
    void Initialize(void* platformData);

    /** Add a cancelable timer
    * @param [in] weakFlag Timer cancellation mechanism; if weakFlag.expired() is true, the timer has been canceled and timer callbacks will no longer be dispatched
    * @param [in] callback Timer callback function
    * @param [in] uElapseMs Timer trigger interval, in milliseconds
    * @param [in] iRepeatTime Timer callback count limit; -1 means the callback is repeated indefinitely
    * @param [in] On success, returns the timer ID (greater than 0); on failure, returns 0
    */
    size_t AddTimer(const std::weak_ptr<WeakFlag>& weakFlag,
                    const TimerCallback& callback,
                    uint32_t uElapseMs,
                    int32_t iRepeatTime = -1);

    /** Remove a timer task
    * @param [in] nTimerId The timer task ID, i.e. the return value of AddTimer
    */
    void RemoveTimer(size_t nTimerId);

    /** Shut down the timer manager and release resources
     */
    void Clear();

private:
    /** Thread function of the background thread
    */
    void WorkerThreadProc();

    /** Timer trigger; dispatches timer event callbacks
    */
    void Poll();

    /** Whether the timer task has been canceled
    */
    bool IsTimerRemoved(size_t nTimerId) const;

    /** Remove the ID of the canceled timer task
    */
    void ClearRemovedTimerId(size_t nTimerId);

private:
    /** Message window function
    */
    void OnTimerMessage(uint32_t msgId, WPARAM wParam, LPARAM lParam);

private:
    /** All registered timers
    */
    std::priority_queue<TimerInfo> m_aTimers;

    /** The next timer task ID
    */
    size_t m_nNextTimerId;

    /** Removed timer task IDs
    */
    std::set<size_t> m_removedTimerIds;

private:
    /** Whether it is running
    */
    volatile bool m_bRunning;

    /** Whether it is waiting for the main thread to process the timer callback events
    */
    volatile bool m_bHasPenddingPoll;

    /** The background thread
    */
    std::unique_ptr<std::thread> m_pWorkerThread;

    /** The event notification mechanism of the thread
    */
    std::condition_variable m_cv;

    /** The lock for the task data container
    */
    std::mutex m_taskMutex;

    /** The inter-thread communication mechanism (with the main thread)
    */
    ThreadMessage m_threadMsg;
};

} // namespace ui

#endif // UI_CORE_TIMER_MANAGER_H_
