#ifndef UI_CORE_FRAMEWORK_THREAD_H_
#define UI_CORE_FRAMEWORK_THREAD_H_

#include "dui/Core/ThreadMessage.h"
#include "dui/Core/Callback.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <vector>
#include <map>
#include <atomic>

namespace ui 
{
/** Thread identifier (these are predefined, but can also be customized to meet project needs)
*/
enum ThreadIdentifier
{
    //Invalid thread identifier
    kThreadNone     = -1,

    //The following are thread identifiers used internally by the library
    kThreadUI       = 0,    //The UI thread
    kThreadWorker   = 1,    //The worker thread (used internally, for time-consuming business processing)
    kThreadNetwork  = 2,    //The worker thread (used internally, for network related business processing)
    kThreadImage1   = 3,    //The worker thread (used internally, for image decoding and related business processing)
    kThreadImage2   = 4,    //The worker thread (used internally, for image decoding and related business processing)

    //The following are custom thread identifiers for the user application layer
    kThreadUser     = 100   //The starting identifier of user-defined threads (identifiers below this value are used internally)
                            //If the application layer creates more threads, increment the identifier after this value
};

/** The framework thread
*/
class DUI_API FrameworkThread : public virtual SupportWeakCallback
{
public:
    /** Construct the thread object with the thread name and thread identifier ID
    * @param [in] threadName The thread name
    * @param [in] nThreadIdentifier The thread identifier ID, needed for cross-thread communication
    */
    FrameworkThread(const DString& threadName, int32_t nThreadIdentifier);
    virtual ~FrameworkThread() override;
    FrameworkThread(const FrameworkThread&) = delete;
    FrameworkThread& operator = (const FrameworkThread&) = delete;

public:
    /** Run the message loop of the UI main thread (blocks until the message loop exits)
    * @param [in] bSupportIdle true means the Idle feature is supported, and when the message queue is empty, the OnMessageLoopIdle virtual function is called for the application layer to process business
    *                          false means the Idle feature is not supported, and the OnMessageLoopIdle virtual function will not be called
    */
    bool RunMessageLoop(bool bSupportIdle = false);

    /** Start the worker thread
    */
    bool Start();

    /** Stop the worker thread (asynchronous stop)
    */
    bool Stop();

    /** Whether it is running
    */
    bool IsRunning() const;

    /** Whether it is the UI thread (i.e. a thread running with the RunMessageLoop function, which has an internal message loop)
    */
    bool IsUIThread() const;

public:
    /** Get the thread ID of the current thread (with the same meaning as the operating system thread ID)
    */
    std::thread::id GetThreadId() const;

    /** Convert the thread ID to a string
    */
    static DString ThreadIdToString(const std::thread::id& threadId);

    /** Get the current thread identifier, the unique identifier of the thread, used for inter-thread communication (the value initialized when the thread is constructed)
    */
    int32_t GetThreadIdentifier() const;

    /** Return the thread name (the value initialized when the thread is constructed)
    */
    const DString& GetThreadName() const;

public:
    /** Send a task to the thread, executed immediately
    * @param [in] task The task callback function
    * @param [in] unlockClosure The function used to release the outer lock (to avoid deadlock)
    * @return Returns the task ID (greater than 0) on success, or 0 on failure
    */
    size_t PostTask(const StdClosure& task, const StdClosure& unlockClosure = nullptr);

    /** Send a task to the thread, executed with a delay
    * @param [in] task The task callback function
    * @param [in] nDelayMs The delay time (unit: milliseconds)
    * @return Returns the task ID (greater than 0) on success, or 0 on failure
    */
    size_t PostDelayedTask(const StdClosure& task, int32_t nDelayMs);

    /** Send a task to the thread, which can be repeated at intervals
    * @param [in] task The task callback function
    * @param [in] nIntervalMs The interval time (unit: milliseconds)
    * @param [in] nTimes The number of repetitions, -1 means execute indefinitely
    * @return Returns the task ID (greater than 0) on success, or 0 on failure
    */
    size_t PostRepeatedTask(const StdClosure& task, int32_t nIntervalMs, int32_t nTimes = -1);

