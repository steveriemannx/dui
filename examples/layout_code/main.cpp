#include "dui/dui.h"
#include "MainForm.h"
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
        //Initialize global resources, using a local folder as the resource
        ui::FilePath resourcePath = ui::FilePathUtil::GetCurrentModuleDirectory();
        resourcePath += _T("resources\\");
        ui::GlobalManager::Instance().Startup(ui::LocalFilesResParam(resourcePath));

        // Create a default centered window with shadow
        MainForm* window = new MainForm();
                ui::WindowCreateParam createParam(_T("layout"), true);
        ui::UiRect rcWork;
        ui::WindowBase::GetPrimaryMonitorWorkRect(rcWork);
        createParam.m_nWidth = (int32_t)(rcWork.Width() * 0.75f);
        createParam.m_nHeight = (int32_t)(rcWork.Height() * 0.75f);
        window->CreateWnd(nullptr, createParam);
        window->ShowWindow(ui::kSW_SHOW_NORMAL);
    }

    virtual void OnCleanup() override
    {
        ui::GlobalManager::Instance().Shutdown();
    }
};

DUI_APP_ENTRY(App)
