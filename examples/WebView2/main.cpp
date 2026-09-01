#include "dui/dui.h"
#include "MainForm.h"
#include "dui/WebView2/WebView2Manager.h"

/** App: FrameworkThread subclass that serves as the application entry.
 *  RunMessageLoop() calls OnInit() -> message loop -> OnCleanup().
 *  WebView2 must be initialized before the window is created and uninitialized
 *  after the message loop exits, so a custom entry (instead of ui::Run) is kept.
 */
class App : public ui::FrameworkThread
{
public:
    App() : FrameworkThread(DUI_T("App"), ui::kThreadUI) {}

    void Run() { RunMessageLoop(); }

private:
    virtual void OnInit() override
    {
        //Initialize the global resources, using a local folder as the resource
        ui::FilePath resourcePath = ui::FilePathUtil::GetCurrentModuleDirectory();
        resourcePath += DUI_T("resources\\");
        ui::GlobalManager::Instance().Startup(ui::LocalFilesResParam(resourcePath));

        //Initialize the basic configuration of WebView2
        DString userDataFolder = ui::WebView2Manager::GetInstance().GetDefaultUserDataFolder(DUI_T("WebView2"));
        ui::WebView2Manager::GetInstance().Initialize(userDataFolder);

        // Create a centered window with a default shadow
        MainForm* window = new MainForm();
        window->CreateWnd(nullptr, ui::WindowCreateParam(DUI_T("WebView2"), true));
        window->PostQuitMsgWhenClosed(true);
        window->ShowWindow(ui::kSW_SHOW_NORMAL);
    }

    virtual void OnCleanup() override
    {
        ui::WebView2Manager::GetInstance().UnInitialize();
        ui::GlobalManager::Instance().Shutdown();
    }
};

int main()
{
    App app;
    app.Run();
    return 0;
}
