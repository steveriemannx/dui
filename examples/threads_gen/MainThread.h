#ifndef EXAMPLES_MAIN_THREAD_H_
#define EXAMPLES_MAIN_THREAD_H_

// duilib
#include "duilib/duilib.h"

class WorkerThread;
class MainForm;

/** Main thread (UI thread)
*/
class MainThread : public ui::FrameworkThread
{
public:
    MainThread();
    virtual ~MainThread() override;

public:
    /** Start the worker thread
    */
    void StartThreads();

    /** Stop the worker thread
    */
    void StopThreads();

    /** Get the number of worker threads in the thread pool
    */
    int32_t GetPoolThreadCount() const;

    /** Output a log; can be called from any thread and is finally executed in the main thread
    * @param [in] log The log content
    */
    void PrintLog(const DString& log);

private:
    /** Initialize before running; called before entering the message loop
    */
    virtual void OnInit() override;

    /** Clean up on exit; called after leaving the message loop
    */
    virtual void OnCleanup() override;

private:
    /** Interface to the main form
    */
    ui::ControlPtrT<MainForm> m_pMainForm;

    /** User-defined thread pool: the thread identifier is ui::kThreadUser + the element index of the vector
    */
    std::vector<std::shared_ptr<WorkerThread>> m_threadPools;
};

#endif // EXAMPLES_MAIN_THREAD_H_
