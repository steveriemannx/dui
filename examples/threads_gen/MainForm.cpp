#include "MainForm.h"
#include "MainThread.h"
#include "generated_ui.inc"  // Build-time generated pure C++ UI code (from threads.xml)

MainForm::MainForm(MainThread* pMainThread):
    m_pMainThread(pMainThread),
    m_pLogEdit(nullptr),
    m_pRunningTimeLabel(nullptr),
    m_nLogLineNumber(0)
{
}

MainForm::~MainForm()
{
}

DString MainForm::GetSkinFolder()
{
    return _T("threads");
}

DString MainForm::GetSkinFile()
{
    // No XML file - UI is generated at build time from threads.xml
    return _T("");
}

void MainForm::OnInitWindow()
{
    SetSizeBox(ui::UiRect(4, 4, 4, 4), false);
    SetCaptionRect(ui::UiRect(0, 0, 0, 36), false);

    // Build-time generated from threads.xml
    InitThreads(this);

    BaseClass::OnInitWindow();
    m_pLogEdit = dynamic_cast<ui::RichEdit*>(FindControl(_T("log_view")));
    m_pRunningTimeLabel = dynamic_cast<ui::Label*>(FindControl(_T("running_time")));
    m_startTime = std::chrono::steady_clock::now();

    ui::Button* pButtonStart = dynamic_cast<ui::Button*>(FindControl(_T("start_threads")));
    ui::Button* pButtonStop = dynamic_cast<ui::Button*>(FindControl(_T("stop_threads")));

    if (pButtonStart != nullptr) {
        pButtonStart->SetEnabled(false);
        pButtonStart->AttachClick([this, pButtonStart, pButtonStop](const ui::EventArgs&) {
            // Start the worker thread
            if (m_pMainThread != nullptr) {
                m_pMainThread->StartThreads();
            }
            pButtonStart->SetEnabled(false);
            if (pButtonStop != nullptr) {
                pButtonStop->SetEnabled(true);
            }
            return true;
            });
    }

    if (pButtonStop != nullptr) {
        pButtonStop->AttachClick([this, pButtonStart, pButtonStop](const ui::EventArgs&) {
            // Stop the worker thread
            if (m_pMainThread != nullptr) {
                m_pMainThread->StopThreads();
            }
            pButtonStop->SetEnabled(false);
            if (pButtonStart != nullptr) {
                pButtonStart->SetEnabled(true);
            }
            return true;
            });
    }

    ui::Button* pRunTaskButton = dynamic_cast<ui::Button*>(FindControl(_T("run_task_in_threads")));
    if (pRunTaskButton != nullptr) {
        pRunTaskButton->AttachClick([this](const ui::EventArgs&) {
            // Execute the task in the worker thread
            int32_t nThreadIdentifier = 1;
            ui::RichEdit* pThreadIdentifier = dynamic_cast<ui::RichEdit*>(FindControl(_T("threads_identifier")));
            if (pThreadIdentifier != nullptr) {
                // Get the worker thread identifier from the UI
                nThreadIdentifier = (int32_t)pThreadIdentifier->GetTextNumber();
            }
            bool bRet = RunTaskInThread(nThreadIdentifier);
            ASSERT(bRet);
            return true;
            });
    }

    // Start a timer to update the running time on the UI periodically (once per second)
    ui::GlobalManager::Instance().Thread().PostRepeatedTask(ui::kThreadUI, UiBind(&MainForm::UpdateRunningTime, this), 1000);
}

bool MainForm::RunTaskInThread(int32_t nThreadIdentifier)
{
    // Execute the task in the worker thread
    bool bRet = false;
    ASSERT(ui::GlobalManager::Instance().IsInUIThread());
    if (!ui::GlobalManager::Instance().IsInUIThread()) {
        return bRet;
    }
    // Execute the ExecuteTaskInThread function in the worker thread (the function can also take parameters)
    bRet = ui::GlobalManager::Instance().Thread().PostTask(nThreadIdentifier, UiBind(&MainForm::ExecuteTaskInThread, this));

    // Execute an anonymous function in the worker thread (demonstration)
    if (bRet) {
        auto task = [this]() {
                ExecuteTaskInThread();
            };
        bRet = ui::GlobalManager::Instance().Thread().PostTask(nThreadIdentifier, task);
    }
    return bRet;
}

