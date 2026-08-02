#include "duilib/duilib.h"
#include "ChatForm.h"
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
        //Initialize global resources, use the local folder as the resource
        ui::FilePath resourcePath = ui::FilePathUtil::GetCurrentModuleDirectory();
        resourcePath += _T("resources\\");
        ui::GlobalManager::Instance().Startup(ui::LocalFilesResParam(resourcePath));

        // A layout example mimicking WeChat (layout generated at build time from wechat.xml)
        ChatForm::ShowCustomWindow(ChatForm::kWechat);

        // A layout example mimicking the login window (layout generated at build time from login.xml)
        ChatForm::ShowCustomWindow(ChatForm::kLogin);
    }

    virtual void OnCleanup() override
    {
        ui::GlobalManager::Instance().Shutdown();
    }
};

DUILIB_APP_ENTRY(App)
