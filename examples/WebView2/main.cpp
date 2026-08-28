#include "dui/dui.h"
#include "MainForm.h"
#include "dui/WebView2/WebView2Manager.h"
#include "dui/Utils/AppEntry.h"

/** App: FrameworkThread subclass that serves as the DUI_APP_ENTRY target.
 *  RunMessageLoop() calls OnInit() -> message loop -> OnCleanup().
 */
class App : public ui::FrameworkThread
{
public:
    App() : FrameworkThread(_T("App"), ui::kThreadUI) {}

    void Run() { RunMessageLoop(); }

private:
    virtual void OnInit() override
    {
        //Initialize the global resources, using a local folder as the resource
        ui::FilePath resourcePath = ui::FilePathUtil::GetCurrentModuleDirectory();
        resourcePath += _T("resources\\");
        ui::GlobalManager::Instance().Startup(ui::LocalFilesResParam(resourcePath));

        //Initialize the basic configuration of WebView2
        DString userDataFolder = ui::WebView2Manager::GetInstance().GetDefaultUserDataFolder(_T("WebView2"));
        ui::WebView2Manager::GetInstance().Initialize(userDataFolder);

        // Create a centered window with a default shadow
        MainForm* window = new MainForm();
        window->CreateWnd(nullptr, ui::WindowCreateParam(_T("WebView2"), true));
        window->PostQuitMsgWhenClosed(true);
        window->ShowWindow(ui::kSW_SHOW_NORMAL);
    }

    virtual void OnCleanup() override
    {
        ui::WebView2Manager::GetInstance().UnInitialize();
        ui::GlobalManager::Instance().Shutdown();
    }
};

DUI_APP_ENTRY(App)
