#include "App.h"
#include "BrowserManager.h"
#include "duilib/WebView2/WebView2Manager.h"
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
    // Run the main thread loop (calls OnInit() -> message loop -> OnCleanup())
    RunMessageLoop();
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

void App::PostQuitMessage()
{
    // WebView2 is Windows-only: quit the UI-thread message loop directly
    ::PostQuitMessage(0);
}

void App::OnInit()
{
    //Initialize the global resources, using a local folder as the resource
    ui::FilePath resourcePath = ui::FilePathUtil::GetCurrentModuleDirectory();
    resourcePath += _T("resources\\");
    ui::GlobalManager::Instance().Startup(ui::LocalFilesResParam(resourcePath));

    //Initialize the basic configuration of WebView2
    DString userDataFolder = ui::WebView2Manager::GetInstance().GetDefaultUserDataFolder(_T("Webview2Browser"));
    ui::WebView2Manager::GetInstance().Initialize(userDataFolder);

    //Create the window and show the default page
    BrowserManager::GetInstance()->CreateBorwserBox(nullptr, "", _T(""));
}

void App::OnCleanup()
{
    ui::WebView2Manager::GetInstance().UnInitialize();
    ui::GlobalManager::Instance().Shutdown();
}

DUILIB_APP_ENTRY_ARGS(App)
