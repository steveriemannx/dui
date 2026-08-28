#include "dui/dui.h"
#include "MainForm.h"
#include "ThemeInit.h"
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
        ui::FilePath resourcePath = ui::FilePathUtil::GetCurrentModuleDirectory();
        resourcePath += _T("resources\\");
        ui::GlobalManager::Instance().Startup(ui::LocalFilesResParam(resourcePath));

        // Compile-time theme (colors, fonts, classes) - zero file dependencies
        InitThemeCompileTime();

        MainForm* window = new MainForm();
                ui::WindowCreateParam createParam(_T("Generated List Demo"), true);
        createParam.m_nWidth = 900;
        createParam.m_nHeight = 600;
        window->CreateWnd(nullptr, createParam);
        window->PostQuitMsgWhenClosed(true);
        window->ShowWindow(ui::kSW_SHOW_NORMAL);
    }

    virtual void OnCleanup() override
    {
        ui::GlobalManager::Instance().Shutdown();
    }
};

DUI_APP_ENTRY(App)