    /** Cancel a task
    * @param [in] nTaskId The task ID, i.e. the return value of the PostXXX functions above
    * @return Returns true on successful cancellation, otherwise returns false
    */
    bool CancelTask(size_t nTaskId);

protected:
    /** Initialization before running, called before entering the message loop
    */
    virtual void OnInit();

    /** Run the message loop; subclasses can override this function to implement their own message loop
    */
    virtual void OnRunMessageLoop();

    /** Cleanup on exit, called after the message loop exits
    */
    virtual void OnCleanup();

    /** Main thread: the message loop is in the Idle state
    */
    virtual void OnMessageLoopIdle();

private:
    /** The main thread has completed initialization
    */
    void OnMainThreadInited();

    /** The main thread message loop has exited
    */
    void OnMainThreadExit();

    /** The thread function of the worker thread
    */
    void WorkerThreadProc();

    /** Notify that a task should be executed
    */
    bool NotifyExecTask(size_t nTaskId,
                        const StdClosure& unlockClosure1 = nullptr,
                        const StdClosure& unlockClosure2 = nullptr);

    /** Execute a task
    */
    void ExecTask(size_t nTaskId);

    /** The message function
    */
    void OnTaskMessage(uint32_t msgId, WPARAM wParam, LPARAM lParam);

    /** Get the next task ID
    */
    size_t GetNextTaskId() const;

private:
    /** The task type
    */
    enum class TaskType
    {
        kTask,          //The normal task, executed immediately
        kDelayedTask,   //The task executed with a delay
        kRepeatedTask   //The task executed repeatedly at intervals
    };

    /** The task info
    */
    struct TaskInfo
    {
        TaskType m_taskType = TaskType::kTask;  //The task type
        StdClosure m_task;                      //The task callback function
        int32_t m_nIntervalMs = 0;              //The interval between task executions
        int32_t m_nTimes = 0;                   //The number of task repetitions, -1 means execute indefinitely

        size_t m_nTaskId = 0;                   //The task ID (incrementing)
        std::chrono::steady_clock::time_point m_startTime;     //The start time (the time the task was put into the queue)
        std::chrono::steady_clock::time_point m_lastExecTime;  //The last execution time of the task
        int32_t m_nTotalExecTimes = 0;          //The total number of times the task has been executed
    };

    /** The task info map
    */
    typedef std::map<size_t, TaskInfo> TaskMap;
    TaskMap m_taskMap;

    /** The multi-thread synchronization lock for task data
    */
    mutable std::mutex m_taskMutex;

private:
    /** The thread name
    */
    DString m_threadName;

    /** The thread identifier ID, needed for cross-thread communication
    */
    int32_t m_nThreadIdentifier;

    /** The thread ID of the current thread
    */
    std::atomic<std::thread::id> m_nThisThreadId;

    /** Whether it is the UI thread
    */
    bool m_bThreadUI;

    /** Whether it is running
    */
    volatile bool m_bRunning;

    /** true means the Idle feature is supported; when the message queue is empty, the OnMessageLoopIdle virtual function is called for the application layer to process business
    */
    bool m_bSupportIdle;

private:
    /** The worker thread
    */
    std::unique_ptr<std::thread> m_pWorkerThread;

    /** The event notification mechanism of the thread
    */
    std::condition_variable m_cv;

    /** The task IDs waiting to be executed
    */
    std::vector<size_t> m_penddingTaskIds;

    /** The lock of the pending task ID container
    */
    std::mutex m_penddingTaskMutex;

    /** The mechanism for communicating with the main thread
    */
    ThreadMessage m_threadMsg;

#if defined (DUI_BUILD_FOR_WIN) && !defined (DUI_BUILD_FOR_SDL)
private:
    /** The delayed messages and their container lock (main thread)
    */
    std::vector<size_t> m_winTaskIds;
    std::mutex m_winTaskMutex;

#endif
};

}
#endif //UI_CORE_FRAMEWORK_THREAD_H_
