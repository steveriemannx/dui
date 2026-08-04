#include "MainForm.h"

MainForm::MainForm(IMainThread* pMainThread):
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
    return _T("");
}

DString MainForm::GetSkinFile()
{
    // Pure code mode: no layout XML is loaded
    return _T("");
}

void MainForm::BuildUI()
{
    // Corresponding to the threads.xml layout
    ui::VBox* pRoot = new ui::VBox(this);
    pRoot->SetBkColor(_T("bk_wnd_darkcolor"));

    // Title bar area
    ui::HBox* pCaption = new ui::HBox(this);
    pCaption->SetAttribute(_T("name"), _T("window_caption_bar"));
    pCaption->SetAttribute(_T("width"), _T("stretch"));
    pCaption->SetAttribute(_T("height"), _T("36"));
    pCaption->SetBkColor(_T("bk_wnd_lightcolor"));
    pRoot->AddItem(pCaption);

    ui::Label* pTitle = new ui::Label(this);
    pTitle->SetText(_T("dui Multithreading Example"));
    pTitle->SetAttribute(_T("height"), _T("32"));
    pTitle->SetAttribute(_T("text_align"), _T("vcenter"));
    pTitle->SetAttribute(_T("margin"), _T("10,0,0,0"));
    pTitle->SetAttribute(_T("mouse_enabled"), _T("false"));
    pCaption->AddItem(pTitle);

    ui::Control* pSpacer = new ui::Control(this);
    pSpacer->SetAttribute(_T("mouse_enabled"), _T("false"));
    pCaption->AddItem(pSpacer);

    ui::Button* pMinBtn = new ui::Button(this);
    pMinBtn->SetClass(_T("btn_wnd_min_11"));
    pMinBtn->SetAttribute(_T("height"), _T("32"));
    pMinBtn->SetAttribute(_T("width"), _T("40"));
    pMinBtn->SetName(_T("minbtn"));
    pMinBtn->SetAttribute(_T("margin"), _T("0,2,0,2"));
    pMinBtn->SetToolTipText(_T("Minimize"));
    pCaption->AddItem(pMinBtn);

    ui::Box* pMaxBox = new ui::Box(this);
    pMaxBox->SetAttribute(_T("height"), _T("stretch"));
    pMaxBox->SetAttribute(_T("width"), _T("40"));
    pMaxBox->SetAttribute(_T("margin"), _T("0,2,0,2"));
    pCaption->AddItem(pMaxBox);

    ui::Button* pMaxBtn = new ui::Button(this);
    pMaxBtn->SetClass(_T("btn_wnd_max_11"));
    pMaxBtn->SetAttribute(_T("height"), _T("32"));
    pMaxBtn->SetAttribute(_T("width"), _T("stretch"));
    pMaxBtn->SetName(_T("maxbtn"));
    pMaxBtn->SetToolTipText(_T("Maximize"));
    pMaxBox->AddItem(pMaxBtn);

    ui::Button* pRestoreBtn = new ui::Button(this);
    pRestoreBtn->SetClass(_T("btn_wnd_restore_11"));
    pRestoreBtn->SetAttribute(_T("height"), _T("32"));
    pRestoreBtn->SetAttribute(_T("width"), _T("stretch"));
    pRestoreBtn->SetName(_T("restorebtn"));
    pRestoreBtn->SetVisible(false);
    pRestoreBtn->SetToolTipText(_T("Restore"));
    pMaxBox->AddItem(pRestoreBtn);

    ui::Button* pCloseBtn = new ui::Button(this);
    pCloseBtn->SetClass(_T("btn_wnd_close_11"));
    pCloseBtn->SetAttribute(_T("height"), _T("stretch"));
    pCloseBtn->SetAttribute(_T("width"), _T("40"));
    pCloseBtn->SetName(_T("closebtn"));
    pCloseBtn->SetAttribute(_T("margin"), _T("0,0,0,2"));
    pCloseBtn->SetToolTipText(_T("Close"));
    pCaption->AddItem(pCloseBtn);

    // Work area
    ui::VBox* pContent = new ui::VBox(this);
    pContent->SetAttribute(_T("padding"), _T("6,6,6,6"));
    pRoot->AddItem(pContent);

    // Toolbar
    ui::HBox* pToolBar = new ui::HBox(this);
    pToolBar->SetAttribute(_T("margin"), _T("0,0,0,0"));
    pToolBar->SetAttribute(_T("valign"), _T("center"));
    pToolBar->SetAttribute(_T("halign"), _T("center"));
    pToolBar->SetAttribute(_T("height"), _T("48"));
    pContent->AddItem(pToolBar);

    ui::Button* pStartBtn = new ui::Button(this);
    pStartBtn->SetClass(_T("btn_global_blue_80x30"));
    pStartBtn->SetName(_T("start_threads"));
    pStartBtn->SetText(_T("Start Worker Threads"));
    pStartBtn->SetAttribute(_T("margin"), _T("2,8,2,2"));
    pToolBar->AddItem(pStartBtn);

    ui::Button* pStopBtn = new ui::Button(this);
    pStopBtn->SetClass(_T("btn_global_blue_80x30"));
    pStopBtn->SetName(_T("stop_threads"));
    pStopBtn->SetText(_T("Stop Worker Threads"));
    pStopBtn->SetAttribute(_T("margin"), _T("2,8,2,2"));
    pToolBar->AddItem(pStopBtn);

    ui::Label* pLabel = new ui::Label(this);
    pLabel->SetText(_T("Run task in the following thread, thread identifier:"));
    pLabel->SetAttribute(_T("height"), _T("100%"));
    pLabel->SetAttribute(_T("width"), _T("auto"));
    pLabel->SetAttribute(_T("text_align"), _T("right,vcenter"));
    pLabel->SetAttribute(_T("margin"), _T("8,0,0,0"));
    pToolBar->AddItem(pLabel);

    ui::RichEdit* pThreadId = new ui::RichEdit(this);
    pThreadId->SetClass(_T("rich_edit_spin simple_border"));
    pThreadId->SetName(_T("threads_identifier"));
    pThreadId->SetText(_T("1"));
    pThreadId->SetBkColor(_T("white"));
    pThreadId->SetAttribute(_T("valign"), _T("center"));
    pToolBar->AddItem(pThreadId);

    ui::Button* pRunTaskBtn = new ui::Button(this);
    pRunTaskBtn->SetClass(_T("btn_global_blue_80x30"));
    pRunTaskBtn->SetName(_T("run_task_in_threads"));
    pRunTaskBtn->SetAttribute(_T("width"), _T("180"));
    pRunTaskBtn->SetText(_T("Run Task in Worker Thread"));
    pRunTaskBtn->SetAttribute(_T("margin"), _T("8,8,2,2"));
    pToolBar->AddItem(pRunTaskBtn);

    ui::Label* pRunningTime = new ui::Label(this);
    pRunningTime->SetName(_T("running_time"));
    pRunningTime->SetText(_T("00:00:00"));
    pRunningTime->SetAttribute(_T("height"), _T("100%"));
    pRunningTime->SetAttribute(_T("width"), _T("100%"));
    pRunningTime->SetAttribute(_T("text_align"), _T("hcenter,vcenter"));
    pRunningTime->SetAttribute(_T("margin"), _T("8,0,0,0"));
    pToolBar->AddItem(pRunningTime);

    ui::Line* pLine = new ui::Line(this);
    pLine->SetAttribute(_T("height"), _T("1"));
    pContent->AddItem(pLine);

    ui::RichEdit* pLogView = new ui::RichEdit(this);
    pLogView->SetClass(_T("simple simple_border"));
    pLogView->SetName(_T("log_view"));
    pLogView->SetAttribute(_T("text_align"), _T("left,top"));
    pLogView->SetBkColor(_T("white"));
    pLogView->SetAttribute(_T("width"), _T("stretch"));
    pLogView->SetAttribute(_T("height"), _T("stretch"));
    pLogView->SetAttribute(_T("hide_selection"), _T("true"));
    pLogView->SetAttribute(_T("multi_line"), _T("true"));
    pLogView->SetAttribute(_T("vscrollbar"), _T("true"));
    pLogView->SetAttribute(_T("auto_vscroll"), _T("true"));
    pLogView->SetAttribute(_T("hscrollbar"), _T("false"));
    pLogView->SetAttribute(_T("normal_text_color"), _T("darkcolor"));
    pLogView->SetAttribute(_T("want_return_msg"), _T("true"));
    pLogView->SetAttribute(_T("rich_text"), _T("false"));
    pLogView->SetAttribute(_T("default_context_menu"), _T("true"));
    pContent->AddItem(pLogView);

    AttachBox(pRoot);
}

void MainForm::OnInitWindow()
{
    SetSizeBox(ui::UiRect(4, 4, 4, 4), false);
    SetCaptionRect(ui::UiRect(0, 0, 0, 36), false);

    BuildUI();

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
