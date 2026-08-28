#include "App.h"
#include "browser/BrowserManager.h"
#include "browser/BrowserForm.h"
#include <chrono>

// dui
#include "dui/dui_cef.h"
#include "dui/Utils/AppEntry.h"

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
#ifdef DUI_BUILD_FOR_WIN
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

    // Browser process: this App object itself is the main (UI) thread.
    // Must be called before CefManager::Initialize to set the DPI adaptive attribute, otherwise the display will be abnormal
    //Initialize the global resources, using a local folder as the resource
    ui::FilePath resourcePath = ui::GlobalManager::GetDefaultResourcePath(true);
    ui::GlobalManager::Instance().Startup(ui::LocalFilesResParam(resourcePath), GetDpiInitParam());

    //Initialize CEF: must be done after GlobalManager is initialized, because GlobalManager is used during CEF initialization
    int32_t nExitCode = 1;
    if (!ui::CefManager::GetInstance()->Initialize(bEnableOSR, _T("cef_browser"), argc, argv, nullptr, nExitCode)) {
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

void App::AddMainWindow(ui::Window* pWindow)
{
    if (pWindow != nullptr) {
        ui::ControlPtrT<ui::Window> pMainWindow(pWindow);
        m_pMainWindows.push_back(pMainWindow);
    }
}

void App::RemoveMainWindow(ui::Window* pWindow)
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

void App::SetActiveMainWindow(ui::Window* pWindow)
{
    m_pActiveWindow = pWindow;
}

void App::ActiveMainWindow()
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

void App::CloseMainWindow()
{
    for (ui::ControlPtrT<ui::Window> pWindow : m_pMainWindows) {
        if ((pWindow != nullptr) && !pWindow->IsClosingWnd()) {
            pWindow->CloseWnd();
        }
    }
}

void App::OnInit()
{
    //Set the callback function that controls the main process singleton
    ui::CefManager::GetInstance()->SetAlreadyRunningAppRelaunch(UiBind(&App::OnAlreadyRunningAppRelaunch, this, std::placeholders::_1));

    //Create the first window
    std::string id = BrowserManager::GetInstance()->CreateBrowserID();
    BrowserManager::GetInstance()->CreateBorwserBox(nullptr, id, _T(""));
}

void App::OnCleanup()
{
    ui::GlobalManager::Instance().Shutdown();
}

void App::OnAlreadyRunningAppRelaunch(const std::vector<DString>& argumentList)
{
    if (ui::GlobalManager::Instance().IsInUIThread()) {
        //CEF 133 calls this interface
        BrowserForm* pBrowserForm = BrowserManager::GetInstance()->GetLastActiveBrowserForm();
        if (pBrowserForm != nullptr) {
            pBrowserForm->SetWindowForeground();
            if (!argumentList.empty()) {
                //Only process the first argument
                DString url = argumentList[0];
                pBrowserForm->OpenLinkUrl(url, false);
            }
        }
    }
    else {
        //Forward to the UI thread for processing
        ui::GlobalManager::Instance().Thread().PostTask(ui::kThreadUI, UiBind(&App::OnAlreadyRunningAppRelaunch, this, argumentList));
    }
}

const ui::DpiInitParam& App::GetDpiInitParam() const
{
    return m_dpiInitParam;
}

// On macOS the entry point is provided by main_macos.mm (Objective-C++ for CEF)
#if !defined(__APPLE__)
DUI_APP_ENTRY_ARGS(App)
#endif