void MainForm::ExecuteTaskInThread()
{
    ASSERT(!ui::GlobalManager::Instance().IsInUIThread());

    DString systemThreadId = ui::FrameworkThread::ThreadIdToString(std::this_thread::get_id());
    int32_t nUIThreadIdentifier = ui::GlobalManager::Instance().Thread().GetCurrentThreadIdentifier();

    // Execute the actual computing task; here it only displays a log message (inter-thread communication is also used to let the main thread update the log data to the UI)
    DString log = ui::StringUtil::Printf(_T("[OS Thread ID: %s][UI library thread identifier: %d]: MainForm::ExecuteTaskInThread is running in a worker thread"),
                                         systemThreadId.c_str(),
                                         nUIThreadIdentifier);
    PrintLog(log);
}

void MainForm::UpdateRunningTime()
{
    ASSERT(ui::GlobalManager::Instance().IsInUIThread());
    if (m_pRunningTimeLabel != nullptr) {
        // Time shown in the UI: hours:minutes:seconds
        auto thisTime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - m_startTime);
        int32_t seconds = (int32_t)thisTime.count();
        DString msg = ui::StringUtil::Printf(_T("%02d:%02d:%02d"), seconds / 60 / 60, seconds / 60, seconds % 60);
        m_pRunningTimeLabel->SetText(msg);
    }
}

void MainForm::UpdateUI()
{
    if (m_pMainThread == nullptr) {
        return;
    }
    if (!ui::GlobalManager::Instance().IsInUIThread()) {
        // Currently executed in the worker thread; send the function execution to the main thread (implemented via inter-thread communication; UiBind ensures no illegal access even when the this pointer becomes invalid)
        ui::GlobalManager::Instance().Thread().PostTask(ui::kThreadUI, UiBind(&MainForm::UpdateUI, this));
    }
    else {
        // Set parameters such as the number of threads
        ui::RichEdit* pThreadIdentifier = dynamic_cast<ui::RichEdit*>(FindControl(_T("threads_identifier")));
        ui::Button* pRunTaskButton = dynamic_cast<ui::Button*>(FindControl(_T("run_task_in_threads")));
        if (pThreadIdentifier != nullptr) {
            if (m_pMainThread->GetPoolThreadCount() > 0) {
                pThreadIdentifier->SetMinNumber(ui::kThreadUser);
                pThreadIdentifier->SetMaxNumber(ui::kThreadUser + m_pMainThread->GetPoolThreadCount() - 1);
                pThreadIdentifier->SetText(_T("1"));
                if (pRunTaskButton != nullptr) {
                    pRunTaskButton->SetEnabled(true);
                }
            }
            else {
                pThreadIdentifier->SetMinNumber(0);
                pThreadIdentifier->SetMaxNumber(0);
                pThreadIdentifier->SetText(_T("0"));
                if (pRunTaskButton != nullptr) {
                    pRunTaskButton->SetEnabled(false);
                }
            }
        }
    }
}

void MainForm::PrintLog(const DString& log)
{
    if (!ui::GlobalManager::Instance().IsInUIThread()) {
        // Currently executed in the worker thread; send the function execution to the main thread (implemented via inter-thread communication; UiBind ensures no illegal access even when the this pointer becomes invalid)
        ui::GlobalManager::Instance().Thread().PostTask(ui::kThreadUI, UiBind(&MainForm::PrintLog, this, log));
    }
    else {
        // Currently executed in the main thread (UI thread): display the information on the UI
        if (m_pLogEdit != nullptr) {
            DString line = ui::StringUtil::Printf(_T("%04d: "), ++m_nLogLineNumber);
            line += log;
            line += _T("\n");
            m_pLogEdit->AppendText(line);
        }
    }
}
