#include "duilib/duilib.h"
#include "MainForm.h"
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
        ui::FilePath resourcePath = ui::FilePathUtil::GetCurrentModuleDirectory();
        resourcePath += _T("resources\\");
        ui::GlobalManager::Instance().Startup(ui::LocalFilesResParam(resourcePath));

        MainForm* window = new MainForm();
        window->CreateWnd(nullptr, ui::WindowCreateParam(_T("Basic (Pure Code)"), true));
        window->PostQuitMsgWhenClosed(true);
        window->ShowWindow(ui::kSW_SHOW_NORMAL);
    }

    virtual void OnCleanup() override
    {
        ui::GlobalManager::Instance().Shutdown();
    }
};

DUILIB_APP_ENTRY(App)
