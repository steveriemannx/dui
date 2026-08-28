#include "dui/dui.h"
#include "ControlForm.h"
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
        //Initialize global resources, use the local folder as the resource
        ui::FilePath resourcePath = ui::FilePathUtil::GetCurrentModuleDirectory();
        resourcePath += _T("resources\\");
        ui::GlobalManager::Instance().Startup(ui::LocalFilesResParam(resourcePath));

        // Create a centered window matching controls.xml
        ControlForm* window = new ControlForm();
        ui::WindowCreateParam createParam(_T("controls"), true);
        createParam.m_nWidth = 1100;
        createParam.m_nHeight = 800;
        window->CreateWnd(nullptr, createParam);
        window->PostQuitMsgWhenClosed(true);
        window->ShowWindow(ui::kSW_SHOW_NORMAL);
    }

    virtual void OnCleanup() override
    {
        ui::GlobalManager::Instance().Shutdown();
    }
};

int main()
{
    App app;
    app.Run();
    return 0;
}
