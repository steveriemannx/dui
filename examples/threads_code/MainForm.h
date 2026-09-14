#ifndef EXAMPLES_THREADS_CODE_MAIN_FORM_H_
#define EXAMPLES_THREADS_CODE_MAIN_FORM_H_

// dui
#include "dui/dui.h"
#include <chrono>
#include <memory>
#include <vector>

class WorkerThread;

class MainForm : public ui::WindowImplBase
{
    typedef ui::WindowImplBase BaseClass;
public:
    MainForm() = default;
    virtual ~MainForm() override = default;

    /** Resource-related interfaces.
     *  This is the pure-code (code) mode: the UI is built entirely in C++
     *  (see BuildUI), so no XML skin is loaded.
     */
    virtual std::string GetSkinFolder() override { return "threads"; }
    virtual std::string GetSkinFile() override { return ""; }

    /** Called after the window is created, for subclasses to do some initialization work
     */
    virtual void OnInitWindow() override;

    /** Called after the window has been closed, for subclasses to do cleanup work
     */
    virtual void OnCloseWindow() override;

private:
    void BindEvents();
    void SetupWindow();
    void BuildUI();

    /** Start the worker thread pool
     */
    void StartThreads();

    /** Stop the worker thread pool
     */
    void StopThreads();

    /** Get the number of worker threads in the thread pool
     */
    int32_t GetPoolThreadCount() const;

    /** Update the UI state (can be called from the worker thread)
     */
    void UpdateUI();

    /** Output a log (can be called from the worker thread)
     */
public:
    void PrintLog(const std::string& log);

    /** Execute a task in the worker thread
     * @param [in] nThreadIdentifier Thread identifier
     */
    bool RunTaskInThread(int32_t nThreadIdentifier);

    /** Execute a specific task in the worker thread
     */
    void ExecuteTaskInThread();

    /** Update the running time
     */
    void UpdateRunningTime();

private:
    /** Log display control
     */
    ui::RichEdit* m_pLogEdit = nullptr;

    /** Running time display
     */
    ui::Label* m_pRunningTimeLabel = nullptr;

    /** Start time
     */
    std::chrono::steady_clock::time_point m_startTime;

    /** Log sequence number
     */
    int32_t m_nLogLineNumber = 0;

    /** User-defined thread pool: the thread identifier is ui::kThreadUser + the element index of the vector
     */
    std::vector<std::shared_ptr<WorkerThread>> m_threadPools;
};

#endif //EXAMPLES_THREADS_CODE_MAIN_FORM_H_
