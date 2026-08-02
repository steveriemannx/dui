#include "TestApplication.h"
#include "MainThread.h"
#include "CefForm.h"

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
    ui::CefManager::ProcessType processType = ui::CefManager::GetProcessType(command_line);
    if (processType != ui::CefManager::BrowserProcess) {
        //Non-Browser process: should not include Browser process code
        int32_t nExitCode = 1;
        if (!ui::CefManager::GetInstance()->Initialize(kEnableOffScreenRendering, _T("cef"), argc, argv, nullptr, nExitCode)) {
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

    //Initialize the additional startup parameters of the CEF module
    //ui::CefManager::GetInstance()->AppendSwitchWithValue(_T("proxy-server"), _T("socks5://127.0.0.1:1080"));

    //Initialize CEF: must be done after GlobalManager is initialized, because GlobalManager is used during CEF initialization
    int32_t nExitCode = 1;
    if (!ui::CefManager::GetInstance()->Initialize(kEnableOffScreenRendering, _T("cef"), argc, argv, nullptr, nExitCode)) {
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

void TestApplication::SetMainWindow(ui::Window* pWindow)
{
    m_pMainWindow = pWindow;
}

void TestApplication::ActiveMainWindow()
{
    if (m_pMainWindow != nullptr) {
        m_pMainWindow->ShowWindow(ui::ShowWindowCommands::kSW_SHOW_NORMAL);
    }
}

void TestApplication::CloseMainWindow()
{
    if ((m_pMainWindow != nullptr) && !m_pMainWindow->IsClosingWnd()) {
        m_pMainWindow->CloseWnd();
    }
}
