#ifndef UI_CORE_THREAD_MANAGER_H_
#define UI_CORE_THREAD_MANAGER_H_

#include "dui/Core/FrameworkThread.h"
#include "dui/Core/ControlPtrT.h"
#include <map>
#include <atomic>

namespace ui 
{
/** Thread manager, used to support inter-thread communication
*/
class DUI_API ThreadManager
{
public:
    ThreadManager();
    ~ThreadManager();
    ThreadManager(const ThreadManager&) = delete;
    ThreadManager& operator = (const ThreadManager&) = delete;

public:
    /** Register a thread with the manager (it can only be used for inter-thread communication after registration)
    * @param [in] nThreadIdentifier Thread identifier ID
    * @param [in] pThread The thread interface
    */
    bool RegisterThread(int32_t nThreadIdentifier, FrameworkThread* pThread);

    /** Determine whether a thread with the specified identifier is contained
    * @param [in] nThreadIdentifier Thread identifier ID
    */
    bool HasThread(int32_t nThreadIdentifier) const;

    /** Unregister a thread from the manager
    * @param [in] nThreadIdentifier Thread identifier ID
    */
    bool UnregisterThread(int32_t nThreadIdentifier);

    /** Get the thread identifier ID of the current thread
    * @return On success, returns the thread identifier ID; on failure, returns kThreadNone (value -1)
    */
    int32_t GetCurrentThreadIdentifier() const;

public:
    /** Send a task to the thread, executed immediately
    * @param [in] nThreadIdentifier Thread identifier ID
    * @param [in] task Task callback function
    * @return On success, returns the task ID (greater than 0); on failure, returns 0
    */
    size_t PostTask(int32_t nThreadIdentifier, const StdClosure& task);

    /** Send a task to the thread, executed with a delay
    * @param [in] nThreadIdentifier Thread identifier ID
    * @param [in] task Task callback function
    * @param [in] nDelayMs Delay time (unit: milliseconds)
    * @return On success, returns the task ID (greater than 0); on failure, returns 0
    */
    size_t PostDelayedTask(int32_t nThreadIdentifier, const StdClosure& task, int32_t nDelayMs);

    /** Send a task to the thread, executed repeatedly at timed intervals
    * @param [in] nThreadIdentifier Thread identifier ID
    * @param [in] task Task callback function
    * @param [in] nIntervalMs Interval time (unit: milliseconds)
    * @param [in] nTimes Number of repetitions; -1 means it executes forever
    * @return On success, returns the task ID (greater than 0); on failure, returns 0
    */
    size_t PostRepeatedTask(int32_t nThreadIdentifier, const StdClosure& task,
                            int32_t nIntervalMs, int32_t nTimes = -1);

    /** Cancel a task
    * @param [in] nTaskId The task ID, i.e. the return value of the PostXXX functions above
    */
    bool CancelTask(size_t nTaskId);

    /** Get the next task ID (thread-safe)
    * @return Returns the next task ID; task IDs start from 1
    */
    size_t GetNextTaskId();

public:
    /** Shut down the thread manager and release resources
    */
    void Clear();

public:
    /** Whether the main thread has exited
    */
    bool IsMainThreadExit() const;

    /** When the main thread exits, call this function to set the main thread exit flag
    */
    void SetMainThreadExit();

private:
    /** Smart pointer for thread objects
    */
    typedef ControlPtrT<FrameworkThread> FrameworkThreadPtr;

    /** Thread information mapping table
    */
    std::map<int32_t, FrameworkThreadPtr> m_threadsMap;

    /** Multithreading synchronization lock
    */
    mutable std::mutex m_threadMutex;

    /** The next task ID
    */
    std::atomic<size_t> m_nNextTaskId;

    /** Whether the main thread has exited
    */
    std::atomic<bool> m_bMainThreadExit;
};

}
#endif //UI_CORE_THREAD_MANAGER_H_
