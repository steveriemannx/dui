#include "MainForm.h"
#include "WorkerThread.h"
#include "dui/Utils/UiBuilder.h"

void MainForm::OnInitWindow()
{
    SetupWindow();
    BuildUI();

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

void MainForm::SetupWindow()
{
    // The content is laid out at the OS display scale (e.g. 125%) on Windows,
    // so the window must be that much larger to keep everything visible;
    // other platforms lay out 1:1 and keep the original size.
#if defined(DUI_BUILD_FOR_WIN)
    SetWindowSize(1250, 875);
#else
    SetWindowSize(1000, 700);
#endif
    CenterWindow();

    // Use the OS-provided system shadow on all platforms.
    SetShadowAttached(true);
#if defined(DUI_BUILD_FOR_LINUX)
    SetShadowType(ui::Shadow::ShadowType::kShadowDrawDefault);
#else
    SetShadowType(ui::Shadow::ShadowType::kShadowSystemDefault);
    SetLayeredWindow(false, false);
#endif
    SetEnableShadowSnap(true);
    SetShadowBorderSize(0);

    SetSizeBox(ui::UiRect(4, 4, 4, 4), true);
    SetCaptionRect(ui::UiRect(0, 0, 0, 36), true);
}

void MainForm::BuildUI()
{
    // Corresponding to the threads.xml layout
    auto* pRoot = ui::Create<ui::VBox>(this, {{"bkcolor", "bk_wnd_darkcolor"}, {"visible", "true"}});

    // Title bar area
    auto* pCaption = ui::Create<ui::HBox>(this, {{"name", "window_caption_bar"}, {"width", "stretch"}, {"height", "36"}, {"bkcolor", "bk_wnd_lightcolor"}});
    pRoot->AddItem(pCaption);

    auto* pTitle = ui::Create<ui::Label>(this, {{"text", "dui Multithreading Example"}, {"height", "32"}, {"text_align", "vcenter"}, {"margin", "10,0,0,0"}, {"mouse_enabled", "false"}});
    pCaption->AddItem(pTitle);

    auto* pSpacer = ui::Create<ui::Control>(this, {{"mouse_enabled", "false"}});
    pCaption->AddItem(pSpacer);

    auto* pMinBtn = ui::Create<ui::Button>(this, {{"class", "btn_wnd_min_11"}, {"name", "minbtn"}, {"height", "32"}, {"width", "40"}, {"margin", "0,2,0,2"}, {"tooltip_text", "Minimize"}});
    pCaption->AddItem(pMinBtn);

    auto* pMaxBox = ui::Create<ui::Box>(this, {{"height", "stretch"}, {"width", "40"}, {"margin", "0,2,0,2"}});
    pCaption->AddItem(pMaxBox);

    auto* pMaxBtn = ui::Create<ui::Button>(this, {{"class", "btn_wnd_max_11"}, {"name", "maxbtn"}, {"height", "32"}, {"width", "stretch"}, {"tooltip_text", "Maximize"}});
    pMaxBox->AddItem(pMaxBtn);

    auto* pRestoreBtn = ui::Create<ui::Button>(this, {{"class", "btn_wnd_restore_11"}, {"name", "restorebtn"}, {"height", "32"}, {"width", "stretch"}, {"visible", "false"}, {"tooltip_text", "Restore"}});
    pMaxBox->AddItem(pRestoreBtn);

    auto* pCloseBtn = ui::Create<ui::Button>(this, {{"class", "btn_wnd_close_11"}, {"name", "closebtn"}, {"height", "stretch"}, {"width", "40"}, {"margin", "0,0,0,2"}, {"tooltip_text", "Close"}});
    pCaption->AddItem(pCloseBtn);

    // Work area
    auto* pContent = ui::Create<ui::VBox>(this, {{"padding", "6,6,6,6"}});
    pRoot->AddItem(pContent);

    // Toolbar
    auto* pToolBar = ui::Create<ui::HBox>(this, {{"margin", "0,0,0,0"}, {"valign", "center"}, {"halign", "center"}, {"height", "48"}});
    pContent->AddItem(pToolBar);

    auto* pStartBtn = ui::Create<ui::Button>(this, {{"class", "btn_global_blue_80x30"}, {"name", "start_threads"}, {"text", "Start Worker Threads"}, {"margin", "2,8,2,2"}});
    pToolBar->AddItem(pStartBtn);

    auto* pStopBtn = ui::Create<ui::Button>(this, {{"class", "btn_global_blue_80x30"}, {"name", "stop_threads"}, {"text", "Stop Worker Threads"}, {"margin", "2,8,2,2"}});
    pToolBar->AddItem(pStopBtn);

    auto* pLabel = ui::Create<ui::Label>(this, {{"text", "Run task in the following thread, thread identifier:"}, {"height", "100%"}, {"width", "auto"}, {"text_align", "right,vcenter"}, {"margin", "8,0,0,0"}});
    pToolBar->AddItem(pLabel);

    auto* pThreadId = ui::Create<ui::RichEdit>(this, {{"class", "rich_edit_spin simple_border"}, {"name", "threads_identifier"}, {"text", "1"}, {"bkcolor", "white"}, {"valign", "center"}});
    pToolBar->AddItem(pThreadId);

    auto* pRunTaskBtn = ui::Create<ui::Button>(this, {{"class", "btn_global_blue_80x30"}, {"name", "run_task_in_threads"}, {"text", "Run Task in Worker Thread"}, {"width", "180"}, {"margin", "8,8,2,2"}});
    pToolBar->AddItem(pRunTaskBtn);

    auto* pRunningTime = ui::Create<ui::Label>(this, {{"name", "running_time"}, {"text", "00:00:00"}, {"height", "100%"}, {"width", "100%"}, {"text_align", "hcenter,vcenter"}, {"margin", "8,0,0,0"}});
    pToolBar->AddItem(pRunningTime);

    auto* pLine = ui::Create<ui::Line>(this, {{"height", "1"}});
    pContent->AddItem(pLine);

    auto* pLogView = ui::Create<ui::RichEdit>(this, {{"class", "simple simple_border"}, {"name", "log_view"}, {"text_align", "left,top"}, {"bkcolor", "white"}, {"width", "stretch"}, {"height", "stretch"}, {"hide_selection", "true"}, {"multi_line", "true"}, {"vscrollbar", "true"}, {"auto_vscroll", "true"}, {"hscrollbar", "false"}, {"normal_text_color", "darkcolor"}, {"want_return_msg", "true"}, {"rich_text", "false"}, {"default_context_menu", "true"}});
    pContent->AddItem(pLogView);

    AttachBox(pRoot);
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

    std::string systemThreadId = ui::FrameworkThread::ThreadIdToString(std::this_thread::get_id());
    int32_t nUIThreadIdentifier = ui::GlobalManager::Instance().Thread().GetCurrentThreadIdentifier();

    // Execute the actual computing task; here it only displays a log message (inter-thread communication is also used to let the main thread update the log data to the UI)
    std::string log = ui::StringUtil::Printf("[OS Thread ID: %s][UI library thread identifier: %d]: MainForm::ExecuteTaskInThread is running in a worker thread",
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
        std::string msg = ui::StringUtil::Printf("%02d:%02d:%02d", seconds / 60 / 60, seconds / 60, seconds % 60);
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

void MainForm::PrintLog(const std::string& log)
{
    if (!ui::GlobalManager::Instance().IsInUIThread()) {
        // Currently executed in the worker thread; send the function execution to the main thread (implemented via inter-thread communication; UiBind ensures no illegal access even when the this pointer becomes invalid)
        ui::GlobalManager::Instance().Thread().PostTask(ui::kThreadUI, UiBind(&MainForm::PrintLog, this, log));
    }
    else {
        // Currently executed in the main thread (UI thread): display the information on the UI
        if (m_pLogEdit != nullptr) {
            std::string line = ui::StringUtil::Printf("%04d: ", ++m_nLogLineNumber);
            line += log;
            line += "\n";
            m_pLogEdit->AppendText(line);
        }
    }
}
