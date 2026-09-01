#include "MainForm.h"
#include "dui/Utils/UiBuilder.h"
#include "WorkerThread.h"

void MainForm::OnInitWindow()
{
    m_pLogEdit = ui::Find<ui::RichEdit>(this, "log_view");
    m_pRunningTimeLabel = ui::Find<ui::Label>(this, "running_time");
    m_startTime = std::chrono::steady_clock::now();

    BindEvents();
    BaseClass::OnInitWindow();

    StartThreads();
}

void MainForm::OnCloseWindow()
{
    StopThreads();
}

void MainForm::BindEvents()
{
    ui::Button* pButtonStart = ui::Find<ui::Button>(this, "start_threads");
    ui::Button* pButtonStop = ui::Find<ui::Button>(this, "stop_threads");

    if (pButtonStart != nullptr) {
        pButtonStart->SetEnabled(false);
        pButtonStart->AttachClick([this, pButtonStart, pButtonStop](const ui::EventArgs&) {
            // Start the worker thread
            if (m_threadPools.empty()) {
                StartThreads();
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
            StopThreads();
            pButtonStop->SetEnabled(false);
            if (pButtonStart != nullptr) {
                pButtonStart->SetEnabled(true);
            }
            return true;
            });
    }

    ui::Button* pRunTaskButton = ui::Find<ui::Button>(this, "run_task_in_threads");
    if (pRunTaskButton != nullptr) {
        pRunTaskButton->AttachClick([this](const ui::EventArgs&) {
            // Execute the task in the worker thread
            int32_t nThreadIdentifier = 1;
            ui::RichEdit* pThreadIdentifier = ui::Find<ui::RichEdit>(this, "threads_identifier");
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

void MainForm::StartThreads()
{
    ui::GlobalManager::Instance().AssertUIThread();
    // Create the thread pool (after a thread is created, the thread identifier is used for inter-thread communication: ui::kThreadUser + nThread)
    const size_t nMaxThreads = 6;
    for (size_t nThread = 0; nThread < nMaxThreads; ++nThread) {
        std::shared_ptr<WorkerThread> pThread = std::make_shared<WorkerThread>(ui::kThreadUser + (int32_t)nThread);
        pThread->SetMainForm(this);
        pThread->Start();
        m_threadPools.push_back(pThread);
    }

    // Update the UI state
    UpdateUI();
}

void MainForm::StopThreads()
{
    ui::GlobalManager::Instance().AssertUIThread();
    for (auto pThread : m_threadPools) {
        if (pThread != nullptr) {
            pThread->Stop();
        }
    }
    m_threadPools.clear();

    // Update the UI state
    UpdateUI();
}

int32_t MainForm::GetPoolThreadCount() const
{
    return (int32_t)m_threadPools.size();
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
    DString log = ui::StringUtil::Printf("[OS Thread ID: %s][UI library thread identifier: %d]: MainForm::ExecuteTaskInThread is running in a worker thread",
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
        DString msg = ui::StringUtil::Printf("%02d:%02d:%02d", seconds / 60 / 60, seconds / 60, seconds % 60);
        m_pRunningTimeLabel->SetText(msg);
    }
}

void MainForm::UpdateUI()
{
    if (!ui::GlobalManager::Instance().IsInUIThread()) {
        // Currently executed in the worker thread; send the function execution to the main thread (implemented via inter-thread communication; UiBind ensures no illegal access even when the this pointer becomes invalid)
        ui::GlobalManager::Instance().Thread().PostTask(ui::kThreadUI, UiBind(&MainForm::UpdateUI, this));
    }
    else {
        // Set parameters such as the number of threads
        ui::RichEdit* pThreadIdentifier = ui::Find<ui::RichEdit>(this, "threads_identifier");
        ui::Button* pRunTaskButton = ui::Find<ui::Button>(this, "run_task_in_threads");
        if (pThreadIdentifier != nullptr) {
            if (GetPoolThreadCount() > 0) {
                pThreadIdentifier->SetMinNumber(ui::kThreadUser);
                pThreadIdentifier->SetMaxNumber(ui::kThreadUser + GetPoolThreadCount() - 1);
                pThreadIdentifier->SetText("1");
                if (pRunTaskButton != nullptr) {
                    pRunTaskButton->SetEnabled(true);
                }
            }
            else {
                pThreadIdentifier->SetMinNumber(0);
                pThreadIdentifier->SetMaxNumber(0);
                pThreadIdentifier->SetText("0");
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
            DString line = ui::StringUtil::Printf("%04d: ", ++m_nLogLineNumber);
            line += log;
            line += "\n";
            m_pLogEdit->AppendText(line);
        }
    }
}
