#include "duilib/duilib.h"
#include "BrowserManager.h"
#include "duilib/WebView2/WebView2Manager.h"
#include "duilib/Utils/AppEntry.h"

/** App: FrameworkThread subclass that serves as the DUILIB_APP_ENTRY target.
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
        DString userDataFolder = ui::WebView2Manager::GetInstance().GetDefaultUserDataFolder(_T("Webview2Browser"));
        ui::WebView2Manager::GetInstance().Initialize(userDataFolder);

        //Create the window and show the default page
        BrowserManager::GetInstance()->CreateBorwserBox(nullptr, "", _T(""));
    }

    virtual void OnCleanup() override
    {
        ui::WebView2Manager::GetInstance().UnInitialize();
        ui::GlobalManager::Instance().Shutdown();
    }
};

DUILIB_APP_ENTRY(App)
