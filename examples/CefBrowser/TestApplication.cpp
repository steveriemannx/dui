#include "TestApplication.h"
#include "MainThread.h"

// duilib
#include "duilib/duilib_cef.h"

TestApplication::TestApplication()
{
}

TestApplication::~TestApplication()
{
}

TestApplication& TestApplication::Instance()
{
    static TestApplication self;
    return self;
}

int TestApplication::Run(int argc, char** argv)
{
    //Pre-initialize the CEF module functions (prepare the basic environment for loading the CEF module)
    ui::CefManager::GetInstance()->InitEnv();

    // Parse command-line arguments.
    CefRefPtr<CefCommandLine> command_line = CefCommandLine::CreateCommandLine();
#ifdef DUILIB_BUILD_FOR_WIN
    command_line->InitFromString(::GetCommandLineW());
#else
    command_line->InitFromArgv(argc, argv);
#endif

    const bool bEnableOSR = true; //Whether to enable off-screen rendering
    ui::CefManager::ProcessType processType = ui::CefManager::GetProcessType(command_line);
    if (processType != ui::CefManager::BrowserProcess) {
        //Non-Browser process: should not include Browser process code
        int32_t nExitCode = 1;
        if (!ui::CefManager::GetInstance()->Initialize(bEnableOSR, _T("cef_browser"), argc, argv, nullptr, nExitCode)) {
            return nExitCode;
        }
        return 0;
    }

    // Browser process, create the main thread
    MainThread thread;

    //Must be called before CefManager::Initialize to set the DPI adaptive attribute, otherwise the display will be abnormal
    //Initialize the global resources, using a local folder as the resource
    ui::FilePath resourcePath = ui::GlobalManager::GetDefaultResourcePath(true);
    ui::GlobalManager::Instance().Startup(ui::LocalFilesResParam(resourcePath), thread.GetDpiInitParam());

    //Initialize CEF: must be done after GlobalManager is initialized, because GlobalManager is used during CEF initialization
    int32_t nExitCode = 1;
    if (!ui::CefManager::GetInstance()->Initialize(bEnableOSR, _T("cef_browser"), argc, argv, nullptr, nExitCode)) {
        return nExitCode;
    }

    //If the CEF message loop is not enabled, a timer needs to be started to call the CEF message processing function
    if (!ui::CefManager::GetInstance()->IsMultiThreadedMessageLoop()) {
        ui::CefManager::GetInstance()->ScheduleCefDoMessageLoopWork();
    }

    // Run the main thread loop
    thread.RunMessageLoop();

    // Clean up CEF
    ui::CefManager::GetInstance()->UnInitialize();
    return 0;
}

void TestApplication::AddMainWindow(ui::Window* pWindow)
{
    if (pWindow != nullptr) {
        ui::ControlPtrT<ui::Window> pMainWindow(pWindow);
        m_pMainWindows.push_back(pMainWindow);
    }
}

void TestApplication::RemoveMainWindow(ui::Window* pWindow)
{
    auto iter = m_pMainWindows.begin();
    while (iter != m_pMainWindows.end()) {
        if (*iter == pWindow) {
            iter = m_pMainWindows.erase(iter);
        }
        else {
            ++iter;
        }
    }
}

void TestApplication::SetActiveMainWindow(ui::Window* pWindow)
{
    m_pActiveWindow = pWindow;
}

void TestApplication::ActiveMainWindow()
{
    if (m_pActiveWindow != nullptr) {
        m_pActiveWindow->ShowWindow(ui::ShowWindowCommands::kSW_SHOW_NORMAL);
    }
    else {
        for (ui::ControlPtrT<ui::Window> pWindow : m_pMainWindows) {
            if ((pWindow != nullptr) && !pWindow->IsClosingWnd()) {
                pWindow->ShowWindow(ui::ShowWindowCommands::kSW_SHOW_NORMAL);
            }
        }
    }
}

void TestApplication::CloseMainWindow()
{
    for (ui::ControlPtrT<ui::Window> pWindow : m_pMainWindows) {
        if ((pWindow != nullptr) && !pWindow->IsClosingWnd()) {
            pWindow->CloseWnd();
        }
    }
}
