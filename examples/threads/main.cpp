#include "dui/dui.h"
#include "MainForm.h"
#include "WorkerThread.h"
#include "dui/Utils/AppEntry.h"

/** App: FrameworkThread subclass that serves as the DUI_APP_ENTRY target.
 *  RunMessageLoop() calls OnInit() -> message loop -> OnCleanup().
 *  Owns the worker-thread pool; the main form controls it through the IMainThread interface.
 */
class App : public ui::FrameworkThread, public IMainThread
{
public:
    App() : FrameworkThread(_T("App"), ui::kThreadUI), m_pMainForm(nullptr) {}

    void Run() { RunMessageLoop(); }

    //IMainThread
    virtual void StartThreads() override;
    virtual void StopThreads() override;
    virtual int32_t GetPoolThreadCount() const override;

private:
    virtual void OnInit() override;
    virtual void OnCleanup() override;

    /** Output a log; can be called from any thread and is finally executed in the main thread
    * @param [in] log The log content
    */
    void PrintLog(const DString& log);

private:
    /** Interface to the main form
    */
    ui::ControlPtrT<MainForm> m_pMainForm;

    /** User-defined thread pool: the thread identifier is ui::kThreadUser + the element index of the vector
    */
    std::vector<std::shared_ptr<WorkerThread>> m_threadPools;
};

void App::OnInit()
{
    // Output a log message
    PrintLog(_T("App::OnInit"));

    // Initialize global resources, using the local folder as the resource
    ui::FilePath resourcePath = ui::FilePathUtil::GetCurrentModuleDirectory();
    resourcePath += _T("resources\\");
    ui::GlobalManager::Instance().Startup(ui::LocalFilesResParam(resourcePath));

    // Create a default centered window with a shadow
    m_pMainForm = new MainForm(this);
    m_pMainForm->CreateWnd(nullptr, ui::WindowCreateParam(_T("threads"), true));
    m_pMainForm->PostQuitMsgWhenClosed(true);
    m_pMainForm->ShowWindow(ui::kSW_SHOW_NORMAL);

    StartThreads();
}

void App::OnCleanup()
{
    // Output a log message
    PrintLog(_T("App::OnCleanup"));

    StopThreads();
    m_pMainForm = nullptr;

    // Clean up common resources
    ui::GlobalManager::Instance().Shutdown();
}

void App::StartThreads()
{
    ui::GlobalManager::Instance().AssertUIThread();
    // Create the thread pool (after a thread is created, the thread identifier is used for inter-thread communication: ui::kThreadUser + nThread)
    const size_t nMaxThreads = 6;
    for (size_t nThread = 0; nThread < nMaxThreads; ++nThread) {
        std::shared_ptr<WorkerThread> pThread = std::make_shared<WorkerThread>(ui::kThreadUser + (int32_t)nThread);
        pThread->SetMainForm(m_pMainForm.get());
        pThread->Start();
        m_threadPools.push_back(pThread);
    }

    // Update the UI state
    if (m_pMainForm != nullptr) {
        m_pMainForm->UpdateUI();
    }
}

void App::StopThreads()
{
    ui::GlobalManager::Instance().AssertUIThread();
    for (auto pThread : m_threadPools) {
        if (pThread != nullptr) {
            pThread->Stop();
        }
    }
    m_threadPools.clear();

    // Update the UI state
    if (m_pMainForm != nullptr) {
        m_pMainForm->UpdateUI();
    }
}

int32_t App::GetPoolThreadCount() const
{
    return (int32_t)m_threadPools.size();
}

void App::PrintLog(const DString& log)
{
    DString logMsg = ui::StringUtil::Printf(_T("[Calling Thread ID: %s][Thread ID: %s, Thread Name: %s, Thread Identifier: %d]: %s"),
                                            ThreadIdToString(std::this_thread::get_id()).c_str(),
                                            ThreadIdToString(GetThreadId()).c_str(),
                                            GetThreadName().c_str(),
                                            GetThreadIdentifier(),
                                            log.c_str());
    if (m_pMainForm != nullptr) {
        m_pMainForm->PrintLog(logMsg);
    }
}

DUI_APP_ENTRY(App)
