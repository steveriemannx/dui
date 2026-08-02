#include "WorkerThread.h"
#include "MainForm.h"

WorkerThread::WorkerThread(int32_t nThreadIdentifier)
    : FrameworkThread(_T("WorkerThread"), nThreadIdentifier),
    m_pMainForm(nullptr)
{
}

WorkerThread::~WorkerThread()
{
}

void WorkerThread::OnInit()
{
    // Output a log
    PrintLog(_T("WorkerThread::OnInit"));
}

void WorkerThread::OnCleanup()
{
    // Output a log
    PrintLog(_T("WorkerThread::OnCleanup"));
}

void WorkerThread::SetMainForm(MainForm* pMainForm)
{
    m_pMainForm = pMainForm;
}

void WorkerThread::PrintLog(const DString& log)
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
