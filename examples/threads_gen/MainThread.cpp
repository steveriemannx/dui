#include "MainThread.h"
#include "WorkerThread.h"
#include "MainForm.h"

MainThread::MainThread() :
    FrameworkThread(_T("MainThread"), ui::kThreadUI),
    m_pMainForm(nullptr)
{
}

MainThread::~MainThread()
{
}

void MainThread::OnInit()
{
    // Output a log message
    PrintLog(_T("MainThread::OnInit"));

    // Initialize global resources, using the local folder as the resource
    ui::FilePath resourcePath = ui::FilePathUtil::GetCurrentModuleDirectory();
    resourcePath += _T("resources\\");
    ui::GlobalManager::Instance().Startup(ui::LocalFilesResParam(resourcePath));

    // Create a default centered window with a shadow
    m_pMainForm = new MainForm(this);
    m_pMainForm->CreateWnd(nullptr, ui::WindowCreateParam(_T("threads (Generated Code)"), true));
    m_pMainForm->PostQuitMsgWhenClosed(true);
    m_pMainForm->ShowWindow(ui::kSW_SHOW_NORMAL);

    StartThreads();
}

void MainThread::OnCleanup()
{
    // Output a log message
    PrintLog(_T("MainThread::OnCleanup"));

    StopThreads();
    m_pMainForm = nullptr;

    // Clean up common resources
    ui::GlobalManager::Instance().Shutdown();
}

void MainThread::StartThreads()
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

void MainThread::StopThreads()
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

int32_t MainThread::GetPoolThreadCount() const
{
    return (int32_t)m_threadPools.size();
}

void MainThread::PrintLog(const DString& log)
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
