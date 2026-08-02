#include "App.h"
#include "CefForm.h"

// duilib
#include "duilib/duilib_cef.h"
#include "duilib/Utils/AppEntry.h"

App::App() :
    FrameworkThread(_T("App"), ui::kThreadUI)
{
}

App::~App()
{
}

App& App::Instance()
{
    static App self;
    return self;
}

int App::Run(int argc, char** argv)
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

    // Browser process: this App object itself is the main (UI) thread.
    // Must be called before CefManager::Initialize to set the DPI adaptive attribute, otherwise the display will be abnormal
    //Initialize the global resources, using a local folder as the resource
    ui::FilePath resourcePath = ui::GlobalManager::GetDefaultResourcePath(true);
    ui::GlobalManager::Instance().Startup(ui::LocalFilesResParam(resourcePath), GetDpiInitParam());

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

    // Run the main thread loop (calls OnInit() -> message loop -> OnCleanup())
    RunMessageLoop();

    // Clean up CEF
    ui::CefManager::GetInstance()->UnInitialize();
    return 0;
}

void App::SetMainWindow(ui::Window* pWindow)
{
    m_pMainWindow = pWindow;
}

void App::ActiveMainWindow()
{
    if (m_pMainWindow != nullptr) {
        m_pMainWindow->ShowWindow(ui::ShowWindowCommands::kSW_SHOW_NORMAL);
    }
}

void App::CloseMainWindow()
{
    if ((m_pMainWindow != nullptr) && !m_pMainWindow->IsClosingWnd()) {
        m_pMainWindow->CloseWnd();
    }
}

void App::OnInit()
{
    // Create a centered window with a default shadow
    CefForm* window = new CefForm();
    uint32_t dwExStyle = 0;
    if (ui::CefManager::GetInstance()->IsEnableOffScreenRendering()) {
        dwExStyle |= ui::kWS_EX_LAYERED;
    }
    ui::WindowCreateParam createWndParam;
    createWndParam.m_className = _T("cef");
    createWndParam.m_windowTitle = createWndParam.m_className;
    createWndParam.m_dwExStyle = dwExStyle;
    createWndParam.m_bCenterWindow = true;
    window->CreateWnd(nullptr, createWndParam);
    SetMainWindow(window);
    window->ShowWindow(ui::kSW_SHOW_NORMAL);
}

void App::OnCleanup()
{
    ui::GlobalManager::Instance().Shutdown();
}

const ui::DpiInitParam& App::GetDpiInitParam() const
{
    return m_dpiInitParam;
}

// On macOS the entry point is provided by main_macos.mm (Objective-C++ for CEF)
#if !defined(__APPLE__)
DUILIB_APP_ENTRY_ARGS(App)
#endif
